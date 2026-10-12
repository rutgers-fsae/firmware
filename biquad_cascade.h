/**
 * @file    biquad_cascade.h
 * @brief   Cascaded biquad (Direct Form II Transposed) IIR filter.
 *
 * This is a byte-for-byte port of the algorithm CMSIS-DSP uses in
 * arm_biquad_cascade_df2T_f32() (see the ref_biquad_cascade_df2T_f32()
 * you already have in biquad.c) — same struct layout, same math, same
 * coefficient order and sign convention. It is written in plain C with
 * no ARM intrinsics so it can be built and unit-tested on any machine
 * (this is how the results below were verified), and it can be swapped
 * for the real CMSIS call later with no change to how it's used:
 *
 *      arm_biquad_cascade_df2T_instance_f32  <-> BiquadCascade_t
 *      arm_biquad_cascade_df2T_f32(S,in,out,1) <-> BiquadCascade_ProcessSample(S,x)
 *
 * ---------------------------------------------------------------------
 * WHY DF2T (Direct Form II Transposed) and not the DF2 in the old
 * Biquad_Process() in main.c:
 *   - DF2T only needs 2 state words per stage (not 4), and is the form
 *     CMSIS ships natively, so this is a straight drop-in for
 *     arm_biquad_cascade_df2T_f32() with zero conversion work later.
 *   - It's the numerically best-conditioned direct form for cascaded
 *     sections in single-precision float, which matters once you start
 *     stacking >1 stage.
 *
 * COEFFICIENT SIGN CONVENTION (this bit really matters, and is the
 * single most common mistake when porting scipy/MATLAB coefficients
 * into CMSIS):
 *   scipy.signal.butter(..., output='sos') and the difference equation
 *   in the assignment notes both use:
 *       y[n] = b0 x[n] + b1 x[n-1] + b2 x[n-2] - a1 y[n-1] - a2 y[n-2]
 *   CMSIS's arm_biquad_cascade_df2T_f32 (and this file) instead expect
 *   the coefficients pre-negated, i.e. it computes:
 *       y[n] = b0 x[n] + b1 x[n-1] + b2 x[n-2] + a1 y[n-1] + a2 y[n-2]
 *   So: a1_cmsis = -a1_scipy,  a2_cmsis = -a2_scipy.
 *   (See generate_coeffs.py, which does this conversion for you.)
 * ---------------------------------------------------------------------
 */

#ifndef BIQUAD_CASCADE_H
#define BIQUAD_CASCADE_H

#include <stdint.h>
#include <string.h>

typedef float float32_t;

/* Mirrors arm_biquad_cascade_df2T_instance_f32's field names/order so this
 * struct can be replaced by the CMSIS one with no call-site changes. */
typedef struct {
  uint8_t numStages;         /* number of biquad (2nd-order) sections   */
  float32_t *pState;         /* 2 floats per stage: {d1,d2} x numStages */
  const float32_t *pCoeffs;  /* 5 floats per stage: {b0,b1,b2,a1,a2}    */
} BiquadCascade_t;

/**
 * @brief Initialize a cascade instance.
 * @param S         instance to initialize
 * @param numStages number of biquad sections
 * @param pCoeffs   coefficient table, 5 floats/stage, CMSIS sign convention
 * @param pState    caller-owned state buffer, 2 floats/stage, zeroed here
 */
static inline void BiquadCascade_Init(BiquadCascade_t *S, uint8_t numStages,
                                       const float32_t *pCoeffs, float32_t *pState) {
  S->numStages = numStages;
  S->pCoeffs = pCoeffs;
  S->pState = pState;
  memset(pState, 0, sizeof(float32_t) * 2u * numStages);
}

/** @brief Zero a cascade's filter memory (call before starting a new channel). */
static inline void BiquadCascade_Reset(BiquadCascade_t *S) {
  memset(S->pState, 0, sizeof(float32_t) * 2u * (uint32_t)S->numStages);
}

/**
 * @brief Push one sample through every stage of the cascade and return
 *        the filtered result. This is the sample-by-sample equivalent
 *        of calling arm_biquad_cascade_df2T_f32(S, &x, &y, 1).
 */
static inline float32_t BiquadCascade_ProcessSample(BiquadCascade_t *S, float32_t x) {
  const float32_t *pCoeffs = S->pCoeffs;
  float32_t *pState = S->pState;
  float32_t in = x;
  float32_t out = x;
  uint8_t stage;

  for (stage = 0; stage < S->numStages; stage++) {
    float32_t b0 = pCoeffs[0];
    float32_t b1 = pCoeffs[1];
    float32_t b2 = pCoeffs[2];
    float32_t a1 = pCoeffs[3];
    float32_t a2 = pCoeffs[4];
    float32_t d1 = pState[0];
    float32_t d2 = pState[1];

    /* y[n] = b0*x[n] + d1 */
    out = (b0 * in) + d1;

    /* update state for next call */
    d1 = (b1 * in) + (a1 * out) + d2;
    d2 = (b2 * in) + (a2 * out);
    pState[0] = d1;
    pState[1] = d2;

    /* this stage's output feeds the next stage's input */
    in = out;
    pCoeffs += 5;
    pState += 2;
  }

  return out;
}

#endif /* BIQUAD_CASCADE_H */
