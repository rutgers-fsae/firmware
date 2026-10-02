# Wiring this into main.c

This replaces the old single-stage, hand-rolled `BiquadState` / `Biquad_Reset` /
`Biquad_Process` in `main.c` (the "vibe coded" version) with the tested
cascade in `biquad_cascade.h`.

## Read this first: the ADC sampling rate changes the whole picture

The old code assumed ~4 us/sample (250 kHz). That was wrong. Based on the
STM32F103T8U6's actual clock tree and ADC settings in `main.c`:

```
SystemClock_Config(): PLLMUL9 off an assumed 8 MHz HSE crystal (standard for
  this part -- confirm against the board if different; everything below
  scales linearly with it) -> SYSCLK = 72 MHz
AHBCLKDivider = RCC_SYSCLK_DIV2            -> HCLK  = 36 MHz
APB2CLKDivider = RCC_HCLK_DIV1 (ADC is on APB2) -> PCLK2 = 36 MHz
AdcClockSelection = RCC_ADCPCLK2_DIV6      -> ADCCLK = 6 MHz

ADC_SAMPLETIME_239CYCLES_5 (already selected in main.c) + the fixed 12.5
conversion cycles of the STM32F1's 12-bit ADC:

    t_conv = (239.5 + 12.5) / 6 MHz = 42.0 us/sample
    fs     = 1 / 42.0us = 23,809.5 Hz   (not 250 kHz)
```

That's ~10x slower than assumed, which puts **Nyquist at 11,905 Hz**.

The Cascadia/Rinehart PM100DX's switching frequency is commonly put around
20 kHz for this hardware (you mentioned ~20 kHz; one published test of this
exact inverter module cites a 10 kHz SVPWM baseline instead -- worth
double-checking which applies to your unit, but **both are close enough to
Nyquist that aliasing is the real concern either way**).

### Why this matters more than any coefficient choice

IGBT switching edges are fast, broadband, harmonic-rich noise -- not a
clean single tone. Once energy in that noise sits above ~12 kHz and gets
sampled at 23.8 kHz, it folds (aliases) back down into the 0-12 kHz band
that's actually recorded -- possibly right on top of the near-DC thermal
signal this filter exists to protect.

`test_biquad_cascade.c`'s Test 3 demonstrates this is not a vague concern
but an exact identity: a true 20 kHz tone and a true 3,809.5 Hz tone
produce **bit-for-bit identical sample sequences** once digitized at
23,809.5 Hz (`cos(2*pi*n - x) = cos(x)` for integer `n`). A digital filter
runs *after* the ADC has already sampled -- it cannot undo information that
was destroyed at the moment of sampling. **This is a hardware problem, and
a software-only fix doesn't fully solve it.**

**What actually fixes it:** an analog anti-aliasing stage in front of the
ADC pin -- a simple passive RC low-pass (or ferrite bead + capacitor) with
a cutoff comfortably below ~12 kHz, sized for the sensor's source
impedance. This is worth raising with whoever owns the sensor board layout
*before* this cascade filter goes in, since the digital filter below is a
second line of defense, not a replacement for the first.

## 1. Delete the old filter code from main.c

Remove:
```c
typedef struct { double w1; double w2; } BiquadState;
static BiquadState biquad_state[NUM_CHANNELS_TOTAL];
static void Biquad_Reset(uint8_t mux, uint8_t ch) { ... }
static double Biquad_Process(uint8_t mux, uint8_t ch, double x) { ... }
```
along with the `#define BIQUAD_B0 ...` block above it.

## 2. Add the new filter

```c
#include "biquad_cascade.h"

/* fs = 23,809.5 Hz, cutoff = 200 Hz, 4th order (2 stages) Butterworth
 * low-pass. Regenerate with generate_coeffs.py if the real HSE crystal
 * isn't 8 MHz, or once you know the inverter's actual switching freq. */
#define FILTER_NUM_STAGES 2u
static const float32_t filter_coeffs[FILTER_NUM_STAGES * 5] = {
  0.0000004531f, 0.0000009061f, 0.0000004531f, 1.9043973333f, -0.9070528626f, /* stage 0 */
  1.0000000000f, 2.0000000000f, 1.0000000000f, 1.9576927508f, -0.9604225962f, /* stage 1 */
};

/* One filter *instance* per channel (holds only the 2-float-per-stage state,
 * the coefficients above are shared/read-only across all channels) */
static float32_t filter_state[NUM_CHANNELS_TOTAL][2 * FILTER_NUM_STAGES];
static BiquadCascade_t filter_inst[NUM_CHANNELS_TOTAL];
```

