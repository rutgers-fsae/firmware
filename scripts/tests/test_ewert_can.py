"""Drive the flasher through the real CANdapter transport against a pseudo-terminal adapter."""

import os
import pty
import random
import select
import sys
import threading
import tty
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from test_can_flash import (  # noqa: E402  (also installs a stand-in `can` if needed)
    Target,
    module,
)

try:
    import serial  # noqa: F401
except ImportError:
    raise unittest.SkipTest("pyserial is required for Ewert integration tests")
import ewert_can as transport  # noqa: E402

BACKGROUND = b"t0AA8040009000001C000\r"


class Adapter:
    """CANdapter stand-in: ACK/BELL per command, bootloader replies and background traffic as text."""

    def __init__(self, bell_rate=0.0):
        self.master, self.slave = pty.openpty()
        tty.setraw(self.slave)
        self.port = os.ttyname(self.slave)
        self.target = Target()
        self.bell_rate, self.random = bell_rate, random.Random(1)
        self.data_frames = self.bells = 0
        self.failure = None
        self.stop = threading.Event()
        self.thread = threading.Thread(target=self.loop, daemon=True)
        self.thread.start()

    def process(self, text):
        if text in (b"C", b"S6", b"A0", b"O"):
            os.write(self.master, b"\x06")
            return
        msg = transport.decode_frame(text)
        if msg.arbitration_id == 0x621:
            self.data_frames += 1
            # Unrelated bus traffic, plus one burst of line noise, interleaved with the transfer.
            os.write(self.master, BACKGROUND)
            if self.data_frames == 10:
                os.write(self.master, b"t0\x00B80000000000000000\x00" + BACKGROUND)
            if self.random.random() < self.bell_rate:
                self.bells += 1
                os.write(self.master, b"\x07")  # Rejected: never reaches the bus.
                return
        self.target.send(msg)
        out = b"".join(
            b"T6818" + bytes(reply.data).hex().upper().encode() + b"\r"
            for reply in self.target.queue
        )
        self.target.queue.clear()
        os.write(self.master, out + b"\x06")

    def loop(self):
        pending = bytearray()
        try:
            while not self.stop.is_set():
                if select.select([self.master], [], [], 0.001)[0]:
                    pending.extend(os.read(self.master, 8192))
                    while b"\r" in pending:
                        text, _, rest = pending.partition(b"\r")
                        pending = bytearray(rest)
                        if text:
                            self.process(bytes(text))
        except BaseException as error:
            self.failure = error

    def close(self):
        self.stop.set()
        self.thread.join(1)
        os.close(self.master)
        os.close(self.slave)


class EwertTests(unittest.TestCase):
    def test_upload_on_busy_bus_with_rejected_frames(self):
        image = bytes(range(256)) * 12 + bytes(range(7))
        adapter = Adapter(
            bell_rate=0.03
        )  # About twice the rate measured on the real adapter.
        bus = None
        try:
            bus = transport.EwertBus(adapter.port)
            flasher = module.CanFlasher(bus, 1)
            flasher.request_reset()
            flasher.flash(image)
            flasher.command(module.START)
            self.assertIsNone(adapter.failure)
            self.assertEqual(adapter.target.contents(), image)
            self.assertGreater(adapter.bells, 5)
            self.assertEqual(bus.adapter_errors, adapter.bells)
            self.assertGreaterEqual(bus.filtered_frames, adapter.data_frames)
            self.assertGreater(bus.bad_packets, 0)
        finally:
            if bus:
                bus.close()
            adapter.close()

    def test_bad_frames_fail_strict_validation(self):
        for raw in (b"T6818BAD", b"T6818FFFFFFFFFFFFFFFF00", b"TXYZ000", b"T6818\x00"):
            with self.assertRaises((ValueError, IndexError)):
                transport.decode_frame(raw)
        self.assertEqual(
            transport.decode_frame(b"T6818000001000C000000").arbitration_id, 0x681
        )


if __name__ == "__main__":
    unittest.main()
