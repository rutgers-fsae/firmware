#!/usr/bin/env python3
"""Flash an RFR STM32 bootloader target over Classical CAN 2.0 (protocol v4)."""

import argparse
import binascii
import pathlib
import struct
import time
import zlib
from contextlib import closing

try:
    import can
except ImportError as exc:
    raise SystemExit(
        "Install python-can first: python3 -m pip install python-can"
    ) from exc

DISCOVERY_ID = 0x5E0
COMMAND_BASE = 0x600
DATA_BASE = 0x620
RESPONSE_BASE = 0x680
ACK, READY, COMPLETE = 0x00, 0x01, 0x02
INFO, BEGIN, SET_CRC, END, RESET, START, ABORT, WINDOW_STATUS = range(1, 9)

# These must match bootloader_protocol.h.
PROTOCOL_VERSION = 4
CHUNK = 5  # image bytes per data frame
WINDOW = 32  # data frames per selective-repeat window
SPAN = CHUNK * WINDOW

ERASE_TIMEOUT = 5.0  # BEGIN erases up to 48 pages at 20-40 ms each before replying.
WINDOW_TIMEOUT = 0.1  # A window status reply normally arrives within ~20 ms.


class FlashError(RuntimeError):
    pass


class TargetError(FlashError):
    """The bootloader answered with an error status; resending the same request will not help."""


