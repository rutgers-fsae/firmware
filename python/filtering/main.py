import numpy as np
from scipy.signal import iirnotch, cheby2, tf2sos
from scipy.signal import tf2sos

fs = 48000

# --- Notch at 20kHz ---
b_n, a_n = iirnotch(20000, Q=30, fs=fs)
sos_notch = tf2sos(b_n, a_n)

# --- Chebyshev II lowpass ---
sos_lp = cheby2(6, 60, 19000, btype='low', fs=fs, output='sos')

# --- Combine ---
sos = np.vstack([sos_notch, sos_lp])

# --- Print as C array ---
print(f"#define NUM_STAGES {len(sos)}\n")
print("// Each row: {b0, b1, b2, a1, a2} (a0 already normalized to 1.0)")
print("static const double sos[NUM_STAGES][5] = {")
for row in sos:
    b0, b1, b2, a0, a1, a2 = row
    # a0 should already be 1.0 after normalization
    print(f"    {{{b0:.10f}, {b1:.10f}, {b2:.10f}, {a1:.10f}, {a2:.10f}}},")
print("};")
