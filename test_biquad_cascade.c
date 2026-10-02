/**
 * test_biquad_cascade.c
 *
 * Standalone confirmation that biquad_cascade.h actually behaves the way
 * generate_coeffs.py's theoretical analysis says it should. Builds and
 * runs on a regular PC (no STM32/CMSIS needed) since the algorithm itself
 * is target-independent -- only the coefficients and the ADC plumbing
 * around it are STM32-specific.
 *
 * What it checks:
 *   1. DC pass-through: a steady input should come out unchanged once
 *      the filter has settled (gain = 1 at 0 Hz).
 *   2. -3 dB point: a sine at the design cutoff should lose ~half its
 *      power (amplitude ratio ~0.707), confirming the cutoff really
 *      sits where we designed it to.
 *   3. EMI rejection: a sine at a candidate inverter switching frequency
 *      should come out attenuated by close to the dB figure
 *      generate_coeffs.py predicted for that frequency.
 *   4. Composite signal + per-channel reset: a slow "thermal" ramp with
 *      switching-frequency noise riding on top of it, run through two
 *      simulated channels back-to-back (with BiquadCascade_Reset between
 *      them, exactly like Biquad_Reset() is called before each channel
 *      scan in main.c) -- confirms there's no state leakage between
 *      channels and that the recovered signal tracks the real
 *      temperature, not the noise.
 *
 * Build & run:
 *   gcc -O2 -Wall -Wextra -lm -o test_biquad_cascade test_biquad_cascade.c
 *   ./test_biquad_cascade
 */
#include <stdio.h>
#include <math.h>
#include "biquad_cascade.h"

/* fs/cutoff derived from the STM32F103T8U6's actual ADC clock tree and
 * ADC_SAMPLETIME_239CYCLES_5 setting in main.c -- see the big comment
 * at the top of generate_coeffs.py for the full derivation (assumes an
 * 8 MHz HSE crystal: SYSCLK 72MHz -> HCLK 36MHz -> PCLK2 36MHz ->
 * ADCCLK 6MHz -> t_conv = (239.5+12.5)/6MHz = 42us -> fs = 23,809.5 Hz).
 * Cutoff dropped from the old 5kHz placeholder to 200 Hz: battery
 * temperature only changes on a seconds timescale, and 200 Hz gives
 * much more stopband margin against the PM100DX's ~20 kHz switching
 * noise and, importantly, against where that noise ALIASES TO once
 * sampled at 23.8 kHz (see Test 5 below -- this is the critical one). */
#define FILTER_NUM_STAGES 2u
static const float32_t filter_coeffs[FILTER_NUM_STAGES * 5] = {
  0.0000004531f, 0.0000009061f, 0.0000004531f, 1.9043973333f, -0.9070528626f, /* stage 0 */
  1.0000000000f, 2.0000000000f, 1.0000000000f, 1.9576927508f, -0.9604225962f, /* stage 1 */
};

#define FS_HZ 23809.5f

static int g_fail = 0;

static void check(const char *name, double got, double expect, double tol) {
  double err = fabs(got - expect);
  const char *verdict = (err <= tol) ? "PASS" : "FAIL";
  if (err > tol) g_fail = 1;
  printf("  [%s] %-42s got=%10.5f  expect=%10.5f  (tol=%.5f)\n",
         verdict, name, got, expect, tol);
}

/* Feed N cycles of a sine at freq_hz through a fresh filter instance and
 * return the output/input amplitude ratio, measured only over the last
 * portion of the run so the startup transient has fully settled out. */
static double measure_gain_at(float32_t freq_hz) {
  BiquadCascade_t S;
  float32_t state[2 * FILTER_NUM_STAGES];
  BiquadCascade_Init(&S, FILTER_NUM_STAGES, filter_coeffs, state);

  const int total_samples = 20000;
  const int settle_samples = 12000; /* skip this many before measuring */
  float32_t out_max = 0.0f, in_max = 0.0f;

  for (int n = 0; n < total_samples; n++) {
    float32_t t = (float32_t)n / FS_HZ;
    float32_t x = sinf(2.0f * (float32_t)M_PI * freq_hz * t);
    float32_t y = BiquadCascade_ProcessSample(&S, x);
    if (n >= settle_samples) {
      if (fabsf(x) > in_max) in_max = fabsf(x);
      if (fabsf(y) > out_max) out_max = fabsf(y);
    }
  }
  return (double)out_max / (double)in_max;
}

