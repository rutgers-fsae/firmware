#!/usr/bin/env python3
"""Read, stage, and verify PM100DX EEPROM settings through a CANdapter."""

from __future__ import annotations

import argparse
import csv
import curses
import json
import os
import queue
import struct
import sys
import tempfile
import textwrap
import threading
import time
from dataclasses import dataclass
from datetime import datetime, timezone
from decimal import Decimal, InvalidOperation
from pathlib import Path

DEFAULT_PORT = "/dev/cu.usbserial-DN7BORLC"
SERIAL_BAUD = 115200
SPEED_CODES = {125: 4, 250: 5, 500: 6, 1000: 8}
IMMEDIATE = {
    106,
    107,
    108,
    109,
    142,
    151,
    152,
    160,
    161,
    162,
    163,
    164,
    165,
    166,
    167,
    168,
    169,
    203,
}
COMMUNICATION = {141, 144, 145, 147, 148, 171, 235, 236, 237}


@dataclass(frozen=True)
class Parameter:
    address: int
    name: str
    alias: str
    signed: bool = False
    scale: int = 1
    unit: str = ""
    minimum: int | None = None
    maximum: int | None = None
    choices: tuple[int, ...] = ()
    restriction: str = ""
    multiplier: int = 1

    def integer(self, word: int) -> int:
        return word - 65536 if self.signed and word >= 32768 else word

    def number(self, word: int) -> Decimal:
        return Decimal(self.integer(word)) * self.multiplier / self.scale

    def display(self, word: int) -> str:
        return f"{self.number(word):f}" + (f" {self.unit}" if self.unit else "")

    def validate(self, word: int) -> int:
        if type(word) is not int or not 0 <= word <= 65535:
            raise ValueError("raw word must be an integer between 0 and 65535")
        value = self.integer(word)
        lo = (
            self.minimum if self.minimum is not None else (-32768 if self.signed else 0)
        )
        hi = (
            self.maximum
            if self.maximum is not None
            else (32767 if self.signed else 65535)
        )
        if not lo <= value <= hi or (self.choices and value not in self.choices):
            raise ValueError(
                f"allowed raw values: {self.choices or f'{lo} through {hi}'}"
            )
        if self.address == 233 and 0 < value < 0x22:
            raise ValueError("slave command ID must be zero or 0x22 through 0x7FD")
        if self.address in (235, 236) and 0 < value < 3:
            raise ValueError(
                "CAN message periods must be zero (disabled) or at least 3 ms"
            )
        return word

    def parse(self, text: str, raw: bool = False) -> int:
        if raw:
            value = (
                int(text, 16)
                if text.lower().startswith(("0x", "-0x"))
                else int(text, 10)
            )
            if value < 0:
                if not self.signed or value < -32768:
                    raise ValueError(
                        "negative raw value is outside the signed word range"
                    )
                value &= 0xFFFF
            return self.validate(value)
        try:
            if len(text) > 128:
                raise ValueError("number is too long")
            value = Decimal(text)
            quantum = Decimal(self.multiplier) / self.scale
            if not value.is_finite() or value.copy_abs() > 65535 * quantum:
                raise ValueError("value is outside the word range")
            if value and value.copy_abs() < quantum:
                raise ValueError(f"value must be an exact multiple of {quantum}")
            numerator, denominator = value.as_integer_ratio()
            integer, remainder = divmod(
                numerator * self.scale, denominator * self.multiplier
            )
            if remainder:
                raise ValueError(f"value must be an exact multiple of {quantum}")
            if self.signed and not -32768 <= integer <= 32767:
                raise ValueError("value is outside the signed word range")
            if integer < 0:
                if not self.signed or integer < -32768:
                    raise ValueError("value is outside the signed word range")
                integer &= 0xFFFF
            return self.validate(integer)
        except InvalidOperation as error:
            raise ValueError("enter a decimal number") from error


