#!/usr/bin/env python3
"""Measure how fast an Ewert CANdapter transmits a burst of CAN frames.

Queues COUNT standard frames in one serial write and times the adapter's
per-frame replies (ACK 0x06 = sent, BELL 0x07 = rejected). At least one other
powered node must be on the bus at 500 kbit/s to acknowledge the frames.
Requires pyserial.
"""

import argparse
import threading
import time

import serial

ACK, BELL = 0x06, 0x07


def setup(ser: serial.Serial, command: str, required: bool = True) -> None:
    ser.write(command.encode("ascii") + b"\r")
    reply = ser.read(1)
    if required and reply != bytes([ACK]):
        raise SystemExit(f"CANdapter rejected setup command {command!r}: {reply!r}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("port", help="e.g. /dev/cu.usbserial-XXXXXXXX")
    parser.add_argument("--count", type=int, default=1000)
    parser.add_argument(
        "--id",
        type=lambda text: int(text, 0),
        default=0x7F0,
        help="11-bit CAN ID no node acts on (default 0x7F0)",
    )
    parser.add_argument(
        "--idle",
        type=float,
        default=2.0,
        help="give up after this many seconds without a reply",
    )
    args = parser.parse_args()
    if not 0 <= args.id <= 0x7FF or args.count < 1:
        parser.error("--id must be 0..0x7FF and --count at least 1")

    with serial.Serial(args.port, 115200, timeout=1, write_timeout=30) as ser:
        setup(ser, "C", required=False)  # BELL here just means it was already closed.
        ser.reset_input_buffer()
        setup(ser, "S6")  # 500 kbit/s
        setup(ser, "A0")  # timestamps off
        setup(ser, "O")
        try:
            burst = b"".join(
                f"T{args.id:03X}8{index:016X}\r".encode("ascii")
                for index in range(args.count)
            )
            # Write from a thread: the adapter stalls unless it is read while transmitting.
            writer = threading.Thread(target=ser.write, args=(burst,), daemon=True)
            acks = bells = 0
            first = last = None
            ser.timeout = 0.05
            start = last_activity = time.monotonic()
            writer.start()
            while (
                acks + bells < args.count
                and time.monotonic() - last_activity < args.idle
            ):
                data = ser.read(ser.in_waiting or 1)
                now = time.monotonic()
                replies = data.count(ACK) + data.count(
                    BELL
                )  # other bytes are bus traffic
                if replies:
                    acks += data.count(ACK)
                    bells += data.count(BELL)
                    first = first if first is not None else now
                    last = last_activity = now
        finally:
            ser.write(b"C\r")

    print(
        f"Sent {args.count} frames: {acks} ACK, {bells} BELL, "
        f"{args.count - acks - bells} unanswered"
    )
    if first is None or last is None:
        raise SystemExit(
            "No replies. Check the port, bus wiring, termination, and that "
            "another node is powered at 500 kbit/s."
        )
    elapsed = last - start
    print(
        f"First reply after {(first - start) * 1000:.1f} ms; last after {elapsed:.3f} s"
    )
    print(
        f"Rate: {(acks + bells) / elapsed:.0f} frames/s "
        f"({elapsed / (acks + bells) * 1000:.2f} ms per frame)"
    )
    if bells or acks < args.count:
        raise SystemExit(
            "Not every frame was acknowledged; the rate above is not a clean result."
        )


if __name__ == "__main__":
    main()
