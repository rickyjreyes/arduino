# Arduino — Hachi Gear Hardware Test Bed

This repository began as a small Arduino LED sequencing experiment during the Hachi Gear prototyping period. The **original extensionless [`led`](led) source is preserved byte-for-byte** (yellow LED on digital 2, red LED on digital 7). This repository now offers an independent, repeatable **hardware I/O bench**, not an alternate Hachi product firmware or a claim that the original LED experiment implemented the full Hachi vision.

The separate [Hachi Gear project](https://github.com/rickyjreyes/Hachi-Gear) contains product concepts, historical materials and its own experimental sensor/BLE versions. Here, simple components can be characterized, failure states exercised and behavior reproduced without a phone, wearable HUD, a moving vehicle or an integrated helmet.

## Choose a sketch

| Path | Purpose | Hardware |
| --- | --- | --- |
| [`led`](led) | Original 2017-era blocking sequence, unchanged | Yellow D2, red D7 + resistors |
| [`firmware/LEDSequence/LEDSequence.ino`](firmware/LEDSequence/LEDSequence.ino) | Nonblocking equivalent of the original sequence | Yellow D2, red D7 + resistors |
| [`firmware/HachiIOBench/HachiIOBench.ino`](firmware/HachiIOBench/HachiIOBench.ino) | HC-SR04-style timing, valid/invalid observations, RGB output and serial fault injection | D11 trigger, D12 echo, D2 red, D4 green, D6 blue + resistors |
| [`firmware/HachiIOBench/BenchLogic.h`](firmware/HachiIOBench/BenchLogic.h) | Hardware-independent approximation and indicator classification, including hysteresis | Host C++11 or Arduino |
| [`tests/test_bench.cpp`](tests/test_bench.cpp) | Native boundary, rollover, invalid-data and transition checks | C++11 compiler |

**No additional Arduino libraries required.** Sketches are separate and must not be flashed together. The Hachi bench uses digital on/off indication (no assumptions about PWM support). It is designed for an Arduino Uno / classic Nano **5 V bench**; check electrical compatibility before using any other board.

## Quick start

1. Wire the selected sketch according to [bench wiring](docs/WIRING.md). Do not connect any ultrasonic module's 5 V ECHO output directly to a 3.3 V input.
2. Open the directory containing the `.ino` sketch in Arduino IDE, select the actual board/port and upload; or run the CLI commands below.
3. For the Hachi I/O bench, open Serial Monitor at **115200 baud**, set newline line endings, and send `?` for help. The LED stays blue when a reading is unavailable, never green as a substitute for unknown data.
4. Follow the [repeatable test plan](docs/TEST_PLAN.md). Record real board model, voltage, wiring, firmware commit and test observations. Source and host-unit-test success are not proof of electrical or outdoor operation.

```sh
# With Arduino CLI and arduino:avr installed:
arduino-cli compile --fqbn arduino:avr:uno firmware/LEDSequence
arduino-cli compile --fqbn arduino:avr:uno firmware/HachiIOBench
# Upload only the sketch you actually wired, replacing the port:
arduino-cli upload -p YOUR_PORT --fqbn arduino:avr:uno firmware/HachiIOBench

# Independent host logic tests (no device required):
g++ -std=c++11 -Wall -Wextra -Werror -pedantic tests/test_bench.cpp -o /tmp/hachi-bench-tests
/tmp/hachi-bench-tests
```

### Serial bench protocol

Text input is newline-terminated: `?` help, `A` switch back to automatic sampling, `M 35` inject an explicitly valid 35 cm observation (and suspend the sensor), `X` inject NO_READING (and suspend the sensor). Manual distance accepts **2..400 cm** only; invalid commands yield `ERR` and leave the active mode unchanged. No dynamic `String` allocation. Output is CSV-like:

```text
HACHI_IO_BENCH,1,UNVALIDATED_HARDWARE
SAMPLE,240,AUTO,1,83,NEAR
SAMPLE,255,INJECT,0,NA,NO_READING
SAMPLE,310,INJECT,1,35,CLOSE
MODE,AUTO
```

Format: `SAMPLE,uptime_ms,source,valid,cm_or_NA,state`. `source` is `AUTO` or `INJECT`. These are example rows, **not a recorded hardware run**. NO_READING is distinct from a valid distant echo. State `FAR` describes an approximate echo distance only; it does **not** assert an unobstructed path or collision safety. The algorithm uses approximate 50/150 cm starting bands and 10 cm transition hysteresis for bench visualization, not validated rider-alert thresholds.

## Provenance and limits

The historical `led` file is intentionally not renamed or reformatted. To run it in the IDE, copy it into a directory named `led` as `led.ino`; use the separate nonblocking sketch for current experiments. The native tests exercise pure logic; CI also requests AVR sketch builds. A compile check cannot establish correct wiring, sensor performance, timing under all serial loads, environmental suitability or actual field safety. **No live integrated hardware tests have been reported here.** The repository includes no explicit grant of license; consult the author before reuse beyond applicable default rights.
