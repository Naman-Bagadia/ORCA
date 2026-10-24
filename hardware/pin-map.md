# Pin Map (to be completed)

Fill this in from the real wiring, then check it against the firmware.

## Boat ESP32

| Function | Interface | ESP32 pin | Notes |
|----------|-----------|-----------|-------|
| LoRa radio | SPI (SCK, MISO, MOSI, NSS) + RESET + DIO | TBC | 433 MHz module |
| HUSKYLENS 1 | I2C (SDA, SCL) | TBC | Reports suspected-oil result |
| L298N motor driver | PWM / direction | TBC | One DC motor |
| MG995 servo | PWM | TBC | Powered from dedicated supply |
| Turbidity sensor | Analog or digital | TBC | Check the chosen ADC channel works alongside the radio setup |

## Ground-station ESP32

| Function | Interface | ESP32 pin | Notes |
|----------|-----------|-----------|-------|
| LoRa radio | SPI + RESET + DIO | TBC | 433 MHz module |
| Operator input | TBC | TBC | Manual control |

## Checks before first power-up

- No two functions share a pin
- Servo and motor supplies share a common ground with the ESP32
- 3.3 V logic levels respected on every module
- Fuse or current limit on the motor supply
