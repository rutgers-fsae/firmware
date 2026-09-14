// arduino_stubs.h — Minimal host-side stubs for the Arduino runtime.
//
// The Airspeed26 sketch is included UNCHANGED into the host test build (see
// include_sketch.cpp). It pulls in <Wire.h> and uses Serial, millis(), the
// F()/__FlashStringHelper flash-string machinery, and Arduino's sqrt(). None of
// that exists off-target, so this header provides just-enough stand-ins so the
// pure helpers (decode_frame, pressure_transfer_fcn, temperature_transfer_fcn,
// air_density, airspeed_from_dp, mean_of) plus the testable seams
// (dispatch_status, diag_message, collect_zero_offset, run_measurement_cycle_with)
// compile and run without hardware.
//
// The stubs intentionally do NOT emulate real I2C/UART behavior. The hardware
// paths (read_sensor_raw, the real Wire transaction, timing cadence) are
// validated on the vehicle per the design's Testing Strategy, not here. The
// Serial stub captures written text into a buffer so host unit tests (e.g. the
// serial-record and diagnostics tests written in later tasks) can assert on it.
//
// Feature: pitot-airspeed-sensor — host test harness (task 4).

#ifndef AIRSPEED26_ARDUINO_STUBS_H
#define AIRSPEED26_ARDUINO_STUBS_H

#include <cstdint>
#include <cmath>
#include <cstdio>
#include <string>

// --- Flash-string machinery -------------------------------------------------
// On AVR, F("...") wraps a string in a __FlashStringHelper*. Host-side we just
// treat flash strings as plain C strings.
class __FlashStringHelper;
#define F(str) (reinterpret_cast<const __FlashStringHelper *>(str))

// --- Serial -----------------------------------------------------------------
// Captures everything "printed" so tests can inspect the emitted records and
// diagnostic messages. print() mirrors the subset of Arduino's Print API the
// sketch uses: floats with an optional digit count, chars, C strings, and
// flash strings.
class SerialStub {
public:
  std::string buffer;

  void begin(unsigned long) {}

  void clear() { buffer.clear(); }

  void print(const char *s) { buffer += s; }
  void print(char c) { buffer += c; }

  void print(double value, int digits = 2) {
    char tmp[64];
    std::snprintf(tmp, sizeof(tmp), "%.*f", digits, value);
    buffer += tmp;
  }

  void print(const __FlashStringHelper *s) {
    buffer += reinterpret_cast<const char *>(s);
  }

  void println() { buffer += "\r\n"; }

  void println(const char *s) { print(s); println(); }

  void println(double value, int digits = 2) { print(value, digits); println(); }

  void println(const __FlashStringHelper *s) { print(s); println(); }
};

extern SerialStub Serial;

// --- Timing -----------------------------------------------------------------
// A settable clock so timing-dependent logic (if exercised host-side) is
// deterministic. The real millis() cadence is validated on hardware.
unsigned long millis();
void __host_set_millis(unsigned long value);

// --- Wire (I2C) -------------------------------------------------------------
// A no-op TwoWire stub. read_sensor_raw is a hardware path validated on the
// vehicle; host tests drive the cycle logic through injected function-pointer
// sources instead of this stub, so the default behavior here is simply "no
// bytes available" (i.e. a read error), which is safe and never used by the
// property tests.
class TwoWireStub {
public:
  void begin() {}
  void setClock(unsigned long) {}
  uint8_t requestFrom(uint8_t, uint8_t) { return 0; }
  int available() { return 0; }
  int read() { return -1; }
};

extern TwoWireStub Wire;

#endif  // AIRSPEED26_ARDUINO_STUBS_H
