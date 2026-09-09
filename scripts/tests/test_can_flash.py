import importlib.util
import pathlib
import struct
import sys
import types
import unittest

fake_can = types.SimpleNamespace(Message=lambda **kwargs: types.SimpleNamespace(**kwargs))
sys.modules.setdefault("can", fake_can)
path = pathlib.Path(__file__).parents[1] / "can_flash.py"
spec = importlib.util.spec_from_file_location("can_flash", path)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class FakeBus:
    def __init__(self): self.sent = []; self.responses = []
    def send(self, message): self.sent.append(message)
    def recv(self, timeout): return self.responses.pop(0) if self.responses else None


class CanFlasherTests(unittest.TestCase):
    def test_rejects_node_outside_supported_range(self):
        with self.assertRaises(ValueError): module.CanFlasher(FakeBus(), 8)

    def test_splits_image_into_six_byte_frames(self):
        bus = FakeBus()
        node = 3
        for command in (module.BEGIN, module.SET_CRC):
            bus.responses.append(types.SimpleNamespace(
                arbitration_id=module.RESPONSE_BASE + node,
                data=struct.pack("<BBHI", module.ACK, command, 0, 0)))
        bus.responses.extend([
            types.SimpleNamespace(arbitration_id=module.RESPONSE_BASE + node,
                                  data=struct.pack("<BBHI", module.ACK, 0, seq, written))
            for seq, written in ((0, 6), (1, 10))
        ])
        bus.responses.append(types.SimpleNamespace(
            arbitration_id=module.RESPONSE_BASE + node,
            data=struct.pack("<BBHI", module.COMPLETE, module.END, 2, 0)))
        module.CanFlasher(bus, node).flash(b"0123456789")
        data_frames = [m for m in bus.sent if m.arbitration_id == module.DATA_BASE + node]
        self.assertEqual(bytes(data_frames[0].data), b"\x00\x00012345")
        self.assertEqual(bytes(data_frames[1].data), b"\x01\x006789")

    def test_target_error_is_reported(self):
        bus = FakeBus()
        bus.responses.append(types.SimpleNamespace(
            arbitration_id=module.RESPONSE_BASE + 2,
            data=struct.pack("<BBHI", 0x84, module.END, 4, 24)))
        with self.assertRaisesRegex(module.FlashError, "target error 0x84"):
            module.CanFlasher(bus, 2).command(module.END)


if __name__ == "__main__": unittest.main()
