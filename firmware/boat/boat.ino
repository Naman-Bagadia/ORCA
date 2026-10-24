// ORCA boat firmware (ESP32 DevKit)
// LoRa control link, L298N motor, MG995 rudder, HUSKYLENS vision (I2C), turbidity sensor.
//
// Libraries (Arduino Library Manager):
//   "LoRa" by Sandeep Mistry, "ESP32Servo", and the DFRobot "HUSKYLENS" library
//   (HUSKYLENSArduino on GitHub: https://github.com/HuskyLens/HUSKYLENSArduino)

#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
#include <ESP32Servo.h>
#include <HUSKYLENS.h>
#include "config.h"
#include "protocol.h"

HUSKYLENS huskylens;
Servo rudderServo;
ESP32PWM motorPwm;

// ---- commanded values ----
int cmdThrottle = 0;
int cmdRudder = 0;
int cmdMode = 0;            // 0 manual, 1 auto
int prevCal = 0;
unsigned long lastCmdMs = 0;
bool linkOk = false;

// ---- outputs ----
int outThrottle = 0;        // after ramping
unsigned long lastControlMs = 0;

// ---- vision ----
bool visionCamOk = false;
bool visionOil = false;     // debounced "suspected oil"
int visionX = -1;
int oilFrames = 0, clearFrames = 0;
unsigned long lastVisionMs = 0;

// ---- turbidity ----
float turbBaseline = 0;
int turbRaw = 0;
int turbDelta = 0;
bool turbDeviates = false;

// ======================= motor / rudder =======================
void setMotor(int pct) {
  pct = constrain(pct, -MAX_THROTTLE_PCT, MAX_THROTTLE_PCT);
  if (pct > 0) {
    digitalWrite(PIN_MOTOR_IN1, HIGH);
    digitalWrite(PIN_MOTOR_IN2, LOW);
  } else if (pct < 0) {
    digitalWrite(PIN_MOTOR_IN1, LOW);
    digitalWrite(PIN_MOTOR_IN2, HIGH);
  } else {
    digitalWrite(PIN_MOTOR_IN1, LOW);
    digitalWrite(PIN_MOTOR_IN2, LOW);
  }
  motorPwm.write(map(abs(pct), 0, 100, 0, 255));
}

void setRudder(int pct) {
  pct = constrain(pct, -100, 100);
  int angle = SERVO_CENTER_DEG + (pct * SERVO_RANGE_DEG) / 100;
  rudderServo.write(angle);
}

void stopAll() {
  cmdThrottle = 0;
  cmdRudder = 0;
  outThrottle = 0;
  setMotor(0);
  setRudder(0);
}

// ======================= turbidity =======================
int readTurbidityRaw() {
  long sum = 0;
  for (int i = 0; i < TURB_SAMPLES; i++) {
    sum += analogRead(PIN_TURBIDITY);
    delay(2);
  }
  return (int)(sum / TURB_SAMPLES);
}

void calibrateTurbidity() {
  turbBaseline = readTurbidityRaw();
  Serial.printf("Turbidity baseline set to %.0f (put the boat in CLEAN water first)\n", turbBaseline);
}

void updateTurbidity() {
  turbRaw = readTurbidityRaw();
  turbDelta = turbRaw - (int)turbBaseline;
  turbDeviates = abs(turbDelta) > TURB_DEVIATION_RAW;
}

// ======================= vision =======================
void updateVision() {
  if (millis() - lastVisionMs < VISION_PERIOD_MS) return;
  lastVisionMs = millis();

  if (!huskylens.request()) {
    visionCamOk = false;
    return;
  }
  visionCamOk = true;

  bool sawOil = false;
  int x = -1;
  while (huskylens.available()) {
    HUSKYLENSResult r = huskylens.read();
    if (r.ID == VISION_OIL_ID) {
      sawOil = true;
      x = r.xCenter;
    }
  }

  // debounce so one odd frame (reflection, glare) does not trigger anything
  if (sawOil) {
    oilFrames++;
    clearFrames = 0;
    visionX = x;
    if (oilFrames >= OIL_CONFIRM_FRAMES) visionOil = true;
  } else {
    clearFrames++;
    oilFrames = 0;
    if (clearFrames >= OIL_CLEAR_FRAMES) {
      visionOil = false;
      visionX = -1;
    }
  }
}

