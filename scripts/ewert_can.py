"""Ewert CANdapter native ASCII transport; requires python-can and pyserial.

Read continuously while writing; retain only the selected bootloader node's replies.
This uses T for standard frames and X for extended frames, unlike generic SLCAN.
"""

import threading
import time
from collections import deque

import can

try:
    import serial
except ImportError as error:
    raise RuntimeError(
        "Install Ewert support: python3 -m pip install pyserial"
    ) from error


def decode_frame(raw):
    text = raw.decode("ascii")
    if not text or text[0] not in "TtXx":
        return None
    extended = text[0] in "Xx"
    width = 8 if extended else 3
    dlc = int(text[width + 1], 16)
    if dlc > 8 or len(text) not in (width + 2 + dlc * 2, width + 6 + dlc * 2):
        raise ValueError("Malformed CANdapter frame: " + repr(text))
    return can.Message(
        arbitration_id=int(text[1 : width + 1], 16),
        is_extended_id=extended,
        data=bytes.fromhex(text[width + 2 : width + 2 + dlc * 2]),
        check=True,
    )


class EwertBus:
    """Continuously drain USB traffic, retaining only the selected node's protocol frames."""

    def __init__(self, port, node=1):
        if node not in range(1, 8):
            raise ValueError("node must be between 1 and 7")
        self.node = node
        self.ser = serial.Serial(port, 115200, timeout=0.01, write_timeout=2)
        self.partial = bytearray()
        self.frames = deque(maxlen=128)
        self.events = deque()
        self.condition = threading.Condition()
        self.tx_lock = threading.Lock()
        self.stop = threading.Event()
        self.reader_error = None
        self.bad_packets = self.total_frames = self.filtered_frames = (
            self.adapter_errors
        ) = 0
        self.ser.reset_input_buffer()
        self.reader = threading.Thread(target=self.read_loop, daemon=True)
        self.reader.start()
        try:
            self.command("C", allow_error=True)
            self.command("S6")
            self.command("A0")
            self.command("O")
        except BaseException:
            self.close()
            raise

    def damaged(self):
        self.bad_packets += 1
        if self.bad_packets == 1:
            print(
                "Warning: damaged serial traffic discarded; waiting for intact CAN responses.",
                flush=True,
            )

    def feed(self, data):
        for value in data:
            if value in (6, 7):
                if self.partial:
                    self.partial.clear()
                    self.damaged()
                with self.condition:
                    self.events.append("ack" if value == 6 else "error")
                    self.adapter_errors += value == 7
                    self.condition.notify_all()
                continue
            if value in (13, 10):
                if not self.partial:
                    continue
                packet = bytes(self.partial)
                self.partial.clear()
                try:
                    frame = decode_frame(packet)
                except (ValueError, IndexError, UnicodeError):
                    self.damaged()
                    continue
                if frame is not None:
                    self.total_frames += 1
                    if frame.is_extended_id or frame.arbitration_id not in (
                        0x680 + self.node,
                    ):
                        self.filtered_frames += 1
                        continue
                    with self.condition:
                        if len(self.frames) == self.frames.maxlen:
                            self.reader_error = RuntimeError(
                                "Target reply queue overflow; stopped rather than lose target replies"
                            )
                        else:
                            self.frames.append(frame)
                        self.condition.notify_all()
                continue
            if value in (84, 116, 88, 120) and self.partial:
                self.partial.clear()
                self.damaged()
            if value < 32 or value > 126:
                self.partial.clear()
                self.damaged()
                continue
            self.partial.append(value)
            if len(self.partial) > 128:
                self.partial.clear()
                self.damaged()

    def read_loop(self):
        try:
            while not self.stop.is_set():
                data = self.ser.read(min(32768, self.ser.in_waiting) or 1)
                if data:
                    self.feed(data)
        except Exception as error:
            if not self.stop.is_set():
                with self.condition:
                    self.reader_error = error
                    self.condition.notify_all()

    def check_reader(self):
        if self.reader_error is not None:
            raise RuntimeError("USB receive failed: " + str(self.reader_error))

    def command(self, text, allow_error=False):
        with self.tx_lock:
            self.check_reader()
            with self.condition:
                self.events.clear()
            self.ser.write((text + "\r").encode("ascii"))
            self.ser.flush()
            deadline = time.monotonic() + 1
            with self.condition:
                while True:
                    self.check_reader()
                    if self.events:
                        event = self.events.popleft()
                        if event == "ack" or allow_error:
                            return
                        raise can.CanOperationError("CANdapter rejected transmission")
                    remaining = deadline - time.monotonic()
                    if remaining <= 0:
                        raise can.CanOperationError(
                            "No CANdapter transmit acknowledgement"
                        )
                    self.condition.wait(remaining)

    @staticmethod
    def encode(msg):
        if msg.is_extended_id or msg.is_remote_frame or msg.is_fd:
            raise ValueError("Bootloader requires standard data frames")
        return (
            f"T{msg.arbitration_id:03X}{len(msg.data):X}{bytes(msg.data).hex().upper()}"
        )

    def send(self, msg):
        self.command(self.encode(msg))

    def send_many(self, messages, idle=0.05):
        """Queue a burst in one write and return once the adapter has answered every frame.

        The adapter rejects roughly 1-2% of queued frames (BELL) and its reply count can be
        off by one, so rejections are only counted here; the caller must confirm delivery
        with the target. Replies to earlier frames are already in the receive queue on return.
        No flush(): on macOS tcdrain() adds about 100 ms per 64-frame burst.
        """
        burst = "".join(self.encode(msg) + "\r" for msg in messages).encode("ascii")
        with self.tx_lock:
            self.check_reader()
            with self.condition:
                self.events.clear()
            self.ser.write(burst)
            answered = 0
            deadline = time.monotonic() + idle
            with self.condition:
                while answered < len(messages):
                    self.check_reader()
                    if self.events:
                        answered += len(self.events)
                        self.events.clear()
                        deadline = time.monotonic() + idle
                        continue
                    remaining = deadline - time.monotonic()
                    if remaining <= 0:
                        break
                    self.condition.wait(remaining)
            if not answered:
                raise can.CanOperationError("No CANdapter transmit acknowledgement")

    def recv(self, timeout):
        deadline = time.monotonic() + max(0, timeout)
        with self.condition:
            while True:
                self.check_reader()
                if self.frames:
                    return self.frames.popleft()
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    return None
                self.condition.wait(remaining)

    def close(self):
        if self.stop.is_set():
            return
        try:
            with self.tx_lock:
                self.ser.write(b"C\r")
                self.ser.flush()
        finally:
            self.stop.set()
            self.reader.join(timeout=0.5)
            self.ser.close()
            if self.total_frames:
                print(
                    f"CAN receive: {self.total_frames} frames; {self.filtered_frames} background frames filtered; "
                    f"{self.bad_packets} damaged packets; {self.adapter_errors} adapter errors.",
                    flush=True,
                )
