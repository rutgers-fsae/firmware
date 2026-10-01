"""Run with ~/Coding/firmware/python/.venv/bin/python test_filter.py.

Compiles the actual C filter and acquisition functions with a small HAL stub.
Requires the existing NumPy/SciPy environment and a host C compiler.
"""

import ctypes
import os
import re
import subprocess
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path

import numpy as np
from scipy.signal import butter, sosfilt, sosfilt_zi, sosfreqz

ROOT = Path(__file__).resolve().parent
source = (ROOT / "main.c").read_text()
filter_c = (
    source.split("/* FILTER BEGIN:", 1)[1]
    .split("*/", 1)[1]
    .split("/* FILTER END */", 1)[0]
)
capture_c = source[
    source.index("void HAL_ADC_ConvCpltCallback(") : source.index(
        "// use linear interperolation"
    )
]
warmup = int(re.search(r"#define FILTER_WARMUP_SAMPLES (\d+)U", source)[1])
capture_samples = warmup + 500
oc_mode = re.search(r"sConfigOC.OCMode = (\w+);", source)[1]
expected_sos = butter(4, 200, fs=45000, output="sos").astype(np.float32)

# Extract the coefficients actually compiled into the firmware.
rows = source.split("static const float sos[NUM_STAGES][5] = {", 1)[1].split("};", 1)[0]
sos = np.array(
    [
        [float(v.strip().removesuffix("f")) for v in row.split(",")]
        for row in re.findall(r"\{([^{}]+)\}", rows)
    ],
    dtype=np.float32,
)
sos = np.insert(sos, 3, 1, axis=1)
np.testing.assert_array_equal(sos, expected_sos)
assert np.all([np.max(np.abs(np.roots(row[3:]))) < 1 for row in sos])
_, response = sosfreqz(sos, worN=[0, 10, 200, *range(18000, 21001, 100)], fs=45000)
assert abs(abs(response[0]) - 1) < 5e-4
assert abs(20 * np.log10(abs(response[1]))) < 0.01
assert abs(20 * np.log10(abs(response[2])) + 3.0103) < 0.01
assert np.max(20 * np.log10(abs(response[3:]))) < -60
# Stub only acquisition hardware, leaving the actual firmware control flow intact.
harness = (
    """
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
"""
    + filter_c
    + "\n#define TIM_OCMODE_TIMING 0\n#define TIM_OCMODE_PWM2 7\nstatic const int timer_mode = "
    + oc_mode
    + ";\n"
    + """
void filter_run(const float *input, float *output, uint32_t n, float seed) {
    BiquadState state[NUM_STAGES];
    Biquad_Reset(state, seed);
    for (uint32_t i=0; i<n; i++) output[i]=Biquad_Process(state, input[i]);
}
bool filter_average(const uint16_t *samples, float *average) {
    return Biquad_Average(samples, average);
}
typedef enum {HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT} HAL_StatusTypeDef;
typedef struct {void *Instance;} ADC_HandleTypeDef;
typedef struct {int unused;} DMA_HandleTypeDef;
typedef struct {int unused;} TIM_HandleTypeDef;
#define ADC1 ((void *)1)
#define TIM_CHANNEL_4 4U
#define TIM_FLAG_UPDATE 1U
#define TIM_FLAG_CC4 2U
#define DMA_IT_HT 1U
ADC_HandleTypeDef hadc1={ADC1};
DMA_HandleTypeDef hdma_adc1;
TIM_HandleTypeDef htim4;
static _Alignas(uint32_t) uint16_t adc_samples[ADC_CAPTURE_SAMPLES];
static volatile HAL_StatusTypeDef adc_capture_status;
static int mode, timer_running, dma_armed, stops, counter_reset;
static uint32_t tick, initial_tick, acquired;
static int oc4ref;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
static void reset_counter(void) {assert(!timer_running); counter_reset=1; oc4ref=0; acquired=0;}
#define __HAL_TIM_SET_COUNTER(h,v) reset_counter()
#define __HAL_TIM_CLEAR_FLAG(h,v) ((void)0)
#define __HAL_DMA_DISABLE_IT(h,v) ((void)0)
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *h, uint32_t channel) {
    (void)h; assert(channel==4U); timer_running=0; return HAL_OK;
}
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *h, uint32_t *buffer, uint32_t n) {
    (void)h; assert(!timer_running && counter_reset && !dma_armed);
    assert(n==ADC_CAPTURE_SAMPLES && (uintptr_t)buffer%4==0);
    if (mode==3) return HAL_ERROR;
    if (mode==5) return HAL_OK; /* HAL silently failed to arm DMA */
    dma_armed=1; return HAL_OK;
}
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *h, uint32_t channel) {
    (void)h; assert(channel==4 && dma_armed && counter_reset);
    if (mode==4) return HAL_ERROR;
    timer_running=1; return HAL_OK;
}

#define HAL_DMA_STATE_BUSY 1
int HAL_DMA_GetState(DMA_HandleTypeDef *h) {(void)h; return dma_armed;}
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef *h) {
    (void)h; assert(!timer_running); dma_armed=0; stops++; return HAL_OK;
}
uint32_t HAL_GetTick(void) {
    tick++;
    if (timer_running) {
        /* Model 45 timer periods per millisecond and rising OC4REF edges.
         * Frozen compare mode raises flags but produces no ADC trigger edges. */
        for (int period=0; period<45; period++) {
            for (int cnt=0; cnt<1600; cnt++) {
                int ref=(timer_mode==TIM_OCMODE_PWM2 && cnt>=800);
                if (ref && !oc4ref) acquired++;
                oc4ref=ref;
            }
        }
        if (mode==0 && acquired>=ADC_CAPTURE_SAMPLES) HAL_ADC_ConvCpltCallback(&hadc1);
        if (mode==2 && acquired>0) HAL_ADC_ErrorCallback(&hadc1);
    }
    return tick;
}
void Error_Handler(void) {abort();}
"""
    + capture_c
    + """
int capture_run(int requested_mode, uint32_t start) {
    mode=requested_mode; initial_tick=tick=start; counter_reset=0;
    adc_capture_status=HAL_OK; /* stale completion must not bypass capture */
    int result=ADC1_Capture();
    assert(!timer_running && !dma_armed && stops>0);
    if (requested_mode==1) assert(tick-start>=ADC_CAPTURE_TIMEOUT_MS);
    return result;
}
"""
)
fp = ctypes.POINTER(ctypes.c_float)
up = ctypes.POINTER(ctypes.c_uint16)
with tempfile.TemporaryDirectory(prefix="rfr-filter-") as temp:
    c_path = Path(temp) / "check.c"
    c_path.write_text(harness)
    for optimization in ["-O0", "-O2"]:
        library = Path(temp) / f"check{optimization}.so"
        subprocess.run(
            [
                os.environ.get("CC", "cc"),
                "-std=c11",
                "-Wall",
                "-Wextra",
                "-Werror",
                optimization,
                "-ffp-contract=off",
                "-shared",
                "-fPIC",
                str(c_path),
                "-o",
                str(library),
            ],
            check=True,
        )
        lib = ctypes.CDLL(str(library))
        lib.filter_run.argtypes = [fp, fp, ctypes.c_uint32, ctypes.c_float]
        lib.filter_average.argtypes = [up, fp]
        lib.filter_average.restype = ctypes.c_bool
        lib.capture_run.argtypes = [ctypes.c_int, ctypes.c_uint32]
        lib.capture_run.restype = ctypes.c_int

        def run(x, seed=0):
            x = np.ascontiguousarray(x, dtype=np.float32)
            y = np.empty_like(x)
            lib.filter_run(x.ctypes.data_as(fp), y.ctypes.data_as(fp), len(x), seed)
            return y

        def average(samples):
            samples = np.ascontiguousarray(samples, dtype=np.uint16)
            value = ctypes.c_float()
            valid = lib.filter_average(samples.ctypes.data_as(up), ctypes.byref(value))
            return valid, value.value

        # Float32 rounding at 200 Hz must stay within one 12-bit ADC count.
        rng = np.random.default_rng(42)
        cases = [
            (np.r_[1.0, np.zeros(999)], 0),
            (np.r_[np.zeros(50), np.full(950, 2500)], 0),
            (rng.uniform(0, 4095, 10000), 2000),
        ]
        for x, seed in cases:
            reference = sosfilt(
                sos.astype(float), x, zi=sosfilt_zi(sos.astype(float)) * seed
            )[0]
            np.testing.assert_allclose(run(x, seed), reference, rtol=0, atol=1.0)
        # Repeated resets must not carry state from the previous mux channel.
        for level in [2500, 2000, 2900, 2000]:
            assert np.max(np.abs(run(np.full(capture_samples, level), level) - level)) < 1.0
            valid, value = average(np.full(capture_samples, level))
            assert valid and abs(value - level) < 1.0
        # A large initial error must settle before the averaging window.
        samples = np.full(capture_samples, 2900, dtype=np.uint16)
        samples[0] = 2000
        valid, value = average(samples)
        assert valid and abs(value - 2900) < 1.0
        for f in [18000, 20000, 21000]:
            n = np.arange(4500)
            x = 2500 + 200 * np.cos(2 * np.pi * f * n / 45000)
            y = run(x, x[0])[warmup:]
            assert np.sqrt(np.mean((y - 2500) ** 2)) < 1.0
        for level in [0, 1911, 2962, 4095]:
            assert not average(np.full(capture_samples, level))[0]
        for level in [1912, 2961]:
            assert average(np.full(capture_samples, level))[0]
        samples = np.full(capture_samples, 2500, dtype=np.uint16)
        samples[warmup : warmup + 424] = 0
        assert average(samples)[0]
        samples[warmup + 424] = 0
        assert not average(samples)[0]
        # Warmup affects state but must not affect raw fault counts.
        samples = np.full(capture_samples, 2500, dtype=np.uint16)
        samples[:warmup] = 4095
        valid, value = average(samples)
        reference = sosfilt(
            sos.astype(float), samples, zi=sosfilt_zi(sos.astype(float)) * samples[0]
        )[0]
        assert valid and abs(value - reference[warmup:].mean()) < 1.0
        # A rejected raw sample must still update the recursive filter.
        samples = np.full(capture_samples, 2500, dtype=np.uint16)
        samples[warmup + 100] = 0
        reference = sosfilt(
            sos.astype(float), samples, zi=sosfilt_zi(sos.astype(float)) * samples[0]
        )[0]
        valid, value = average(samples)
        accepted = samples[warmup:] > 1911
        assert valid and abs(value - reference[warmup:][accepted].mean()) < 1.0
        assert not np.isfinite(run([2500], float("inf"))[0])
        for start in [0, 0xFFFFFFF0]:
            for mode, result in [
                (0, 0),
                (1, 3),
                (2, 1),
                (3, 1),
                (4, 1),
                (5, 1),
                (0, 0),
            ]:
                assert lib.capture_run(mode, start) == result