# CAN Protocol 6.3, section 2.3.4. Aliases are explicit RMS GUI export names.
# Table fields: address, label, alias, signed, scale, units, min/max, choices, restriction.
PARAMETERS = [
    Parameter(100, "Iq limit", "IQ_Limit_EEPROM_(Amps)_x_10", True, 10, "A"),
    Parameter(101, "Id limit", "ID_Limit_EEPROM_(Amps)_x_10", True, 10, "A"),
    Parameter(102, "DC voltage limit", "DC_Volt_Limit_EEPROM_(V)_x_10", True, 10, "V"),
    Parameter(
        103, "DC voltage hysteresis", "DC_Volt_Hyst_EEPROM_(V)_x_10", True, 10, "V"
    ),
    Parameter(
        104,
        "DC undervoltage limit",
        "DC_UnderVolt_Thresh_EEPROM_(V)_x_10",
        True,
        10,
        "V",
    ),
    Parameter(106, "Vehicle flux", "Veh_Flux_EEPROM_(Wb)_x_1000", True, 1000, "Wb"),
    Parameter(107, "Ia ADC offset", "Ia_Offset_EEPROM", maximum=4095),
    Parameter(108, "Ib ADC offset", "Ib_Offset_EEPROM", maximum=4095),
    Parameter(109, "Ic ADC offset", "Ic_Offset_EEPROM", maximum=4095),
    Parameter(111, "Motor overspeed", "Motor_Overspeed_EEPROM_(RPM)", True, 1, "rpm"),
    Parameter(
        112,
        "Inverter overtemperature",
        "Inv_OverTemp_Limit_EEPROM_(C)_x_10",
        True,
        10,
        "C",
    ),
    Parameter(
        113,
        "Motor overtemperature",
        "Mtr_OverTemp_Limit_EEPROM_(C)_x_10",
        True,
        10,
        "C",
    ),
    Parameter(
        114,
        "Zero torque temperature",
        "Zero_Torque_Temp_EEPROM_(C)_x_10",
        True,
        10,
        "C",
    ),
    Parameter(
        115,
        "Full torque temperature",
        "Full_Torque_Temp_EEPROM_(C)_x_10",
        True,
        10,
        "C",
    ),
    Parameter(120, "Accelerator low", "Pedal_Lo_EEPROM_(V)_x_100", True, 100, "V"),
    Parameter(121, "Accelerator minimum", "Accel_Min_EEPROM_(V)_x_100", True, 100, "V"),
    Parameter(
        122, "Accelerator coast low", "Coast_Lo_EEPROM_(V)_x_100", True, 100, "V"
    ),
    Parameter(
        123, "Accelerator coast high", "Coast_Hi_EEPROM_(V)_x_100", True, 100, "V"
    ),
    Parameter(124, "Accelerator maximum", "Accel_Max_EEPROM_(V)_x_100", True, 100, "V"),
    Parameter(
        125,
        "Accelerator high",
        "Pedal_Hi_EEPROM_(V)_x_100",
        True,
        100,
        "V",
        maximum=499,
    ),
    Parameter(126, "Regen fade speed", "Regen_Fade_Speed_EEPROM_(RPM)", True, 1, "rpm"),
    Parameter(127, "Break speed", "Break_Speed_EEPROM_(RPM)", True, 1, "rpm"),
    Parameter(128, "Maximum speed", "Max_Speed_EEPROM_(RPM)", True, 1, "rpm"),
    Parameter(
        129, "Motor torque limit", "Motor_Torque_Limit_EEPROM_(Nm)_x_10", True, 10, "Nm"
    ),
    Parameter(
        130, "Regen torque limit", "Regen_Torque_Limit_EEPROM_(Nm)_x_10", True, 10, "Nm"
    ),
    Parameter(
        131,
        "Braking torque limit",
        "Braking_Torque_Limit_EEPROM_(Nm)_x_10",
        True,
        10,
        "Nm",
    ),
    Parameter(
        132,
        "Accelerator flipped",
        "Accel_Pedal_Flipped_EEPROM_(0=N_1=Y)",
        choices=(0, 1),
    ),
    Parameter(
        140, "Precharge bypassed", "Precharge_Bypassed_EEPROM_(0=N_1=Y)", choices=(0, 1)
    ),
    Parameter(141, "CAN ID offset", "CAN_ID_Offset_EEPROM", maximum=0xFFC0),
    Parameter(
        142,
        "Run mode (0 torque, 1 speed)",
        "Run_Mode_EEPROM(Trq=0_Spd=1)",
        choices=(0, 1),
    ),
    Parameter(
        143,
        "Command mode (0 CAN, 1 VSM)",
        "Inv_Cmd_Mode_EEPROM(CAN=0_VSM=1)",
        choices=(0, 1),
    ),
    Parameter(
        144,
        "Extended CAN identifiers",
        "CAN_Extended_Msg_ID_EEPROM(0=N_1=Y)",
        choices=(0, 1),
    ),
    Parameter(
        145,
        "CAN termination resistor",
        "CAN_Term_Resistor_Present_EEPROM",
        choices=(0, 1),
    ),
    Parameter(
        146,
        "CAN command timeout enabled",
        "CAN_Command_Message_Active_EEPROM",
        choices=(0, 1),
    ),
    Parameter(
        147,
        "CAN bitrate",
        "CAN_Bit_Rate_EEPROM_(kbps)",
        unit="kbit/s",
        choices=(125, 250, 500, 1000),
    ),
    Parameter(148, "CAN active messages low word", "CAN_ACTIVE_MSGS_EEPROM_(Lo_Word)"),
    Parameter(149, "Key switch mode", "Key_Switch_Mode_EEPROM", choices=(0, 1)),
    Parameter(150, "Motor parameter set", "Motor_Type_EEPROM", maximum=255),
    Parameter(
        151, "Resolver PWM delay", "Resolver_PWM_Delay_EEPROM_(Counts)", maximum=6250
    ),
    Parameter(
        152,
        "Gamma adjust",
        "Gamma_Adjust_EEPROM_(Deg)_x_10",
        True,
        10,
        "deg",
        -3599,
        3599,
    ),
    Parameter(
        154, "Sine voltage offset", "Sin_Offset_EEPROM_(Voltsx100)", True, 100, "V"
    ),
    Parameter(
        155, "Cosine voltage offset", "Cos_Offset_EEPROM_(Voltsx100)", True, 100, "V"
    ),
    Parameter(156, "Sine ADC offset", "Sin_Offset_EEPROM_(ADC_Counts)", maximum=4095),
    Parameter(157, "Cosine ADC offset", "Cos_Offset_EEPROM_(ADC_Counts)", maximum=4095),
    Parameter(
        158,
        "CAN diagnostic broadcast",
        "CAN_Diag_Data_Tx_Active_EEPROM",
        choices=(0, 1),
    ),
    Parameter(
        159,
        "CAN inverter enable switch",
        "CAN_Inv_Enab_Switch_Active_EEPROM",
        choices=(0, 1),
    ),
    Parameter(160, "Speed proportional gain", "Kp_Speed_EEPROM_x_100", scale=100),
    Parameter(161, "Speed integral gain", "Ki_Speed_EEPROM_x_10000", scale=10000),
    Parameter(162, "Speed derivative gain", "Kd_Speed_EEPROM_x_100", scale=100),
    Parameter(163, "Speed low-pass gain", "Klp_Speed_EEPROM_x_10000", scale=10000),
    Parameter(164, "Torque proportional gain", "Kp_Torque_EEPROM_x_10000", scale=10000),
    Parameter(165, "Torque integral gain", "Ki_Torque_EEPROM_x_10000", scale=10000),
    Parameter(166, "Torque derivative gain", "Kd_Torque_EEPROM_x_100", scale=100),
    Parameter(167, "Torque low-pass gain", "Klp_Torque_EEPROM_x_10000", scale=10000),
    Parameter(
        168,
        "Torque rate limit",
        "Torque_Rate_Limit_EEPROM_(Nm)_x_10",
        True,
        10,
        "Nm",
        1,
        2500,
    ),
    Parameter(
        169,
        "Speed rate limit",
        "Speed_Rate_Limit_EEPROM_(RPM/sec)",
        True,
        1,
        "rpm/s",
        100,
        5100,
    ),
    Parameter(170, "Relay output state", "Relay_Output_State_EEPROM_(0=OFF_1=ON)"),
    Parameter(
        171, "CAN J1939 format", "CAN_J1939_Option_Active_EEPROM", choices=(0, 1)
    ),
    Parameter(172, "CAN timeout", "CAN_TimeOut_(/3ms)_EEPROM", unit="ms", multiplier=3),
    Parameter(173, "Discharge enabled", "Discharge_Enable_EEPROM"),
    Parameter(174, "Serial number", "Serial_Number_EEPROM"),
    Parameter(
        177, "OBD2 offset (0 disabled)", "CAN_OBD2_Enable_EEPROM", minimum=0, maximum=7
    ),
    Parameter(
        178, "CAN BMS limit enabled", "CAN_BMS_Limit_Enable_EEPROM", choices=(0, 1)
    ),
    Parameter(
        180,
        "Brake mode (0 switch, 1 pot)",
        "Brake_Mode_EEPROM_(0=SWITCH_1=POT)",
        choices=(0, 1),
    ),
    Parameter(181, "Brake low", "Brake_Lo_EEPROM_(V)_x_100", True, 100, "V"),
    Parameter(182, "Brake minimum", "Brake_Min_EEPROM_(V)_x_100", True, 100, "V"),
    Parameter(183, "Brake maximum", "Brake_Max_EEPROM_(V)_x_100", True, 100, "V"),
    Parameter(184, "Brake high", "Brake_Hi_EEPROM_(V)_x_100", True, 100, "V"),
    Parameter(
        185,
        "Regen ramp period",
        "Regen_Ramp_Rate_EEPROM_(Sec)_x_1000",
        scale=1000,
        unit="s",
    ),
    Parameter(
        186,
        "Brake pedal flipped",
        "Brake_Pedal_Flipped_EEPROM_(0=N_1=Y)",
        choices=(0, 1),
    ),
    Parameter(
        187,
        "Shudder compensation enabled",
        "Shudder_Compensation_Enable_EEPROM",
        choices=(0, 1),
    ),
    Parameter(188, "Shudder proportional gain", "Kp_Shudder_EEPROM_x_100", scale=100),
    Parameter(
        189, "Shudder torque clamp", "TCLAMP_Shudder_EEPROM_(Nm)_x_10", True, 10, "Nm"
    ),
    Parameter(
        190,
        "Shudder filter frequency",
        "Shudder_Filter_Freq_EEPROM_(Hz)_x_10",
        True,
        10,
        "Hz",
    ),
    Parameter(
        191, "Shudder fade speed", "Shudder_Speed_Fade_EEPROM_(RPM)", True, 1, "rpm"
    ),
    Parameter(
        192, "Shudder speed low", "Shudder_Speed_Lo_EEPROM_(RPM)", True, 1, "rpm"
    ),
    Parameter(
        193, "Shudder speed high", "Shudder_Speed_Hi_EEPROM_(RPM)", True, 1, "rpm"
    ),
    Parameter(
        199, "Brake input bypassed", "Brake_Switch_Bypassed_EEPROM", choices=(0, 1)
    ),
    Parameter(
        203,
        "RTD selection bitmask",
        "RTD_Selection_EEPROM_(BITS_1_0)",
        maximum=3,
        restriction="Gen 3 only",
    ),
    Parameter(
        204,
        "Analog output function",
        "Analog_Output_Function_Select_EEPROM",
        restriction="Gen 3 only",
    ),
    Parameter(233, "Slave command CAN ID", "CAN_Slave_Cmd_ID_EEPROM", maximum=0x7FD),
    Parameter(
        234,
        "Slave direction flipped",
        "CAN_Slave_Dir_EEPROM_(0=SAME_1=FLIP)",
        choices=(0, 1),
    ),
    Parameter(
        235,
        "CAN fast message period",
        "CAN_Fast_Msg_Rate_EEPROM_(ms)",
        unit="ms",
        restriction="Firmware >= 2025",
    ),
    Parameter(
        236,
        "CAN slow message period",
        "CAN_Slow_Msg_Rate_EEPROM_(ms)",
        unit="ms",
        restriction="Firmware >= 2025",
    ),
    Parameter(
        237,
        "CAN active messages high word",
        "CAN_ACTIVE_MSGS_EEPROM_(Hi_Word)",
        restriction="Firmware >= 2025",
    ),
]
BY_ADDRESS = {p.address: p for p in PARAMETERS}
BY_ALIAS = {p.alias.casefold(): p for p in PARAMETERS}


