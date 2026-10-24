# Bill of Materials

Source: technical abstract and the labelled circuit diagram. "To confirm" means the diagram marks the identity, model or rating as unverified. Fill in the real values as you verify them.

| # | Part | Qty | Role | Status |
|---|------|-----|------|--------|
| 1 | ESP32 DevKit | 2 | Boat controller; ground-station controller | Board variant to confirm |
| 2 | ESP32-CAM | 1 | Retained from original design | Role to define |
| 3 | HUSKYLENS 1 | 1 | On-board vision, I2C to boat ESP32 | OK |
| 4 | 433 MHz LoRa module (drawn as DRF1278F) | 2 | Boat and ground-station link | Verify actual module |
| 5 | L298N motor driver | 1 | Drives propulsion motor | OK |
| 6 | 12 V DC motor + propeller | 1 | Propulsion | Voltage and stall current to confirm |
| 7 | MG995 servo + rudder | 1 | Steering | Confirm actual servo model |
| 8 | Turbidity sensor + interface module | 1 | Water clarity reading | Confirm interface component |
| 9 | DC-DC converter A | 1 | Main supply (4R7 inductor board) | Output setting to confirm |
| 10 | DC-DC converter B | 1 | Dedicated servo supply | Output setting to confirm |
| 11 | Three-terminal regulator A (TO-220) | 1 | Regulation | Model and voltage to confirm |
| 12 | Three-terminal regulator B (TO-220) | 1 | Regulation, near HUSKYLENS | Model and voltage to confirm |
| 13 | AA battery holder | 1 | Power source in diagram | Cell count, chemistry and suitability to confirm |
| 14 | Inline power component | 1 | Unidentified in diagram | Identify |
| 15 | Rocker power switch | 1 | Main power | OK |
| 16 | Removable PU sponge | 1+ | Oil collection | Material grade to specify |

## Notes

- The diagram uses an AA battery holder, while the design calls for a 12 V motor and a servo with high current peaks. Confirm the real power source and current budget before powering the motor.
- The labelled diagram is an annotation of the original drawing. It is not a validated wiring or electrical-safety review.