# Verify configured clock relationships, including the independent CMSIS fallback.
config = (ROOT.parent / "Inc/stm32f1xx_hal_conf.h").read_text()
system = (ROOT / "system_stm32f1xx.c").read_text()
ioc = {
    key.replace(r"\ ", " "): value
    for key, value in (
        line.split("=", 1)
        for line in (ROOT.parent.parent / "rfr26-tempSensor.ioc")
        .read_text()
        .splitlines()
        if "=" in line
    )
}
project = ET.parse(ROOT.parent.parent / ".cproject")
assert (
    sum(
        option.get("value") == "HSE_VALUE=12000000U"
        for option in project.iter("listOptionValue")
    )
    == 2
)
assert re.search(r"#define HSE_VALUE\s+12000000U", config)
assert re.search(r"#define HSE_VALUE\s+12000000U", system)
assert "RCC_PLL_MUL6" in source and "RCC_SYSCLK_DIV1;" in source
prescaler = int(re.search(r"hcan.Init.Prescaler = (\d+)", source)[1])
assert 36000000 / (prescaler * (1 + 13 + 4)) == 500000
assert ioc["CAN.Prescaler"] == str(prescaler)
assert ioc["RCC.HSE_VALUE"] == "12000000"
assert (
    ioc["TIM4.Period"] == "1599"
    and ioc["TIM4.Pulse-PWM Generation4 No Output"] == "800"
)
assert int(re.search(r"#define ADC_SAMPLE_RATE_HZ (\d+)U", source)[1]) == 45000
assert 72000000 / (1599 + 1) == 45000
assert (239.5 + 12.5) / 12000000 < 1 / 45000
assert ioc["TIM4.OCMode_PWM_4"] == oc_mode == "TIM_OCMODE_PWM2"
assert "HAL_TIM_PWM_ConfigChannel" in source
assert "ADC_EXTERNALTRIGCONV_T4_CC4" in source
assert "SPI_BAUDRATEPRESCALER_8" in source
print(
    "PASS: actual C filter at O0/O2 matches SciPy; DC, EMI, faults, timer trigger edges, DMA failures/timeouts/retries, and clocks"
)
print(
    f"Digital attenuation at 20 kHz: {-20 * np.log10(abs(sosfreqz(sos, worN=[20000], fs=45000)[1][0])):.2f} dB"
)
