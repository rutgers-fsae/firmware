import sys, os, pty, select, threading, time, struct, unittest, zlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'scripts'))
try:
 import serial
except ImportError:
 raise unittest.SkipTest('pyserial is required for Ewert integration tests')
import ewert_can as transport
import can_flash as module
from can_flash import FlashError

class Adapter:
    def __init__(self):
        self.master,self.slave=pty.openpty(); self.port=os.ttyname(self.slave)
        self.stop=threading.Event(); self.image=bytearray(); self.seq=0
        self.lost=False; self.rejected=False; self.validated=False; self.expected_crc=None
        self.background=0; self.failure=None
        self.thread=threading.Thread(target=self.loop,daemon=True); self.thread.start()
    def write(self,data): os.write(self.master,data)
    def response(self,cmd,status=0,seq=0,detail=0):
        data=struct.pack('<BBHI',status,cmd,seq,detail)
        return b'T6818'+data.hex().upper().encode()+b'\r'
    def process(self,text):
        if text in (b'C',b'S6',b'A0',b'O'): self.write(b'\x06'); return
        msg=transport.decode_frame(text)
        if msg.arbitration_id==0x601:
            cmd=msg.data[0]
            if cmd==2:
                self.image.clear(); self.seq=0; self.size=int.from_bytes(msg.data[1:],'little')
            if cmd==3: self.expected_crc=int.from_bytes(msg.data[1:],'little')
            if cmd==4:
                assert len(self.image)==self.size
                assert zlib.crc32(self.image)==self.expected_crc
                self.validated=True
            detail=self.size if cmd==2 else (self.expected_crc if cmd in (3,4) else 0)
            if cmd==1: detail=(2<<24)|48128
            self.write(self.response(cmd,2 if cmd==4 else 0,detail=detail)+b'\x06')
            if cmd==6: self.write(b'T701852465244454D4F41\r'*3)
        elif msg.arbitration_id==0x621:
            seq=int.from_bytes(msg.data[:2],'little')
            if seq==25 and not self.rejected:
                self.rejected=True; self.write(b'\x07'); return
            self.write(b'\x06')
            if seq==self.seq:
                self.image.extend(msg.data[2:]); self.seq+=1
            else: assert seq==self.seq-1
            if seq==13 and not self.lost:
                self.lost=True; return
            reply=self.response(0,seq=seq,detail=len(self.image))
            self.write(reply)
            if seq%7==0: self.write(reply)
    def loop(self):
        pending=bytearray(); last=time.monotonic()
        try:
            while not self.stop.is_set():
                now=time.monotonic()
                if now-last>=0.01:
                    self.write(b't0AA8040009000001C000\r'*18)
                    self.background+=18; last=now
                    if self.background==1800:
                        self.write(b't0\x00B80000000000000000\x00t0AA8040009000001C000\r')
                if select.select([self.master],[],[],0.001)[0]:
                    pending.extend(os.read(self.master,8192))
                    while b'\r' in pending:
                        text,_,rest=pending.partition(b'\r'); pending=bytearray(rest)
                        if text: self.process(text)
        except BaseException as error: self.failure=error
    def close(self):
        self.stop.set(); self.thread.join(1)
        os.close(self.master); os.close(self.slave)

class Test(unittest.TestCase):
    def test_upload_on_busy_serial_with_loss_duplicates_and_bell(self):
        adapter=Adapter(); bus=None
        try:
            bus=transport.EwertBus(adapter.port)
            bus.usb_character_gap=0
            bus.min_tx_interval=0.01  # Keep stress test fast; real adapter uses 150 ms.
            flasher=module.CanFlasher(bus,1,timeout=0.1,retries=5,packet_crc=False)
            flasher.enter_bootloader()
            image=bytes(range(256))*3+bytes(range(7))
            flasher.flash(image)
            flasher.command(module.START)
            self.assertTrue(adapter.validated)
            self.assertEqual(adapter.image,image)
            self.assertTrue(adapter.validated and adapter.lost and adapter.rejected)
            self.assertGreater(bus.filtered_frames,1000)
            self.assertGreater(bus.bad_packets,0)
            self.assertIsNone(adapter.failure)
            self.assertEqual(bus.adapter_errors,1)
        finally:
            if bus: bus.close()
            adapter.close()
    def test_stale_ack_times_out(self):
        class Stale:
            def recv(self,timeout): return transport.can.Message(arbitration_id=0x681,is_extended_id=False,data=struct.pack('<BBHI',0,0,12,78))
        with self.assertRaisesRegex(FlashError,'timed out'):
            module.CanFlasher(Stale(),1,timeout=0.01)._response(0,(0,),expected_sequence=13)
    def test_bad_frames_fail_strict_validation(self):
        for raw in (b'T6818BAD',b'T6818FFFFFFFFFFFFFFFF00',b'TXYZ000',b'T6818\x00'):
            with self.assertRaises((ValueError,IndexError)): transport.decode_frame(raw)
        self.assertEqual(transport.decode_frame(b'T6818000001000C000000').arbitration_id,0x681)

if __name__=='__main__': unittest.main()
