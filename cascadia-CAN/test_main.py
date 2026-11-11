"""Offline protocol and write-safety checks: uv run python -m unittest -v."""

import io
import json
import runpy
import tempfile
import time
import unittest
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

# Load this file explicitly: the repository also has python/main.py.
app = runpy.run_path(str(Path(__file__).with_name("main.py")))
BY_ADDRESS = app["BY_ADDRESS"]
PARAMETERS = app["PARAMETERS"]
TUI = app["TUI"]
Candapter = app["Candapter"]
Observation = app["Observation"]
Unsupported = app["Unsupported"]
Worker = app["Worker"]
candapter_frame = app["candapter_frame"]
import_values = app["import_values"]
parse_args = app["parse_args"]
parse_frame = app["parse_frame"]
run_single = app["run_single"]
save_snapshot = app["save_snapshot"]
snapshot = app["snapshot"]
validate_changes = app["validate_changes"]


class FakeSerial:
    def __init__(self, args, values=None, fragment=4096):
        self.args = args
        self.values = dict(values or {})
        self.buffer = bytearray()
        self.fragment = fragment
        self.writes = []
        self.requests = []
        self.closed = False
        self.reject = False
        self.silent = False
        self.enabled = False
        self.status = True
        self.mismatch = False
        self.write_success = True
        self.reject_command = None
        self.before_request = None
        self.after_write = None

    @property
    def in_waiting(self):
        return len(self.buffer)

    def read(self, size):
        size = min(size, self.fragment)
        result = bytes(self.buffer[:size])
        del self.buffer[:size]
        if not result:
            time.sleep(0.0001)
        return result

    def inject(self, relative, data, extended=None):
        identifier = self.args.base + relative
        if self.args.mode == "j1939":
            identifier = 0x0CFF0001 | (identifier << 8)
        frame = candapter_frame(
            identifier,
            self.args.mode != "standard" if extended is None else extended,
            data,
        )
        self.buffer.extend(frame.encode() + b"\r")

    def status_frame(self):
        data = bytearray(8)
        data[6] = int(self.enabled)
        self.inject(0x0A, data)

    def write(self, data):
        self.writes.append(data)
        if data == self.reject_command:
            self.buffer.extend(b"\x07")
            return len(data)
        frame = parse_frame(data.rstrip(b"\r"))
        if frame is None:
            self.buffer.extend(b"\x06")
            return len(data)
        identifier, extended, payload = frame
        self.requests.append(payload)
        address = int.from_bytes(payload[:2], "little")
        writing = payload[2] == 1
        if self.before_request:
            self.before_request(address, writing)
        if self.reject:
            self.buffer.extend(b"\x07")
            return len(data)
        self.buffer.extend(b"\x06")
        if self.status:
            self.status_frame()
        if self.silent:
            return len(data)
        if address not in self.values:
            self.inject(0x22, bytes(8))
            return len(data)
        word = int.from_bytes(payload[4:6], "little")
        if writing and self.write_success:
            self.values[address] = word + int(self.mismatch)
            if self.after_write:
                self.after_write(address)
        response = (
            address.to_bytes(2, "little")
            + bytes([int(writing and self.write_success), 0])
            + self.values[address].to_bytes(2, "little")
            + bytes(2)
        )
        self.inject(0x22, response)
        return len(data)

    def flush(self):
        pass

    def reset_input_buffer(self):
        self.buffer.clear()

    def close(self):
        self.closed = True


def adapter(values=None, fragment=4096, mode="standard"):
    args = parse_args(["--timeout", "0.04", "--mode", mode, "--request-gap", "0"])
    serial = FakeSerial(args, values, fragment)
    bus = Candapter(serial, args)
    serial.status_frame()
    return bus, serial


def worker(values):
    bus, serial = adapter(values)
    instance = Worker(bus.args)
    instance.adapter = bus
    instance.values = dict(values)
    return instance, serial


