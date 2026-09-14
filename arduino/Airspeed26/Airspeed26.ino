// Airspeed Board 2026 v0.0.1 - Rutgers Formula Racing Electronics
// Reads a Matek ASPD-4525 pitot-tube airspeed sensor (TE MS4525DO-DS5AI001DP,
// +/-1 PSI differential, I2C address 0x28) and reports indicated airspeed,
// offset-corrected differential pressure, and die temperature over serial.
// v0.0.1: Initial scaffold - constants, enums, data structures, setup()/loop() stubs

#include <Wire.h>

// ── Constants ───────────────────────────────────────────────────────────────

#define MS4525_ADDR                0x28      // 7-bit I2C address
#define I2C_CLOCK_HZ               100000    // Bus clock (Req 1.1)
#define SERIAL_BAUD                9600      // UART baud (Req 1.2)
#define MEASUREMENT_INTERVAL_MS    15        // Cycle cadence, within 10-20 ms (Req 9.1)
#define READ_TIMEOUT_MS            10        // Soft read time budget (Req 1.4)

#define P_COUNT_MIN                1638      // Pressure count at -full scale
#define P_COUNT_MAX                14746     // Pressure count at +full scale
#define P_FULL_SCALE_PA            6894.76f  // +/-1 PSI in Pa
#define T_COUNT_MAX                2047      // Temperature count full scale

#define CAL_SAMPLES                100       // Required valid tare samples (Req 6.1)
#define CAL_MAX_ATTEMPTS           1000      // Max reads during tare (Req 6.2)

#define RHO_DEFAULT                1.225f    // Default air density (Req 7.3)
#define TEMP_CORRECTION_ENABLED    false     // Compile-time density-correction flag (Req 7)
#define STD_PRESSURE_PA            101325.0f // P for density correction (Req 7.4)
#define GAS_CONSTANT_R             287.05f   // R for density correction (Req 7.4)
#define T_MIN_K                    233.15f   // Lower Kelvin bound for correction (Req 7.4/7.5)
#define T_MAX_K                    358.15f   // Upper Kelvin bound for correction (Req 7.4/7.5)

#define FIELD_DELIM                ','       // Single consistent record delimiter (Req 8.1)

// ── Enumerations ──────────────────────────────────────────────────────────────

enum ReadStatus  { READ_OK, READ_ERROR };
enum SensorState { STATE_NORMAL = 0, STATE_RESERVED = 1, STATE_STALE = 2, STATE_FAULT = 3 };

// ── Data Structures ───────────────────────────────────────────────────────────

struct Frame {
  SensorState status;            // two MSBs of byte 0        (Req 2.1)
  uint16_t    pressureCount;     // 14-bit, range 0..16383    (Req 2.2)
  uint16_t    temperatureCount;  // 11-bit, range 0..2047     (Req 2.3)
};

// ── Module-Level State ────────────────────────────────────────────────────────

// Derived / in-memory state (design: Data Models).
float         zeroOffset   = 0.0f;  // Tare offset (Pa), computed at startup; subtracted from every dP (Req 6.3)
float         lastAirspeed = 0.0f;  // Last valid indicated airspeed (m/s), retained across read-error cycles (Req 1.4)
unsigned long lastCycleTime = 0;    // millis() timestamp gate for the measurement interval (Req 9.2)

// ── Forward Declarations ──────────────────────────────────────────────────────

void setup();
void loop();

Frame      decode_frame(const uint8_t b[4]);
float      pressure_transfer_fcn(uint16_t pressureCount);
float      temperature_transfer_fcn(uint16_t temperatureCount);
float      air_density(float tempC, bool correctionEnabled);
float      airspeed_from_dp(float dP, float rho);
float      mean_of(const float *samples, int n);
void       emit_record(float airspeed_ms, float dP_corrected_pa, float tempC);
ReadStatus read_sensor_raw(uint8_t out[4]);
float      calibrate_zero_offset();
void       run_measurement_cycle();  // one read->decode->dispatch->emit cycle (implemented in task 9.3)

// ── Decode Layer (pure) ───────────────────────────────────────────────────────

