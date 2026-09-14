// test_main.cpp — Host-side test entry point for the Airspeed26 firmware.
//
// This is the harness established in task 4. It proves the infrastructure:
//   * the pure helpers are compiled UNCHANGED from Airspeed26.ino (via the
//     test/Wire.h + arduino_stubs.h shim; see include_sketch.cpp),
//   * RapidCheck (the prescribed C++ property-based testing library) is linked
//     and runs,
//   * a single-run build+test command works (see Makefile / run_tests.sh).
//
// It now hosts the full property tests for Properties 1-7 (tasks 2.2, 3.2, 3.4,
// 3.6, 3.8, 3.10, 9.4) plus the host-side unit/example tests for serial record
// formatting (6.2), calibration reporting/failure (8.2), and read-error
// airspeed retention (9.5).
//
// The pure helpers and the testable seams (dispatch_status, diag_message,
// collect_zero_offset with CalSampleSource, run_measurement_cycle_with with
// RawSensorSource) are compiled unchanged from the sketch via include_sketch.cpp
// behind arduino_stubs.h and linked into this binary. This TU declares them so
// it need not re-include the sketch. Enum values and struct layouts are kept
// identical to the sketch to avoid ODR/linkage mismatches.
//
// Feature: pitot-airspeed-sensor — host test suite.

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

#include <rapidcheck.h>

// The Arduino host stubs: this gives us the SerialStub type, the `Serial`
// capture buffer, the F()/__FlashStringHelper machinery, and the settable
// host clock — all shared with the sketch translation unit (defined once in
// include_sketch.cpp). Including it here keeps the Serial stub type identical
// across both TUs so the captured-output assertions below read the same buffer
// the sketch writes to.
#include "arduino_stubs.h"

// ── Sketch types & symbols under test ────────────────────────────────────────
// Declared exactly as in Airspeed26.ino (same enum values, same struct layout)
// so linkage matches the definitions compiled from the sketch. No Arduino
// runtime is pulled into this TU beyond the host stubs above.

enum ReadStatus  { READ_OK, READ_ERROR };
enum SensorState { STATE_NORMAL = 0, STATE_RESERVED = 1, STATE_STALE = 2, STATE_FAULT = 3 };

struct Frame {
  SensorState status;
  uint16_t    pressureCount;
  uint16_t    temperatureCount;
};

// Pure helpers.
Frame decode_frame(const uint8_t b[4]);
float pressure_transfer_fcn(uint16_t pressureCount);
float temperature_transfer_fcn(uint16_t temperatureCount);
float air_density(float tempC, bool correctionEnabled);
float airspeed_from_dp(float dP, float rho);
float mean_of(const float *samples, int n);

// Output layer.
void emit_record(float airspeed_ms, float dP_corrected_pa, float tempC);

// Status-dispatch seam (Property 7). CycleAction must match the sketch's enum
// order exactly for linkage/ODR consistency.
enum CycleAction {
  ACTION_COMPUTE,
  ACTION_DIAG_RESERVED,
  ACTION_DIAG_STALE,
  ACTION_DIAG_FAULT
};
CycleAction dispatch_status(SensorState status);
const __FlashStringHelper *diag_message(CycleAction action);

// Calibration seam (task 8.2). Signature must match the sketch's typedef.
typedef ReadStatus (*CalSampleSource)(float *sampleOut);
bool  collect_zero_offset(CalSampleSource source, float *offsetOut);

// Measurement-cycle seam (task 9.5). Signature must match the sketch's typedef.
typedef ReadStatus (*RawSensorSource)(uint8_t out[4]);
void run_measurement_cycle_with(RawSensorSource source, float offset,
                                float *lastAirspeedOut);