def candapter_frame(can_id: int, extended: bool, data: bytes) -> str:
    """Same transmit framing as ../scripts/precharge_can_sim.py."""
    if not 0 <= can_id <= (0x1FFFFFFF if extended else 0x7FF):
        raise ValueError("CAN ID is outside its frame type")
    if len(data) > 8:
        raise ValueError("CAN payload cannot exceed eight bytes")
    return f"{'X' if extended else 'T'}{can_id:0{8 if extended else 3}X}{len(data):X}{data.hex().upper()}"


def parse_frame(line: bytes) -> tuple[int, bool, bytes] | None:
    """CANdapter T/X and lowercase receive forms; also SLCAN t/T."""
    try:
        text = line.decode("ascii")
        if not text or text[0] not in "tTxX":
            return None
        widths = (3,) if text[0] == "t" else ((8,) if text[0] in "xX" else (3, 8))
        for width in widths:
            if len(text) < width + 2:
                continue
            dlc = int(text[width + 1], 16)
            end = width + 2 + 2 * dlc
            if dlc > 8 or len(text) not in (end, end + 4):
                continue
            identifier = int(text[1 : width + 1], 16)
            data = bytes.fromhex(text[width + 2 : end])
            if len(data) != dlc or identifier > (0x7FF if width == 3 else 0x1FFFFFFF):
                continue
            if len(text) == end + 4:
                int(text[end:], 16)  # Optional adapter timestamp.
            return identifier, width == 8, data
    except (ValueError, UnicodeError):
        pass
    return None


class Unsupported(ValueError):
    pass


class Cancelled(RuntimeError):
    pass


