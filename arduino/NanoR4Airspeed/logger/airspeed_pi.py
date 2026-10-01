"""Headless ASPD-4525 acquisition on Linux I2C; no Arduino or Tk required."""

import argparse
import fcntl
import json
import logging
import math
import os
import signal
import socket
import threading
import time
from pathlib import Path

from airspeed_core import QUANTUM_PA, RunWriter, ZeroCalibration, calculate, stamp, utc, validate_rate

LOG = logging.getLogger(__name__)


def read_sensor(bus, address):
    from smbus2 import i2c_msg

    # Plain read: this sensor has no register address to write first.
    message = i2c_msg.read(address, 2)
    bus.i2c_rdwr(message)
    high, low = list(message)
    return ((high & 0x3F) << 8) | low, high >> 6


def load_calibration(path, bus, address):
    if not path.exists():
        LOG.warning("No zero calibration at %s; recording pressure only", path)
        return None
    cal = json.loads(path.read_text())
    if cal.get("i2c_bus") != bus or cal.get("i2c_address") != address:
        raise ValueError("Calibration belongs to a different I2C bus/address")
    for key in ("zero_offset_pa", "noise_std_pa"):
        value = cal[key]
        if not isinstance(value, (int, float)) or not math.isfinite(value):
            raise ValueError(f"Invalid calibration {key}")
    if not 0 <= cal["noise_std_pa"] <= 10 or abs(cal["zero_offset_pa"]) > 6894.757:
        raise ValueError("Calibration is outside the sensor limits")
    if not isinstance(cal.get("calibration_id"), str) or not cal["calibration_id"]:
        raise ValueError("Calibration ID is missing")
    return cal


def sample_row(raw, status, cal, density):
    pa, dp, speed, state = calculate(raw, status, cal, density)
    row = dict(raw_counts=raw, sensor_status=status, pressure_pa=pa,
               corrected_pressure_pa=dp, airspeed_m_s=speed,
               airspeed_km_h=speed * 3.6 if speed is not None else None,
               reading_state=state)
    if cal:
        row.update(zero_offset_pa=cal["zero_offset_pa"],
                   zero_noise_band_pa=max(3 * cal["noise_std_pa"], 2 * QUANTUM_PA),
                   calibration_id=cal["calibration_id"])
    return row


def poll_control(control, state):
    """Same local datagram request/response format as the VectorNav logger."""
    try:
        data, address = control.recvfrom(4096)
    except BlockingIOError:
        return False
    try:
        request = json.loads(data)
        command = request.get("command")
        start_zero = command == "zero" and state["state"] != "zeroing"
        success = command == "status" or start_zero
        response = dict(request_id=request.get("request_id"), success=success,
                        airspeed=dict(state))
        if not success:
            response["error"] = "Zero already in progress" if command == "zero" else "Unknown command"
        if start_zero:
            response["airspeed"] = dict(state="zeroing", samples=0, elapsed_s=0,
                                        message="Keep both ports at equal pressure in still air")
        control.sendto(json.dumps(response).encode(), address)
        return start_zero
    except (ValueError, AttributeError, OSError):
        LOG.warning("Invalid or abandoned airspeed control request")
        return False


