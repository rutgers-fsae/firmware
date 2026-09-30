"""Generate the firmware Butterworth cascade; prints C, never edits firmware."""

import argparse

import numpy as np
from scipy.signal import butter, sosfreqz


def design(fs=45000, cutoff=5000, order=4):
    if fs <= 0 or not 0 < cutoff < fs / 2 or order < 2 or order % 2:
        raise ValueError(
            "Require fs > 0, 0 < cutoff < fs/2, and a positive even order >= 2"
        )
    return butter(order, cutoff, fs=fs, output="sos").astype(np.float32)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--fs", type=int, default=45000)
    parser.add_argument("--cutoff", type=int, default=5000)
    parser.add_argument("--order", type=int, default=4)
    args = parser.parse_args()
    try:
        sos = design(args.fs, args.cutoff, args.order)
    except ValueError as error:
        parser.error(str(error))
    print(f"// Butterworth order={args.order}, fc={args.cutoff} Hz, fs={args.fs} Hz")
    print(f"#define NUM_STAGES {len(sos)}U")
    print("// Rows: b0, b1, b2, a1, a2; a0 = 1. Feedback is subtracted.")
    print("static const float sos[NUM_STAGES][5] = {")
    for row in sos:
        values = []
        for value in row[[0, 1, 2, 4, 5]]:
            literal = f"{value:.9g}"
            if "." not in literal and "e" not in literal:
                literal += ".0"
            values.append(literal + "f")
        print("    {" + ", ".join(values) + "},")
    print("};")
    if 20000 < args.fs / 2:
        _, response = sosfreqz(sos, worN=[20000], fs=args.fs)
        print(
            f"// Digital attenuation at 20 kHz: {-20 * np.log10(abs(response[0])):.2f} dB"
        )


if __name__ == "__main__":
    main()