Why 200 Hz instead of keeping the old 5 kHz: battery temperature changes on
a seconds timescale, so there's no benefit to a high cutoff, and at the
corrected (slower) fs, 5 kHz is uncomfortably close to the new 11.9 kHz
Nyquist anyway. 200 Hz buys a lot more stopband margin against the
switching noise (and its aliases) while still tracking real temperature
swings with a settling time of tens of ms -- plenty fast for a sensor that
itself has thermal mass.

## 3. Reset per channel (same place the old `Biquad_Reset` was called)

```c
uint16_t idx = (uint16_t)mux * MUX_CHANNELS_PER_CHIP + ch;
BiquadCascade_Init(&filter_inst[idx], FILTER_NUM_STAGES, filter_coeffs, filter_state[idx]);
```

## 4. Filter each raw sample instead of only range-checking it

Inside the existing 500-sample loop in `ScanAllMuxChannels()` -- note this
loop now spans ~21 ms of real time at the corrected fs, not 2 ms, so there's
more headroom to spend on settling than the old assumption implied:

```c
for (int i = 0; i < 500; i++) {
  uint16_t raw = ADC1_ReadRawSettled();
  float32_t filtered = BiquadCascade_ProcessSample(&filter_inst[idx], (float32_t)raw);

  if (i < 400) {
    continue;  /* filter + EMI ring-down still settling -- see note below */
  }
  if (filtered <= 1911.0f || filtered >= 2962.0f) {
    faults++;
    continue;
  }
  sum += filtered;
  count++;
}
```

## Important things the test run surfaced (measured, not guessed)

- **The ADC is ~10x slower than the old code assumed** (23.8 kHz, not
  250 kHz) -- see the derivation above. If the board's actual HSE crystal
  isn't 8 MHz, re-derive this before trusting anything else here.
- **Aliasing, not insufficient filtering, is the primary risk.** See the
  "read this first" section above -- this needs a hardware conversation,
  not just a firmware change.
- **Settling takes longer than it looks at first.** The step response
  settles in ~145 samples (~6 ms), but with EMI riding on top, the full
  ring-down takes closer to 400 of the 500 samples before the reading is
  trustworthy -- confirmed by direct measurement in `test_biquad_cascade.c`
  (Test 5). Skipping the first 400 samples, as shown above, is a
  reasonable starting point; tune it against real hardware once you have
  it.
- **Coefficient sign convention differs from the old code.** The old
  `Biquad_Process` used scipy's coefficients directly (`y = b0*w0 + b1*w1 +
  b2*w2`, with `w0 = x - a1*w1 - a2*w2`). CMSIS's `arm_biquad_cascade_df2T_f32`
  -- and this `biquad_cascade.h`, on purpose, so it's a drop-in for that
  function later -- expects `a1`/`a2` pre-negated. `generate_coeffs.py` does
  this conversion; don't paste raw scipy/MATLAB `a` coefficients in directly.

## 5. Later: swap in the real CMSIS-DSP call

Once `arm_math.h` is in the project (per Jeevan's Slack message, it ships with
the STM32 toolchain), `BiquadCascade_t`'s field names/order already match
`arm_biquad_cascade_df2T_instance_f32`, so the only change needed is:

```c
#include "arm_math.h"
arm_biquad_cascade_df2T_instance_f32 S = { FILTER_NUM_STAGES, filter_state[idx], filter_coeffs };
arm_biquad_cascade_df2T_f32(&S, &raw_f32, &filtered, 1); /* blockSize = 1 */
```
in place of `BiquadCascade_ProcessSample`. Same math, same coefficients,
CMSIS's version will just use hardware FPU instructions instead of this
plain-C version.