// Reconstructs the frame from 4 raw bytes in the MS4525DO wire layout.
//   status  = (b0 >> 6) & 0x03
//   pCount  = ((b0 & 0x3F) << 8) | b1
//   tCount  = (b2 << 3) | (b3 >> 5)
// Pure: depends only on its argument (no globals, no peripheral calls). (Req 2.1, 2.2, 2.3)
Frame decode_frame(const uint8_t b[4]) {
  Frame f;
  f.status           = (SensorState)((b[0] >> 6) & 0x03);
  f.pressureCount    = (uint16_t)(((uint16_t)(b[0] & 0x3F) << 8) | b[1]);
  f.temperatureCount = (uint16_t)(((uint16_t)b[2] << 3) | (b[3] >> 5));
  return f;
}

// ── Conversion Layer (pure helpers) ───────────────────────────────────────────

// Linear map: 1638 -> -6894.76 Pa, 14746 -> +6894.76 Pa.  (Req 4.1, 4.2, 4.3)
// dP = -FS + (count - P_COUNT_MIN) * (2*FS) / (P_COUNT_MAX - P_COUNT_MIN)
float pressure_transfer_fcn(uint16_t pressureCount) {
  return -P_FULL_SCALE_PA +
         ((float)pressureCount - (float)P_COUNT_MIN) *
             (2.0f * P_FULL_SCALE_PA) /
             ((float)P_COUNT_MAX - (float)P_COUNT_MIN);
}

// tempC = (count * 200 / 2047) - 50, mapping count 0 -> -50 C, 2047 -> +150 C. (Req 5.1, 5.2)
float temperature_transfer_fcn(uint16_t temperatureCount) {
  return ((float)temperatureCount * 200.0f / (float)T_COUNT_MAX) - 50.0f;
}

// Returns rho in kg/m^3. If correction disabled -> RHO_DEFAULT.
// If enabled and T = tempC + 273.15 within [T_MIN_K, T_MAX_K] -> P/(R*T),
// else RHO_DEFAULT.  (Req 7.3, 7.4, 7.5)
float air_density(float tempC, bool correctionEnabled) {
  if (!correctionEnabled) {
    return RHO_DEFAULT;
  }
  float T = tempC + 273.15f;
  if (T >= T_MIN_K && T <= T_MAX_K) {
    return STD_PRESSURE_PA / (GAS_CONSTANT_R * T);
  }
  return RHO_DEFAULT;
}

// v = sqrt(2*dP/rho) for dP > 0, else 0.  (Req 7.1, 7.2)
float airspeed_from_dp(float dP, float rho) {
  if (dP > 0.0f) {
    return sqrt(2.0f * dP / rho);
  }
  return 0.0f;
}

// Arithmetic mean of n samples. Used by tare and host tests.  (Req 6.1)
float mean_of(const float *samples, int n) {
  float sum = 0.0f;
  for (int i = 0; i < n; i++) {
    sum += samples[i];
  }
  return sum / (float)n;
}

// ── Output Layer ──────────────────────────────────────────────────────────────

// Writes one delimited record in fixed field order (airspeed, corrected dP, temp):
//   <airspeed:2dp><DELIM><corrected_dP_pa><DELIM><temp_c>\n
// Single consistent delimiter; airspeed printed with 2 fractional digits;
// terminated by a line ending.  (Req 8.1, 8.2, 8.3)
void emit_record(float airspeed_ms, float dP_corrected_pa, float tempC) {
  Serial.print(airspeed_ms, 2);   // airspeed, 2 fractional digits (Req 8.2)
  Serial.print(FIELD_DELIM);
  Serial.print(dP_corrected_pa);  // offset-corrected differential pressure (Pa)
  Serial.print(FIELD_DELIM);
  Serial.println(tempC);          // die temperature (C), terminated by line ending
}

// ── Calibration (tare) ────────────────────────────────────────────────────────

// A calibration sample source. Returns READ_OK and writes the offset-corrected
// differential-pressure sample (Pa) into *sampleOut only when the underlying
// reading is valid (read succeeded AND status == STATE_NORMAL); otherwise
// returns READ_ERROR and *sampleOut is left unspecified.
//
// This indirection lets the pure collection loop below be exercised host-side
// with a mock source (no Wire/Serial), while the sketch supplies a source
// backed by the real sensor. (design: Testability strategy; Req 6.1, 6.2)
typedef ReadStatus (*CalSampleSource)(float *sampleOut);