// ── Constants mirrored from the sketch ───────────────────────────────────────
// Kept in sync with Airspeed26.ino; used only to shape generators and to state
// expected values in assertions. (These are compile-time #defines in the
// sketch and are not linked symbols, so mirroring the literals is correct.)
static const int    K_P_COUNT_MIN     = 1638;
static const int    K_P_COUNT_MAX     = 14746;
static const double K_P_FULL_SCALE_PA = 6894.76;
static const int    K_T_COUNT_MAX     = 2047;
static const int    K_CAL_SAMPLES     = 100;
static const int    K_CAL_MAX_ATTEMPTS = 1000;
static const double K_RHO_DEFAULT     = 1.225;

// The exact linear pressure map used as the property oracle (Property 2).
static double exact_pressure(double count) {
  return -K_P_FULL_SCALE_PA +
         (count - (double)K_P_COUNT_MIN) * (2.0 * K_P_FULL_SCALE_PA) /
             ((double)K_P_COUNT_MAX - (double)K_P_COUNT_MIN);
}

// The exact temperature map used as the property oracle (Property 3).
static double exact_temp(double count) {
  return (count * 200.0 / (double)K_T_COUNT_MAX) - 50.0;
}

// Inverse of decode_frame: packs (status, pressureCount, temperatureCount) into
// 4 bytes in the MS4525DO wire layout. Lives in the harness (design: Testing
// Strategy). Used by Property 1.
//   byte0 = (status << 6) | (pressureCount >> 8)      // status in bits 7..6, pCount MSBs
//   byte1 = pressureCount & 0xFF                        // pCount LSBs
//   byte2 = (temperatureCount >> 3) & 0xFF              // tCount MSBs
//   byte3 = (temperatureCount & 0x07) << 5              // tCount LSBs in bits 7..5
static void pack_frame(uint8_t status, uint16_t pressureCount,
                       uint16_t temperatureCount, uint8_t out[4]) {
  out[0] = (uint8_t)(((status & 0x03) << 6) | ((pressureCount >> 8) & 0x3F));
  out[1] = (uint8_t)(pressureCount & 0xFF);
  out[2] = (uint8_t)((temperatureCount >> 3) & 0xFF);
  out[3] = (uint8_t)((temperatureCount & 0x07) << 5);
}

// ── Mock sources for the seam-based tests ────────────────────────────────────
// Function-pointer sources can't capture lambdas, so mock state lives in file
// static globals that the plain-function sources below read.

// Calibration mock (task 8.2): serves a fixed sequence of samples. Reads beyond
// the valid budget return READ_ERROR. `g_cal_values` holds the sample stream;
// `g_cal_valid` is how many of the served attempts are valid (READ_OK); every
// call consumes one attempt regardless.
static std::vector<float> g_cal_values;   // one value per VALID sample, in order
static int  g_cal_valid_remaining = 0;    // valid samples still to serve
static int  g_cal_invalid_every   = 0;    // if >0, every Nth attempt is invalid
static int  g_cal_attempt         = 0;    // attempts consumed so far
static int  g_cal_value_idx       = 0;    // next value in g_cal_values

static void cal_mock_reset() {
  g_cal_values.clear();
  g_cal_valid_remaining = 0;
  g_cal_invalid_every   = 0;
  g_cal_attempt         = 0;
  g_cal_value_idx       = 0;
}

// Always-valid source: yields the next value from g_cal_values until exhausted,
// then READ_ERROR.
static ReadStatus cal_source_all_valid(float *sampleOut) {
  g_cal_attempt++;
  if (g_cal_value_idx < (int)g_cal_values.size()) {
    *sampleOut = g_cal_values[g_cal_value_idx++];
    return READ_OK;
  }
  return READ_ERROR;
}

// Never-enough source: serves at most g_cal_valid_remaining valid samples
// (value 0.0), the rest READ_ERROR. Models the calibration-failure path.
static ReadStatus cal_source_insufficient(float *sampleOut) {
  g_cal_attempt++;
  if (g_cal_valid_remaining > 0) {
    g_cal_valid_remaining--;
    *sampleOut = 0.0f;
    return READ_OK;
  }
  return READ_ERROR;
}