class Candapter:
    def __init__(self, serial_port, args):
        self.serial = serial_port
        self.args = args
        self.buffer = bytearray()
        self.tokens = []
        self.enabled = None
        self.status_at = 0.0
        self.firmware = None
        self.opened = False
        self.tainted = False

    def identifier(self, relative: int) -> int:
        identifier = self.args.base + relative
        return (
            (0x0CFF0001 | (identifier << 8))
            if self.args.mode == "j1939"
            else identifier
        )

    def pump(self) -> list[tuple[int, bool, bytes]]:
        self.buffer.extend(self.serial.read(self.serial.in_waiting or 1))
        frames = []
        while self.buffer:
            if self.buffer[0] == 10:
                del self.buffer[0]
                continue
            # ACK/BELL may stand alone, or precede a CR-terminated frame.
            if self.buffer[0] in (6, 7):
                self.tokens.append(self.buffer.pop(0))
                continue
            try:
                end = self.buffer.index(13)
            except ValueError:
                if len(self.buffer) > 128:
                    self.buffer.clear()
                break
            line = bytes(self.buffer[:end])
            del self.buffer[: end + 1]
            if not line:
                self.tokens.append(6)
                continue
            frame = parse_frame(line)
            if frame is None:
                continue
            identifier, extended, data = frame
            if extended == (self.args.mode != "standard") and len(data) == 8:
                if identifier == self.identifier(0x0A):
                    self.enabled = bool(data[6] & 1)
                    self.status_at = time.monotonic()
                elif identifier == self.identifier(0x0E):
                    project, version, mmdd, year = struct.unpack("<4H", data)
                    self.firmware = {
                        "project": project,
                        "version_hex": f"{version:04X}",
                        "date_mmdd": mmdd,
                        "date_year": year,
                    }
            frames.append(frame)
        return frames

    def rejected(self, command=None):
        rejected = 7 in self.tokens
        self.tokens.clear()
        if rejected:
            detail = f" {command!r}" if command is not None else ""
            raise RuntimeError(f"CANdapter rejected command{detail} (BELL)")

    def command(self, command: str, *, allow_bell=False):
        self.tokens.clear()
        self.serial.write((command + "\r").encode("ascii"))
        self.serial.flush()
        deadline = time.monotonic() + self.args.timeout
        while time.monotonic() < deadline:
            self.pump()
            if self.tokens:
                if allow_bell:
                    self.tokens.clear()
                else:
                    self.rejected(command)
                return
        raise TimeoutError(f"CANdapter did not acknowledge {command!r}")

    def open(self):
        self.serial.reset_input_buffer()
        # C returns BELL when the channel is already closed.
        self.command("C", allow_bell=True)
        self.command(f"S{SPEED_CODES[self.args.bitrate]}")
        # O may succeed even if its ACK is lost: attempt C on cleanup either way.
        self.opened = True
        self.command("O")

    def close(self):
        try:
            if self.opened:
                self.command("C")
        finally:
            self.serial.close()

    def disabled(self):
        self.pump()
        self.rejected()
        if self.enabled is True:
            raise RuntimeError("write blocked: inverter is enabled")
        if (
            self.enabled is None
            or time.monotonic() - self.status_at > self.args.freshness
        ):
            raise RuntimeError(
                "write blocked: disabled-state telemetry is missing or stale"
            )

    def transaction(self, address: int, word: int | None = None) -> int:
        if address not in BY_ADDRESS:
            raise ValueError("only documented PM EEPROM addresses can be requested")
        if word is not None:
            BY_ADDRESS[address].validate(word)
        if self.tainted:
            raise RuntimeError(
                "reconnect after a transaction timeout before further requests"
            )
        # Keep the USB FIFO draining while leaving time between parameter requests.
        deadline = time.monotonic() + self.args.request_gap
        while time.monotonic() < deadline:
            self.pump()
            self.rejected()
        self.pump()  # Discard old replies while retaining status telemetry.
        self.rejected()
        if word is not None:
            self.disabled()
        payload = struct.pack(
            "<HBBHBB", address, int(word is not None), 0, word or 0, 0, 0
        )
        request = (
            candapter_frame(
                self.identifier(0x21), self.args.mode != "standard", payload
            )
            + "\r"
        ).encode("ascii")
        self.serial.write(request)
        self.serial.flush()
        deadline = time.monotonic() + self.args.timeout
        retries = 2 if word is None else 0
        while True:
            if time.monotonic() >= deadline:
                if not retries:
                    break
                # A delayed reply still belongs to this same read; never retry writes.
                retries -= 1
                self.serial.write(request)
                self.serial.flush()
                deadline = time.monotonic() + self.args.timeout
            frames = self.pump()
            if 7 in self.tokens and word is None and retries:
                self.tokens.clear()
                # Drain traffic for 100 ms before retrying a rejected read.
                deadline = min(deadline, time.monotonic() + 0.1)
                continue
            self.rejected(
                f"{'write' if word is not None else 'read'} parameter {address}"
            )
            for identifier, extended, data in frames:
                if (
                    identifier != self.identifier(0x22)
                    or extended != (self.args.mode != "standard")
                    or len(data) != 8
                ):
                    continue
                returned_address, success, _, value, _, _ = struct.unpack(
                    "<HBBHBB", data
                )
                if returned_address == 0:
                    raise Unsupported(
                        f"parameter {address} is not supported by this inverter"
                    )
                if returned_address != address:
                    continue
                if word is not None:
                    if success != 1:
                        raise RuntimeError(
                            f"inverter rejected write to parameter {address}"
                        )
                    return value
                # Spec defines byte 2 only for writes; ignore it on reads.
                return value
        self.tainted = True
        raise TimeoutError(
            f"parameter {address} timed out; reconnect required (writes are never retried)"
        )


@dataclass
class Observation:
    word: int | None = None
    status: str = "unread"
    read_at: str | None = None


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat()


def snapshot(args, observations, firmware):
    return {
        "format": "pm100dx-eeprom",
        "version": 1,
        "captured_at": utc_now(),
        "connection": {
            key: getattr(args, key) for key in ("port", "bitrate", "base", "mode")
        },
        "firmware": firmware,
        "complete": all(o.status == "ok" for o in observations.values()),
        "parameters": [
            {
                "address": p.address,
                "name": p.alias,
                "raw_word": o.word,
                "value": p.display(o.word) if o.word is not None else None,
                "units": p.unit,
                "status": o.status,
                "read_at": o.read_at,
            }
            for p in PARAMETERS
            for o in [observations[p.address]]
        ],
    }


def save_snapshot(path: Path, data):
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="w",
            encoding="utf-8",
            dir=path.parent,
            prefix=f".{path.name}.",
            delete=False,
        ) as stream:
            temporary = Path(stream.name)
            json.dump(data, stream, indent=2, allow_nan=False)
            stream.write("\n")
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


def import_values(path: Path) -> tuple[dict[int, int], list[str]]:
    text = path.read_text(encoding="utf-8-sig")
    entries = []
    issues = []
    if text.lstrip().startswith(("{", "[")):
        data = json.loads(text)
        if (
            not isinstance(data, dict)
            or data.get("format") != "pm100dx-eeprom"
            or type(data.get("version")) is not int
            or data.get("version") != 1
        ):
            raise ValueError("not a supported version-1 PM100DX EEPROM snapshot")
        if not isinstance(data.get("parameters"), list):
            raise ValueError("snapshot parameters must be a list")
        for row in data["parameters"]:
            if not isinstance(row, dict) or type(row.get("address")) is not int:
                issues.append("invalid JSON parameter record")
                continue
            address = row["address"]
            parameter = BY_ADDRESS.get(address)
            if parameter is None:
                issues.append(f"unsupported address: {address}")
            else:
                entries.append(
                    (parameter, row.get("raw_word"), True, row.get("status") == "ok")
                )
    else:
        for number, line in enumerate(text.splitlines(), 1):
            if not line.strip() or line.lstrip().startswith("#"):
                continue
            row = next(csv.reader([line]))
            if len(row) != 2:
                issues.append(f"line {number}: expected name,value")
                continue
            parameter = BY_ALIAS.get(row[0].strip().casefold())
            if parameter is None:
                issues.append(f"unsupported name: {row[0].strip()}")
            else:
                entries.append((parameter, row[1].strip(), False, True))
    values = {}
    seen = set()
    duplicates = set()
    for parameter, value, json_word, valid_source in entries:
        address = parameter.address
        if address in seen:
            duplicates.add(address)
            values.pop(address, None)
            issues.append(f"duplicate parameter: {parameter.name}; omitted")
            continue
        seen.add(address)
        if not valid_source:
            issues.append(f"{parameter.name}: source value is missing or stale")
            continue
        try:
            values[address] = (
                parameter.validate(value)
                if json_word
                else parameter.parse(value, raw=True)
            )
        except (ValueError, TypeError) as error:
            issues.append(f"{parameter.name}: {error}")
    for address in duplicates:
        values.pop(address, None)
    return values, issues


