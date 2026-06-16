# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Upload

This is an Arduino sketch with no build system in the repo. All compilation and uploading is done through the Arduino IDE:

- Open `MorseTrainer.ino` in the Arduino IDE
- Select the correct board and port
- Use Sketch → Upload (or Ctrl+U)

There are no automated tests. The Serial Monitor (9600 baud) is the primary debugging tool; enable it by uncommenting `#define DEBUG` in `MorseTrainer.ino`.

**Flash capacity warning**: Exceeding ~50–60% of flash capacity can brick the Arduino. Keep an eye on the size report after compiling.

## Architecture

### Stream pipeline

The core design is a chain of `Enumerable<char>` iterators:

```
[Word source] → MorseStream → MorseClock → DigitalLED
```

- **Word source** (`RandomWords`, `RepeatStream`): yields characters of words/phrases
- **`MorseStream`**: translates each character into its Morse dot/dash string using `AsciiMorse`
- **`MorseClock`**: consumes the dot/dash stream and drives the LED output with correct Farnsworth timing
- **`DigitalLED`**: writes the boolean LED state to the physical pin

All classes implement the `Enumerable<char>` interface from the external Arduino library (not in this repo). The `/` character is used internally to distinguish Farnsworth word-space from letter-space.

### Morse timing

Timing follows the PARIS standard: `dit = 1200ms / WPM`. Dahs are 3× dits. `MorseClock` uses two independent speeds — `charSpeed` for character timing and `farnsworthSpeed` (typically lower) for inter-letter and inter-word gaps.

### PROGMEM pattern

All string data is stored in PROGMEM (flash) rather than RAM, which is scarce on Arduino. `ProgmemStrings<BUF_LEN>` (`ProgmemStrings.hpp`) is a template wrapper that reads strings out of PROGMEM via `operator[]`. The template parameter `BUF_LEN` must be at least as large as the longest string in that collection.

### Word lists

Each word list is a self-contained `.hpp` that defines `Words` (a `ProgmemStrings` instance), `Words_length`, `KochSeries[]`, and `KochLevels`. Exactly one is `#include`d in the main sketch — they cannot coexist:

| File | Contents |
|---|---|
| `ElizaWords.hpp` | ELIZA therapy-style sentences (~112 entries) |
| `KochWords.hpp` | Koch-method words ordered by character difficulty (~779 entries, 10 levels) |
| `Words.hpp` | ~963 common English words |
| `EasyWords.hpp` | ~301 short common words (older style, not yet converted to `ProgmemStrings`) |
| `TestWords.hpp` | ~100 long text excerpts for advanced practice (older style) |

`RandomWords` picks a word at random from `Words[0 .. KochSeries[kochLevel]]` — lower Koch levels restrict to simpler characters.

### Hardware config

Exactly one hardware config header must be included (defines pin assignments via `Config`):

```cpp
// #include <BreadboardConfig.hpp>
// #include <LeonardoConfig.hpp>
#include <MinimaConfig.hpp>
```

### External library

`Scheduler.hpp`, `Clock.hpp`, `Led.hpp`, `Mapper.hpp`, `EncoderWheel.hpp`, `ButtonHandler.hpp`, and the config headers are from an external Arduino library (not in this repo). They provide cooperative scheduling (`MainSchedule`/`schedule.poll()`), `Enumerable<char>`, `Timer`, `Scheduled`, `Enabled`, `EdgeDetectorBase`, and hardware abstractions.
