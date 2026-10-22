# Bench wiring (proposed, not hardware-verified)

Use **one sketch at a time**. Power down before changing wiring; connect grounds together and place suitable current-limiting resistors in series with each LED channel. Check component polarity, current limits and board pin ratings against the actual parts on the workbench. These sketches assume active-HIGH outputs and a **common-cathode** RGB LED (or separate LEDs); a common-anode module needs different logic and wiring.

## Original / nonblocking LED sequence

| Arduino Uno digital output | Component |
| --- | --- |
| D2 | Yellow LED via resistor to LED anode; cathode to GND |
| D7 | Red LED via resistor to LED anode; cathode to GND |

Expected loop: yellow blinks ten times, 100 ms on / 100 ms off; then red blinks once, 100 ms on / 100 ms off. No button, sensor or radio is attached.

## Hachi I/O bench

| Arduino Uno | Connection |
| --- | --- |
| 5 V, GND | Compatible HC-SR04-style module VCC, common GND |
| D11 (OUTPUT) | Ultrasonic TRIG |
| D12 (INPUT) | Ultrasonic ECHO (**5 V on typical modules**) |
| D2 (OUTPUT) | Red LED anode through resistor |
| D4 (OUTPUT) | Green LED anode through resistor |
| D6 (OUTPUT) | Blue LED anode through resistor |
| GND | Common-cathode RGB common leg |

**Voltage warning:** Many HC-SR04 modules drive ECHO at 5 V. For an ESP32, RP2040, or other 3.3 V input, a suitable level shifter / divider and updated pin mapping are mandatory. Do not infer interchangeability from Arduino-IDE compatibility. This version is targeted at a 5 V Uno / classic Nano only. It makes no PWM brightness assumptions.

## Indication

| Sample | Requested light | Meaning |
| --- | --- | --- |
| Valid CLOSE | Red | Approximate proximity in the close band |
| Valid NEAR | Red + green (yellow) | Approximate proximity in the middle band |
| Valid FAR | Green | Valid distant ultrasonic echo **only**, not safety clearance |
| Timeout, out of range, injected X | Blue | NO_READING: measurement unavailable |

Physical color/intensity varies by RGB package, resistor values and wiring. A failure of the LED itself is not detected in firmware. No wiring here constitutes a road- or helmet-qualified warning system.
