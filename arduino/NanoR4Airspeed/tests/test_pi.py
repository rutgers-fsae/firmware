"""Portable Pi acquisition check; no I2C hardware or smbus2 installation needed."""
import argparse
import csv
import json
import socket
import sys
import tempfile
import threading
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "logger"))
import airspeed_pi as pi


class PiAcquisition(unittest.TestCase):
    def test_dashboard_zero_protocol_and_calibration_during_recording(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            with socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM) as server, \
                 socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM) as client:
                server.bind(str(root / "server.sock"))
                client.bind(str(root / "client.sock"))
                client.settimeout(1)
                server.setblocking(False)
                state = dict(state="needs_zero")
                for command, current, expected in [("zero", "needs_zero", True),
                                                    ("zero", "zeroing", False),
                                                    ("status", "zeroing", False)]:
                    state["state"] = current
                    client.sendto(json.dumps(dict(request_id="check", command=command)).encode(),
                                  str(root / "server.sock"))
                    self.assertEqual(pi.poll_control(server, state), expected)
                    response = json.loads(client.recv(4096))
                    self.assertEqual(response["request_id"], "check")
                    self.assertEqual(response["success"], command == "status" or expected)

            args = argparse.Namespace(bus=1, address=0x28, rate=20, density=1.225,
                                      output_dir=root, calibration=root / "zero.json",
                                      title="Dashboard zero", zero=False)
            stop = threading.Event()
            states = []

            def control(_socket, state):
                states.append(dict(state))
                if len(states) == 1:
                    return True
                if state["state"] in ("ready", "error"):
                    stop.set()
                return False

            ticks = iter(i * 0.25 for i in range(1000))
            with patch.object(pi, "poll_control", side_effect=control), \
                 patch.object(pi.time, "monotonic", side_effect=lambda: next(ticks)), \
                 patch.object(stop, "wait", return_value=False), \
                 patch.object(pi, "read_sensor", return_value=(8220, 0)):
                pi.acquire(args, None, stop, object())
            self.assertEqual(states[-1]["state"], "ready")
            original = args.calibration.read_text()
            with next(root.glob("*.csv")).open() as source:
                rows = list(csv.DictReader(source))
            self.assertEqual(rows[0]["airspeed_m_s"], "")
            self.assertEqual(rows[-1]["airspeed_m_s"], "0.0")
            # A noisy dashboard zero fails without losing the saved baseline.
            states.clear()
            stop.clear()
            ticks = iter(i * 0.25 for i in range(1000))
            readings = iter([(8220 if i % 2 else 8320, 0) for i in range(100)])
            with patch.object(pi, "poll_control", side_effect=control), \
                 patch.object(pi.time, "monotonic", side_effect=lambda: next(ticks)), \
                 patch.object(stop, "wait", return_value=False), \
                 patch.object(pi, "read_sensor", side_effect=lambda *_: next(readings)):
                pi.acquire(args, None, stop, object())
            self.assertEqual(states[-1]["state"], "error")
            self.assertEqual(args.calibration.read_text(), original)

    def test_plain_i2c_read(self):
        class Message:
            @staticmethod
            def read(address, count):
                self.assertEqual((address, count), (0x28, 2))
                return [0xA0, 0x1C]

        class Bus:
            def i2c_rdwr(self, message):
                pass

        with patch.dict(sys.modules, smbus2=argparse.Namespace(i2c_msg=Message)):
            self.assertEqual(pi.read_sensor(Bus(), 0x28), (8220, 2))

    def test_acquisition_zero_failures_and_saved_csv(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            args = argparse.Namespace(bus=1, address=0x28, rate=20, density=1.225,
                                      output_dir=root, calibration=root / "zero.json",
                                      title="Pi check", zero=True)
            stop = threading.Event()
            # Advance monotonic time to cover the actual five-second zero requirement.
            ticks = iter(i * 0.25 for i in range(1000))
            with patch.object(pi.time, "monotonic", side_effect=lambda: next(ticks)), \
                 patch.object(stop, "wait", return_value=False), \
                 patch.object(pi, "read_sensor", return_value=(8220, 0)):
                pi.acquire(args, None, stop)
            cal = pi.load_calibration(args.calibration, 1, 0x28)
            self.assertGreaterEqual(cal["sample_count"], 20)
            self.assertGreaterEqual(cal["duration_s"], 5)
            self.assertEqual(pi.sample_row(8220, 0, cal, 1.225)["airspeed_m_s"], 0)
            self.assertIsNone(pi.sample_row(8220, 2, cal, 1.225)["airspeed_m_s"])
            self.assertEqual(pi.sample_row(8220, 0, None, 1.225)["reading_state"], "needs_zero")
            with self.assertRaises(ValueError):
                pi.load_calibration(args.calibration, 2, 0x28)
            args.zero = False
            with patch.object(stop, "wait", return_value=False), \
                 patch.object(pi, "read_sensor", side_effect=[(8300, 0), OSError("no ACK"),
                                                            OSError("no ACK"), OSError("no ACK")]):
                with self.assertRaises(RuntimeError):
                    pi.acquire(args, None, stop)
            csv_path = next(root.glob("*.csv"))
            with csv_path.open() as source:
                rows = list(csv.DictReader(source))
            self.assertEqual(len(rows), 4)
            self.assertEqual(rows[0]["reading_state"], "valid")
            self.assertEqual(rows[-1]["reading_state"], "read_failed")
            self.assertEqual(rows[-1]["airspeed_m_s"], "")
            self.assertTrue(json.loads(csv_path.with_suffix(".run.json").read_text())["ended_utc"])
            # SIGTERM sets this event; cancellation closes and syncs the CSV too.
            stop.set()
            pi.acquire(args, None, stop)
            self.assertEqual(len(list(root.glob("*.csv"))), 2)


if __name__ == "__main__":
    unittest.main()
