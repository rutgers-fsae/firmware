"""Sensor protocol, model isolation and acquisition checks without hardware."""

import argparse
import csv
import errno
import json
import math
import sys
import tempfile
import threading
import unittest
from pathlib import Path
from unittest.mock import MagicMock, Mock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "logger"))
import airspeed_pi as pi
from airspeed_core import ZeroCalibration, calculate
from airspeed_sensors import (
    MS4525DO,
    SDP811,
    PressureReader,
    Reading,
    SensorReadError,
    crc8,
    decode_sdp811,
)


def frame(raw=6000, temperature=5000, scale=60):
    result = bytearray()
    for value in (raw & 0xFFFF, temperature & 0xFFFF, scale):
        word = value.to_bytes(2, "big")
        result.extend(word)
        result.append(crc8(word))
    return bytes(result)


class Message(bytearray):
    address: int
    is_read: bool

    @classmethod
    def read(cls, address, count):
        result = cls(count)
        result.address, result.is_read = address, True
        return result

    @classmethod
    def write(cls, address, data):
        result = cls(data)
        result.address, result.is_read = address, False
        return result


class Bus:
    def __init__(self, payload=None):
        self.payload = frame() if payload is None else payload
        self.operations = []
        self.closed = False

    def __enter__(self):
        return self

    def __exit__(self, *_):
        self.closed = True

    def i2c_rdwr(self, message):
        if message.is_read:
            self.operations.append(("read", message.address, len(message)))
            message[:] = self.payload
        else:
            self.operations.append(("write", message.address, bytes(message)))


def calibration(model=SDP811):
    return dict(
        i2c_bus=1,
        i2c_address=model.address,
        sensor_model=model.name,
        zero_offset_pa=0,
        noise_std_pa=0.01,
        calibration_id="test-zero",
    )


class SDP811Protocol(unittest.TestCase):
    def test_datasheet_crc_reference_vectors(self):
        self.assertEqual(crc8(bytes.fromhex("beef")), 0x92)
        self.assertEqual(crc8(bytes.fromhex("0000")), 0x81)

    def test_signed_pressure_temperature_and_scale(self):
        for raw in (0, 1, -1, 6000, -6000, 15000, 30000, -30000):
            with self.subTest(raw=raw):
                reading = decode_sdp811(frame(raw, -2000))
                self.assertEqual(reading, Reading(raw, 0, -10.0, 60))
                self.assertAlmostEqual(SDP811.pressure(reading.raw), raw / 60)

    def test_every_word_is_crc_checked_and_truncation_rejected(self):
        original = frame()
        for index in range(9):
            corrupt = bytearray(original)
            corrupt[index] ^= 1
            with self.subTest(index=index), self.assertRaises(SensorReadError) as error:
                decode_sdp811(corrupt)
            self.assertEqual(error.exception.reading_state, "crc_failed")
        for data in (b"", original[:8], original + b"\0"):
            with self.assertRaises(SensorReadError) as error:
                decode_sdp811(data)
            self.assertEqual(error.exception.reading_state, "short_read")

    def test_wrong_sensor_scale_is_rejected_even_with_valid_crc(self):
        for scale in (0, 1, 61, 240, 65535):
            with self.subTest(scale=scale), self.assertRaises(SensorReadError) as error:
                decode_sdp811(frame(scale=scale))
            self.assertEqual(error.exception.reading_state, "scale_mismatch")

    def test_start_read_stop_use_correct_transactions_and_waits(self):
        bus = Bus()
        reader = PressureReader(bus, 0x26, SDP811)
        with (
            patch.dict(sys.modules, smbus2=argparse.Namespace(i2c_msg=Message)),
            patch("airspeed_sensors.time.sleep") as sleep,
        ):
            reader.start()
            self.assertEqual(reader.read(), Reading(6000, 0, 25.0, 60))
            reader.stop()
            reader.stop()  # No duplicate stop when already stopped.
        self.assertEqual(
            bus.operations,
            [
                ("write", 0x26, b"\x3f\xf9"),
                ("write", 0x26, b"\x36\x15"),
                ("read", 0x26, 9),
                ("write", 0x26, b"\x3f\xf9"),
            ],
        )
        self.assertEqual(
            [call.args[0] for call in sleep.call_args_list], [0.001, 0.020, 0.001]
        )

    def test_only_idle_nack_is_ignored_during_initial_stop(self):
        for error_number in (errno.ENXIO, 121, errno.EIO):
            reader = PressureReader(None, 0x26, SDP811)
            with (
                patch.object(
                    reader, "command", side_effect=[OSError(error_number, "test"), None]
                ) as command,
                patch("airspeed_sensors.time.sleep"),
            ):
                if error_number == errno.EIO:
                    with self.assertRaises(OSError):
                        reader.start()
                    self.assertEqual(command.call_count, 1)
                else:
                    reader.start()
                    self.assertEqual(command.call_args.args, (0x3615,))

    def test_old_sensor_still_uses_two_byte_read_and_no_commands(self):
        bus = Bus(b"\xa0\x1c")
        reader = PressureReader(bus, 0x28, MS4525DO)
        with patch.dict(sys.modules, smbus2=argparse.Namespace(i2c_msg=Message)):
            reader.start()
            self.assertEqual(reader.read(), Reading(8220, 2))
            reader.stop()
        self.assertEqual(bus.operations, [("read", 0x28, 2)])


