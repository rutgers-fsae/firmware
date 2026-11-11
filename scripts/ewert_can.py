"""Ewert CANdapter native ASCII transport; requires python-can and pyserial.

Read continuously while writing; retain only the selected bootloader node's replies.
This uses T for standard frames and X for extended frames, unlike generic SLCAN.
"""
from collections import deque
import threading
import time
import can
try:
    import serial
except ImportError as error:
    raise RuntimeError("Install Ewert support: python3 -m pip install pyserial") from error

def decode_frame(raw):
    text = raw.decode('ascii')
    if not text or text[0] not in 'TtXx':
        return None
    extended = text[0] in 'Xx'
    width = 8 if extended else 3
    dlc = int(text[width + 1], 16)
    if dlc > 8 or len(text) not in (width+2+dlc*2, width+6+dlc*2):
        raise ValueError('Malformed CANdapter frame: '+repr(text))
    return can.Message(arbitration_id=int(text[1:width+1],16),
                       is_extended_id=extended,
                       data=bytes.fromhex(text[width+2:width+2+dlc*2]),check=True)


class EwertBus:
    """Continuously drain USB traffic, retaining only node-1 protocol frames."""
    def __init__(self, port, node=1, fast=False):
        if node not in range(1, 8):
            raise ValueError("node must be between 1 and 7")
        self.node = node
        self.ser = serial.Serial(port,115200,timeout=0.01,write_timeout=2)
        self.partial = bytearray()
        self.frames = deque(maxlen=128)
        self.events = deque(maxlen=128)
        self.condition = threading.Condition()
        self.tx_lock = threading.Lock()
        self.stop = threading.Event()
        self.reader_error = None
        self.bad_packets = self.total_frames = self.filtered_frames = self.adapter_errors = 0
        self.last_tx = 0
        self.min_tx_interval = 0.01 if fast else 0.15
        self.ser.reset_input_buffer()
        self.reader = threading.Thread(target=self.read_loop,daemon=True)
        self.reader.start()
        try:
            self.command('C',allow_error=True)
            self.command('S6')
            self.command('A0')
            self.command('O')
        except BaseException:
            self.close()
            raise

    def damaged(self):
        self.bad_packets += 1
        if self.bad_packets == 1:
            print('Warning: damaged serial traffic discarded; waiting for intact CAN responses.',flush=True)

    def feed(self, data):
        for value in data:
            if value in (6,7):
                if self.partial:
                    self.partial.clear(); self.damaged()
                with self.condition:
                    self.events.append('ack' if value==6 else 'error')
                    self.adapter_errors += value==7
                    self.condition.notify_all()
                continue
            if value in (13,10):
                if not self.partial: continue
                packet=bytes(self.partial); self.partial.clear()
                try:
                    frame=decode_frame(packet)
                except (ValueError,IndexError,UnicodeError):
                    self.damaged(); continue
                if frame is not None:
                    self.total_frames += 1
                    if frame.is_extended_id or frame.arbitration_id not in (0x680 + self.node,):
                        self.filtered_frames += 1
                        continue
                    with self.condition:
                        if len(self.frames)==self.frames.maxlen:
                            self.reader_error=RuntimeError('Target reply queue overflow; stopped rather than lose target replies')
                        else:
                            self.frames.append(frame)
                        self.condition.notify_all()
                continue
            if value in (84,116,88,120) and self.partial:
                self.partial.clear(); self.damaged()
            if value<32 or value>126:
                self.partial.clear(); self.damaged(); continue
            self.partial.append(value)
            if len(self.partial)>128:
                self.partial.clear(); self.damaged()

    def read_loop(self):
        try:
            while not self.stop.is_set():
                data=self.ser.read(min(32768,self.ser.in_waiting) or 1)
                if data: self.feed(data)
        except Exception as error:
            if not self.stop.is_set():
                with self.condition:
                    self.reader_error=error
                    self.condition.notify_all()

    def check_reader(self):
        if self.reader_error is not None:
            raise RuntimeError('USB receive failed: '+str(self.reader_error))

    def command(self,text,allow_error=False):
        with self.tx_lock:
            self.check_reader()
            # Pace writes while the receive thread continues draining the bus.
            time.sleep(max(0,self.min_tx_interval-(time.monotonic()-self.last_tx)))
            with self.condition: self.events.clear()
            self.ser.write((text+'\r').encode('ascii')); self.ser.flush()
            self.last_tx=time.monotonic()
            deadline=time.monotonic()+1
            with self.condition:
                while True:
                    self.check_reader()
                    if self.events:
                        event=self.events.popleft()
                        if event=='ack' or allow_error: return
                        raise can.CanOperationError('CANdapter rejected transmission')
                    remaining=deadline-time.monotonic()
                    if remaining<=0:
                        raise can.CanOperationError('No CANdapter transmit acknowledgement')
                    self.condition.wait(remaining)

    def send(self,msg):
        if msg.is_extended_id or msg.is_remote_frame or msg.is_fd:
            raise ValueError('Bootloader requires standard data frames')
        self.command(f'T{msg.arbitration_id:03X}{len(msg.data):X}{msg.data.hex().upper()}')

    def recv(self,timeout):
        deadline=time.monotonic()+max(0,timeout)
        with self.condition:
            while True:
                self.check_reader()
                if self.frames: return self.frames.popleft()
                remaining=deadline-time.monotonic()
                if remaining<=0: return None
                self.condition.wait(remaining)

    def close(self):
        if self.stop.is_set(): return
        try:
            with self.tx_lock:
                self.ser.write(b'C\r'); self.ser.flush()
        finally:
            self.stop.set()
            self.reader.join(timeout=0.5)
            self.ser.close()
            if self.total_frames:
                print(f'CAN receive: {self.total_frames} frames; {self.filtered_frames} background frames filtered; '
                      f'{self.bad_packets} damaged packets; {self.adapter_errors} adapter errors.',flush=True)