def acquire(args, bus, stop, control=None):
    cal = None if args.zero else load_calibration(args.calibration, args.bus, args.address)
    zero = ZeroCalibration() if args.zero else None
    writer = None if args.zero else RunWriter(args.output_dir, args.title, args.rate)
    previous_cal = cal
    state = dict(state="ready" if cal else "needs_zero", samples=0, elapsed_s=0,
                 message="Zero loaded" if cal else "Zero in still air before calculating speed")
    period = 1 / args.rate
    started = deadline = last_sync = time.monotonic()
    skipped = sequence = failures = 0
    if writer:
        LOG.info("Recording %s at requested %d Hz", writer.path, args.rate)
    try:
        while not stop.is_set():
            if control and poll_control(control, state):
                previous_cal = cal
                cal = None
                zero = ZeroCalibration()
                state.update(state="zeroing", samples=0, elapsed_s=0,
                             message="Keep both ports at equal pressure in still air")
            if stop.wait(max(0, deadline - time.monotonic())):
                break
            now = time.monotonic()
            late = max(0, int((now - deadline) / period))
            skipped += late
            deadline += (late + 1) * period
            sequence += 1
            received = utc()
            row = dict(received_utc=received, sample_utc=received,
                       device_sequence=sequence, device_elapsed_s=now - started,
                       requested_rate_hz=args.rate, device_skipped_slots=skipped,
                       missing_serial_samples=0, density_kg_m3=args.density)
            try:
                raw, status = read_sensor(bus, args.address)
            except OSError as exc:
                failures += 1
                row.update(reading_state="read_failed", source_line=str(exc))
                if failures == 1:
                    LOG.warning("I2C read failed: %s", exc)
            else:
                failures = 0
                if zero:
                    try:
                        candidate = zero.add(raw, status, now)
                        state.update(samples=len(zero.samples), elapsed_s=round(now - zero.started, 1))
                        if candidate:
                            candidate.update(i2c_bus=args.bus, i2c_address=args.address,
                                             calibration_id="zero-calibration-" + stamp())
                            args.calibration.parent.mkdir(parents=True, exist_ok=True)
                            temporary = args.calibration.with_suffix(".tmp")
                            with temporary.open("w") as target:
                                target.write(json.dumps(candidate, indent=2) + "\n")
                                target.flush()
                                os.fsync(target.fileno())
                            temporary.replace(args.calibration)
                            cal = candidate
                            zero = None
                            state.update(state="ready", message="Airspeed zero saved")
                            LOG.info("Zero saved to %s", args.calibration)
                            if args.zero:
                                return
                    except (ValueError, OSError) as exc:
                        if args.zero:
                            raise
                        zero = None
                        cal = previous_cal
                        state.update(state="error", message=str(exc))
                        LOG.warning("Zero failed; previous calibration retained: %s", exc)
                row.update(sample_row(raw, status, cal, args.density))
                row["source_line"] = f"I2C,{args.bus},{args.address:#x},{raw},{status}"
            if zero and now - zero.started > 45:
                if args.zero:
                    raise ValueError("Zero timed out; check sensor and keep both ports at equal pressure")
                zero = None
                cal = previous_cal
                state.update(state="error", message="Zero timed out; previous calibration retained")
            if writer:
                writer.append(row)
                if now - last_sync >= 5:
                    writer.file.flush()
                    os.fsync(writer.file.fileno())
                    last_sync = now
                    LOG.info("Rows=%d, measured=%.1f Hz, skipped=%d, state=%s",
                             writer.rows, writer.rows / (now - started), skipped, row["reading_state"])
            if failures >= 3:
                raise RuntimeError("Three consecutive I2C failures; check wiring/power")
        if zero:
            LOG.info("Zero cancelled; previous calibration file was preserved")
    finally:
        if writer:
            try:
                writer.file.flush()
                os.fsync(writer.file.fileno())
            finally:
                writer.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bus", type=int, default=1)
    parser.add_argument("--address", type=lambda s: int(s, 0), default=0x28)
    parser.add_argument("--rate", type=validate_rate, default=20)
    parser.add_argument("--density", type=float, default=1.225)
    parser.add_argument("--output-dir", type=Path, default=Path("/var/lib/airspeed/logs"))
    parser.add_argument("--calibration", type=Path, default=Path("/var/lib/airspeed/zero.json"))
    parser.add_argument("--title", default="Pi airspeed")
    parser.add_argument("--zero", action="store_true", help="Collect a still-air zero, save it, and exit")
    parser.add_argument("--control-socket", type=Path, help="Local dashboard control socket (service only)")
    args = parser.parse_args(argv)
    if args.bus < 0 or not 0x08 <= args.address <= 0x77:
        parser.error("Use a nonnegative bus and a 7-bit I2C address in 0x08..0x77")
    if not math.isfinite(args.density) or args.density <= 0:
        parser.error("Density must be finite and positive")
    logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
    stop = threading.Event()
    previous = {sig: signal.signal(sig, lambda *_: stop.set())
                for sig in (signal.SIGINT, signal.SIGTERM)}
    control = None
    try:
        from smbus2 import SMBus

        args.output_dir.mkdir(parents=True, exist_ok=True)
        # Same directory for manual runs, calibration, and the service.
        with (args.output_dir / ".airspeed-logger.lock").open("a") as lock:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
            with SMBus(args.bus) as bus:
                stop.wait(0.1)  # Sensor startup allowance.
                if args.control_socket and not args.zero:
                    control = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
                    args.control_socket.unlink(missing_ok=True)
                    control.bind(str(args.control_socket))
                    os.chmod(args.control_socket, 0o660)
                    control.setblocking(False)
                acquire(args, bus, stop, control)
        return 0
    except (OSError, ValueError, KeyError, TypeError, RuntimeError, ImportError):
        LOG.exception("Airspeed logger stopped")
        return 1
    finally:
        if control:
            control.close()
            args.control_socket.unlink(missing_ok=True)
        for sig, handler in previous.items():
            signal.signal(sig, handler)


if __name__ == "__main__":
    raise SystemExit(main())
