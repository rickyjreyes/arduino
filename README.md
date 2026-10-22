# Arduino LED Sequence

A small Arduino sketch that blinks a yellow LED ten times, then briefly flashes a red LED, repeating the sequence indefinitely.

## Source

The repository contains one extensionless sketch file, [`led`](led):

- Yellow LED output: digital pin **2**.
- Red LED output: digital pin **7**.
- Yellow LED: ten cycles of 100 ms on and 100 ms off.
- Red LED: 100 ms on and 100 ms off at the end of each cycle.

There are no input sensors or external libraries.

## Run it

1. Install the Arduino IDE and select an appropriate Arduino-compatible board.
2. Wire the two LEDs with current-limiting resistors and suitable ground connections.
3. Copy `led` to a local Arduino sketch named `led.ino` (the repository's original filename has no `.ino` extension).
4. Open `led.ino`, select the board and port, then upload.

The sequence is generated using `digitalWrite` and blocking `delay` calls; it is intended as a basic output demonstration, not a nonblocking scheduler.

## Status

Minimal 2017 learning example. Hardware has not been re-tested and no explicit license is included.
