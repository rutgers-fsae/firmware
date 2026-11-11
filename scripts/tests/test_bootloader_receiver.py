from pathlib import Path
import ctypes as c, subprocess, sys, struct, zlib, binascii, unittest
REPO=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(REPO/'scripts'))
import tempfile
TEMP=tempfile.TemporaryDirectory()
BUILD=Path(TEMP.name)
import types
try:
 import can
except ImportError:
 sys.modules.setdefault('can',types.SimpleNamespace(Message=lambda **kw: types.SimpleNamespace(**kw),CanError=RuntimeError,CanOperationError=RuntimeError))
import can_flash as fl
src=(REPO/'stm32f103-can-bootloader/src/main.c').read_text();a=src.index('static uint16_t packet_crc16(');b=src.index('\nstatic bool write_bytes(',a)
header=(REPO/'stm32f103-can-bootloader/include/bootloader_protocol.h')
mstart=src.index('static bool metadata_write(void) {'); mend=src.index('\nstatic bool application_is_valid',mstart)
harness='''#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "HEADER"
enum {RX_IDLE,RX_WAITING_CRC,RX_DATA,RX_COMPLETE};
static uint32_t rx_state,image_size,bytes_written,expected_crc;
#define BL_METADATA_MAGIC 0x52465242UL
#define BL_METADATA_ADDRESS 0x0800FC00UL
#define FLASH_TYPEPROGRAM_HALFWORD 1
#define HAL_OK 0
static uint16_t metadata_words[8];
static void HAL_FLASH_Unlock(void) {}
static void HAL_FLASH_Lock(void) {}
static int HAL_FLASH_Program(int type,uint32_t address,uint16_t word) {(void)type;metadata_words[(address-BL_METADATA_ADDRESS)/2]=word;return 0;}
static uint16_t expected_sequence;
static unsigned char image[48128];
static uint8_t result_status; static uint16_t result_sequence; static uint32_t result_detail;
static bool write_bytes(const uint8_t *data,uint8_t n) {memcpy(image+bytes_written,data,n);return true;}
static uint32_t HAL_FLASH_GetError(void) {return 0;}
static void send_status(bl_status_t s,uint8_t cmd,uint16_t seq,uint32_t detail) {(void)cmd;result_status=s;result_sequence=seq;result_detail=detail;}
'''.replace('HEADER',str(header))+src[a:b]+src[mstart:mend]+'''
void test_metadata(uint16_t *out) {image_size=3968;expected_crc=0xE5900042;metadata_write();for(int i=0;i<8;++i)out[i]=metadata_words[i];}
void reset_receiver(uint32_t size) {memset(image,255,sizeof(image));image_size=size;bytes_written=0;expected_sequence=0;rx_state=RX_DATA;}
void receive(const uint8_t *data,uint8_t n) {process_data(data,n);}
uint8_t status(void) {return result_status;}
uint16_t sequence(void) {return result_sequence;}
uint32_t written(void) {return bytes_written;}
uint32_t detail(void) {return result_detail;}
void copy_image(uint8_t *out,uint32_t n) {memcpy(out,image,n);}
uint16_t check_crc(const uint8_t *in,uint8_t n) {return packet_crc16(in,n);}
'''
(BUILD/'packet_receiver_test.c').write_text(harness)
subprocess.run(['cc','-shared','-fPIC','-O2','-Wall','-Wextra','-Werror',str(BUILD/'packet_receiver_test.c'),'-o',str(BUILD/'receiver.so')],check=True)
lib=c.CDLL(str((BUILD/'receiver.so').resolve()))
lib.receive.argtypes=[c.c_char_p,c.c_uint8];lib.check_crc.argtypes=[c.c_char_p,c.c_uint8];lib.check_crc.restype=c.c_uint16
lib.written.restype=c.c_uint32;lib.detail.restype=c.c_uint32

def packet(seq,data):
 p=struct.pack('<H',seq)+data
 return p+struct.pack('<H',binascii.crc_hqx(p,0xffff))