// Pure collection core: pulls samples from `source`, making at most
// CAL_MAX_ATTEMPTS attempts to gather CAL_SAMPLES valid samples, then stores
// their arithmetic mean in *offsetOut. Returns true iff exactly CAL_SAMPLES
// valid samples were collected within the attempt cap; on failure *offsetOut
// is set to 0.0. Depends only on its argument (no globals besides compile-time
// constants, no peripheral calls) so it is host-testable. (Req 6.1, 6.2, 6.5)
bool collect_zero_offset(CalSampleSource source, float *offsetOut) {
  float samples[CAL_SAMPLES];
  int collected = 0;

  for (int attempt = 0; attempt < CAL_MAX_ATTEMPTS && collected < CAL_SAMPLES; attempt++) {
    float sample;
    if (source(&sample) == READ_OK) {
      samples[collected] = sample;
      collected++;
    }
  }

  if (collected < CAL_SAMPLES) {
    *offsetOut = 0.0f;   // insufficient valid samples (Req 6.5)
    return false;
  }

  *offsetOut = mean_of(samples, CAL_SAMPLES);  // arithmetic mean (Req 6.1)
  return true;
}

// Hardware-backed calibration sample source: performs one sensor read, decodes
// it, and yields a valid, pressure-converted sample only for a good frame
// (READ_OK and STATE_NORMAL). (Req 6.1)
static ReadStatus sensor_cal_sample(float *sampleOut) {
  uint8_t raw[4];
  if (read_sensor_raw(raw) != READ_OK) {
    return READ_ERROR;
  }
  Frame f = decode_frame(raw);
  if (f.status != STATE_NORMAL) {
    return READ_ERROR;
  }
  *sampleOut = pressure_transfer_fcn(f.pressureCount);
  return READ_OK;
}

// Runs once in setup(). Collects CAL_SAMPLES valid samples within
// CAL_MAX_ATTEMPTS reads and returns their mean, reporting the computed offset
// (Pa) over serial on success (Req 6.4). On too few valid samples, reports a
// calibration-failure message and returns 0.0 (Req 6.5). (Req 6.1, 6.2)
float calibrate_zero_offset() {
  float offset;
  if (collect_zero_offset(sensor_cal_sample, &offset)) {
    Serial.print(F("Calibration complete, zero offset (Pa): "));  // Req 6.4
    Serial.println(offset);
    return offset;
  }
  Serial.println(F("Calibration failed: insufficient valid samples; offset set to 0"));  // Req 6.5
  return 0.0f;
}

// ── I/O Layer (hardware-bound) ────────────────────────────────────────────────

// Performs the I2C transaction. Fills out[4] on success and returns READ_OK.
// Returns READ_ERROR if the address is not ACKed, fewer than 4 bytes are
// received, or the transaction exceeds READ_TIMEOUT_MS. The time budget is
// enforced with a millis() soft budget; no blocking wait exceeds 1 ms. (Req 1.3, 1.4, 2.4, 3.5)
ReadStatus read_sensor_raw(uint8_t out[4]) {
  unsigned long start = millis();

  // Request 4 bytes from the sensor. requestFrom() returns the number of bytes
  // actually made available; 0 indicates an address NACK / no response.
  uint8_t received = Wire.requestFrom((uint8_t)MS4525_ADDR, (uint8_t)4);
  if (received < 4) {
    return READ_ERROR;  // address NACK or short read (Req 1.4, 2.4, 3.5)
  }

  for (int i = 0; i < 4; i++) {
    // Soft time budget: bail out if the transaction outruns READ_TIMEOUT_MS.
    if ((millis() - start) > READ_TIMEOUT_MS) {
      return READ_ERROR;  // exceeded 10 ms budget (Req 1.4)
    }
    if (Wire.available() < 1) {
      return READ_ERROR;  // fewer than 4 bytes actually readable (Req 1.4, 2.4)
    }
    out[i] = (uint8_t)Wire.read();
  }

  return READ_OK;
}

// ── Setup ─────────────────────────────────────────────────────────────────────

