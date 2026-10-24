# ORCA: Oil Removal and Collection Assistant

A compact, remotely operated robotic boat (unmanned surface vehicle) that helps an operator inspect a water surface and collect floating oil with a removable polyurethane (PU) sponge.

ORCA is **not** a replacement for professional oil-spill response equipment. It explores how small, low-cost robotics can make small-scale, localised surface-oil cleanup safer and more accessible, with a human operator in control and honest reporting of what works and what does not.

> **Status:** design and prototype stage. Contained-tank trials are planned. No performance results are claimed yet.

## The problem

Thin oily films near boat landings, drain outlets and pond edges are easy to miss and hard to reach. Floating oil drifts with wind and current, and manual collection can mean working from slippery banks or boats. See [docs/problem-statement.md](docs/problem-statement.md).

## How it works

- **Boat controller:** ESP32 DevKit. Receives commands, drives the motor and servo, reads sensors.
- **Propulsion:** one 12 V DC motor with propeller, driven through an L298N motor driver.
- **Steering:** one MG995 servo on a rudder, on a dedicated supply.
- **Control link:** two 433 MHz LoRa modules (one on the boat, one at the ground station). Carries movement commands and selected sensor/status readings. **Not** live video.
- **Ground station:** second ESP32 DevKit with a LoRa module. Manual control is the starting point.
- **Vision:** HUSKYLENS 1 reports results to the boat ESP32 over I2C. The model is a two-class classifier (clean water vs. water with visible signs of oil), trained on Edge Impulse. Output is called **suspected oil**, never confirmed contamination. An ESP32-CAM is retained from the original design.
- **Confirmation signal:** a turbidity sensor. Turbidity and vision are treated as separate observations that are combined to reduce false positives. Cloudy water is not necessarily oily, and an oil film can sit on clear water.
- **Collection:** a removable PU sponge, replaceable for disposal.

Full details: [docs/technical-abstract.md](docs/technical-abstract.md).

## Repository layout

```
docs/       problem statement, technical abstract
hardware/   circuit diagram, bill of materials, pin map
firmware/   boat/ and ground-station/ ESP32 code
vision/     dataset notes and model details (Edge Impulse)
tests/      sponge and tank-trial protocols and logs
media/      photos and demo videos
```

## Test plan

- Oil collected per gram of dry sponge (g oil / g sponge)
- Water uptake, assessed separately
- Collection time
- Whether the sponge holds oil while the boat moves
- How easily the sponge is replaced
- Performance change after reuse
- Vision testing on **unfamiliar scenes**, not only the training images (reflections, shadows, ripples, floating debris)

Protocol and log template: [tests/README.md](tests/README.md).

## Known limitations and open items

- The Argonne Oleo Sponge figure (reported up to 90x its weight in oil) applies to a specially engineered research material. It is **not** a result for ORCA's sponge, and ordinary PU foam should not be assumed to match it.
- Visual classification can be fooled by reflections, shadows and debris.
- Several parts are unverified in the circuit diagram. See [hardware/BOM.md](hardware/BOM.md).
- The circuit diagram is an annotated drawing, not a validated wiring or electrical-safety review.
- Intended for controlled and suitable water conditions only.

## Safety

Do not operate near people or animals in the water. Keep a human operator in control. Dispose of collected oil and used sponges appropriately.