// 0 = nothing, 1 = vision only (suspected oil), 2 = vision + turbidity deviation
int detectionState() {
  if (!visionOil) return 0;
  return turbDeviates ? 2 : 1;
}

// ======================= radio =======================
void sendTelemetry(unsigned long seq) {
  int visionStatus = !visionCamOk ? 0 : (visionOil ? 2 : 1);
  char body[80];
  snprintf(body, sizeof(body), "T,%d,%d,%d,%d,%d,%d,%lu",
           cmdMode, visionStatus, visionX, turbRaw, turbDelta, detectionState(), seq);
  String pkt = orcaWrap(String(body));
  LoRa.beginPacket();
  LoRa.print(pkt);
  LoRa.endPacket();
  LoRa.receive();
}

void handleRadio() {
  int size = LoRa.parsePacket();
  if (size == 0) return;

  String packet;
  while (LoRa.available()) packet += (char)LoRa.read();

  String body;
  if (!orcaUnwrap(packet, body)) return;  // corrupted, ignore

  int t, r, m, cal;
  unsigned long seq;
  if (sscanf(body.c_str(), "C,%d,%d,%d,%d,%lu", &t, &r, &m, &cal, &seq) != 5) return;

  cmdThrottle = constrain(t, -100, 100);
  cmdRudder = constrain(r, -100, 100);
  cmdMode = (m == 1) ? 1 : 0;
  lastCmdMs = millis();
  linkOk = true;

  if (cal == 1 && prevCal == 0) calibrateTurbidity();
  prevCal = cal;

  sendTelemetry(seq);
}

// ======================= control loop =======================
void controlTick() {
  if (millis() - lastControlMs < CONTROL_PERIOD_MS) return;
  lastControlMs = millis();

  // failsafe
  if (millis() - lastCmdMs > FAILSAFE_MS) {
    if (linkOk) Serial.println("LINK LOST: stopping");
    linkOk = false;
    stopAll();
    return;
  }

  int targetThrottle = cmdThrottle;
  int targetRudder = cmdRudder;

  if (cmdMode == 1) {  // auto: only act on suspected oil, otherwise hold still
    if (visionOil && visionX >= 0) {
      float err = (float)(visionX - VISION_FRAME_WIDTH / 2) / (VISION_FRAME_WIDTH / 2);  // -1..1
      targetRudder = constrain((int)(err * 100.0f * AUTO_STEER_GAIN), -100, 100);
      targetThrottle = AUTO_THROTTLE_PCT;
    } else {
      targetThrottle = 0;
      targetRudder = 0;
    }
  }

  // ramp throttle so the motor does not jerk
  if (outThrottle < targetThrottle) outThrottle = min(outThrottle + RAMP_STEP_PCT, targetThrottle);
  else if (outThrottle > targetThrottle) outThrottle = max(outThrottle - RAMP_STEP_PCT, targetThrottle);

  setMotor(outThrottle);
  setRudder(targetRudder);
}

// ======================= setup / loop =======================
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nORCA boat starting");

  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  motorPwm.attachPin(PIN_MOTOR_ENA, 1000, 8);
  rudderServo.setPeriodHertz(50);
  rudderServo.attach(PIN_SERVO, SERVO_MIN_US, SERVO_MAX_US);
  stopAll();

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_TURBIDITY, ADC_11db);
  calibrateTurbidity();

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  if (huskylens.begin(Wire)) {
    huskylens.writeAlgorithm(ALGORITHM_OBJECT_CLASSIFICATION);
    Serial.println("HUSKYLENS ready");
  } else {
    Serial.println("HUSKYLENS NOT FOUND: check I2C wiring and set its protocol to I2C");
  }

  SPI.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI, PIN_LORA_NSS);
  LoRa.setPins(PIN_LORA_NSS, PIN_LORA_RST, PIN_LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ_HZ)) {
    Serial.println("LoRa init FAILED: check wiring and antenna");
    while (true) delay(1000);
  }
  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.setTxPower(LORA_TX_POWER_DBM);
  LoRa.receive();
  Serial.println("LoRa ready. Waiting for ground station.");
}

void loop() {
  handleRadio();
  updateVision();
  updateTurbidity();
  controlTick();
}
