# Firmware

Two Arduino sketches for ESP32 DevKit boards.

- `boat/`: LoRa receive, L298N motor, MG995 rudder, HUSKYLENS (I2C), turbidity reading, telemetry reply
- `ground-station/`: sends commands over LoRa from the Serial Monitor and prints the boat's telemetry

`protocol.h` is identical in both folders. If you change it, change both.

## Setup

1. Install the ESP32 board package in Arduino IDE.
2. Install libraries from Library Manager: **LoRa** (Sandeep Mistry), **ESP32Servo**. Install the **HUSKYLENS** library from https://github.com/HuskyLens/HUSKYLENSArduino.
3. Open `boat/boat.ino`, check the pins in `config.h` against your wiring, upload to the boat ESP32.
4. Open `ground-station/ground-station.ino`, upload to the second ESP32, open Serial Monitor at **115200** baud.
5. On the HUSKYLENS, set the protocol to **I2C** (General Settings > Protocol Type) and train the two classes: ID 1 = clean water, ID 2 = oil.

## Ground station keys

| Key | Action |
|-----|--------|
| w / s | throttle up / down |
| a / d | rudder left / right |
| c | centre rudder |
| x | stop |
| m | toggle manual / auto |
| k | recalibrate turbidity baseline (boat in clean water) |

## How it behaves

- The ground station sends a command every 250 ms. The boat answers each one with telemetry, so the radios never transmit at the same time.
- **Failsafe:** if the boat hears nothing valid for 2 s it stops the motor and centres the rudder.
- **Throttle cap:** `MAX_THROTTLE_PCT` (60%) in both `config.h` files. Raise it only after bench tests.
- **Auto mode** only moves when vision reports suspected oil for 3 frames in a row, steering toward the detected block. With nothing detected it holds still. It never wanders.
- **Reporting:** state 1 = vision only, state 2 = vision and turbidity both deviate. Both are reported as *suspected* oil, matching the project docs.
- Turbidity uses a baseline taken at boot (and on `k`), then flags a deviation in either direction, so it does not depend on which way your sensor's output moves.

## Status

Written from the project documents. **Not yet compiled or run on hardware.** First power-up checklist:

1. Test the ground station and boat with the motor and servo disconnected, watch the Serial Monitor.
2. Check LoRa link and RSSI.
3. Test the rudder with no load, then the motor with the propeller out of the water and clear of hands.
4. Calibrate turbidity in clean water, tune `TURB_DEVIATION_RAW`.
5. Only then do tank trials.
