// Wire.h — host-side shadow of the Arduino Wire library.
//
// Airspeed26.ino does `#include <Wire.h>`. In the host test build this test/
// directory is placed on the compiler's include search path (via -I) so this
// file is found instead of a real Arduino Wire.h. It simply forwards to the
// shared Arduino stubs, which declare the Wire/Serial/millis/F() stand-ins.
//
// Feature: pitot-airspeed-sensor — host test harness (task 4).

#ifndef AIRSPEED26_HOST_WIRE_H
#define AIRSPEED26_HOST_WIRE_H

#include "arduino_stubs.h"

#endif  // AIRSPEED26_HOST_WIRE_H
