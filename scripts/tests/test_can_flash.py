"""Host-side flasher tests against a pure-Python stand-in for the v4 bootloader."""

import binascii
import pathlib
import struct
import sys
import types
import unittest
import zlib
from unittest.mock import patch

try:
    import can  # noqa: F401
except ImportError:

    class FakeMessage:
        def __init__(self, arbitration_id=0, data=b"", is_extended_id=False, **_):
            self.arbitration_id, self.data, self.is_extended_id = (
                arbitration_id,
                bytes(data),
                is_extended_id,
            )
            self.is_remote_frame = self.is_error_frame = self.is_fd = False
            self.dlc = len(self.data)

    fake_can = types.ModuleType("can")
    fake_can.__dict__.update(
        Message=FakeMessage, CanError=RuntimeError, CanOperationError=RuntimeError
    )
    sys.modules["can"] = fake_can

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
import can_flash as module  # noqa: E402

MAX_IMAGE = 48128


class Target:
    """Bootloader stand-in. Subclasses may route data frames through the real C receiver."""

    def __init__(
        self, version=module.PROTOCOL_VERSION, drop_reply_to=None, wrong_crc=False
    ):
        self.version, self.drop_reply_to, self.wrong_crc = (
            version,
            drop_reply_to,
            wrong_crc,
        )
        self.sent, self.queue, self.dropped = [], [], False
        self.size = self.crc = 0
        self.image = bytearray()
        self.frames = 0

    # Receiver state; overridden by the C-backed target.
    def begin(self, size):
        self.image, self.frames, self.window = bytearray(), 0, {}

    def accepted_frames(self):
        return self.frames

    def window_mask(self):
        return sum(1 << slot for slot in self.window)

    def contents(self):
        return bytes(self.image)

    def on_data(self, data):
        if binascii.crc_hqx(data[:-2], 0xFFFF) != int.from_bytes(data[-2:], "little"):
            raise AssertionError("host sent a wrong packet CRC")
        remaining = self.size - len(self.image)
        frames = min(module.WINDOW, -(-remaining // module.CHUNK))
        slot = (data[0] - self.frames) & 0xFF
        if slot >= frames:
            return
        self.window[slot] = data[1:-2]
        if len(self.window) == frames:
            self.image.extend(b"".join(self.window[index] for index in range(frames)))
            self.frames += frames
            self.window = {}

    def reply(self, status, command, sequence=0, detail=0):
        self.queue.append(
            sys.modules["can"].Message(
                arbitration_id=0x681,
                is_extended_id=False,
                data=struct.pack("<BBHI", status, command, sequence, detail),
            )
        )

    def send(self, msg):
        self.sent.append(msg)
        data = bytes(msg.data)
        if msg.arbitration_id != 0x601:
            self.on_data(data)
            return
        command, value = data[0], int.from_bytes(data[1:5], "little")
        status, sequence = module.ACK, 0
        if command == module.INFO:
            status, sequence, value = (
                module.READY,
                self.accepted_frames(),
                (self.version << 24) | MAX_IMAGE,
            )
        elif command == module.BEGIN:
            self.size = value
            self.begin(value)
        elif command == module.SET_CRC:
            self.crc = value
            value ^= int(self.wrong_crc)
        elif command == module.WINDOW_STATUS:
            sequence, value = self.accepted_frames(), self.window_mask()
        elif command == module.END:
            status = (
                module.COMPLETE if zlib.crc32(self.contents()) == self.crc else 0x84
            )
            value = self.crc
        if command == self.drop_reply_to and not self.dropped:
            self.dropped = True
            return
        self.reply(status, command, sequence, value)

    def recv(self, timeout):
        return self.queue.pop(0) if self.queue else None


def flasher(bus):
    return module.CanFlasher(bus, 1, timeout=0.01)


class CanFlasherTests(unittest.TestCase):
    def test_rejects_invalid_node(self):
        with self.assertRaises(ValueError):
            module.CanFlasher(Target(), 8)

    def test_frames_carry_sequence_byte_five_bytes_and_crc(self):
        bus = Target()
        flasher(bus).flash(b"0123456789AB", wait=0.05)
        self.assertEqual(bus.contents(), b"0123456789AB")
        packets = [bytes(m.data) for m in bus.sent if m.arbitration_id == 0x621]
        self.assertEqual(
            [p[:-2] for p in packets], [b"\x0001234", b"\x0156789", b"\x02AB"]
        )
        for packet in packets:
            self.assertEqual(
                binascii.crc_hqx(packet[:-2], 0xFFFF),
                int.from_bytes(packet[-2:], "little"),
            )

    def test_multi_window_image(self):
        image = bytes(range(256)) * 5 + b"tail"
        bus = Target()
        flasher(bus).flash(image, wait=0.05)
        self.assertEqual(bus.contents(), image)

    def test_other_protocol_version_rejected_before_erase(self):
        bus = Target(version=2)
        with self.assertRaisesRegex(module.FlashError, "protocol v2"):
            flasher(bus).flash(b"1234", wait=0.05)
        self.assertEqual({m.data[0] for m in bus.sent}, {module.INFO})

    def test_oversized_image_rejected_before_erase(self):
        bus = Target()
        with self.assertRaisesRegex(module.FlashError, "Nothing was erased"):
            flasher(bus).flash(bytes(MAX_IMAGE + 1), wait=0.05)
        self.assertEqual({m.data[0] for m in bus.sent}, {module.INFO})

    def test_silent_board_reports_power_cycle_hint(self):
        class Silent(Target):
            def send(self, msg):
                self.sent.append(msg)

        with self.assertRaisesRegex(module.FlashError, "Power-cycle"):
            flasher(Silent()).flash(b"1234", wait=0.05)

    def test_wrong_crc_echo_stops_before_data(self):
        bus = Target(wrong_crc=True)
        with self.assertRaisesRegex(module.FlashError, "CRC setup mismatch"):
            flasher(bus).flash(b"1234", wait=0.05)
        self.assertFalse(any(m.arbitration_id == 0x621 for m in bus.sent))

    def test_lost_command_replies_are_resent(self):
        for command in (module.INFO, module.BEGIN, module.SET_CRC, module.END):
            with self.subTest(command=command):
                bus = Target(drop_reply_to=command)
                with patch.object(module, "ERASE_TIMEOUT", 0.01):
                    flasher(bus).flash(b"0123456789", wait=1.0)
                self.assertTrue(bus.dropped)
                self.assertEqual(bus.contents(), b"0123456789")

    def test_begin_waits_for_the_erase(self):
        bus = Target()
        target = flasher(bus)
        timeouts = {}
        original = target._response

        def record(expected_command, allowed, timeout=None):
            timeouts[expected_command] = timeout
            return original(expected_command, allowed, timeout)

        target._response = record
        target.flash(b"1234", wait=0.05)
        self.assertGreaterEqual(timeouts[module.BEGIN], 5.0)

    def test_target_error_is_not_retried(self):
        class Refuses(Target):
            def send(self, msg):
                if msg.arbitration_id == 0x601 and msg.data[0] == module.BEGIN:
                    self.sent.append(msg)
                    self.reply(0x82, module.BEGIN)
                else:
                    super().send(msg)

        bus = Refuses()
        with self.assertRaisesRegex(module.TargetError, "0x82"):
            flasher(bus).flash(b"1234", wait=0.05)
        self.assertEqual(sum(m.data[0] == module.BEGIN for m in bus.sent), 1)

    def test_window_status_skips_stale_and_damaged_replies(self):
        bus = Target()
        bus.queue = [types.SimpleNamespace(arbitration_id=0x681, data=b"short")]
        bus.reply(module.ACK, module.WINDOW_STATUS, 0, 0b1)  # about an earlier window
        bus.reply(
            module.ACK, module.WINDOW_STATUS, 32, 1 << 5
        )  # bit beyond a 4-frame window
        bus.reply(module.ACK, module.WINDOW_STATUS, 32, 0b0101)
        self.assertEqual(flasher(bus)._window_status(160, 180, 4, 180), 0b0101)

    def test_window_status_reports_a_committed_window(self):
        bus = Target()
        bus.reply(module.ACK, module.WINDOW_STATUS, 36, 0)
        self.assertIsNone(flasher(bus)._window_status(160, 180, 4, 180))

    def test_flash_error_during_data_is_fatal(self):
        bus = Target()
        bus.reply(0x85, 0, 0, 16)
        with self.assertRaisesRegex(module.TargetError, "0x85"):
            flasher(bus)._window_status(0, 160, 32, 1000)


if __name__ == "__main__":
    unittest.main()