def validate_changes(changes, current, args):
    """Validate the resulting CAN format and the explicitly documented special limits."""
    for address, (_, word) in changes.items():
        BY_ADDRESS[address].validate(word)
    resulting = dict(current)
    resulting.update({a: w for a, (_, w) in changes.items()})
    extended = resulting.get(144, int(args.mode != "standard"))
    j1939 = resulting.get(171, int(args.mode == "j1939"))
    base = resulting.get(141, args.base)
    if {141, 144, 171} & changes.keys():
        if j1939 and not extended:
            raise ValueError("J1939 requires extended CAN identifiers")
        maximum = 0xC0 if j1939 else (0xFFC0 if extended else 0x7C0)
        if base > maximum:
            raise ValueError(
                f"CAN base exceeds the resulting mode's maximum 0x{maximum:X}"
            )
    if {113, 114, 115} & changes.keys():
        if not all(a in resulting for a in (113, 114, 115)):
            raise ValueError(
                "read motor, zero-torque, and full-torque temperature limits first"
            )
        over, zero, full = (
            BY_ADDRESS[a].integer(resulting[a]) for a in (113, 114, 115)
        )
        if not full < zero < over:
            raise ValueError(
                "temperatures must satisfy full torque < zero torque < motor overtemperature"
            )


class Worker(threading.Thread):
    def __init__(self, args, serial_factory=None):
        super().__init__(daemon=True)
        self.args = args
        self.serial_factory = serial_factory
        self.jobs = queue.Queue()
        self.events = queue.Queue()
        self.cancel = threading.Event()
        self.stop = threading.Event()
        self.adapter = None
        self.values = {}

    def emit(self, kind, *values):
        self.events.put((kind, *values))

    def check_cancel(self):
        if self.cancel.is_set() or self.stop.is_set():
            raise Cancelled("operation cancelled; completed writes remain stored")

    def read(self, address):
        try:
            if self.adapter is None:
                raise RuntimeError("not connected; press c to reconnect")
            word = self.adapter.transaction(address)
            self.values[address] = word
            self.emit("value", address, word, "ok", utc_now())
            self.emit(
                "telemetry",
                self.adapter.enabled,
                self.adapter.status_at,
                self.adapter.firmware,
            )
            return word
        except Exception as error:
            self.emit("value", address, self.values.get(address), str(error), None)
            raise

    def scan(self, addresses):
        failed = 0
        for index, address in enumerate(addresses):
            self.check_cancel()
            self.emit(
                "message",
                f"Reading {index + 1}/{len(addresses)}: {BY_ADDRESS[address].name}",
            )
            try:
                self.read(address)
            except Unsupported:
                failed += 1
            except Exception:
                raise
        self.emit(
            "message",
            f"Read complete: {len(addresses) - failed} values, {failed} unsupported",
        )

    def apply(self, changes, *, refresh=True):
        adapter = self.adapter
        if adapter is None:
            raise RuntimeError("not connected; press c to reconnect")
        for address, (baseline, _) in changes.items():
            self.check_cancel()
            if self.read(address) != baseline:
                raise RuntimeError(
                    f"conflict: {BY_ADDRESS[address].name} changed since staging; reread and restage"
                )
        # CAN format validation needs fresh values, not a guess from the CLI.
        dependencies = set()
        if {141, 144, 171} & changes.keys():
            dependencies.update((141, 144, 171))
        if {113, 114, 115} & changes.keys():
            dependencies.update((113, 114, 115))
        for address in sorted(dependencies - changes.keys()):
            self.read(address)
        validate_changes(changes, self.values, self.args)
        order = sorted(changes, key=lambda a: (a in COMMUNICATION, a != 150, a))
        verified = []
        uncertain = []
        failure = None
        try:
            for address in order:
                self.check_cancel()
                baseline, desired = changes[address]
                # Motor-type writes can reset flux/gamma. Check again after that side effect.
                if self.read(address) != baseline:
                    raise RuntimeError(
                        f"conflict: {BY_ADDRESS[address].name} changed during the batch"
                    )
                self.emit("message", f"Writing {BY_ADDRESS[address].name}")
                attempted = False
                try:
                    adapter.disabled()
                    attempted = True
                    adapter.transaction(address, desired)
                    actual = self.read(address)
                    if actual != desired:
                        raise RuntimeError(
                            f"readback mismatch: wanted 0x{desired:04X}, got 0x{actual:04X}"
                        )
                except Exception as error:
                    if attempted:
                        uncertain.append(address)
                        self.emit(
                            "value",
                            address,
                            self.values.get(address),
                            f"write unverified: {error}",
                            None,
                        )
                    raise
                verified.append(address)
                self.emit("verified", address, desired)
                if address == 150 and refresh:
                    self.scan([p.address for p in PARAMETERS])
            if refresh:
                self.scan([p.address for p in PARAMETERS])
        except Exception as error:
            failure = str(error)
            raise
        finally:
            unattempted = [a for a in order if a not in verified and a not in uncertain]
            self.emit("batch", verified, uncertain, unattempted, failure)
            if verified:
                restart = [BY_ADDRESS[a].name for a in verified if a not in IMMEDIATE]
                connection = {
                    "base": self.values.get(141, self.args.base),
                    "bitrate": self.values.get(147, self.args.bitrate),
                    "mode": (
                        "j1939"
                        if self.values.get(171, self.args.mode == "j1939")
                        else "extended"
                        if self.values.get(144, self.args.mode != "standard")
                        else "standard"
                    ),
                }
                self.emit("restart", restart, connection)

    def connect(self, *, scan=True):
        self.disconnect()
        if self.serial_factory is None:
            import serial

            self.serial_factory = serial.Serial
        port = self.serial_factory(
            self.args.port,
            SERIAL_BAUD,
            timeout=0.002,
            write_timeout=self.args.timeout,
            exclusive=True,
        )
        self.adapter = Candapter(port, self.args)
        self.adapter.open()
        self.emit("connected", True)
        if scan:
            self.scan([p.address for p in PARAMETERS])

    def disconnect(self):
        if self.adapter is not None:
            try:
                self.adapter.close()
            except Exception as error:
                self.emit("message", f"CAN cleanup: {error}")
            self.adapter = None
        self.emit("connected", False)

    def run(self):
        self.jobs.put(("connect", None))
        try:
            while not self.stop.is_set():
                try:
                    kind, data = self.jobs.get(timeout=0.002)
                except queue.Empty:
                    if self.adapter:
                        try:
                            self.adapter.pump()
                            self.adapter.rejected()
                            self.emit(
                                "telemetry",
                                self.adapter.enabled,
                                self.adapter.status_at,
                                self.adapter.firmware,
                            )
                        except Exception as error:
                            self.disconnect()
                            self.emit("message", str(error))
                    continue
                self.emit("busy", True)
                try:
                    if kind == "connect":
                        self.connect(scan=False)
                        self.emit(
                            "message",
                            "Connected; r reads the selected value, R reads all",
                        )
                    elif self.adapter is None:
                        raise RuntimeError("not connected; press c to reconnect")
                    elif kind == "read":
                        self.scan(data)
                    elif kind == "apply":
                        self.apply(data, refresh=len(data) > 1)
                except Exception as error:
                    if self.adapter is not None and (
                        kind == "connect"
                        or self.adapter.tainted
                        or isinstance(error, OSError)
                    ):
                        self.disconnect()
                    self.emit("message", str(error))
                finally:
                    self.cancel.clear()
                    self.emit("busy", False)
        finally:
            self.disconnect()