// Measurement-cycle mock (task 9.5): a raw source that always fails, to drive
// the read-error retention path.
static ReadStatus raw_source_always_error(uint8_t out[4]) {
  (void)out;
  return READ_ERROR;
}

// A raw source that returns a fixed 4-byte frame once (then keeps returning it).
static uint8_t g_raw_frame[4] = {0, 0, 0, 0};
static ReadStatus raw_source_fixed_frame(uint8_t out[4]) {
  for (int i = 0; i < 4; i++) out[i] = g_raw_frame[i];
  return READ_OK;
}

// ── Small helpers ────────────────────────────────────────────────────────────

// Count occurrences of a delimiter char in a string.
static int count_char(const std::string &s, char c) {
  return (int)std::count(s.begin(), s.end(), c);
}

// Lightweight host-side unit assertion: reports the first failure on a line and
// records it into the aggregate pass/fail flag. Keeps the non-RapidCheck
// example/unit tests (tasks 6.2, 8.2, 9.5) self-diagnosing.
#define CHECK(cond, label)                                                \
  do {                                                                    \
    if (!(cond)) {                                                        \
      std::fprintf(stderr, "UNIT FAIL: %s (line %d)\n", (label), __LINE__); \
      ok = false;                                                         \
    }                                                                     \
  } while (0)

