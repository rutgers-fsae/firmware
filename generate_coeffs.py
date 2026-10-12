#!/usr/bin/env python3
"""
generate_coeffs.py

Design a cascaded-biquad Butterworth low-pass filter and print it as a
CMSIS-DSP-ready C coefficient array (b0,b1,b2,a1,a2 per stage, with a1/a2
already sign-flipped to CMSIS's convention -- see biquad_cascade.h).

*** This is the "Next Steps #1: calculate coefficients using Python" step
    from the assignment. Re-run it once you have the real numbers for: ***
      FS_HZ        - actual ADC sampling rate for a channel (not the old
                      "4us per raw read" number, if that changes)
      CUTOFF_HZ    - should sit above the thermal signal bandwidth (which
                      is essentially DC / seconds-scale) and comfortably
                      below the inverter's fundamental switching frequency
      ORDER        - 2 = 1 stage, 4 = 2 stages, 6 = 3 stages, ...
                      more stages = steeper rolloff, more state/CPU per
                      channel (cheap either way on this scale)

Requires: numpy, scipy (pip install numpy scipy --break-system-packages)

-------------------------------------------------------------------------
fs DERIVATION (STM32F103T8U6, from main.c's actual register settings --
this replaces the old "~4us/sample" comment, which was off by ~10x):

  SystemClock_Config(): PLLMUL9 off an assumed 8 MHz HSE crystal (the
    standard external crystal for this part -- confirm against the
    board's actual crystal if it's not 8 MHz, the whole calculation
    below scales linearly with it) -> SYSCLK = 8 MHz * 9 = 72 MHz
  AHBCLKDivider = RCC_SYSCLK_DIV2                -> HCLK  = 36 MHz
  APB2CLKDivider = RCC_HCLK_DIV1 (ADC is on APB2) -> PCLK2 = 36 MHz
  AdcClockSelection = RCC_ADCPCLK2_DIV6           -> ADCCLK = 6 MHz

  ADC1_SetLongSampleTime() / MX_ADC1_Init() both select
  ADC_SAMPLETIME_239CYCLES_5 (239.5 ADC clock cycles). The STM32F1 ADC
  is fixed 12-bit and always adds 12.5 conversion cycles on top of the
  sampling time, so:

      t_conv = (239.5 + 12.5) cycles / 6 MHz = 42.0 us/sample
      fs     = 1 / 42.0us      = 23,809.5 Hz   (NOT 250 kHz)

  This is a hardware lower bound -- HAL_ADC_Start()/Stop() being called
  per-sample (main.c does this, rather than continuous/DMA mode) adds
  software overhead on top, so the real achieved rate is likely a bit
  slower than this, not faster.

*** NYQUIST WARNING -- read this before trusting any cutoff choice ***
  Nyquist = fs/2 = 11,905 Hz. The Cascadia/Rinehart PM100DX's switching
  frequency is commonly cited around 20 kHz for this hardware (one
  published test of this exact inverter module used 10 kHz SVPWM as a
  baseline -- if that's closer to what's actually running, it's still
  within ~2 kHz of Nyquist, which is barely better). Either way, the
  IGBT switching edges are fast, harmonic-rich, broadband noise, not a
  single clean tone -- once anything in that noise extends above
  ~12 kHz and it's sampled at ~23.8 kHz, it folds (aliases) back down
  into the 0-12 kHz band that's actually being recorded, including
  potentially right on top of the near-DC thermal signal this filter
  is trying to protect. A digital filter -- no matter how good -- runs
  *after* the ADC has already sampled, so it physically cannot remove
  energy that has already folded into the signal it's given; it can
  only filter the frequencies it can still tell apart.
  This makes an ANALOG anti-aliasing stage (a passive RC low-pass right
  at the ADC pin, cutoff well below ~12 kHz, or a ferrite + capacitor)
  the thing that actually prevents this corruption -- this digital
  cascade is a second layer on top of that, not a replacement for it.
  Worth raising with the team before or alongside deploying this code.
-------------------------------------------------------------------------
"""
import numpy as np
from scipy import signal

# ----------------------------------------------------------------------
# fs: derived above from main.c's actual clock-tree/ADC registers
#     (assumes an 8 MHz HSE crystal -- see derivation above).
# CUTOFF_HZ: battery temperature genuinely changes on a seconds-to-
#     minutes timescale, so there's no need to keep the old 5 kHz
#     cutoff (which, at this corrected fs, is uncomfortably close to
#     the new 11.9 kHz Nyquist anyway). 200 Hz gives a lot more
#     stopband margin against the switching noise and its aliases
#     while still tracking real temperature changes with a settling
#     time on the order of tens of ms -- plenty fast.
# ----------------------------------------------------------------------
FS_HZ = 23_809.5      # see derivation above -- was wrongly 250_000.0
CUTOFF_HZ = 200.0     # was 5_000.0 -- see note above on why it's lower now
ORDER = 4              # 4th order (2 biquad stages)


def design(fs, cutoff, order):
    sos = signal.butter(order, cutoff, btype="low", fs=fs, output="sos")
    return sos


def to_cmsis(sos):
    """Convert scipy SOS rows [b0,b1,b2,a0,a1,a2] (a0==1) to the CMSIS
    per-stage order (b0,b1,b2,a1,a2) with a1,a2 negated -- see the
    coefficient-sign note at the top of biquad_cascade.h."""
    coeffs = []
    for b0, b1, b2, a0, a1, a2 in sos:
        assert abs(a0 - 1.0) < 1e-9
        coeffs += [b0, b1, b2, -a1, -a2]
    return coeffs


def report(sos, fs, check_freqs):
    w, h = signal.sosfreqz(sos, worN=check_freqs, fs=fs)
    print("Theoretical magnitude response of this design:")
    for f, hv in zip(check_freqs, h):
        mag = abs(hv)
        db = 20 * np.log10(mag + 1e-30)
        print(f"  {f:>7.0f} Hz : gain = {mag:.6f}  ({db:+7.2f} dB)")


if __name__ == "__main__":
    sos = design(FS_HZ, CUTOFF_HZ, ORDER)
    coeffs = to_cmsis(sos)

    print(f"fs = {FS_HZ:.0f} Hz, cutoff = {CUTOFF_HZ:.0f} Hz, order = {ORDER} "
          f"({sos.shape[0]} biquad stage(s))\n")

    print(f"#define FILTER_NUM_STAGES {sos.shape[0]}u")
    print("static const float32_t filter_coeffs["
          f"FILTER_NUM_STAGES * 5] = {{")
    for i in range(0, len(coeffs), 5):
        b0, b1, b2, a1, a2 = coeffs[i:i + 5]
        print(f"  {b0:.10f}f, {b1:.10f}f, {b2:.10f}f, {a1:.10f}f, {a2:.10f}f,"
              f"  /* stage {i // 5} */")
    print("};\n")

    # Check both the raw switching frequency AND where it (and its
    # harmonics) alias to post-ADC -- the filter can only act on the
    # latter, since that's all it ever sees.
    report(sos, FS_HZ, [0, 1, 10, 60, CUTOFF_HZ, 1000, 3810, 7619, 11429, 11904])
    print("\n(3810 / 7619 / 11429 Hz above are where the PM100DX's switching")
    print(" fundamental and 2nd/3rd harmonics land AFTER aliasing at this fs --")
    print(" see the big comment at the top of this file.)")