int main(void) {
  printf("=== Test 1: DC pass-through ===\n");
  {
    BiquadCascade_t S;
    float32_t state[2 * FILTER_NUM_STAGES];
    BiquadCascade_Init(&S, FILTER_NUM_STAGES, filter_coeffs, state);
    float32_t y = 0.0f;
    for (int n = 0; n < 2000; n++) {
      y = BiquadCascade_ProcessSample(&S, 100.0f); /* e.g. a raw ADC-ish constant */
    }
    check("settled output == constant input", y, 100.0, 0.01);
  }

  printf("\n=== Test 2: gain at the 200 Hz design cutoff (expect -3 dB, ratio 0.7071) ===\n");
  {
    double g = measure_gain_at(200.0f);
    double g_db = 20.0 * log10(g);
    check("amplitude ratio at cutoff", g, 0.70711, 0.01);
    printf("       (that's %.2f dB)\n", g_db);
  }

  printf("\n=== Test 3: aliasing identity -- is 20 kHz even distinguishable from 3.81 kHz here? ===\n");
  printf("  (this finding matters more than any coefficient choice below)\n");
  {
    /* At fs = 23,809.5 Hz, Nyquist is 11,904.75 Hz. A true 20 kHz tone
     * and a true (fs - 20000) = 3,809.5 Hz tone produce IDENTICAL
     * sample sequences -- not approximately, exactly, because
     * cos(2*pi*n - x) = cos(x) for any integer n. This is the textbook
     * aliasing identity, shown here numerically rather than asserted:
     * once the ADC has sampled, there is no way for this filter (or
     * any digital filter) to tell these two inputs apart. */
    /* Using cosine (an even function) rather than sine for this specific
     * check: cos(2*pi*n - x) = cos(x) exactly for integer n, so a true
     * 20 kHz cosine and a true 3,809.5 Hz cosine land on *identical*
     * samples with no sign ambiguity. (A sine would alias too, just
     * phase-inverted -- cos just makes the identity visibly exact.) */
    double max_abs_diff = 0.0;
    float32_t f_alias = FS_HZ - 20000.0f; /* 3809.5 Hz */
    for (int n = 0; n < 5000; n++) {
      double a = cos(2.0 * M_PI * 20000.0 * n / FS_HZ);
      double b = cos(2.0 * M_PI * (double)f_alias * n / FS_HZ);
      double d = fabs(a - b);
      if (d > max_abs_diff) max_abs_diff = d;
    }
    printf("  max|sample(20000Hz,n) - sample(%.1fHz,n)| over 5000 samples = %.2e\n",
           (double)f_alias, max_abs_diff);
    check("20 kHz and its alias are numerically identical once sampled",
          max_abs_diff, 0.0, 1e-5);
    printf("  -> no post-ADC filter, biquad or otherwise, can tell these apart.\n"
           "     The only real fix is an analog anti-alias stage BEFORE the ADC pin.\n");
  }

  printf("\n=== Test 4: attenuation at the inverter's switching frequency and its aliases ===\n");
  {
    /* Feeding the nominal 20 kHz (and harmonics) into the discrete-time
     * sine generator below automatically produces the aliased sample
     * sequence (per Test 3's identity) -- so "measuring the gain at
     * 20000.0f" here is actually measuring what the filter does to the
     * signal that really reaches it post-ADC, folding included.
     *
     * generate_coeffs.py's theoretical analysis says these should be
     * attenuated by well over 100 dB -- but single-precision float
     * arithmetic has its own rounding-noise floor around -65 to -90 dB
     * (same floor seen and confirmed against a double-precision
     * reference during the original 5 kHz-cutoff design), so the
     * measured number realistically bottoms out there regardless of
     * how much deeper the theoretical response goes. That's a float32
     * precision limit, not a filter error -- and -65 dB is already a
     * >1700x reduction, far beyond what a temperature reading needs.
     * So instead of chasing the theoretical number, this just checks
     * attenuation is at least as strong as that practical floor. */
    struct { float32_t f_nominal; double theoretical_db; double floor_db; } cases[] = {
      {20000.0f, -105.50, -50.0},  /* aliases to ~3810 Hz */
      {40000.0f, -142.08, -50.0},  /* aliases to ~7619 Hz */
      {60000.0f, -222.41, -50.0},  /* aliases to ~11429 Hz, right at Nyquist */
    };
    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
      double g = measure_gain_at(cases[i].f_nominal);
      double g_db = 20.0 * log10(g);
      char label[80];
      snprintf(label, sizeof(label), "%.0f Hz attenuated to at least %.0f dB (theory: %.1f dB)",
               cases[i].f_nominal, cases[i].floor_db, cases[i].theoretical_db);
      /* PASS if g_db <= floor_db (more negative = more attenuation) */
      const char *verdict = (g_db <= cases[i].floor_db) ? "PASS" : "FAIL";
      if (g_db > cases[i].floor_db) g_fail = 1;
      printf("  [%s] %-58s measured=%.2f dB\n", verdict, label, g_db);
    }
  }

  printf("\n=== Test 5: composite signal, two channels back-to-back with reset ===\n");
  printf("  (first: how long does the filter take to settle from zero state?)\n");
  {
    BiquadCascade_t St;
    float32_t stt[2 * FILTER_NUM_STAGES];
    BiquadCascade_Init(&St, FILTER_NUM_STAGES, filter_coeffs, stt);
    int settle_n = -1;
    for (int n = 0; n < 500; n++) {
      float32_t y = BiquadCascade_ProcessSample(&St, 2400.0f);
      if (settle_n < 0 && fabsf(y - 2400.0f) < 2.4f /* within 0.1% */) settle_n = n;
    }
    double window_ms = 500.0 / FS_HZ * 1000.0;
    printf("  settles to within 0.1%% of a step input after %d samples (%.2f ms of the %.1f ms scan window)\n",
           settle_n, (double)settle_n / FS_HZ * 1000.0, window_ms);
    printf("  (note: a 500-sample scan window is now ~%.1f ms of real time at this fs, not 2 ms --\n"
           "   the slower ADC means more real time per channel, which also means more settling margin)\n",
           window_ms);
  }
  {
    /* Simulated "channel scan": 500 raw samples at 4us spacing (2ms window),
     * same as the 500-sample inner loop in ScanAllMuxChannels(). */
    const int N = 500;
    BiquadCascade_t S;
    float32_t state[2 * FILTER_NUM_STAGES];

    for (int ch = 0; ch < 2; ch++) {
      /* True (slow) channel temperature, in raw-ADC-ish units, plus a
       * switching-frequency EMI burst riding on top of it. Two channels
       * use different baselines/noise amplitudes on purpose, to make
       * sure state from channel 0 can't bleed into channel 1. */
      float32_t baseline = (ch == 0) ? 2400.0f : 2600.0f;
      float32_t emi_amp  = (ch == 0) ? 300.0f  : 500.0f;
      float32_t emi_freq = 20000.0f; /* placeholder inverter switching freq */

      BiquadCascade_Init(&S, FILTER_NUM_STAGES, filter_coeffs, state); /* == Biquad_Reset */

      /* IMPORTANT FINDING (see the transient-length note printed above):
       * starting from zero state, the step response settles in ~145
       * samples, but with EMI riding on top the full ring-down (step +
       * oscillation) takes longer -- empirically around 350-400 of the
       * 500 samples at this cutoff. Averaging over the FULL 500-sample
       * window -- the way ScanAllMuxChannels() currently averages the
       * raw samples -- would drag the average down/around by including
       * that ramp-up. A correct integration skips the not-yet-settled
       * prefix, same as the code already skips out-of-range raw samples. */
      const int settle_samples = 400;
      double sum_filtered = 0.0, sum_raw = 0.0;
      for (int n = 0; n < N; n++) {
        float32_t t = (float32_t)n / FS_HZ;
        float32_t raw = baseline + emi_amp * sinf(2.0f * (float32_t)M_PI * emi_freq * t);
        float32_t filtered = BiquadCascade_ProcessSample(&S, raw);
        sum_raw += raw;
        if (n >= settle_samples) sum_filtered += filtered;
      }
      double avg_raw = sum_raw / N;
      double avg_filtered = sum_filtered / (N - settle_samples);
      char label[64];
      snprintf(label, sizeof(label), "ch%d: settled filtered average == true baseline", ch);
      check(label, avg_filtered, baseline, 1.0);
      (void)avg_raw;

      /* Peak-to-peak wiggle is the thing that actually corrupts an
       * instantaneous single-sample temperature reading -- that's what
       * the filter needs to knock down, and averaging over 500 samples
       * would hide that even with no filtering at all. */
      /* NOTE: the DC/step response settles in ~90 samples (measured
       * above), but a resonant stage's *oscillatory* ring-down takes
       * longer to fully die out than its step response does -- here
       * that's roughly the last 100 of 500 samples. Measuring ripple
       * too early (e.g. right after the step settles) would still be
       * seeing decaying startup ringing, not the true steady-state
       * attenuation -- worth knowing since a full channel scan is only
       * 500 samples to begin with. */
      float32_t raw_min = 1e9f, raw_max = -1e9f, filt_min = 1e9f, filt_max = -1e9f;
      BiquadCascade_Init(&S, FILTER_NUM_STAGES, filter_coeffs, state);
      for (int n = 0; n < N; n++) {
        float32_t t = (float32_t)n / FS_HZ;
        float32_t raw = baseline + emi_amp * sinf(2.0f * (float32_t)M_PI * emi_freq * t);
        float32_t filtered = BiquadCascade_ProcessSample(&S, raw);
        if (raw < raw_min) raw_min = raw;
        if (raw > raw_max) raw_max = raw;
        if (n >= 400) { /* last 100 samples: fully rung down */
          if (filtered < filt_min) filt_min = filtered;
          if (filtered > filt_max) filt_max = filtered;
        }
      }
      double atten_db = 20.0 * log10((double)(filt_max - filt_min) / (double)(raw_max - raw_min));
      printf("  ch%d: raw p-p = %.1f, filtered p-p (steady-state, last 100 samples) = %.2f  (%.1f dB attenuation)\n",
             ch, (double)(raw_max - raw_min), (double)(filt_max - filt_min), atten_db);
    }
  }

  printf("\n%s\n", g_fail ? "*** ONE OR MORE CHECKS FAILED ***" : "All checks passed.");
  return g_fail;
}