class CanFlasher:
    def __init__(self, bus, node: int, timeout: float = 1.0, retries: int = 8):
        if node not in range(1, 8):
            raise ValueError("node must be between 1 and 7")
        self.bus, self.node, self.timeout, self.retries = bus, node, timeout, retries

    def _send(self, arbitration_id: int, payload: bytes) -> None:
        self.bus.send(
            can.Message(
                arbitration_id=arbitration_id, data=payload, is_extended_id=False
            )
        )

    def _parse(self, msg):
        """Return (status, command, sequence, detail) for a well-formed reply from this node."""
        if not msg or msg.arbitration_id != RESPONSE_BASE + self.node:
            return None
        if (
            getattr(msg, "is_extended_id", False)
            or getattr(msg, "is_remote_frame", False)
            or getattr(msg, "is_error_frame", False)
        ):
            return None
        payload = bytes(msg.data)
        if getattr(msg, "dlc", len(payload)) != 8 or len(payload) != 8:
            return None
        return struct.unpack("<BBHI", payload)

    def _response(self, expected_command: int, allowed, timeout: float | None = None):
        deadline = time.monotonic() + (self.timeout if timeout is None else timeout)
        while time.monotonic() < deadline:
            reply = self._parse(self.bus.recv(deadline - time.monotonic()))
            if reply is None or reply[1] != expected_command:
                continue
            status, command, _, detail = reply
            if status not in allowed:
                raise TargetError(
                    f"target error 0x{status:02x}, command=0x{command:02x}, detail={detail}"
                )
            return reply
        raise FlashError("timed out waiting for target")

    def command(
        self,
        opcode: int,
        value: int | None = None,
        allowed=(ACK, READY, COMPLETE),
        timeout: float | None = None,
        attempts: int = 1,
    ):
        """Send a command; resend it after a lost reply or adapter rejection, never after a target error."""
        payload = (
            bytes([opcode]) if value is None else struct.pack("<BI", opcode, value)
        )
        for attempt in range(attempts):
            try:
                self._send(COMMAND_BASE + self.node, payload)
                return self._response(opcode, allowed, timeout)
            except TargetError:
                raise
            except (FlashError, can.CanError) as error:
                if attempt == attempts - 1:
                    raise FlashError(
                        f"command 0x{opcode:02x} failed: {error}"
                    ) from error

    def request_reset(self) -> None:
        """Ask a running application (or an idle bootloader) to reset into the bootloader."""
        try:
            self._send(COMMAND_BASE + self.node, bytes([RESET]))
        except can.CanError:
            pass  # wait_for_bootloader() reports a board that never answers.

    def wait_for_bootloader(self, wait: float = 10.0) -> int:
        """Poll INFO until the bootloader answers; each INFO also extends its update window.

        Returns the maximum image size. Raises before anything is erased if the board never
        answers or runs a different protocol version.
        """
        deadline = time.monotonic() + wait
        while True:
            try:
                info = self.command(INFO, allowed=(READY,), timeout=0.1)
                break
            except TargetError:
                raise
            except FlashError as error:
                if time.monotonic() >= deadline:
                    raise FlashError(
                        f"No bootloader reply from node {self.node} ({error}). Power-cycle the board "
                        "while this command is running; nothing was erased."
                    ) from error
                time.sleep(0.02)
        version = info[3] >> 24
        if version != PROTOCOL_VERSION:
            raise FlashError(
                f"Bootloader speaks protocol v{version}; this uploader needs v{PROTOCOL_VERSION}. "
                "Update the bootloader over ST-LINK. No erase or image data sent"
            )
        return info[3] & 0xFFFFFF

    def _send_burst(self, messages) -> None:
        try:
            if hasattr(self.bus, "send_many"):
                self.bus.send_many(messages)
            else:
                # ponytail: unpaced loop; add pacing if a socketcan transmit queue overflows.
                for message in messages:
                    self.bus.send(message)
        except can.CanError:
            pass  # Whatever did not get out shows up as missing in the window status.

    def _window_status(self, offset: int, end: int, frames: int, total: int):
        """Wait for a status reply about the open window.

        Returns the mask of frames the bootloader holds, or None once the window is in
        flash. Raises FlashError("no window status") on silence. Replies that fit neither
        case are late or were damaged by the adapter and are skipped.
        """
        deadline = time.monotonic() + min(self.timeout, WINDOW_TIMEOUT)
        while True:
            remaining = deadline - time.monotonic()
            reply = self._parse(self.bus.recv(max(0.0, remaining)))
            if reply is None:
                if remaining <= 0:
                    raise FlashError("no window status")
                continue
            status, command, committed_frames, mask = reply
            if command == 0 and status >= 0x80:
                raise TargetError(
                    f"target error 0x{status:02x} during image data, detail={mask}"
                )
            if command != WINDOW_STATUS or status != ACK:
                continue
            committed = min(committed_frames * CHUNK, total)
            if committed == end and mask == 0:
                return None
            if committed == offset and mask >> frames == 0:
                return mask

    def flash(
        self, image: bytes, progress=lambda done, total: None, wait: float = 10.0
    ) -> None:
        total = len(image)
        crc = zlib.crc32(image) & 0xFFFFFFFF
        print("Setup: waiting for bootloader", flush=True)
        max_size = self.wait_for_bootloader(wait)
        if not 0 < total <= max_size:
            raise FlashError(
                f"image is {total} bytes; the bootloader accepts 1..{max_size}. Nothing was erased"
            )
        print("Setup: erasing application area", flush=True)
        begin_reply = self.command(
            BEGIN, total, allowed=(ACK,), timeout=ERASE_TIMEOUT, attempts=3
        )
        if begin_reply[3] != total:
            raise FlashError(
                f"image-size echo mismatch: sent {total}, board reports {begin_reply[3]}"
            )
        crc_reply = self.command(SET_CRC, crc, allowed=(ACK,), attempts=3)
        if crc_reply[3] != crc:
            raise FlashError(
                f"CRC setup mismatch: sent 0x{crc:08X}, board reports 0x{crc_reply[3]:08X}; "
                "upload stopped before image data"
            )

        def data_frame(offset: int):
            payload = bytes([(offset // CHUNK) & 0xFF]) + image[offset : offset + CHUNK]
            payload += struct.pack("<H", binascii.crc_hqx(payload, 0xFFFF))
            return can.Message(
                arbitration_id=DATA_BASE + self.node, data=payload, is_extended_id=False
            )

        poll = can.Message(
            arbitration_id=COMMAND_BASE + self.node,
            data=bytes([WINDOW_STATUS]),
            is_extended_id=False,
        )
        # Selective repeat: send a window, ask which frames arrived, resend only the rest.
        # The bootloader programs the window once it holds every frame.
        for offset in range(0, total, SPAN):
            end = min(total, offset + SPAN)
            frames = -(-(end - offset) // CHUNK)
            missing = resend = list(range(frames))
            stalls = 0
            while True:
                for _ in range(256):  # Discard replies left over from earlier rounds.
                    if self.bus.recv(0) is None:
                        break
                self._send_burst(
                    [data_frame(offset + slot * CHUNK) for slot in resend] + [poll]
                )
                try:
                    mask = self._window_status(offset, end, frames, total)
                except TargetError:
                    raise
                except FlashError:
                    resend, still_missing = (
                        [],
                        missing,
                    )  # Lost poll or reply: ask again.
                else:
                    if mask is None:
                        break
                    still_missing = [s for s in range(frames) if not mask >> s & 1]
                    resend = still_missing
                if len(still_missing) < len(missing):
                    stalls = 0
                else:
                    stalls += 1
                    if stalls > self.retries:
                        raise FlashError(
                            f"no progress at byte {offset} after {stalls} attempts"
                        )
                missing = still_missing
            progress(end, total)

        end_reply = self.command(END, allowed=(COMPLETE,), attempts=3)
        if end_reply[3] != crc:
            raise FlashError(
                f"CRC completion echo mismatch: expected 0x{crc:08X}, received 0x{end_reply[3]:08X}"
            )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("image", type=pathlib.Path, nargs="?")
    parser.add_argument("--node", type=int, required=True)
    parser.add_argument("--interface", default="socketcan")
    parser.add_argument("--channel", default="can0")
    parser.add_argument("--bitrate", type=int, default=500000)
    parser.add_argument("--reset", action="store_true")
    parser.add_argument(
        "--wait",
        type=float,
        default=10.0,
        help="seconds to keep looking for the bootloader (time to power-cycle the board)",
    )
    args = parser.parse_args()
    if args.interface == "ewert":
        if args.bitrate != 500000:
            parser.error("Ewert transport currently supports 500000 bit/s")
        from ewert_can import EwertBus

        connection = closing(EwertBus(args.channel, node=args.node))
    else:
        connection = can.Bus(
            interface=args.interface, channel=args.channel, bitrate=args.bitrate
        )
    with connection as bus:
        flasher = CanFlasher(bus, args.node)
        flasher.request_reset()
        if args.reset:
            print(f"Reset command sent to node {args.node}")
            return
        if not args.image:
            parser.error("image is required unless --reset is used")
        image = args.image.read_bytes()
        started = time.monotonic()
        flasher.flash(
            image,
            lambda done, total: print(f"\r{done}/{total} bytes", end="", flush=True),
            args.wait,
        )
        print(
            f"\nFlash verified in {time.monotonic() - started:.1f} s; starting application"
        )
        try:
            flasher.command(START)
        except FlashError as error:
            print(
                f"No START acknowledgement ({error}); the bootloader starts the verified "
                "application by itself within 1.5 s."
            )


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        raise SystemExit(
            "Stopped; an incomplete application is not bootable. Restart the upload."
        )
    except (FlashError, can.CanError, RuntimeError, ValueError) as error:
        raise SystemExit(str(error))