class Bus:
 def __init__(self):self.queue=[];self.corrupted=set();self.lost=False;self.crc=0;self.size=0;self.rejected=0;self.sent=[]
 def reply(self,cmd,status=0,seq=0,detail=0):
  self.queue.append(fl.can.Message(arbitration_id=0x681,is_extended_id=False,data=struct.pack('<BBHI',status,cmd,seq,detail)))
 def send(self,msg):
  self.sent.append(msg);p=bytes(msg.data)
  if msg.arbitration_id==0x601:
   cmd=p[0];value=int.from_bytes(p[1:],'little')
   if cmd==fl.INFO:self.reply(cmd,fl.READY,detail=(2<<24)|48128)
   elif cmd==fl.BEGIN:self.size=value;lib.reset_receiver(value);self.reply(cmd,detail=value)
   elif cmd==fl.SET_CRC:self.crc=value;self.reply(cmd,detail=value)
   elif cmd==fl.END:
    out=c.create_string_buffer(self.size);lib.copy_image(out,self.size)
    self.reply(cmd,fl.COMPLETE if zlib.crc32(out.raw)==self.crc else 0x84,detail=self.crc)
  else:
   seq=int.from_bytes(p[:2],'little')
   if seq%7==0 and seq not in self.corrupted:
    self.corrupted.add(seq);broken=bytearray(p);broken[2]^=0x40;p=bytes(broken)
   before=lib.written();lib.receive(p,len(p));status=lib.status()
   if status==0x87:
    self.rejected+=1;assert lib.written()==before
   if seq==13 and not self.lost and status==0:self.lost=True;return
   self.reply(0,status,lib.sequence(),lib.detail())
 def recv(self,timeout):return self.queue.pop(0) if self.queue else None
class Tests(unittest.TestCase):
 def test_metadata_serialization_with_optimization(self):
  words=(c.c_uint16*8)();lib.test_metadata(words)
  self.assertEqual(bytes(words),struct.pack('<IIII',0x52465242,3968,0xE5900042,0xffffffff))
 def test_every_single_bit_corruption_rejected_before_write(self):
  for n in range(1,5):
   good=packet(0,bytes(range(n)));self.assertEqual(lib.check_crc(good[:-2],len(good)-2),int.from_bytes(good[-2:],'little'))
   for bit in range(len(good)*8):
    lib.reset_receiver(n);bad=bytearray(good);bad[bit//8]^=1<<(bit%8);lib.receive(bytes(bad),len(bad))
    self.assertEqual(lib.status(),0x87);self.assertEqual(lib.written(),0)
 def test_full_upload_corruption_and_lost_ack(self):
  bus=Bus();image=bytes(range(256))*3+bytes(range(7))
  
  from unittest.mock import patch
  with patch.object(fl.time,'sleep'):
   fl.CanFlasher(bus,1,timeout=.002,retries=3,packet_crc=True).flash(image)
  out=c.create_string_buffer(len(image));lib.copy_image(out,len(image));self.assertEqual(out.raw,image)
  self.assertGreater(bus.rejected,20);self.assertTrue(bus.lost)
 def test_lost_setup_reply_restarts_safely(self):
  for dropped in (fl.INFO,fl.BEGIN,fl.SET_CRC):
   class DropSetup(Bus):
    def __init__(self):super().__init__();self.dropped=False
    def send(self,msg):
     super().send(msg)
     if msg.arbitration_id==0x601 and msg.data[0]==dropped and not self.dropped:
      self.queue.clear();self.dropped=True
   bus=DropSetup();image=b'12345678'
   
  from unittest.mock import patch
  with patch.object(fl.time,'sleep'):
   fl.CanFlasher(bus,1,timeout=.002,retries=3,packet_crc=True).flash(image)
   out=c.create_string_buffer(len(image));lib.copy_image(out,len(image))
   self.assertEqual(out.raw,image);self.assertTrue(bus.dropped)
 def test_v1_guard_no_erase(self):
  bus=Bus();original=bus.send
  def old(msg):
   if msg.data[0]==fl.INFO:bus.reply(fl.INFO,fl.READY,detail=48128)
   else:original(msg)
  bus.send=old
  with self.assertRaisesRegex(fl.FlashError,'v2 is not installed'):fl.CanFlasher(bus,1,packet_crc=True).flash(b'1234')
  self.assertFalse(bus.sent)
if __name__=='__main__': unittest.main()