def put(screen, y, x, text, attribute=0):
    height, width = screen.getmaxyx()
    if 0 <= y < height and 0 <= x < width - 1:
        try:
            screen.addnstr(y, x, str(text), width - x - 1, attribute)
        except curses.error:
            pass


def dialog(screen, title, lines):
    """Scrollable review/help dialog. Enter accepts the review; Esc cancels."""
    offset = 0
    while True:
        height, width = screen.getmaxyx()
        wrapped = [
            part
            for line in lines
            for part in (textwrap.wrap(line, max(1, width - 2)) or [""])
        ]
        visible = max(1, height - 4)
        offset = max(0, min(offset, len(wrapped) - visible))
        screen.erase()
        put(screen, 0, 0, title, curses.A_BOLD)
        for y, line in enumerate(wrapped[offset : offset + visible], 2):
            put(screen, y, 0, line)
        put(
            screen,
            height - 1,
            0,
            "j/k scroll | Ctrl-d/u page | Enter continue | Esc cancel",
        )
        screen.refresh()
        key = screen.getch()
        if key in (10, 13):
            return True
        if key == 27:
            return False
        if key in (ord("j"), curses.KEY_DOWN):
            offset += 1
        elif key in (ord("k"), curses.KEY_UP):
            offset -= 1
        elif key == 4:
            offset += visible
        elif key == 21:
            offset -= visible
        elif key == ord("G"):
            offset = len(wrapped)
        elif key == ord("g"):
            offset = 0


def prompt(screen, label, initial="", toggle=False):
    text = initial
    raw = False
    screen.timeout(-1)
    try:
        curses.curs_set(1)
        while True:
            height, width = screen.getmaxyx()
            mode = (
                " [RAW integer; Ctrl-r toggles]"
                if raw
                else " [engineering; Ctrl-r toggles]"
            )
            screen.erase()
            put(screen, 0, 0, label, curses.A_BOLD)
            put(screen, 2, 0, mode if toggle else "Enter accepts | Esc cancels")
            put(screen, 4, 0, "> " + text[-max(1, width - 4) :])
            if height > 4 and width > 3:
                screen.move(4, min(width - 2, 2 + len(text)))
            screen.refresh()
            key = screen.get_wch()
            if key in ("\n", "\r"):
                return text, raw
            if key == "\x1b":
                return None
            if key == "\x12" and toggle:
                raw = not raw
                text = ""
            elif key in (curses.KEY_BACKSPACE, "\x7f", "\b"):
                text = text[:-1]
            elif key == "\x15":
                text = ""
            elif isinstance(key, str) and key.isprintable():
                text += key
    finally:
        curses.curs_set(0)
        screen.timeout(100)


HELP = [
    "j/k or arrows: move | gg/G: first/last | Ctrl-u/d: half page",
    "/: search names, aliases, or addresses | n/N: next/previous match",
    "Enter/i: stage an edit; Ctrl-r switches engineering/raw entry",
    "r: reread selected | R: read all | u: unstage selected",
    "Connect does not scan; use r to read a value before editing it.",
    "w: review diff, then type WRITE to apply and verify sequentially",
    "e: export observed EEPROM to JSON | o: preview/import JSON or RMS text",
    "c: reconnect using CLI settings | Esc: cancel scan/batch between transactions",
    "?: this help | q: quit (confirm discarding staged changes)",
    "Writes require disabled-state telemetry no older than --freshness seconds.",
    "No motor control, fault clearing, automatic write retries, or rollback.",
    "Current means observed EEPROM, not necessarily the inverter's active settings.",
    "NOW: takes effect immediately. CYCLE: operator power cycle required.",
    "Gen 3-only entries are probed; unsupported entries remain visible.",
]