// Runs once at boot. Brings up the I2C bus as a controller at 100 kHz and the
// serial port at 9600 baud (8N1), then tares the sensor in still air and stores
// the resulting zero offset. Also seeds the retained-airspeed and timing-gate
// state so the measurement loop starts from a defined baseline. (Req 1.1, 1.2, 6.3)
void setup() {
  // I2C as controller @ 100 kHz (Req 1.1)
  Wire.begin();
  Wire.setClock(I2C_CLOCK_HZ);

  // Serial @ 9600 8N1 (Req 1.2). 8N1 is the Arduino default framing for begin(baud).
  Serial.begin(SERIAL_BAUD);

  // Startup zero-offset calibration (tare) in still air; store the result (Req 6.3).
  zeroOffset = calibrate_zero_offset();

  // Seed retained airspeed and the millis()-based timing gate.
  lastAirspeed  = 0.0f;
  lastCycleTime = millis();
}

// ── Measurement Cycle ─────────────────────────────────────────────────────────

// The action a measurement cycle should take for a decoded frame, based solely
// on its status field. This is the pure status-dispatch decision extracted so
// it can be exercised host-side (Property 7 / task 9.4) without Wire/Serial.
enum CycleAction {
  ACTION_COMPUTE,       // status NORMAL: convert + emit an airspeed data record
  ACTION_DIAG_RESERVED, // status RESERVED (01): emit reserved-status diagnostic
  ACTION_DIAG_STALE,    // status STALE (10):    emit stale-data diagnostic
  ACTION_DIAG_FAULT     // status FAULT (11):    emit fault diagnostic
};

// Pure status dispatch: maps a sensor status to the cycle action. An airspeed
// data record is produced iff the status is STATE_NORMAL; every other status
// yields a distinct diagnostic action identifying the condition and produces no
// airspeed value. Depends only on its argument (no globals, no peripheral
// calls) so it is host-testable. (Req 3.1, 3.2, 3.3, 3.4)
CycleAction dispatch_status(SensorState status) {
  switch (status) {
    case STATE_NORMAL:   return ACTION_COMPUTE;
    case STATE_RESERVED: return ACTION_DIAG_RESERVED;
    case STATE_STALE:    return ACTION_DIAG_STALE;
    case STATE_FAULT:    return ACTION_DIAG_FAULT;
  }
  // Unreachable: SensorState covers all 2-bit values. Treat unknown as fault.
  return ACTION_DIAG_FAULT;
}

// The human-readable diagnostic string for a non-computing cycle action.
// Kept alongside dispatch_status so the message-per-condition mapping is
// exercised together with the dispatch decision host-side. (Req 3.2, 3.3, 3.4)
const __FlashStringHelper *diag_message(CycleAction action) {
  switch (action) {
    case ACTION_DIAG_RESERVED: return F("Sensor status: reserved (01); skipping cycle");
    case ACTION_DIAG_STALE:    return F("Sensor status: stale data (10); skipping cycle");
    case ACTION_DIAG_FAULT:    return F("Sensor status: fault (11); skipping cycle");
    default:                   return F("Sensor status: unknown; skipping cycle");
  }
}

// A raw-frame sensor source. Returns READ_OK and writes 4 raw bytes into out[4]
// on a successful read; otherwise returns READ_ERROR and out[] is unspecified.
//
// This indirection lets the cycle core below be driven host-side with a mock
// source (no Wire/Serial) to verify read-error retention of lastAirspeed
// (task 9.5), while the sketch supplies a source backed by the real sensor.
// (design: Testability strategy; Req 1.4)
typedef ReadStatus (*RawSensorSource)(uint8_t out[4]);