int main() {
  bool ok = true;

  // ══════════════════════════════════════════════════════════════════════════
  // PROPERTY 1 — task 2.2
  // Feature: pitot-airspeed-sensor, Property 1: For any status in {0,1,2,3},
  // pressure count in 0..16383, and temperature count in 0..2047, packing the
  // fields into 4 bytes in the MS4525DO wire layout and applying decode_frame
  // recovers the original status, pressureCount, and temperatureCount, with the
  // counts remaining in range.
  // Validates: Requirements 2.1, 2.2, 2.3
  // ══════════════════════════════════════════════════════════════════════════
  ok &= rc::check(
      "Property 1: frame decode round-trip recovers status/pressure/temperature",
      [](uint8_t statusPick, uint16_t pRaw, uint16_t tRaw) {
        uint8_t  status = (uint8_t)(statusPick & 0x03);           // {0,1,2,3}
        uint16_t pCount = (uint16_t)(pRaw & 0x3FFF);              // 0..16383 (14-bit)
        uint16_t tCount = (uint16_t)(tRaw & 0x07FF);              // 0..2047  (11-bit)

        uint8_t b[4];
        pack_frame(status, pCount, tCount, b);
        Frame f = decode_frame(b);

        RC_ASSERT((uint8_t)f.status == status);
        RC_ASSERT(f.pressureCount == pCount);
        RC_ASSERT(f.temperatureCount == tCount);
        RC_ASSERT(f.pressureCount <= 16383);
        RC_ASSERT(f.temperatureCount <= 2047);
      });

  // ══════════════════════════════════════════════════════════════════════════
  // PROPERTY 2 — task 3.2
  // Feature: pitot-airspeed-sensor, Property 2: pressure_transfer_fcn is within
  // 1 Pa of the exact linear map for counts in 1638..14746, is monotonic
  // non-decreasing for any a<=b in 0..16383, and maps the endpoints 1638 and
  // 14746 to -6894.76 Pa and +6894.76 Pa within tolerance.
  // Validates: Requirements 4.1, 4.2
  // ══════════════════════════════════════════════════════════════════════════
  ok &= rc::check(
      "Property 2: pressure conversion is an accurate, monotonic linear map",
      [](uint16_t r1, uint16_t r2) {
        // Two arbitrary counts in 0..16383, ordered a <= b, for monotonicity.
        uint16_t a = (uint16_t)(r1 % 16384);
        uint16_t b = (uint16_t)(r2 % 16384);
        if (a > b) std::swap(a, b);

        float pa = pressure_transfer_fcn(a);
        float pb = pressure_transfer_fcn(b);
        RC_ASSERT(pa <= pb + 1e-3f);  // monotonic non-decreasing

        // Accuracy within 1 Pa for counts inside the rated span 1638..14746.
        uint16_t span = (uint16_t)(K_P_COUNT_MIN +
                                   (r1 % (K_P_COUNT_MAX - K_P_COUNT_MIN + 1)));
        float got = pressure_transfer_fcn(span);
        double want = exact_pressure((double)span);
        RC_ASSERT(std::fabs((double)got - want) <= 1.0);

        // Endpoints map to +/- full scale within tolerance.
        RC_ASSERT(std::fabs((double)pressure_transfer_fcn(K_P_COUNT_MIN) -
                            (-K_P_FULL_SCALE_PA)) <= 1.0);
        RC_ASSERT(std::fabs((double)pressure_transfer_fcn(K_P_COUNT_MAX) -
                            (K_P_FULL_SCALE_PA)) <= 1.0);
      });

  // ══════════════════════════════════════════════════════════════════════════
  // PROPERTY 3 — task 3.4
  // Feature: pitot-airspeed-sensor, Property 3: for any temperature count in
  // 0..2047, temperature_transfer_fcn(count) equals (count*200/2047)-50 within
  // tolerance (count 0 -> -50C, count 2047 -> +150C) and is monotonic
  // non-decreasing.
  // Validates: Requirements 5.1
  // ══════════════════════════════════════════════════════════════════════════
  ok &= rc::check(
      "Property 3: temperature conversion endpoint mapping and monotonicity",
      [](uint16_t r1, uint16_t r2) {
        uint16_t a = (uint16_t)(r1 % (K_T_COUNT_MAX + 1));  // 0..2047
        uint16_t b = (uint16_t)(r2 % (K_T_COUNT_MAX + 1));
        if (a > b) std::swap(a, b);

        // Accuracy vs. the exact oracle.
        RC_ASSERT(std::fabs((double)temperature_transfer_fcn(a) -
                            exact_temp((double)a)) <= 1e-2);
        RC_ASSERT(std::fabs((double)temperature_transfer_fcn(b) -
                            exact_temp((double)b)) <= 1e-2);

        // Monotonic non-decreasing.
        RC_ASSERT(temperature_transfer_fcn(a) <= temperature_transfer_fcn(b) + 1e-3f);

        // Endpoints.
        RC_ASSERT(std::fabs((double)temperature_transfer_fcn(0) - (-50.0)) <= 1e-2);
        RC_ASSERT(std::fabs((double)temperature_transfer_fcn(K_T_COUNT_MAX) - 150.0) <= 1e-2);
      });

  // ══════════════════════════════════════════════════════════════════════════
  // PROPERTY 4 — task 3.6
  // Feature: pitot-airspeed-sensor, Property 4: air_density selects over all
  // branches — disabled -> 1.225; enabled with T=tempC+273.15 in
  // [233.15,358.15] -> 101325/(287.05*T) within tolerance; enabled and out of
  // range -> 1.225. Covers well below, within, and above the band plus the
  // boundary values -40.0C and 85.0C.
  // Validates: Requirements 7.3, 7.4, 7.5
  // ══════════════════════════════════════════════════════════════════════════
  ok &= rc::check(
      "Property 4: air density selection over all branches",
      [](int32_t milliC, uint8_t pick) {
        // tempC spans well below to well above the band; boundary values are
        // exercised explicitly on selected iterations.
        double tempC;
        switch (pick % 5) {
          case 0: tempC = -40.0; break;   // boundary: T = 233.15 K (in-range, edge)
          case 1: tempC = 85.0;  break;   // boundary: T = 358.15 K (in-range, edge)
          case 2: tempC = -60.0 + (milliC % 20000) / 1000.0; break;  // below band-ish
          case 3: tempC = 90.0 + (milliC % 60000) / 1000.0;  break;  // above band
          default:
            // Anywhere in a broad range spanning the band.
            tempC = -80.0 + (double)((uint32_t)milliC % 200000) / 1000.0;
            break;
        }

        // Disabled: always default.
        RC_ASSERT(std::fabs((double)air_density((float)tempC, false) - K_RHO_DEFAULT) <= 1e-4);

        // Enabled: branch on the Kelvin band. The sketch performs this branch
        // in single precision (T = tempC + 273.15f, compared against
        // 233.15f/358.15f), so the oracle mirrors that float arithmetic exactly
        // to decide the expected branch. Directly at a boundary the float
        // rounding of the band edges is what the sketch itself uses, so this
        // stays consistent. Within a tiny epsilon of either edge we accept
        // either outcome to absorb float rounding of the comparison.
        const float T_MIN_Kf = 233.15f;
        const float T_MAX_Kf = 358.15f;
        float Tf  = (float)tempC + 273.15f;
        float got = air_density((float)tempC, true);
        double defaultDensity = K_RHO_DEFAULT;
        double bandDensity    = 101325.0 / (287.05 * (double)Tf);

        bool inBand   = (Tf >= T_MIN_Kf && Tf <= T_MAX_Kf);
        bool nearEdge = std::fabs((double)Tf - (double)T_MIN_Kf) < 1e-3 ||
                        std::fabs((double)Tf - (double)T_MAX_Kf) < 1e-3;

        if (nearEdge) {
          // Accept either branch's value at the boundary (float-rounding slack).
          bool matchesBand    = std::fabs((double)got - bandDensity) <= 1e-2;
          bool matchesDefault = std::fabs((double)got - defaultDensity) <= 1e-4;
          RC_ASSERT(matchesBand || matchesDefault);
        } else if (inBand) {
          RC_ASSERT(std::fabs((double)got - bandDensity) <= 1e-2);
        } else {
          RC_ASSERT(std::fabs((double)got - defaultDensity) <= 1e-4);
        }
      });

  // ══════════════════════════════════════════════════════════════════════════
  // PROPERTY 5 — task 3.8
  // Feature: pitot-airspeed-sensor, Property 5: for measured dP and offset
  // spanning +/- full scale and rho in (0,2], with dP = measured - offset,
  // airspeed_from_dp(dP,rho) is always >= 0; equals 0 when dP <= 0; equals
  // sqrt(2*dP/rho) within tolerance when dP > 0; and is non-decreasing in dP for
  // fixed rho. Includes dP exactly 0 and just above/below 0.
  // Validates: Requirements 6.3, 7.1, 7.2
  // ══════════════════════════════════════════════════════════════════════════
  ok &= rc::check(
      "Property 5: airspeed is non-negative, zero-clamped, offset-corrected, monotonic in dP",
      [](int32_t mRaw, int32_t oRaw, uint16_t rhoRaw, uint8_t edgePick) {
        // measured & offset across +/- full scale (Pa).
        double measured = -K_P_FULL_SCALE_PA +
                          (double)((uint32_t)mRaw % 200001) / 200000.0 * (2.0 * K_P_FULL_SCALE_PA);
        double offset   = -K_P_FULL_SCALE_PA +
                          (double)((uint32_t)oRaw % 200001) / 200000.0 * (2.0 * K_P_FULL_SCALE_PA);
        double dP = measured - offset;

        // Explicitly exercise dP == 0 and just above/below 0.
        switch (edgePick % 4) {
          case 0: dP = 0.0; break;
          case 1: dP = 1e-6; break;
          case 2: dP = -1e-6; break;
          default: break;  // keep the generated dP
        }

        // rho in (0, 2].
        double rho = 0.0009765625 + (double)rhoRaw / 65535.0 * (2.0 - 0.0009765625);
        RC_ASSERT(rho > 0.0);

        float v = airspeed_from_dp((float)dP, (float)rho);
        RC_ASSERT(v >= 0.0f);                        // always non-negative

        if (dP <= 0.0) {
          RC_ASSERT(v == 0.0f);                      // zero-clamped
        } else {
          double want = std::sqrt(2.0 * dP / rho);
          RC_ASSERT(std::fabs((double)v - want) <= 1e-3 + 1e-3 * want);  // matches Bernoulli
        }

        // Monotonic non-decreasing in dP for fixed rho: a larger corrected dP
        // never yields a smaller airspeed.
        double dPbig = dP + (double)(std::abs(mRaw) % 1000) + 1.0;
        float vbig = airspeed_from_dp((float)dPbig, (float)rho);
        RC_ASSERT(vbig >= v - 1e-4f);
      });

  // ══════════════════════════════════════════════════════════════════════════
  // PROPERTY 6 — task 3.10
  // Feature: pitot-airspeed-sensor, Property 6: for non-empty arrays of length
  // 1..100 with values across the +/- full-scale range, mean_of(samples,n)
  // equals sum/n within tolerance.
  // Validates: Requirements 6.1
  // ══════════════════════════════════════════════════════════════════════════
  ok &= rc::check(
      "Property 6: tare offset equals the arithmetic mean of collected samples",
      [](const std::vector<int32_t> &raw) {
        // Build a non-empty array of length 1..100 with values across +/- FS.
        int n = (int)(raw.size() % 100) + 1;  // 1..100
        std::vector<float> samples((size_t)n);
        double sum = 0.0;
        for (int i = 0; i < n; i++) {
          int32_t r = raw.empty() ? (int32_t)(i * 7 - 3) : raw[(size_t)i % raw.size()];
          double v = -K_P_FULL_SCALE_PA +
                     (double)((uint32_t)r % 200001) / 200000.0 * (2.0 * K_P_FULL_SCALE_PA);
          samples[(size_t)i] = (float)v;
          sum += (double)samples[(size_t)i];
        }
        double want = sum / (double)n;
        float got = mean_of(samples.data(), n);
        // Tolerance scales with magnitude to absorb float accumulation error.
        RC_ASSERT(std::fabs((double)got - want) <= 1e-2 + 1e-4 * std::fabs(want));
      });

  // ══════════════════════════════════════════════════════════════════════════
  // PROPERTY 7 — task 9.4
  // Feature: pitot-airspeed-sensor, Property 7: status dispatch reports airspeed
  // only for normal readings — for frames covering all four status values, an
  // airspeed data record is produced iff status is STATE_NORMAL; reserved/stale/
  // fault produce a diagnostic message identifying the condition with no airspeed
  // value.
  // Validates: Requirements 3.1, 3.2, 3.3, 3.4
  // ══════════════════════════════════════════════════════════════════════════
  ok &= rc::check(
      "Property 7: status dispatch reports airspeed only for normal readings",
      [](uint8_t statusPick) {
        SensorState status = (SensorState)(statusPick & 0x03);
        CycleAction action = dispatch_status(status);

        if (status == STATE_NORMAL) {
          // Only NORMAL yields the compute (data-record) action.
          RC_ASSERT(action == ACTION_COMPUTE);
        } else {
          // Every other status yields a distinct diagnostic (no compute).
          RC_ASSERT(action != ACTION_COMPUTE);

          // The diagnostic message identifies the specific condition and carries
          // no airspeed value. diag_message returns a host C string via the F()
          // stub, so we can inspect it directly.
          std::string msg =
              reinterpret_cast<const char *>(diag_message(action));
          RC_ASSERT(!msg.empty());

          if (status == STATE_RESERVED) {
            RC_ASSERT(action == ACTION_DIAG_RESERVED);
            RC_ASSERT(msg.find("reserved") != std::string::npos);
          } else if (status == STATE_STALE) {
            RC_ASSERT(action == ACTION_DIAG_STALE);
            RC_ASSERT(msg.find("stale") != std::string::npos);
          } else {  // STATE_FAULT
            RC_ASSERT(action == ACTION_DIAG_FAULT);
            RC_ASSERT(msg.find("fault") != std::string::npos);
          }
          // No airspeed value: the diagnostic contains no data-record delimiter.
          RC_ASSERT(msg.find(',') == std::string::npos);
        }
      });

  // ══════════════════════════════════════════════════════════════════════════
  // UNIT/EXAMPLE TESTS — task 6.2: serial record formatting (emit_record)
  // Validates: Requirements 8.1, 8.2, 8.3
  // ══════════════════════════════════════════════════════════════════════════
  {
    struct Row { float airspeed; float dP; float temp; };
    const Row rows[] = {
        {12.47f, 152.30f, 24.06f},
        {0.00f, -3.14f, -50.00f},
        {123.456f, 6894.76f, 150.00f},
    };
    for (const Row &r : rows) {
      Serial.clear();
      emit_record(r.airspeed, r.dP, r.temp);
      const std::string out = Serial.buffer;

      // Trailing line ending (Req 8.1): record terminates with a line ending.
      CHECK(out.size() >= 2 && out.compare(out.size() - 2, 2, "\r\n") == 0,
            "6.2 trailing line ending");

      // Exactly one line: the only line ending is the trailing one.
      CHECK(out.find('\n') == out.size() - 1, "6.2 exactly one line");

      // The record body (without the trailing line ending).
      std::string body = out.substr(0, out.size() - 2);

      // Single consistent delimiter, exactly one per field boundary => 3 fields
      // means exactly 2 delimiters (Req 8.1, 8.3).
      CHECK(count_char(body, ',') == 2, "6.2 field count (2 delimiters)");

      // Field count and fixed order: airspeed, corrected dP, temp (Req 8.3).
      size_t c1 = body.find(',');
      size_t c2 = body.find(',', c1 + 1);
      CHECK(c1 != std::string::npos && c2 != std::string::npos,
            "6.2 two delimiters present");
      std::string f0 = body.substr(0, c1);
      std::string f1 = body.substr(c1 + 1, c2 - c1 - 1);
      std::string f2 = body.substr(c2 + 1);

      // Airspeed has exactly 2 fractional digits (Req 8.2).
      size_t dot = f0.find('.');
      CHECK(dot != std::string::npos && (f0.size() - dot - 1) == 2,
            "6.2 airspeed 2 fractional digits");

      // Airspeed field matches the expected 2dp rendering (order/position check).
      char expA[64];
      std::snprintf(expA, sizeof(expA), "%.2f", r.airspeed);
      CHECK(f0 == expA, "6.2 airspeed field first / correct value");

      // dP and temp fields are present and non-empty decimal values.
      CHECK(!f1.empty() && !f2.empty(), "6.2 dP/temp fields present");
      CHECK(f2.find('.') != std::string::npos, "6.2 temp rendered as decimal");
    }
    CHECK(count_char("a,b,c", ',') == 2, "6.2 count_char helper sanity");
  }

  // ══════════════════════════════════════════════════════════════════════════
  // UNIT TESTS — task 8.2: calibration reporting and failure (collect_zero_offset)
  // Validates: Requirements 6.4, 6.5
  // ══════════════════════════════════════════════════════════════════════════
  {
    // Success: exactly CAL_SAMPLES valid samples available -> mean returned,
    // true reported.
    cal_mock_reset();
    double sum = 0.0;
    for (int i = 0; i < K_CAL_SAMPLES; i++) {
      float v = (float)(i - 50) * 3.5f;  // spread across +/-, arbitrary
      g_cal_values.push_back(v);
      sum += v;
    }
    float offset = 123.0f;
    bool okCal = collect_zero_offset(cal_source_all_valid, &offset);
    CHECK(okCal, "8.2 success path returns true");     // success (Req 6.4 precondition)
    double wantMean = sum / (double)K_CAL_SAMPLES;
    CHECK(std::fabs((double)offset - wantMean) <= 1e-2 + 1e-4 * std::fabs(wantMean),
          "8.2 success yields mean of CAL_SAMPLES");
    // Exactly CAL_SAMPLES valid samples consumed (all valid -> CAL_SAMPLES attempts).
    CHECK(g_cal_attempt == K_CAL_SAMPLES, "8.2 collected exactly CAL_SAMPLES");

    // Failure: fewer than CAL_SAMPLES valid samples within CAL_MAX_ATTEMPTS ->
    // offset 0, false reported (Req 6.5).
    cal_mock_reset();
    g_cal_valid_remaining = K_CAL_SAMPLES - 1;  // one short of the requirement
    float offset2 = 999.0f;
    bool okCal2 = collect_zero_offset(cal_source_insufficient, &offset2);
    CHECK(!okCal2, "8.2 failure path returns false");          // failure path
    CHECK(offset2 == 0.0f, "8.2 failure sets offset to 0");    // offset set to 0 (Req 6.5)
    CHECK(g_cal_attempt == K_CAL_MAX_ATTEMPTS,
          "8.2 failure exhausts attempt cap");                 // exhausted the attempt cap

    // Reporting message via calibrate_zero_offset is exercised on hardware
    // (sensor-backed source). Here we assert the collection-core contract that
    // the reporting is built on: success yields the mean (reported as the
    // offset over serial in the sketch, Req 6.4) and failure yields 0.0
    // (reported as a calibration-failure message, Req 6.5).
  }

  // ══════════════════════════════════════════════════════════════════════════
  // UNIT TEST — task 9.5: read-error airspeed retention (run_measurement_cycle_with)
  // Validates: Requirements 1.4
  // ══════════════════════════════════════════════════════════════════════════
  {
    // On a read error the cycle emits an error message and leaves lastAirspeed
    // unchanged (retains the last valid value).
    Serial.clear();
    float lastAirspeed = 42.5f;   // a prior valid airspeed to be retained
    run_measurement_cycle_with(raw_source_always_error, 0.0f, &lastAirspeed);

    std::string errOut = Serial.buffer;
    CHECK(lastAirspeed == 42.5f, "9.5 lastAirspeed retained on read error");  // Req 1.4
    CHECK(!errOut.empty(), "9.5 error message emitted");
    // The emitted output is a human-readable read-failure diagnostic, not a
    // numeric data record. Identify it by its diagnostic wording (the read
    // failure message) rather than by delimiter count, since the diagnostic
    // sentence itself contains a comma.
    CHECK(errOut.find("read failure") != std::string::npos ||
              errOut.find("retaining") != std::string::npos,
          "9.5 read error emits the read-failure diagnostic message");

    // Sanity contrast: a valid NORMAL frame DOES update lastAirspeed and emits a
    // data record — confirms the retention above is specific to the error path.
    Serial.clear();
    // Pack a normal frame with pressure count at +full scale (large dP -> v>0).
    pack_frame((uint8_t)STATE_NORMAL, (uint16_t)K_P_COUNT_MAX, (uint16_t)1024, g_raw_frame);
    float lastAirspeed2 = 7.0f;
    run_measurement_cycle_with(raw_source_fixed_frame, 0.0f, &lastAirspeed2);
    CHECK(lastAirspeed2 != 7.0f, "9.5 lastAirspeed updated on valid reading");
    CHECK(count_char(Serial.buffer, ',') == 2, "9.5 valid reading emits data record");
  }

  if (ok) {
    std::fprintf(stderr, "ALL UNIT TESTS (6.2, 8.2, 9.5) PASSED\n");
  }

  return ok ? 0 : 1;
}
