#pragma once
// ===================== BOAT ESP32 CONFIG =====================
// Pin numbers below are PROPOSED defaults for a standard ESP32 DevKit.
// Your circuit diagram does not list pin numbers, so change these to match your real wiring.

// LoRa module (SX1278, e.g. DRF1278F) on VSPI
#define PIN_LORA_SCK   18
#define PIN_LORA_MISO  19
#define PIN_LORA_MOSI  23
#define PIN_LORA_NSS    5
#define PIN_LORA_RST   14
#define PIN_LORA_DIO0  26

// HUSKYLENS 1 (I2C)
#define PIN_I2C_SDA    21
#define PIN_I2C_SCL    22

// L298N motor driver (one motor on channel A)
#define PIN_MOTOR_ENA  25   // PWM speed
#define PIN_MOTOR_IN1  32
#define PIN_MOTOR_IN2  33

// Rudder servo (MG995, powered from its own supply, grounds shared with ESP32)
#define PIN_SERVO      13

// Turbidity sensor analog output. Use an ADC1 pin (32-39). Pin 34 is input-only.
// The sensor board can output up to ~4.5 V: scale it down to 3.3 V max before the ESP32 pin.
#define PIN_TURBIDITY  34

// ---------- Radio ----------
#define LORA_FREQ_HZ        433E6
#define LORA_SYNC_WORD      0xA5   // must match the ground station
#define LORA_TX_POWER_DBM   14

// ---------- Safety ----------
#define MAX_THROTTLE_PCT    60     // hard cap on motor power while testing
#define FAILSAFE_MS         2000   // no valid command for this long -> stop motor, centre rudder
#define RAMP_STEP_PCT       5      // max throttle change per control tick
#define CONTROL_PERIOD_MS   50

// ---------- Rudder ----------
#define SERVO_CENTER_DEG    90
#define SERVO_RANGE_DEG     40     // +/- degrees from centre at 100%
#define SERVO_MIN_US        500
#define SERVO_MAX_US        2400

// ---------- Vision (HUSKYLENS) ----------
// Assumes HUSKYLENS is trained in Object Classification mode:
//   ID 1 = clean water, ID 2 = water with visible signs of oil
#define VISION_OIL_ID           2
#define VISION_PERIOD_MS        100
#define VISION_FRAME_WIDTH      320
#define OIL_CONFIRM_FRAMES      3  // consecutive oil frames before "suspected oil"
#define OIL_CLEAR_FRAMES        5  // consecutive non-oil frames before clearing

// ---------- Auto mode ----------
#define AUTO_THROTTLE_PCT   35
#define AUTO_STEER_GAIN     1.0f   // 1.0 = full rudder when target is at the frame edge

// ---------- Turbidity ----------
#define TURB_SAMPLES            16
#define TURB_DEVIATION_RAW      250  // |reading - baseline| above this counts as a deviation (tune in tank)
