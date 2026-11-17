"""Run: python3 test_adc_average.py (uses only stdlib and a host C compiler)."""

import subprocess
import tempfile
from pathlib import Path

source = (Path(__file__).parent / "main.c").read_text()
average = source.split("/* RAW AVERAGE BEGIN */")[1].split("/* RAW AVERAGE END */")[0]
harness = (
    """
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#define ADC_CAPTURE_SAMPLES 500U
"""
    + average
    + """
int main(void) {
    uint16_t samples[500];
    float mean;
    for (unsigned i=0; i<500; i++) samples[i]=2500;
    assert(ADC1_Average(samples, &mean) && mean==2500.0f);
    samples[0]=2501;
    assert(ADC1_Average(samples, &mean) && mean>2500.001f && mean<2500.003f);
    for (unsigned i=0; i<424; i++) samples[i]=0;
    assert(ADC1_Average(samples, &mean) && mean==2500.0f);
    samples[424]=4095;
    assert(!ADC1_Average(samples, &mean));
    const uint16_t edges[]={1911,1912,2961,2962};
    for (unsigned k=0; k<4; k++) {
        for (unsigned i=0; i<500; i++) samples[i]=edges[k];
        assert(ADC1_Average(samples, &mean)==(k==1 || k==2));
    }
}
"""
)
with tempfile.TemporaryDirectory() as temp:
    path = Path(temp)
    (path / "check.c").write_text(harness)
    subprocess.run(
        [
            "cc",
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-Werror",
            str(path / "check.c"),
            "-o",
            str(path / "check"),
        ],
        check=True,
    )
    subprocess.run([str(path / "check")], check=True)
assert "Biquad" not in source and "FILTER_WARMUP_SAMPLES" not in source
print("PASS: raw averaging, fractional mean, fault thresholds, and filter removal")
