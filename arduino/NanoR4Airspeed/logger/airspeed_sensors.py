"""Pressure models and plain-I2C drivers; importable without smbus2/hardware.

SDP811 protocol: Sensirion SDP8xx-Digital datasheet, sections 6.1-6.5.
https://sensirion.com/resource/datasheet/sdp800-d/
"""

import errno
import time
from dataclasses import dataclass


@dataclass(frozen=True)
class SensorModel:
    name: str
    address: int
    raw_min: float
    raw_max: float
    quantum_pa: float
    full_scale_pa: float

    def pressure(self, raw):
        if self.name == "ms4525do":
            return ((raw - 0.1 * 16383) / (0.8 * 16383) * 2 - 1) * 6894.757
        return raw * self.quantum_pa

    def in_range(self, raw):
        return self.raw_min <= raw <= self.raw_max


MS4525DO = SensorModel(
    "ms4525do", 0x28, 1638.3, 14744.7, 2 * 6894.757 / (0.8 * 16383), 6894.757
)
SDP811 = SensorModel("sdp811-500pa", 0x26, -30000, 30000, 1 / 60, 500)
SENSORS = {model.name: model for model in (MS4525DO, SDP811)}


@dataclass(frozen=True)
class Reading:
    raw: int
    status: int = 0
    temperature_c: float | None = None
    scale_factor: int | None = None


class SensorReadError(OSError):
    def __init__(self, state, message):
        super().__init__(message)
        self.reading_state = state


def crc8(data):
    """Sensirion CRC-8: polynomial 0x31, initial 0xFF, no reflection/XOR."""
    crc = 0xFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc << 1) ^ (0x31 if crc & 0x80 else 0)) & 0xFF
    return crc


def decode_sdp811(data):
    if len(data) != 9:
        raise SensorReadError("short_read", "SDP811 requires a nine-byte frame")
    for offset in (0, 3, 6):
        if crc8(data[offset : offset + 2]) != data[offset + 2]:
            raise SensorReadError("crc_failed", f"SDP811 CRC failed at byte {offset}")
    scale = int.from_bytes(data[6:8], "big")
    # Reject a different sensor/range rather than applying the 500 Pa model to it.
    if scale != 60:
        raise SensorReadError(
            "scale_mismatch", f"SDP811-500Pa scale must be 60, received {scale}"
        )
    return Reading(
        raw=int.from_bytes(data[0:2], "big", signed=True),
        temperature_c=int.from_bytes(data[3:5], "big", signed=True) / 200,
        scale_factor=scale,
    )


class PressureReader:
    def __init__(self, bus, address, model):
        self.bus, self.address, self.model = bus, address, model
        self.started = False

    def command(self, value):
        from smbus2 import i2c_msg  # ty: ignore[unresolved-import]

        # Separate write/read transactions: no SMBus register or length byte.
        self.bus.i2c_rdwr(i2c_msg.write(self.address, value.to_bytes(2, "big")))

    def start(self):
        if self.model != SDP811:
            return
        try:
            self.command(
                0x3FF9
            )  # Stop a measurement left running by an earlier process.
        except OSError as exc:
            # An idle sensor can NACK stop; other transport errors must propagate.
            if exc.errno not in (errno.ENXIO, getattr(errno, "EREMOTEIO", 121)):
                raise
        time.sleep(0.001)  # Stop-to-next-command minimum is 500 us.
        self.started = True  # Also attempt cleanup if a start write partially succeeds.
        self.command(0x3615)  # Differential-pressure compensation, average until read.
        time.sleep(0.020)  # First data at 8 ms; allow the following 12 ms to settle.

    def read(self):
        from smbus2 import i2c_msg  # ty: ignore[unresolved-import]

        count = 9 if self.model == SDP811 else 2
        message = i2c_msg.read(self.address, count)
        self.bus.i2c_rdwr(message)
        data = bytes(message)
        if self.model == SDP811:
            return decode_sdp811(data)
        if len(data) != 2:
            raise SensorReadError("short_read", "MS4525DO requires two pressure bytes")
        high, low = data
        return Reading(((high & 0x3F) << 8) | low, high >> 6)

    def stop(self):
        if self.started:
            self.command(0x3FF9)
            self.started = False
            time.sleep(0.001)