class SDP811Calculations(unittest.TestCase):
    def test_zero_positive_negative_and_range_limit(self):
        cal = calibration()
        self.assertEqual(
            calculate(0, 0, cal, sensor=SDP811)[2:], (0.0, "within_zero_noise")
        )
        pa, dp, speed, state = calculate(6000, 0, cal, sensor=SDP811)
        self.assertEqual((pa, dp, state), (100, 100, "valid"))
        self.assertAlmostEqual(speed, math.sqrt(200 / 1.225))
        self.assertEqual(
            calculate(-6000, 0, cal, sensor=SDP811)[2:],
            (None, "negative_pressure_check_tubing"),
        )
        self.assertEqual(calculate(30000, 0, cal, sensor=SDP811)[0], 500)
        self.assertEqual(calculate(-30000, 0, cal, sensor=SDP811)[0], -500)
        for raw in (-30001, 30001):
            self.assertEqual(calculate(raw, 0, cal, sensor=SDP811)[3], "out_of_range")
        self.assertEqual(calculate(0, 0, None, sensor=SDP811)[3], "needs_zero")

    def test_noise_floor_uses_new_sensor_resolution(self):
        cal = calibration()
        cal["noise_std_pa"] = 0
        self.assertEqual(calculate(2, 0, cal, sensor=SDP811)[3], "within_zero_noise")
        self.assertEqual(calculate(3, 0, cal, sensor=SDP811)[3], "valid")
        self.assertAlmostEqual(
            pi.sample_row(0, 0, cal, 1.225, SDP811)["zero_noise_band_pa"], 2 / 60
        )

    def test_zero_accepts_signed_counts_and_ignores_out_of_range(self):
        zero = ZeroCalibration(now=0, sensor=SDP811)
        self.assertIsNone(zero.add(32767, 0, now=0))
        self.assertEqual(zero.samples, [])
        result = None
        for index in range(21):
            result = zero.add(-60, 0, now=index / 4)
        assert result is not None
        self.assertEqual(result["zero_offset_pa"], -1)
        self.assertEqual(result["noise_std_pa"], 0)

    def test_calibration_cannot_cross_sensor_models(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "zero.json"
            cal = calibration()
            path.write_text(json.dumps(cal))
            self.assertEqual(pi.load_calibration(path, 1, 0x26, SDP811), cal)
            for model in (None, MS4525DO.name):
                old = dict(cal)
                if model is None:
                    old.pop("sensor_model")
                else:
                    old["sensor_model"] = model
                path.write_text(json.dumps(old))
                with self.assertRaisesRegex(ValueError, "sensor model"):
                    pi.load_calibration(path, 1, 0x26, SDP811)
            cal["zero_offset_pa"] = 501
            path.write_text(json.dumps(cal))
            with self.assertRaisesRegex(ValueError, "sensor limits"):
                pi.load_calibration(path, 1, 0x26, SDP811)
            legacy = calibration(MS4525DO)
            legacy.pop("sensor_model")
            path.write_text(json.dumps(legacy))
            self.assertEqual(pi.load_calibration(path, 1, 0x28), legacy)


class SDP811Acquisition(unittest.TestCase):
    def test_main_selects_sdp811_and_stops_after_normal_or_failed_acquisition(self):
        for failure in (None, OSError("bus disconnected"), RuntimeError("bad frames")):
            with (
                self.subTest(failure=failure),
                tempfile.TemporaryDirectory() as directory,
            ):
                reader = Mock(model=SDP811)
                bus_type = MagicMock()
                with (
                    patch.dict(sys.modules, smbus2=argparse.Namespace(SMBus=bus_type)),
                    patch.object(pi, "PressureReader", return_value=reader) as factory,
                    patch.object(pi, "acquire", side_effect=failure) as acquire,
                    patch.object(pi.LOG, "exception"),
                ):
                    result = pi.main(
                        ["--sensor", SDP811.name, "--output-dir", directory]
                    )
                self.assertEqual(result, 0 if failure is None else 1)
                self.assertEqual(factory.call_args.args[1:], (0x26, SDP811))
                self.assertEqual(
                    acquire.call_args.args[0].calibration.name, "zero-sdp811.json"
                )
                reader.start.assert_called_once()
                reader.stop.assert_called_once()
                bus_type.return_value.__exit__.assert_called_once()

    def test_failed_start_attempts_cleanup_without_entering_acquisition(self):
        with tempfile.TemporaryDirectory() as directory:
            reader = Mock(model=SDP811)
            reader.start.side_effect = OSError("start failed")
            with (
                patch.dict(sys.modules, smbus2=argparse.Namespace(SMBus=MagicMock())),
                patch.object(pi, "PressureReader", return_value=reader),
                patch.object(pi, "acquire") as acquire,
                patch.object(pi.LOG, "exception"),
            ):
                self.assertEqual(
                    pi.main(["--sensor", SDP811.name, "--output-dir", directory]), 1
                )
            acquire.assert_not_called()
            reader.stop.assert_called_once()

    def test_zero_saves_sensor_identity_and_bad_crc_never_becomes_airspeed(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            args = argparse.Namespace(
                bus=1,
                address=0x26,
                rate=20,
                density=1.225,
                output_dir=root,
                calibration=root / "zero.json",
                title="SDP811 test",
                zero=True,
            )
            reader = PressureReader(None, 0x26, SDP811)
            stop = threading.Event()
            ticks = iter(index / 4 for index in range(1000))
            with (
                patch.object(reader, "read", return_value=Reading(-60, 0, 25, 60)),
                patch.object(pi.time, "monotonic", side_effect=lambda: next(ticks)),
                patch.object(stop, "wait", return_value=False),
            ):
                pi.acquire(args, reader, stop)
            saved = args.calibration.read_text()
            cal = json.loads(saved)
            self.assertEqual(
                (cal["sensor_model"], cal["i2c_address"], cal["zero_offset_pa"]),
                (SDP811.name, 0x26, -1),
            )
            args.zero = False
            readings = [Reading(6000, 0, 25, 60)] + [
                SensorReadError("crc_failed", "test corruption") for _ in range(3)
            ]
            with (
                patch.object(reader, "read", side_effect=readings),
                patch.object(stop, "wait", return_value=False),
            ):
                with self.assertRaisesRegex(RuntimeError, "Three consecutive"):
                    pi.acquire(args, reader, stop)
            self.assertEqual(args.calibration.read_text(), saved)
            with next(root.glob("*.csv")).open() as source:
                rows = list(csv.DictReader(source))
            self.assertEqual(rows[0]["pressure_pa"], "100.0")
            self.assertEqual(rows[0]["sensor_model"], SDP811.name)
            for row in rows[1:]:
                self.assertEqual(row["reading_state"], "crc_failed")
                self.assertEqual(row["airspeed_m_s"], "")
                self.assertEqual(row["pressure_pa"], "")


if __name__ == "__main__":
    unittest.main()
