# Airspeed26 host-side test harness

Host-side C++ test build for the pure helpers in `../Airspeed26.ino`. It lets
the hardware-independent logic be property-tested off-target, without an Arduino
board, `Wire`, or `Serial`.

## Quick start

```sh
make test          # build (fetching RapidCheck if needed) and run tests once
# or
./run_tests.sh     # same thing
```

Other targets:

```sh
make               # build only
make clean         # remove build artifacts (build/)
./fetch_rapidcheck.sh   # (re)vendor RapidCheck at the pinned commit
```

Requirements: a C++14 compiler (`c++`/`clang++`/`g++`), `make`, and `git`
(used once to fetch the test library). No CMake required.

## How the pure helpers are reused (not forked)

The sketch is the single source of truth. `include_sketch.cpp` `#include`s
`../Airspeed26.ino` **unchanged** and compiles it against small host stubs for
the Arduino runtime:

- `arduino_stubs.h` — stand-ins for `Serial`, `Wire`, `millis()`, and the
  `F()` / `__FlashStringHelper` flash-string machinery. `Serial` captures output
  into a buffer so later unit tests can assert on emitted records/diagnostics.
- `Wire.h` — a local shadow of the Arduino header. Because this directory is
  first on the include path (`-I.`), the sketch's `#include <Wire.h>` resolves
  here instead of to a real Arduino library.

This is the "include the .ino behind stubs" mechanism called out in the design's
Testing Strategy, chosen so `decode_frame`, `pressure_transfer_fcn`,
`temperature_transfer_fcn`, `air_density`, `airspeed_from_dp`, `mean_of`, and the
testable seams (`dispatch_status`, `diag_message`, `collect_zero_offset`,
`run_measurement_cycle_with`) are compiled exactly as they ship on-target.

The hardware paths (`read_sensor_raw`, the real I2C transaction, the `millis()`
cadence) are **not** exercised here — they are validated on the vehicle per the
design.

## Property-based testing library

Uses [RapidCheck](https://github.com/emil-e/rapidcheck), the library prescribed
by the design. It is **not** committed to this repo; `fetch_rapidcheck.sh`
vendors it into `./rapidcheck/` at a pinned commit
(`6e8dadfdafa3a74eabb52ead87f8787f72eccd0b`) and `make` invokes it automatically
when the sources are missing. RapidCheck is compiled from source by the Makefile
(no CMake dependency).

## Files

| File | Purpose |
|------|---------|
| `Makefile` | Single-run build + `test` target; builds RapidCheck from source. |
| `run_tests.sh` | One-shot `make test` wrapper. |
| `fetch_rapidcheck.sh` | Vendors RapidCheck at the pinned commit (idempotent). |
| `arduino_stubs.h` | Host stubs for `Serial`/`Wire`/`millis()`/`F()`. |
| `Wire.h` | Local shadow so `#include <Wire.h>` resolves to the stubs. |
| `include_sketch.cpp` | Compiles `../Airspeed26.ino` unchanged behind the stubs. |
| `test_main.cpp` | Test entry point: smoke test + placeholders for Properties 1–6. |

## Test status

Task 4 establishes the harness with a **smoke test** (a RapidCheck property over
`decode_frame`, 100+ iterations) plus compile-time reachability checks for every
pure helper. The full property tests for Properties 1–6 are written in tasks
2.2, 3.2, 3.4, 3.6, 3.8, and 3.10 — their landing spots are marked in
`test_main.cpp`.
