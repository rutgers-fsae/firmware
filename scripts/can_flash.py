#!/usr/bin/env python3
"""Flash an RFR STM32 bootloader target over Classical CAN 2.0."""

import argparse
import pathlib
import struct
import time
import zlib
import binascii
from contextlib import closing

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
    def __init__(self, bus, node: int, timeout: float = 1.0, retries: int = 3, packet_crc: bool = True):
        if node not in range(1, 8):
            raise ValueError("node must be between 1 and 7")
        self.bus, self.node, self.timeout, self.retries = bus, node, timeout, retries
        self.packet_crc = packet_crc

    def _send(self, arbitration_id: int, payload: bytes) -> None:
        self.bus.send(can.Message(arbitration_id=arbitration_id, data=payload, is_extended_id=False))

    def _response(self, expected_command: int, allowed=(ACK, READY, COMPLETE), expected_sequence=None):
        deadline = time.monotonic() + self.timeout
        while time.monotonic() < deadline:
            msg = self.bus.recv(deadline - time.monotonic())
            if not msg or msg.arbitration_id != RESPONSE_BASE + self.node:
                continue
            if (getattr(msg, "is_extended_id", False) or
                    getattr(msg, "is_remote_frame", False) or
                    getattr(msg, "is_error_frame", False)):
                continue
            payload = bytes(msg.data)
            if getattr(msg, "dlc", len(payload)) != 8 or len(payload) != 8:
                continue
            status, command, sequence, detail = struct.unpack("<BBHI", payload)
            if command != expected_command:
                continue
            if status not in allowed:
                raise FlashError(f"target error 0x{status:02x}, command=0x{command:02x}, detail={detail}")
            if expected_sequence is not None:
                if sequence < expected_sequence:
                    continue  # Delayed/duplicate ACK from an earlier data packet.
                if sequence > expected_sequence:
                    raise FlashError(f"expected ACK {expected_sequence}, received future ACK {sequence}")
            return status, command, sequence, detail
        raise FlashError("timed out waiting for target")

    def command(self, opcode: int, value: int | None = None, allowed=(ACK, READY, COMPLETE)):
        payload = bytes([opcode]) if value is None else struct.pack("<BI", opcode, value)
        self._send(COMMAND_BASE + self.node, payload)
        return self._response(opcode, allowed)

    def flash(self, image: bytes, progress=lambda done, total: None) -> None:
        crc = zlib.crc32(image) & 0xFFFFFFFF
        def setup_request(opcode, value=None, allowed=(ACK,), label="setup"):
            print(f"Setup: {label}", flush=True)
            return self.command(opcode, value, allowed)
        if self.packet_crc:
            for attempt in range(5):
                try:
                    info = setup_request(INFO, allowed=(READY,), label="checking bootloader v2")
                    break
                except (FlashError, can.CanError) as error:
                    if attempt == 4: raise FlashError(f"Bootloader INFO failed: {error}") from error
                    print(f"Setup reply lost: {error}; retrying INFO", flush=True)
                    time.sleep(0.25)
            if info[3] >> 24 != 2:
                raise FlashError("Packet-CRC bootloader v2 is not installed; no erase or image data sent")
        for attempt in range(5 if self.packet_crc else 1):
            try:
                begin_reply = setup_request(BEGIN, len(image), label="erasing application area")
                if begin_reply[3] != len(image):
                    raise FlashError(f"image-size echo mismatch: sent {len(image)}, board reports {begin_reply[3]}")
                crc_reply = setup_request(SET_CRC, crc, label="setting expected image CRC")
            except (FlashError, can.CanError) as error:
                if attempt == (4 if self.packet_crc else 0):
                    raise FlashError(f"Upload setup failed before image data: {error}") from error
                # No image data has been sent: BEGIN can safely restart the setup.
                print(f"Setup failed: {error}; restarting setup before any image data", flush=True)
                time.sleep(0.5)
                continue
            if crc_reply[3] != crc:
                raise FlashError(f"CRC setup mismatch: sent 0x{crc:08X}, board reports 0x{crc_reply[3]:08X}; upload stopped before image data")
            break
        chunk_size = 4 if self.packet_crc else 6
        for sequence, offset in enumerate(range(0, len(image), chunk_size)):
            payload = struct.pack("<H", sequence) + image[offset:offset + chunk_size]
            if self.packet_crc:
                payload += struct.pack("<H", binascii.crc_hqx(payload, 0xFFFF))
            for attempt in range(self.retries + 1):
                try:
                    self._send(DATA_BASE + self.node, payload)
                    _, _, ack_sequence, written = self._response(0, (ACK,), expected_sequence=sequence)
                    if written != min(offset + chunk_size, len(image)):
                        raise FlashError(f"incorrect written-byte count {written}")
                    break
                except (FlashError, can.CanError) as error:
                    if attempt == self.retries:
                        raise FlashError(f"packet {sequence} failed after {self.retries + 1} attempts: {error}") from error
                    pause = min(2.0, 0.2 * (2 ** min(attempt, 4)))
                    print(f"\nPacket {sequence}: {error}; retrying in {pause:.1f}s", flush=True)
                    time.sleep(pause)
            if ack_sequence != sequence:
                raise FlashError(f"expected ACK {sequence}, received {ack_sequence}")
            progress(written, len(image))
        end_reply = self.command(END, allowed=(COMPLETE,))
        if end_reply[3] != crc:
            raise FlashError(f"CRC completion echo mismatch: expected 0x{crc:08X}, received 0x{end_reply[3]:08X}")

    def enter_bootloader(self) -> None:
        """Request an application reset, then discard any stale bootloader ACK."""
        self._send(COMMAND_BASE + self.node, bytes([RESET]))
        time.sleep(0.2)
        drain_deadline = time.monotonic() + 0.05
        for _ in range(64):
            if time.monotonic() >= drain_deadline or self.bus.recv(0) is None:
                break


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("image", type=pathlib.Path, nargs="?")
    parser.add_argument("--node", type=int, required=True)
    parser.add_argument("--interface", default="socketcan")
    parser.add_argument("--channel", default="can0")
    parser.add_argument("--bitrate", type=int, default=500000)
    parser.add_argument("--reset", action="store_true")
    parser.add_argument("--fast", action="store_true", help="Ewert: 10 ms packet spacing instead of 150 ms")
    args = parser.parse_args()
    if args.interface == "ewert":
        if args.bitrate != 500000:
            parser.error("Ewert transport currently supports 500000 bit/s")
        from ewert_can import EwertBus
        connection = closing(EwertBus(args.channel, node=args.node, fast=args.fast))
    else:
        if args.fast:
            parser.error("--fast is only used by --interface ewert")
        connection = can.Bus(interface=args.interface, channel=args.channel, bitrate=args.bitrate)
    with connection as bus:
        flasher = CanFlasher(bus, args.node, retries=20 if args.interface == "ewert" else 3)
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
    try:
        main()
    except KeyboardInterrupt:
        raise SystemExit("Stopped; an incomplete application is not bootable. Restart the upload.")
    except (FlashError, can.CanError, RuntimeError, ValueError) as error:
        raise SystemExit(str(error))
