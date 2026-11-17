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
        self.channel_resets = self.consecutive_rejections = 0
        self.bad_packets = self.total_frames = self.filtered_frames = (
            self.adapter_errors
        ) = 0
        self.ser.reset_input_buffer()
        self.reader = threading.Thread(target=self.read_loop, daemon=True)
        self.reader.start()
        try:
            self.open_channel()
        except BaseException:
            self.close()
            raise

    def open_channel(self):
        with self.tx_lock:
            self._open_channel()

    def _open_channel(self):
        for text, allow_error in (
            ("C", True),
            ("S6", False),
            ("A0", False),
            ("O", False),
        ):
            self._command(text, allow_error)

    def reset_channel(self, reason="stopped transmitting"):
        """Close and reopen the CAN channel; this clears an adapter that rejects every frame."""
        self.channel_resets += 1
        if self.channel_resets == 1:
            print(
                f"Warning: CANdapter {reason}; reopening its CAN channel.", flush=True
            )
        self.open_channel()

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
                except ValueError, IndexError, UnicodeError:
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
            self._command(text, allow_error)

    def _command(self, text, allow_error=False):
        """Send one command and wait for its ACK/BELL. The caller holds tx_lock."""
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
                    raise can.CanOperationError("No CANdapter transmit acknowledgement")
                self.condition.wait(remaining)

    @staticmethod
    def encode(msg):
        if msg.is_extended_id or msg.is_remote_frame or msg.is_fd:
            raise ValueError("Bootloader requires standard data frames")
        return (
            f"T{msg.arbitration_id:03X}{len(msg.data):X}{bytes(msg.data).hex().upper()}"
        )

    def send(self, msg):
        try:
            self.command(self.encode(msg))
        except can.CanOperationError as error:
            if "rejected" in str(error):
                self.consecutive_rejections += 1
                if self.consecutive_rejections >= 3:
                    self.consecutive_rejections = 0
                    self.reset_channel("rejected three transmissions in a row")
            raise
        self.consecutive_rejections = 0

    # Frames in flight at once. The adapter has a small transmit queue: 33 frames written back to
    # back on a busy bus got 15 rejections and then wedged the adapter until its channel was reopened.
    CHUNK = 8

    def _write_chunk(self, lines, idle):
        """Write frames, wait for the adapter to answer each; return (accepted, rejected)."""
        with self.condition:
            self.events.clear()
        self.ser.write(b"".join(lines))
        accepted = rejected = 0
        deadline = time.monotonic() + idle
        with self.condition:
            while accepted + rejected < len(lines):
                self.check_reader()
                if self.events:
                    event = self.events.popleft()
                    accepted += event == "ack"
                    rejected += event != "ack"
                    deadline = time.monotonic() + idle
                    continue
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    break
                self.condition.wait(remaining)
        return accepted, rejected

    def send_many(self, messages, idle=0.05):
        """Send frames in small flow-controlled chunks; return once the adapter has answered them all.

        The adapter rejects a few percent of frames (BELL) and its reply count can be off by one,
        so rejections are only counted; the caller must confirm delivery with the target. If a
        whole chunk is rejected the adapter is wedged, so its channel is reopened and the chunk
        sent once more. No flush(): on macOS tcdrain() adds about 100 ms per burst.
        """
        lines = [(self.encode(msg) + "\r").encode("ascii") for msg in messages]
        with self.tx_lock:
            self.check_reader()
            for start in range(0, len(lines), self.CHUNK):
                chunk = lines[start : start + self.CHUNK]
                accepted, rejected = self._write_chunk(chunk, idle)
                if not accepted:
                    self.channel_resets += 1
                    if self.channel_resets == 1:
                        print(
                            "Warning: CANdapter stopped transmitting; reopening its CAN channel.",
                            flush=True,
                        )
                    self._open_channel()
                    accepted, rejected = self._write_chunk(chunk, idle)
                    if not accepted:
                        raise can.CanOperationError(
                            "CANdapter rejects every transmission"
                        )

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
                    f"{self.bad_packets} damaged packets; {self.adapter_errors} adapter errors; "
                    f"{self.channel_resets} channel reopenings.",
                    flush=True,
                )
