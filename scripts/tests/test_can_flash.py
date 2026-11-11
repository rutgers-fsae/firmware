import binascii
import importlib.util
import pathlib
import struct
import sys
import types
import unittest
from unittest.mock import patch

fake_can = types.SimpleNamespace(Message=lambda **kw: types.SimpleNamespace(**kw), CanError=RuntimeError)
try:
    import can
except ImportError:
    sys.modules.setdefault('can', fake_can)
path = pathlib.Path(__file__).parents[1] / 'can_flash.py'
spec = importlib.util.spec_from_file_location('can_flash', path)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


def response(status, command, sequence=0, detail=0):
    return types.SimpleNamespace(arbitration_id=0x681,
        data=struct.pack('<BBHI', status, command, sequence, detail), dlc=8,
        is_extended_id=False, is_remote_frame=False, is_error_frame=False)


class Bus:
    def __init__(self, version=2, drop=None, corrupt=False, duplicate=False, wrong_crc=False):
        self.sent=[]; self.queue=[]; self.image=bytearray(); self.version=version
        self.drop=drop; self.dropped=False; self.corrupt=corrupt; self.rejected=False
        self.duplicate=duplicate; self.wrong_crc=wrong_crc; self.sequence=0

    def send(self, msg):
        self.sent.append(msg); data=bytes(msg.data)
        if msg.arbitration_id == 0x601:
            command=data[0]; value=int.from_bytes(data[1:], 'little'); status=module.ACK
            if command == module.INFO: status=module.READY; value=(self.version<<24)|48128
            if command == module.BEGIN: self.image.clear(); self.sequence=0
            if command == module.SET_CRC: self.crc=value; value ^= int(self.wrong_crc)
            if command == module.END:
                status=module.COMPLETE; value=self.crc
                import zlib
                if zlib.crc32(self.image) != self.crc: status=0x84
            reply=response(status, command, detail=value)
            if command == self.drop and not self.dropped: self.dropped=True; return
            self.queue.append(reply)
        else:
            sequence=int.from_bytes(data[:2], 'little')
            if binascii.crc_hqx(data[:-2], 0xffff) != int.from_bytes(data[-2:], 'little'):
                raise AssertionError('Host packet CRC is wrong')
            if self.corrupt and not self.rejected:
                self.rejected=True; self.queue.append(response(0x87,0,sequence,len(self.image))); return
            if sequence==self.sequence:
                self.image.extend(data[2:-2]); self.sequence+=1
            elif sequence != self.sequence-1: raise AssertionError('Unexpected packet order')
            if self.drop=='data' and sequence==1 and not self.dropped: self.dropped=True; return
            reply=response(module.ACK,0,sequence,len(self.image)); self.queue.append(reply)
            if self.duplicate: self.queue.append(reply)

    def recv(self, timeout):
        return self.queue.pop(0) if self.queue else None


class CanFlasherTests(unittest.TestCase):
    def flash(self,bus,image=b'0123456789'):
        with patch.object(module.time,'sleep'):
            module.CanFlasher(bus,1,timeout=0.002).flash(image)
        self.assertEqual(bytes(bus.image),image)

    def test_rejects_invalid_node(self):
        with self.assertRaises(ValueError): module.CanFlasher(Bus(),8)

    def test_four_byte_chunks_have_crc(self):
        bus=Bus(); self.flash(bus)
        packets=[bytes(m.data) for m in bus.sent if m.arbitration_id==0x621]
        self.assertEqual([p[2:-2] for p in packets],[b'0123',b'4567',b'89'])
        self.assertEqual([len(p) for p in packets],[8,8,6])

    def test_old_bootloader_rejected_before_erase(self):
        bus=Bus(version=1)
        with self.assertRaisesRegex(module.FlashError,'v2 is not installed'):
            module.CanFlasher(bus,1).flash(b'1234')
        self.assertEqual([m.data[0] for m in bus.sent],[module.INFO])

    def test_wrong_crc_echo_stops_before_data(self):
        bus=Bus(wrong_crc=True)
        with self.assertRaisesRegex(module.FlashError,'CRC setup mismatch'):
            module.CanFlasher(bus,1).flash(b'1234')
        self.assertFalse(any(m.arbitration_id==0x621 for m in bus.sent))

    def test_rejected_packet_is_retried(self): self.flash(Bus(corrupt=True))
    def test_lost_ack_and_duplicate_ack(self): self.flash(Bus(drop='data',duplicate=True))

    def test_lost_setup_replies(self):
        for command in (module.INFO,module.BEGIN,module.SET_CRC):
            with self.subTest(command=command): self.flash(Bus(drop=command))

    def test_ignores_wrong_and_malformed_responses(self):
        bus=Bus(); bus.queue=[response(0,2),types.SimpleNamespace(arbitration_id=0x681,data=b'short'),response(0,0,12,78),response(0,0,13,84)]
        result=module.CanFlasher(bus,1)._response(0,(0,),expected_sequence=13)
        self.assertEqual(result[2:],(13,84))

    def test_drain_is_bounded(self):
        class Busy(Bus):
            def __init__(self): super().__init__(); self.reads=0
            def recv(self, timeout): self.reads+=1; return response(0,5)
        bus=Busy()
        with patch.object(module.time,'sleep'): module.CanFlasher(bus,1).enter_bootloader()
        self.assertLessEqual(bus.reads,64)

if __name__=='__main__': unittest.main()
