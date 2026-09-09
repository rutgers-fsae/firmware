#!/usr/bin/env python3
"""Flash an RFR STM32 bootloader target over Classical CAN 2.0."""

import argparse
import pathlib
import struct
import time
import zlib

try:
    import can
except ImportError as exc:
    raise SystemExit("Install python-can first: python3 -m pip install python-can") from exc

DISCOVERY_ID = 0x5E0
COMMAND_BASE = 0x600
DATA_BASE = 0x620
RESPONSE_BASE = 0x680
ACK, READY, COMPLETE = 0x00, 0x01, 0x02
INFO, BEGIN, SET_CRC, END, RESET, START, ABORT = range(1, 8)


class FlashError(RuntimeError):
    pass


class CanFlasher:
    def __init__(self, bus, node: int, timeout: float = 1.0, retries: int = 3):
        if node not in range(1, 8):
            raise ValueError("node must be between 1 and 7")
        self.bus, self.node, self.timeout, self.retries = bus, node, timeout, retries

    def _send(self, arbitration_id: int, payload: bytes) -> None:
        self.bus.send(can.Message(arbitration_id=arbitration_id, data=payload, is_extended_id=False))

    def _response(self, allowed=(ACK, READY, COMPLETE)):
        deadline = time.monotonic() + self.timeout
        while time.monotonic() < deadline:
            msg = self.bus.recv(deadline - time.monotonic())
            if msg and msg.arbitration_id == RESPONSE_BASE + self.node:
                status, command, sequence, detail = struct.unpack("<BBHI", bytes(msg.data))
                if status not in allowed:
                    raise FlashError(f"target error 0x{status:02x}, command=0x{command:02x}, detail={detail}")
                return status, command, sequence, detail
        raise FlashError("timed out waiting for target")

    def command(self, opcode: int, value: int | None = None, allowed=(ACK, READY, COMPLETE)):
        payload = bytes([opcode]) if value is None else struct.pack("<BI", opcode, value)
        self._send(COMMAND_BASE + self.node, payload)
        return self._response(allowed)

    def flash(self, image: bytes, progress=lambda done, total: None) -> None:
        crc = zlib.crc32(image) & 0xFFFFFFFF
        self.command(BEGIN, len(image))
        self.command(SET_CRC, crc)
        for sequence, offset in enumerate(range(0, len(image), 6)):
            payload = struct.pack("<H", sequence) + image[offset:offset + 6]
            for attempt in range(self.retries + 1):
                self._send(DATA_BASE + self.node, payload)
                try:
                    _, _, ack_sequence, written = self._response((ACK,))
                    break
                except FlashError:
                    if attempt == self.retries:
                        raise
            if ack_sequence != sequence:
                raise FlashError(f"expected ACK {sequence}, received {ack_sequence}")
            progress(written, len(image))
        self.command(END, allowed=(COMPLETE,))

    def enter_bootloader(self) -> None:
        """Request an application reset, then discard any stale bootloader ACK."""
        self._send(COMMAND_BASE + self.node, bytes([RESET]))
        time.sleep(0.2)
        while self.bus.recv(0) is not None:
            pass


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("image", type=pathlib.Path, nargs="?")
    parser.add_argument("--node", type=int, required=True)
    parser.add_argument("--interface", default="socketcan")
    parser.add_argument("--channel", default="can0")
    parser.add_argument("--bitrate", type=int, default=500000)
    parser.add_argument("--reset", action="store_true")
    args = parser.parse_args()
    with can.Bus(interface=args.interface, channel=args.channel, bitrate=args.bitrate) as bus:
        flasher = CanFlasher(bus, args.node)
        if args.reset:
            flasher.enter_bootloader()
            print(f"Reset command sent to node {args.node}")
            return
        if not args.image:
            parser.error("image is required unless --reset is used")
        image = args.image.read_bytes()
        flasher.enter_bootloader()
        flasher.flash(image, lambda done, total: print(f"\r{done}/{total} bytes", end="", flush=True))
        print("\nFlash verified; starting application")
        flasher.command(START)


if __name__ == "__main__":
    main()
