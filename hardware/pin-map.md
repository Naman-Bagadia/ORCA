# Pin Map

The pins below are the **firmware defaults** in `firmware/*/config.h`. They were proposed, not read from the circuit diagram, so wire to match them or edit `config.h` to match your wiring. Update this file once the real wiring is confirmed.

## Boat ESP32

| Function | Interface | ESP32 pin | Notes |
|----------|-----------|-----------|-------|
| LoRa SCK / MISO / MOSI | SPI (VSPI) | 18 / 19 / 23 | 433 MHz SX1278 module |
| LoRa NSS / RST / DIO0 | SPI control | 5 / 14 / 26 | |
| HUSKYLENS SDA / SCL | I2C | 21 / 22 | Set HUSKYLENS protocol to I2C |
| L298N ENA (PWM) | PWM | 25 | Speed |
| L298N IN1 / IN2 | Digital | 32 / 33 | Direction |
| MG995 servo signal | PWM | 13 | Dedicated supply, shared ground |
| Turbidity analog out | ADC1 | 34 | Scale sensor output to 3.3 V max |

## Ground-station ESP32

| Function | Interface | ESP32 pin | Notes |
|----------|-----------|-----------|-------|
| LoRa SCK / MISO / MOSI | SPI (VSPI) | 18 / 19 / 23 | |
| LoRa NSS / RST / DIO0 | SPI control | 5 / 14 / 26 | |
| Operator input | USB serial | n/a | Serial Monitor, 115200 baud |

## Checks before first power-up

- No two functions share a pin
- Servo and motor supplies share a common ground with the ESP32
- 3.3 V logic levels respected on every module (LoRa is 3.3 V only)
- Fuse or current limit on the motor supply
- Antenna attached to each LoRa module before transmitting
