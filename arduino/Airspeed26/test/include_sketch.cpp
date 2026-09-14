// include_sketch.cpp — Lifts the Airspeed26 sketch into the host test build.
//
// Rather than fork or duplicate the pure helpers, we compile the sketch itself
// with the Arduino runtime replaced by host stubs. This keeps Airspeed26.ino
// the single source of truth for decode_frame, pressure_transfer_fcn,
// temperature_transfer_fcn, air_density, airspeed_from_dp, mean_of, and the
// testable seams (dispatch_status, diag_message, collect_zero_offset,
// run_measurement_cycle_with). The design's Testing Strategy explicitly allows
// "including the .ino behind stubs" for exactly this purpose.
//
// The <Wire.h> the sketch includes is shadowed by test/Wire.h (this dir is on
// the include path ahead of anything else), which pulls in arduino_stubs.h.
//
// Feature: pitot-airspeed-sensor — host test harness (task 4).

#include "arduino_stubs.h"

// Provide the single definitions for the extern stub globals + host clock.
SerialStub  Serial;
TwoWireStub Wire;

static unsigned long g_millis = 0;
unsigned long millis() { return g_millis; }
void __host_set_millis(unsigned long value) { g_millis = value; }

// setup()/loop() are Arduino entry points with no host main() caller, but the
// sketch defines them; that's fine — they're just ordinary functions here and
// go unreferenced by the tests. Pull the whole sketch in as-is.
#include "../Airspeed26.ino"
