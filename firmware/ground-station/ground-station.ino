// ORCA ground station firmware (ESP32 DevKit + LoRa)
// Control the boat from the Arduino Serial Monitor (115200 baud).
//
// Keys:
//   w / s  throttle +/-      x  stop (throttle 0, rudder centre)
//   a / d  rudder left/right c  centre rudder
//   m      toggle manual / auto mode
//   k      recalibrate turbidity baseline (boat must be in CLEAN water)
//   h      help
//
// Library: "LoRa" by Sandeep Mistry (Arduino Library Manager)

#include <SPI.h>
#include <LoRa.h>
#include "config.h"
#include "protocol.h"

int throttle = 0;
int rudder = 0;
int mode = 0;            // 0 manual, 1 auto
int calCountdown = 0;    // send cal=1 for a few packets so one dropped packet does not lose it
unsigned long seq = 0;
unsigned long lastSendMs = 0;
unsigned long lastReplyMs = 0;
bool warnedNoReply = false;
int printCounter = 0;

void printHelp() {
  Serial.println(F("Keys: w/s throttle, a/d rudder, c centre rudder, x stop, m manual/auto, k calibrate turbidity, h help"));
}

void printStatus() {
  Serial.printf("[cmd] mode=%s throttle=%d rudder=%d\n", mode ? "AUTO" : "MANUAL", throttle, rudder);
}

void handleKey(char ch) {
  switch (ch) {
    case 'w': throttle = constrain(throttle + THROTTLE_STEP, -MAX_THROTTLE_PCT, MAX_THROTTLE_PCT); break;
    case 's': throttle = constrain(throttle - THROTTLE_STEP, -MAX_THROTTLE_PCT, MAX_THROTTLE_PCT); break;
    case 'a': rudder = constrain(rudder - RUDDER_STEP, -100, 100); break;
    case 'd': rudder = constrain(rudder + RUDDER_STEP, -100, 100); break;
    case 'c': rudder = 0; break;
    case 'x': throttle = 0; rudder = 0; break;
    case 'm': mode = !mode; if (mode) { throttle = 0; rudder = 0; } break;
    case 'k': calCountdown = 3; Serial.println(F("Sending turbidity recalibration")); break;
    case 'h': printHelp(); return;
    default: return;  // ignore newlines and anything else
  }
  printStatus();
}

void sendCommand() {
  int cal = (calCountdown > 0) ? 1 : 0;
  if (calCountdown > 0) calCountdown--;
  seq++;

  char body[64];
  snprintf(body, sizeof(body), "C,%d,%d,%d,%d,%lu", throttle, rudder, mode, cal, seq);
  String pkt = orcaWrap(String(body));

  LoRa.beginPacket();
  LoRa.print(pkt);
  LoRa.endPacket();
  LoRa.receive();
}

const char *stateText(int s) {
  if (s == 2) return "SUSPECTED OIL (vision + turbidity agree)";
  if (s == 1) return "SUSPECTED OIL (vision only)";
  return "nothing suspected";
}

void handleReply() {
  int size = LoRa.parsePacket();
  if (size == 0) return;

  String packet;
  while (LoRa.available()) packet += (char)LoRa.read();
  int rssi = LoRa.packetRssi();

  String body;
  if (!orcaUnwrap(packet, body)) return;

  int m, vision, vx, turb, delta, state;
  unsigned long rseq;
  if (sscanf(body.c_str(), "T,%d,%d,%d,%d,%d,%d,%lu", &m, &vision, &vx, &turb, &delta, &state, &rseq) != 7) return;

  lastReplyMs = millis();
  warnedNoReply = false;

  if (++printCounter >= PRINT_EVERY_N) {
    printCounter = 0;
    Serial.printf("[boat] RSSI %d dBm | mode=%s | camera=%s | x=%d | turbidity raw=%d (delta %d) | %s\n",
                  rssi, m ? "AUTO" : "MANUAL",
                  vision == 0 ? "NOT RESPONDING" : (vision == 2 ? "oil seen" : "no oil"),
                  vx, turb, delta, stateText(state));
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nORCA ground station starting");

  SPI.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI, PIN_LORA_NSS);
  LoRa.setPins(PIN_LORA_NSS, PIN_LORA_RST, PIN_LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ_HZ)) {
    Serial.println("LoRa init FAILED: check wiring and antenna");
    while (true) delay(1000);
  }
  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.setTxPower(LORA_TX_POWER_DBM);
  LoRa.receive();
  printHelp();
}

void loop() {
  while (Serial.available()) handleKey((char)Serial.read());

  if (millis() - lastSendMs >= CMD_PERIOD_MS) {
    lastSendMs = millis();
    sendCommand();
  }

  handleReply();

  if (!warnedNoReply && millis() - lastReplyMs > NO_REPLY_MS) {
    Serial.println(F("[boat] no reply: check power, antennas, range"));
    warnedNoReply = true;
  }
}