class TUI:
    def __init__(self, worker):
        self.worker = worker
        self.observations = {p.address: Observation() for p in PARAMETERS}
        self.staged = {}  # address -> (observed baseline, desired raw word)
        self.selected = 0
        self.top = 0
        self.query = ""
        self.previous_g = False
        self.busy = True
        self.connected = False
        self.enabled = None
        self.status_at = 0.0
        self.firmware = None
        self.message = "Connecting..."
        self.notices = []

    def events(self):
        while True:
            try:
                event = self.worker.events.get_nowait()
            except queue.Empty:
                return
            kind, *data = event
            if kind == "value":
                address, word, status, timestamp = data
                old = self.observations[address]
                self.observations[address] = Observation(
                    word, status, timestamp or old.read_at
                )
            elif kind == "verified":
                address, word = data
                if address in self.staged and self.staged[address][1] == word:
                    del self.staged[address]
            elif kind == "busy":
                self.busy = data[0]
            elif kind == "connected":
                self.connected = data[0]
                if not self.connected:
                    self.enabled, self.status_at = None, 0.0
                    for observation in self.observations.values():
                        if observation.word is not None:
                            observation.status = "stale: disconnected"
            elif kind == "telemetry":
                self.enabled, self.status_at, self.firmware = data
            elif kind == "message":
                self.message = data[0]
            elif kind == "batch":
                verified, uncertain, unattempted, failure = data
                for label, addresses in (
                    ("Verified", verified),
                    ("Unverified", uncertain),
                    ("Unattempted", unattempted),
                ):
                    self.notices.append(
                        f"{label}: {', '.join(BY_ADDRESS[a].name for a in addresses) or 'none'}"
                    )
                if failure:
                    self.notices.append(f"Stopped: {failure}")
            elif kind == "restart":
                names, settings = data
                if names:
                    self.notices.append("Power cycle required for: " + ", ".join(names))
                self.notices.append(
                    f"After power cycle: --base 0x{settings['base']:X} --bitrate {settings['bitrate']} --mode {settings['mode']}"
                )

    def job(self, kind, data):
        self.worker.cancel.clear()
        self.busy = True
        self.worker.jobs.put((kind, data))

    def stage(self, address, word):
        observation = self.observations[address]
        if observation.status != "ok" or observation.word is None:
            raise ValueError("read this parameter successfully before staging it")
        BY_ADDRESS[address].validate(word)
        if word == observation.word:
            self.staged.pop(address, None)
        else:
            self.staged[address] = (observation.word, word)

    def search(self, direction):
        for step in range(1, len(PARAMETERS) + 1):
            index = (self.selected + direction * step) % len(PARAMETERS)
            p = PARAMETERS[index]
            if (
                self.query.casefold()
                in f"{p.name} {p.alias} {p.address} 0x{p.address:X}".casefold()
            ):
                self.selected = index
                return
        self.message = "No search matches"

    def draw(self, screen):
        height, width = screen.getmaxyx()
        screen.erase()
        fresh = (
            self.connected
            and time.monotonic() - self.status_at <= self.worker.args.freshness
        )
        state = (
            ("ENABLED - writes blocked" if self.enabled else "disabled")
            if fresh
            else "unknown/stale - writes blocked"
        )
        put(
            screen,
            0,
            0,
            f"PM100DX EEPROM | {'connected' if self.connected else 'disconnected'} | {state}",
            curses.A_BOLD,
        )
        firmware = (
            f" | firmware {self.firmware['version_hex']}" if self.firmware else ""
        )
        put(
            screen,
            1,
            0,
            f"{self.worker.args.port} | {self.worker.args.bitrate} kbit/s | {self.worker.args.mode} base 0x{self.worker.args.base:X}{firmware}",
        )
        put(
            screen,
            2,
            0,
            f"{'BUSY' if self.busy else 'READY'} | {len(self.staged)} staged | NOW=immediate; CYCLE=power cycle; current=observed EEPROM",
        )
        name_width = 30 if width >= 115 else max(10, min(25, width - 60))
        value_width = 19 if width >= 115 else 13
        raw_width = 16 if width >= 115 else 12
        staged_width = 16 if width >= 115 else 13
        put(
            screen,
            4,
            0,
            f"Addr {'Parameter':{name_width}} {'Current':{value_width}} {'Raw dec/hex':{raw_width}} {'Staged':{staged_width}} Effect / Status",
            curses.A_UNDERLINE,
        )
        rows = max(1, height - 10)
        self.top = max(0, min(self.top, self.selected))
        if self.selected >= self.top + rows:
            self.top = self.selected - rows + 1
        for y, p in enumerate(PARAMETERS[self.top : self.top + rows], 5):
            observation = self.observations[p.address]
            current = (
                p.display(observation.word) if observation.word is not None else "--"
            )
            raw = (
                f"{p.integer(observation.word)}/{observation.word:04X}"
                if observation.word is not None
                else "--"
            )
            staged = (
                p.display(self.staged[p.address][1]) if p.address in self.staged else ""
            )
            effect = "NOW" if p.address in IMMEDIATE else "CYCLE"
            status = (
                observation.status
                if observation.status != "ok"
                else p.restriction or "ok"
            )
            line = f"{p.address:3}  {p.name[:name_width]:{name_width}} {current[:value_width]:{value_width}} {raw:{raw_width}} {staged[:staged_width]:{staged_width}} {effect} {status}"
            put(
                screen,
                y,
                0,
                line,
                curses.A_REVERSE
                if p.address == PARAMETERS[self.selected].address
                else 0,
            )
        selected = PARAMETERS[self.selected]
        put(
            screen,
            height - 4,
            0,
            selected.alias
            + (f" [{selected.restriction}]" if selected.restriction else ""),
        )
        observation = self.observations[selected.address]
        put(
            screen,
            height - 3,
            0,
            f"{'NOW' if selected.address in IMMEDIATE else 'CYCLE'} | {observation.status} | read {observation.read_at or 'never'}",
        )
        put(screen, height - 2, 0, self.message)
        put(
            screen,
            height - 1,
            0,
            "j/k move / search i edit r/R read u undo w apply e export o import c reconnect ? help q quit Esc cancel",
        )
        screen.refresh()

    def run(self, screen):
        curses.curs_set(0)
        screen.timeout(100)
        while True:
            self.events()
            if self.notices and not self.busy:
                notices, self.notices = self.notices, []
                screen.timeout(-1)
                dialog(screen, "Batch result", notices)
                screen.timeout(100)
            self.draw(screen)
            key = screen.getch()
            if key == -1:
                continue
            p = PARAMETERS[self.selected]
            try:
                if key in (ord("j"), curses.KEY_DOWN):
                    self.selected = min(len(PARAMETERS) - 1, self.selected + 1)
                elif key in (ord("k"), curses.KEY_UP):
                    self.selected = max(0, self.selected - 1)
                elif key in (4, 21):
                    self.selected = max(
                        0,
                        min(
                            len(PARAMETERS) - 1,
                            self.selected
                            + (1 if key == 4 else -1)
                            * max(1, (screen.getmaxyx()[0] - 9) // 2),
                        ),
                    )
                elif key == ord("g") and self.previous_g:
                    self.selected = 0
                elif key == ord("G"):
                    self.selected = len(PARAMETERS) - 1
                elif key == ord("/"):
                    result = prompt(
                        screen, "Search parameter name or address", self.query
                    )
                    if result:
                        self.query = result[0]
                        self.search(1)
                elif key in (ord("n"), ord("N")):
                    self.search(1 if key == ord("n") else -1)
                elif key == 27:
                    self.worker.cancel.set()
                    self.message = (
                        "Cancellation requested; waiting for current transaction"
                    )
                elif key == ord("?"):
                    screen.timeout(-1)
                    dialog(screen, "Keys and behavior", HELP)
                    screen.timeout(100)
                elif key == ord("q"):
                    if self.busy:
                        self.message = "Cancel the operation with Esc before quitting"
                    elif not self.staged or prompt(
                        screen, "Type DISCARD to quit with staged edits"
                    ) == ("DISCARD", False):
                        return
                elif self.busy and key != ord("e"):
                    self.message = (
                        "Operation in progress; Esc cancels between transactions"
                    )
                elif key in (ord("i"), 10, 13):
                    existing = self.staged.get(
                        p.address, (None, self.observations[p.address].word)
                    )[1]
                    initial = f"{p.number(existing):f}" if existing is not None else ""
                    result = prompt(
                        screen,
                        f"Edit {p.name} ({p.unit or 'unitless'}); Enter stages only",
                        initial,
                        toggle=True,
                    )
                    if result:
                        self.stage(p.address, p.parse(result[0], result[1]))
                        self.message = f"Staged {p.name}"
                elif key == ord("u"):
                    self.staged.pop(p.address, None)
                    self.message = f"Unstaged {p.name}"
                elif key in (ord("r"), ord("R")):
                    self.job(
                        "read",
                        [p.address]
                        if key == ord("r")
                        else [p.address for p in PARAMETERS],
                    )
                elif key == ord("c"):
                    self.job("connect", None)
                elif key == ord("w"):
                    if not self.staged:
                        raise ValueError("no staged changes")
                    if not self.connected:
                        raise ValueError("not connected")
                    lines = [
                        f"{a}: {BY_ADDRESS[a].name}: {BY_ADDRESS[a].display(old)} -> {BY_ADDRESS[a].display(new)} (0x{old:04X} -> 0x{new:04X})"
                        for a, (old, new) in sorted(self.staged.items())
                    ]
                    lines += [
                        "Writes persist immediately. Partial batches are not rolled back.",
                        "Motor type can reset flux/gamma; CAN changes may require reconnecting after a power cycle.",
                    ]
                    screen.timeout(-1)
                    accepted = dialog(screen, "Review staged EEPROM changes", lines)
                    screen.timeout(100)
                    if accepted and prompt(screen, "Type WRITE to apply this diff") == (
                        "WRITE",
                        False,
                    ):
                        self.job("apply", dict(self.staged))
                elif key == ord("e"):
                    result = prompt(
                        screen,
                        "Export observed values to JSON",
                        f"eeprom-{datetime.now():%Y%m%d-%H%M%S}.json",
                    )
                    if result and result[0]:
                        path = Path(result[0]).expanduser()
                        if path.exists() and prompt(
                            screen, "File exists. Type OVERWRITE to replace it"
                        ) != ("OVERWRITE", False):
                            continue
                        self.events()
                        save_snapshot(
                            path,
                            snapshot(
                                self.worker.args, self.observations, self.firmware
                            ),
                        )
                        self.message = (
                            f"Exported {path} (observed values; excludes staged edits)"
                        )
                elif key == ord("o"):
                    result = prompt(
                        screen, "Import JSON or RMS name/value text; preview only"
                    )
                    if result and result[0]:
                        values, issues = import_values(Path(result[0]).expanduser())
                        eligible = {}
                        for address, word in values.items():
                            observation = self.observations[address]
                            if observation.status != "ok" or observation.word is None:
                                issues.append(
                                    f"{BY_ADDRESS[address].name}: no successful live baseline"
                                )
                            elif observation.word != word:
                                eligible[address] = (observation.word, word)
                        lines = [
                            f"{a}: {BY_ADDRESS[a].name}: {BY_ADDRESS[a].display(old)} -> {BY_ADDRESS[a].display(word)}"
                            for a, (old, word) in eligible.items()
                        ]
                        lines += [f"SKIP: {issue}" for issue in issues]
                        screen.timeout(-1)
                        accepted = dialog(
                            screen,
                            f"Import preview: {len(eligible)} changes; Enter stages only",
                            lines or ["No changes"],
                        )
                        screen.timeout(100)
                        if accepted:
                            for address, (_, word) in eligible.items():
                                self.stage(address, word)
                            self.message = f"Staged {len(eligible)} imported changes; skipped {len(issues)} entries"
            except (ValueError, OSError, csv.Error) as error:
                self.message = str(error)
            self.previous_g = key == ord("g") and not self.previous_g


def parse_args(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", nargs="?", choices=("read", "write"))
    parser.add_argument("address", nargs="?", type=lambda value: int(value, 0))
    parser.add_argument("value", nargs="?", help="engineering value to write")
    parser.add_argument("--raw", action="store_true", help="write a raw EEPROM word")
    parser.add_argument("--port", default=DEFAULT_PORT)
    parser.add_argument(
        "--bitrate", type=int, choices=SPEED_CODES, default=500, help="CAN kbit/s"
    )
    parser.add_argument("--base", type=lambda value: int(value, 0), default=0xA0)
    parser.add_argument(
        "--mode", choices=("standard", "extended", "j1939"), default="standard"
    )
    parser.add_argument(
        "--timeout", type=float, default=5.0, help="response deadline in seconds"
    )
    parser.add_argument(
        "--freshness",
        type=float,
        default=1.0,
        help="maximum disabled-state telemetry age in seconds",
    )
    parser.add_argument(
        "--request-gap",
        type=float,
        default=0.1,
        help="receive-drain interval before each parameter request in seconds",
    )
    args = parser.parse_args(argv)
    if args.action is None:
        if args.address is not None or args.value is not None or args.raw:
            parser.error("address, value and --raw require a read or write command")
    else:
        if args.address not in BY_ADDRESS:
            parser.error("address must be a documented EEPROM parameter")
        if args.action == "read" and (args.value is not None or args.raw):
            parser.error("read accepts only an address")
        if args.action == "write":
            if args.value is None:
                parser.error("write requires an address and value")
            try:
                args.word = BY_ADDRESS[args.address].parse(args.value, raw=args.raw)
            except ValueError as error:
                parser.error(str(error))
    maximum = {"standard": 0x7C0, "extended": 0xFFC0, "j1939": 0xC0}[args.mode]
    if not 0 <= args.base <= maximum:
        parser.error(f"base must be between 0 and 0x{maximum:X}")
    if not 0 < args.timeout < float("inf") or not 0 < args.freshness < float("inf"):
        parser.error("timeout and freshness must be finite and positive")
    if not 0 <= args.request_gap < float("inf"):
        parser.error("request-gap must be finite and nonnegative")
    return args


def run_single(args, serial_factory=None):
    worker = Worker(args, serial_factory=serial_factory)
    parameter = BY_ADDRESS[args.address]
    try:
        worker.connect(scan=False)
        current = worker.read(args.address)
        print(
            f"{parameter.address} {parameter.name}: {parameter.display(current)}; raw {current} (0x{current:04X})"
        )
        if args.action == "write":
            if args.word == current:
                print("Already matches; no write needed.")
                return
            print(
                f"Writing {parameter.display(args.word)}; raw {args.word} (0x{args.word:04X})",
                flush=True,
            )
            worker.apply({args.address: (current, args.word)}, refresh=False)
            print("Write verified by readback.")
            if args.address not in IMMEDIATE:
                print("Power cycle required for the new setting to take effect.")
            if args.address == 150:
                print(
                    "Motor-type changes can reset flux/gamma; reread those parameters."
                )
            if args.address in COMMUNICATION:
                settings = next(e[2] for e in worker.events.queue if e[0] == "restart")
                print(
                    f"After power cycle, connect with --base 0x{settings['base']:X} --mode {settings['mode']} --bitrate {settings['bitrate']}"
                )
    finally:
        worker.disconnect()
        for event in worker.events.queue:
            if event[0] == "message" and event[1].startswith("CAN cleanup:"):
                print(event[1], file=sys.stderr)


def main():
    args = parse_args()
    if args.action:
        try:
            run_single(args)
        except (ValueError, RuntimeError, OSError) as error:
            raise SystemExit(str(error)) from error
        return
    worker = Worker(args)
    worker.start()
    try:
        curses.wrapper(TUI(worker).run)
    except KeyboardInterrupt:
        pass
    finally:
        worker.cancel.set()
        worker.stop.set()
        worker.join()


if __name__ == "__main__":
    main()