class Checks(unittest.TestCase):
    def test_tui_worker_reads_and_writes_individually(self):
        args = parse_args(["--timeout", "0.04", "--request-gap", "0"])
        serial = FakeSerial(args, {100: 4250})
        instance = Worker(args, serial_factory=lambda *a, **kw: serial)

        def wait_for_job():
            while instance.events.get(timeout=1) != ("busy", False):
                pass

        instance.start()
        try:
            wait_for_job()
            self.assertFalse(serial.requests)
            instance.jobs.put(("read", [100]))
            wait_for_job()
            self.assertEqual(instance.values, {100: 4250})
            instance.jobs.put(("apply", {100: (4250, 4200)}))
            wait_for_job()
            self.assertEqual(instance.values, {100: 4200})
            self.assertEqual(
                {int.from_bytes(p[:2], "little") for p in serial.requests}, {100}
            )
            self.assertEqual(sum(p[2] == 1 for p in serial.requests), 1)
            instance.jobs.put(("connect", None))
            wait_for_job()
            self.assertEqual(len(serial.requests), 5)
        finally:
            instance.stop.set()
            instance.join(timeout=1)
        self.assertFalse(instance.is_alive())
        self.assertTrue(serial.closed)

    def test_individual_read_write_and_interlocks(self):
        for command in (
            ["read", "129"],
            ["write", "129", "150"],
            ["write", "129", "1500", "--raw"],
        ):
            with self.subTest(command=command):
                args = parse_args(command + ["--timeout", "0.04", "--request-gap", "0"])
                serial = FakeSerial(args, {129: 1200})
                output = io.StringIO()
                with redirect_stdout(output):
                    run_single(args, serial_factory=lambda *a, **kw: serial)
                self.assertTrue(serial.closed)
                self.assertEqual(
                    {int.from_bytes(p[:2], "little") for p in serial.requests}, {129}
                )
                writes = [p for p in serial.requests if p[2]]
                self.assertEqual(len(writes), int(command[0] == "write"))
                if writes:
                    self.assertEqual(serial.values[129], 1500)
                    self.assertIn("Write verified by readback", output.getvalue())
        for enabled, mismatch, message in (
            (True, False, "enabled"),
            (False, True, "readback mismatch"),
        ):
            args = parse_args(
                ["write", "129", "150", "--timeout", "0.04", "--request-gap", "0"]
            )
            serial = FakeSerial(args, {129: 1200})
            serial.enabled, serial.mismatch = enabled, mismatch
            with (
                redirect_stdout(io.StringIO()),
                self.assertRaisesRegex(RuntimeError, message),
            ):
                run_single(args, serial_factory=lambda *a, **kw: serial)
            self.assertTrue(serial.closed)
            self.assertEqual(sum(p[2] == 1 for p in serial.requests), int(not enabled))

    def test_individual_command_validation(self):
        for command in (
            ["read"],
            ["write", "100"],
            ["read", "100", "1"],
            ["write", "234", "200"],
            ["read", "999"],
        ):
            with (
                self.subTest(command=command),
                redirect_stderr(io.StringIO()),
                self.assertRaises(SystemExit),
            ):
                parse_args(command)
        self.assertEqual(parse_args(["write", "152", "-98.4"]).word, 0xFC28)

    def test_catalog_and_scaling(self):
        self.assertEqual(len(PARAMETERS), len(BY_ADDRESS))
        self.assertFalse(
            {153, 194, 195, 196, 197, 198, 200, 201, 238, 241, 251} & BY_ADDRESS.keys()
        )
        gamma = BY_ADDRESS[152]
        self.assertEqual(gamma.parse("-98.4"), 0xFC28)
        self.assertEqual(gamma.display(0xFC28), "-98.4 deg")
        self.assertEqual(gamma.parse("-984", raw=True), 0xFC28)
        self.assertEqual(gamma.parse("0xfc28", raw=True), 0xFC28)
        self.assertEqual(BY_ADDRESS[164].parse("0.01"), 100)
        self.assertEqual(BY_ADDRESS[177].parse("7"), 7)
        self.assertEqual(BY_ADDRESS[203].parse("3"), 3)
        self.assertEqual(BY_ADDRESS[172].parse("999"), 333)
        self.assertEqual(BY_ADDRESS[172].display(333), "999 ms")
        for address, value in [
            (100, "4000"),
            (152, "0.01"),
            (152, "360"),
            (140, "2"),
            (168, "0"),
            (169, "99"),
            (177, "8"),
            (100, "NaN"),
            (100, "Infinity"),
            (164, "6.5536"),
            (100, "1.0000000000000000000000000000001"),
            (100, "1e999999999"),
            (100, "1e-999999999"),
            (172, "998"),
            (125, "5"),
            (233, "1"),
            (235, "2"),
        ]:
            with (
                self.subTest(address=address, value=value),
                self.assertRaises(ValueError),
            ):
                BY_ADDRESS[address].parse(value)
        with self.assertRaises(ValueError):
            BY_ADDRESS[140].validate(True)

    def test_frames(self):
        payload = bytes.fromhex("9800010028fc0000")
        self.assertEqual(candapter_frame(0xC1, False, payload), "T0C189800010028FC0000")
        self.assertEqual(parse_frame(b"T0C189800010028FC0000"), (0xC1, False, payload))
        self.assertEqual(
            parse_frame(b"t0C189800010028FC0000ABCD"), (0xC1, False, payload)
        )
        self.assertEqual(
            parse_frame(b"X000000C189800010028FC0000"), (0xC1, True, payload)
        )
        self.assertEqual(
            parse_frame(b"T000000C189800010028FC0000"), (0xC1, True, payload)
        )
        for malformed in (
            b"",
            b"t0C29",
            b"t0C28FF",
            b"tFFF0",
            b"T0C28nothex!!",
            b"\xff",
        ):
            self.assertIsNone(parse_frame(malformed))
        with self.assertRaises(ValueError):
            candapter_frame(0x800, False, bytes(8))

    def test_setup_fragmented_reads_and_modes(self):
        for mode in ("standard", "extended", "j1939"):
            with self.subTest(mode=mode):
                bus, serial = adapter({152: 0xFC28}, fragment=1, mode=mode)
                bus.open()
                serial.inject(
                    0x22, bytes.fromhex("9700000001000000")
                )  # Wrong parameter.
                if mode != "j1939":
                    serial.inject(
                        0x22,
                        bytes.fromhex("9800000001000000"),
                        extended=mode == "standard",
                    )
                self.assertEqual(bus.transaction(152), 0xFC28)
                self.assertEqual(serial.requests[-1], bytes.fromhex("9800000000000000"))
                bus.transaction(152, 123)
                self.assertEqual(serial.requests[-1], bytes.fromhex("980001007b000000"))
                self.assertEqual(bus.transaction(152), 123)
                # Byte 2 is write-only per spec; a read must not depend on it.
                serial.silent = True
                serial.inject(0x22, bytes.fromhex("9800010063000000"))
                self.assertEqual(bus.transaction(152), 99)
                serial.silent = False
                bus.close()
                self.assertTrue(serial.closed)
                self.assertEqual(serial.writes[:3], [b"C\r", b"S6\r", b"O\r"])
                self.assertEqual(serial.writes[-1], b"C\r")

    def test_startup_close_bell_and_setup_failures(self):
        bus, serial = adapter()
        serial.reject_command = b"C\r"
        bus.open()
        self.assertEqual(serial.writes, [b"C\r", b"S6\r", b"O\r"])
        for command in ("S6", "O"):
            with self.subTest(command=command):
                bus, serial = adapter()
                serial.reject_command = (command + "\r").encode()
                with self.assertRaisesRegex(RuntimeError, repr(command) + " .*BELL"):
                    bus.open()

    def test_unsupported_bell_timeout_and_stale(self):
        bus, serial = adapter({100: 5})
        with self.assertRaises(Unsupported):
            bus.transaction(101)
        serial.reject = True
        with self.assertRaisesRegex(RuntimeError, "BELL"):
            bus.transaction(100)
        serial.reject = False
        serial.silent = True
        with self.assertRaises(TimeoutError):
            bus.transaction(100)
        self.assertTrue(bus.tainted)
        count = len(serial.requests)
        with self.assertRaisesRegex(RuntimeError, "reconnect"):
            bus.transaction(100)
        self.assertEqual(count, len(serial.requests))
        bus, serial = adapter({100: 5})
        serial.enabled = True
        serial.status_frame()
        with self.assertRaisesRegex(RuntimeError, "enabled"):
            bus.transaction(100, 6)
        self.assertFalse(serial.requests)
        serial.enabled = False
        serial.buffer.clear()
        bus.enabled = False
        bus.status_at = time.monotonic() - 2
        with self.assertRaisesRegex(RuntimeError, "stale"):
            bus.transaction(100, 6)

    def test_read_retries_but_write_does_not(self):
        bus, serial = adapter({100: 4250})
        serial.before_request = lambda address, writing: setattr(
            serial, "silent", len(serial.requests) < 3
        )
        self.assertEqual(bus.transaction(100), 4250)
        self.assertEqual(len(serial.requests), 3)
        bus, serial = adapter({100: 4250})
        serial.silent = True
        with self.assertRaises(TimeoutError):
            bus.transaction(100, 4200)
        self.assertEqual(len(serial.requests), 1)

        bus, serial = adapter({100: 4250})
        serial.before_request = lambda address, writing: setattr(
            serial, "reject", len(serial.requests) < 3
        )
        self.assertEqual(bus.transaction(100), 4250)
        self.assertEqual(len(serial.requests), 3)
        bus, serial = adapter({100: 4250})
        serial.reject = True
        with self.assertRaisesRegex(RuntimeError, "write parameter 100.*BELL"):
            bus.transaction(100, 4200)
        self.assertEqual(len(serial.requests), 1)

    def test_receive_drain_before_documented_read(self):
        bus, serial = adapter({100: 4250}, fragment=1)
        bus.args.request_gap = 0.03
        ready_at = time.monotonic() + 0.02
        serial.before_request = lambda address, writing: setattr(
            serial, "silent", time.monotonic() < ready_at
        )
        self.assertEqual(bus.transaction(100), 4250)
        self.assertEqual(len(serial.requests), 1)
        # CAN Protocol 6.3 pp. 14, 37–38: 0xC1, DLC 8, little-endian address.
        self.assertEqual(serial.writes[-1], b"T0C186400000000000000\r")

    def test_batch_verifies_and_stops_at_failure(self):
        instance, serial = worker({100: 5, 101: 8})
        instance.apply({100: (5, 6), 101: (8, 9)})
        self.assertEqual(serial.values, {100: 6, 101: 9})
        events = list(instance.events.queue)
        self.assertEqual(
            [event[1] for event in events if event[0] == "verified"], [100, 101]
        )
        instance, serial = worker({100: 5, 101: 8})
        serial.mismatch = True
        with self.assertRaisesRegex(RuntimeError, "mismatch"):
            instance.apply({100: (5, 6), 101: (8, 9)})
        self.assertEqual([r[:2] for r in serial.requests if r[2]], [b"d\x00"])
        self.assertEqual(serial.values[101], 8)
        self.assertTrue(
            any(e[0] == "value" and "unverified" in e[3] for e in instance.events.queue)
        )

    def test_conflict_motor_side_effect_and_cancellation(self):
        instance, serial = worker({100: 7})
        with self.assertRaisesRegex(RuntimeError, "conflict"):
            instance.apply({100: (5, 6)})
        self.assertFalse(any(r[2] for r in serial.requests))
        instance, serial = worker({150: 1, 106: 26})
        serial.after_write = lambda address: (
            serial.values.update({106: 35}) if address == 150 else None
        )
        with self.assertRaisesRegex(RuntimeError, "conflict"):
            instance.apply({106: (26, 27), 150: (1, 2)})
        self.assertEqual(
            [int.from_bytes(r[:2], "little") for r in serial.requests if r[2]], [150]
        )
        instance, serial = worker({100: 5, 101: 8})
        serial.after_write = lambda _: instance.cancel.set()
        with self.assertRaisesRegex(RuntimeError, "cancelled"):
            instance.apply({100: (5, 6), 101: (8, 9)})
        self.assertEqual(serial.values, {100: 6, 101: 8})
        self.assertTrue(
            any(e[0] == "verified" and e[1] == 100 for e in instance.events.queue)
        )

    def test_communication_settings(self):
        args = parse_args([])
        with self.assertRaises(ValueError):
            validate_changes({141: (160, 0x800)}, {144: 0, 171: 0}, args)
        validate_changes({141: (160, 0x800), 144: (0, 1)}, {171: 0}, args)
        with self.assertRaises(ValueError):
            validate_changes({171: (0, 1)}, {144: 0}, args)
        for address in (235, 236):
            with self.assertRaises(ValueError):
                validate_changes({address: (10, 2)}, {}, args)
        instance, serial = worker({100: 5, 141: 160, 144: 0, 171: 0})
        instance.apply({141: (160, 161), 100: (5, 6)})
        self.assertEqual(
            [int.from_bytes(r[:2], "little") for r in serial.requests if r[2]],
            [100, 141],
        )

    def test_rejection_and_status_change_stop_batch(self):
        instance, serial = worker({100: 5, 101: 8})
        serial.write_success = False
        with self.assertRaisesRegex(RuntimeError, "rejected write"):
            instance.apply({100: (5, 6), 101: (8, 9)})
        self.assertEqual(serial.values, {100: 5, 101: 8})
        result = next(e for e in instance.events.queue if e[0] == "batch")
        self.assertEqual(result[1:4], ([], [100], [101]))
        instance, serial = worker({100: 5, 101: 8})
        serial.after_write = lambda _: setattr(serial, "enabled", True)
        with self.assertRaisesRegex(RuntimeError, "enabled"):
            instance.apply({100: (5, 6), 101: (8, 9)})
        self.assertEqual(serial.values, {100: 6, 101: 8})
        result = next(e for e in instance.events.queue if e[0] == "batch")
        self.assertEqual(result[1:4], ([100], [], [101]))

    def test_temperature_constraints_and_connection_failure(self):
        args = parse_args(["--timeout", "0.04", "--request-gap", "0"])
        validate_changes({114: (700, 750)}, {113: 800, 115: 600}, args)
        with self.assertRaises(ValueError):
            validate_changes({114: (700, 900)}, {113: 800, 115: 600}, args)
        serial = FakeSerial(args)
        serial.reject_command = b"O\r"
        original_write = serial.write

        def write_with_cleanup_timeout(data):
            if data == b"C\r" and data in serial.writes:
                serial.writes.append(data)
                return len(data)
            return original_write(data)

        serial.write = write_with_cleanup_timeout  # ty: ignore[invalid-assignment]
        instance = Worker(args, serial_factory=lambda *a, **kw: serial)
        instance.start()
        deadline = time.monotonic() + 1
        while not serial.closed and time.monotonic() < deadline:
            time.sleep(0.001)
        instance.stop.set()
        instance.join(timeout=1)
        self.assertTrue(serial.closed)
        self.assertIsNone(instance.adapter)
        self.assertFalse(instance.is_alive())
        messages = [e[1] for e in instance.events.queue if e[0] == "message"]
        self.assertTrue(any("CAN cleanup:" in message for message in messages))
        self.assertEqual(messages[-1], "CANdapter rejected command 'O' (BELL)")

    def test_files_and_staging(self):
        args = parse_args([])
        observations = {p.address: Observation() for p in PARAMETERS}
        observations[152] = Observation(0xFC28, "ok", "2026-09-30T00:00:00+00:00")
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "snapshot.json"
            save_snapshot(path, snapshot(args, observations, None))
            values, issues = import_values(path)
            self.assertEqual(values, {152: 0xFC28})
            self.assertTrue(issues)
            self.assertFalse(json.loads(path.read_text())["complete"])
            row = {"address": 152, "raw_word": 0xFC28, "status": "ok"}
            path.write_text(
                json.dumps(
                    {"format": "pm100dx-eeprom", "version": 1, "parameters": [row, row]}
                )
            )
            values, issues = import_values(path)
            self.assertFalse(values)
            self.assertTrue(any("duplicate" in issue for issue in issues))
            row["status"] = "stale"
            path.write_text(
                json.dumps(
                    {
                        "format": "pm100dx-eeprom",
                        "version": 1,
                        "parameters": [row, {**row, "status": "ok"}],
                    }
                )
            )
            self.assertFalse(import_values(path)[0])
            path.write_text(
                "#decimal:\nGamma_Adjust_EEPROM_(Deg)_x_10 , -984\nCAN_OBD2_Enable_EEPROM,7\nUser_EEPROM_1,0\nCAN_Bit_Rate_EEPROM_(kbps),123\n"
            )
            values, issues = import_values(path)
            self.assertEqual(values, {152: 0xFC28, 177: 7})
            self.assertEqual(len(issues), 2)
            path.write_text(
                "#decimal:\nCAN_Bit_Rate_EEPROM_(kbps),500\nCAN_Bit_Rate_EEPROM_(kbps),250\n"
            )
            self.assertFalse(import_values(path)[0])
        instance, serial = worker({100: 5})
        tui = TUI(instance)
        with self.assertRaises(ValueError):
            tui.stage(100, 6)
        tui.observations[100] = Observation(5, "ok", "now")
        tui.stage(100, 6)
        self.assertEqual(tui.staged, {100: (5, 6)})
        self.assertFalse(serial.requests)
        instance.emit("value", 100, 7, "ok", "later")
        tui.events()
        self.assertEqual(tui.staged, {100: (5, 6)})
        instance.emit("connected", False)
        tui.events()
        self.assertIn("stale", tui.observations[100].status)


if __name__ == "__main__":
    unittest.main()