// Cycle core: performs one read -> decode -> status dispatch -> convert -> emit
// cycle using the injected raw sensor `source`, the tare `offset` (Pa), and the
// retained-airspeed cell `*lastAirspeedOut`.
//
//  - On READ_ERROR: emit a read-error / sensor-read-failure message, skip the
//    computation, and leave *lastAirspeedOut unchanged (retain last valid
//    airspeed). (Req 1.4, 2.4, 3.5)
//  - On READ_OK + STATE_NORMAL: convert pressure, subtract `offset`, compute
//    air density (TEMP_CORRECTION_ENABLED, temperature_transfer_fcn) and
//    airspeed_from_dp, update *lastAirspeedOut, and emit the data record.
//    (Req 3.1, 6.3, 7.1, 7.2, 7.3, 8.1, 8.2, 8.3)
//  - On READ_OK + reserved/stale/fault: emit the matching diagnostic message,
//    skip the computation, and emit no airspeed value. (Req 3.2, 3.3, 3.4)
//
// Reads/writes only its arguments (aside from compile-time constants), so the
// full cycle logic can be driven host-side with a mock source. The Serial/Wire
// dependency is confined to `source`, emit_record, and the diagnostic prints.
void run_measurement_cycle_with(RawSensorSource source, float offset,
                                float *lastAirspeedOut) {
  uint8_t raw[4];

  // Read failure: report, skip computation, retain last valid airspeed (Req 1.4, 2.4, 3.5).
  if (source(raw) != READ_OK) {
    Serial.println(F("Sensor read failure; skipping cycle, retaining last airspeed"));
    return;  // *lastAirspeedOut deliberately untouched
  }

  Frame f = decode_frame(raw);
  CycleAction action = dispatch_status(f.status);

  // Non-normal status: emit the condition-specific diagnostic, no airspeed (Req 3.2-3.4).
  if (action != ACTION_COMPUTE) {
    Serial.println(diag_message(action));
    return;  // *lastAirspeedOut deliberately untouched
  }

  // Normal reading: convert, offset-correct, compute density + airspeed, emit.
  float dP_measured  = pressure_transfer_fcn(f.pressureCount);   // Pa (Req 4.1)
  float dP_corrected = dP_measured - offset;                     // subtract tare (Req 6.3)
  float tempC        = temperature_transfer_fcn(f.temperatureCount); // C (Req 5.1)
  float rho          = air_density(tempC, TEMP_CORRECTION_ENABLED);  // kg/m^3 (Req 7.3-7.5)
  float airspeed     = airspeed_from_dp(dP_corrected, rho);      // m/s (Req 7.1, 7.2)

  *lastAirspeedOut = airspeed;                    // update retained airspeed
  emit_record(airspeed, dP_corrected, tempC);     // one data record (Req 8.1-8.3)
}

// Hardware-backed raw sensor source: one I2C read of 4 bytes. (Req 1.3)
static ReadStatus sensor_raw_source(uint8_t out[4]) {
  return read_sensor_raw(out);
}

// Runs exactly one measurement cycle against the real sensor. Dispatched by the
// non-blocking timing gate in loop() once per MEASUREMENT_INTERVAL_MS. Thin
// wrapper that binds the hardware source, the module-level tare offset, and the
// retained-airspeed cell to the host-testable cycle core above.
void run_measurement_cycle() {
  run_measurement_cycle_with(sensor_raw_source, zeroOffset, &lastAirspeed);
}

// ── Loop ──────────────────────────────────────────────────────────────────────

// Non-blocking, millis()-gated measurement loop. Triggers a new measurement
// cycle once the elapsed time since the previous cycle reaches or exceeds
// MEASUREMENT_INTERVAL_MS, and runs exactly one cycle per trigger (Req 9.2).
// The loop performs no blocking delay > 1 ms; between intervals it simply
// falls through (Req 9.3).
//
// Overrun handling (Req 9.4): on trigger the gate advances lastCycleTime rather
// than resetting it to `now`, which preserves cadence when a cycle finishes on
// time. If the loop has fallen more than one full interval behind (e.g. a cycle
// overran the interval), lastCycleTime is snapped forward to `now` so that
// missed intervals are dropped instead of accumulating as queued back-to-back
// cycles. Using unsigned `millis()` subtraction keeps the gate correct across
// the millis() rollover.
void loop() {
  unsigned long now = millis();

  if ((now - lastCycleTime) >= MEASUREMENT_INTERVAL_MS) {
    // Advance the gate before running the cycle so timing is measured from the
    // scheduled tick, not from when the cycle happens to finish.
    lastCycleTime += MEASUREMENT_INTERVAL_MS;

    // If we're still an interval or more behind, an overrun (or long pause)
    // occurred; drop the backlog by snapping to now so cycles do not queue up.
    if ((now - lastCycleTime) >= MEASUREMENT_INTERVAL_MS) {
      lastCycleTime = now;
    }

    // Exactly one measurement cycle per trigger. The cycle body (read, decode,
    // status dispatch, compute, emit) is implemented in task 9.3.
    run_measurement_cycle();
  }
}
