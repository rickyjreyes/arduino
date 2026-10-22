# Hachi I/O bench reproducibility checklist

Separate **native source checks**, **board compilation** and **observed hardware tests**. Only mark an item passed after actually executing it. Record date, operator, board/core version, supply voltage, sensor module, wiring, commit, serial transcript and observed LED state.

| ID | Procedure | Expected observation |
| --- | --- | --- |
| S01 | Compile native `tests/test_bench.cpp` and run | Exit 0; boundary/hysteresis/rollover assertions pass |
| S02 | Compile both new sketches for `arduino:avr:uno` | Both compile independently |
| H01 | Flash LEDSequence; observe for several cycles | Ten yellow pulses, then one red, repeating; no sensor required |
| H02 | Flash HachiIOBench; open 115200 baud newline serial | Boot header then automatic readings; initial blue until a valid reading |
| H03 | Disconnect ECHO signal safely or remove sensor power with board off, then power on | After echo timeout sample is `valid=0,NA,NO_READING`; blue, not green |
| H04 | Send `M 35` | Injected valid sample, `CLOSE`, red; automatic sampling pauses |
| H05 | Send `M 55`, then `M 61` after H04 | CLOSE remains CLOSE at 55, then transitions NEAR at 61 |
| H06 | Send `M 160`, `M 145`, `M 139` | FAR, FAR, NEAR (hysteresis) |
| H07 | Send `X`, then `M 200` | NO_READING blue, then valid FAR green; no fake zero reading |
| H08 | Send `M 1`, `M 401`, `M abc`, overlong line | ERR, no change to active mode or previous indication |
| H09 | Send `A` | Automatic polling resumes, emits sensor-based AUTO rows |
| H10 | Remove USB serial/host client while in AUTO | Local sampling and LEDs continue without host connection |

For a manual transcript, send one command per line. Compare field order to `SAMPLE,uptime_ms,source,valid,cm_or_NA,state`; do not treat the sample `uptime_ms` or sensor range as calibrated distance. Since `M` and `X` are intentional injected observations, **do not pass their results off as physical sensor measurements**. Long blocking Serial writes or heavy host traffic can perturb microsecond echo timing on an 8-bit board; evaluate this separately before any timing-sensitive application.

## Failure boundaries

- Unknown/invalid data never maps to an all-clear indication.
- The 50/150 cm bands are retained as a historical Hachi illustration, not empirically validated alerts.
- No vehicle classification, speed estimation, collision probability, helmet certification, BLE, navigation, AR, phone link or field testing is implemented here.
- If a bench test fails, preserve the raw observation and reproduce it before changing thresholds or filtering.
