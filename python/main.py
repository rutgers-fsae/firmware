import serial
import time
import csv
import os
from rich import print
from datetime import datetime

PORT = "/dev/cu.usbserial-DN7BORLC"
SERIAL_BAUD = 115200
CAN_SPEED = 6
RUN_ID = datetime.now().strftime("run-%Y-%m-%d_%H-%M-%S")
RUN_DIR = os.path.join("logs", RUN_ID)
LOG_FILE = os.path.join(RUN_DIR, "temperature.csv")
FAULT_LOG_FILE = os.path.join(RUN_DIR, "fault.csv")
INV_LOG_FILE = os.path.join(RUN_DIR, "inverter.csv")
DAQ_LOG_FILE = os.path.join(RUN_DIR, "daq.csv")
START_TIME = time.time()
DAQ_BASE_ID = 0x18FF5000
DAQ_LAST_ID = 0x18FF5047
DAQ_SCALE = 100.0

ACK = b"\x06"
BELL = b"\x07"

LABELS = [
    "TEMP101",
    "TEMP102",
    "TEMP103",
    "TEMP104",
    "TEMP105",
    "TEMP106",
    "TEMP107",
    "TEMP108",
    "TEMP109",
    "TEMP110",
    "TEMP111",
    "TEMP112",
    "TEMP113",
    "TEMP114",
    "TEMP115",
    "TEMP116",
    "TEMP117",
    "TEMP118",
    "TEMP201",
    "TEMP202",
    "TEMP203",
    "TEMP204",
    "TEMP205",
    "TEMP206",
    "TEMP207",
    "TEMP208",
    "TEMP209",
    "TEMP210",
    "TEMP211",
    "TEMP212",
    "TEMP213",
    "TEMP214",
    "TEMP215",
    "TEMP216",
    "TEMP217",
    "TEMP218",
    "TEMP301",
    "TEMP302",
    "TEMP303",
    "TEMP304",
    "TEMP305",
    "TEMP306",
    "TEMP307",
    "TEMP308",
    "TEMP309",
    "TEMP310",
    "TEMP311",
    "TEMP312",
    "TEMP313",
    "TEMP314",
    "TEMP315",
    "TEMP316",
    "TEMP317",
    "TEMP318",
    "TEMP401",
    "TEMP402",
    "TEMP403",
    "TEMP404",
    "TEMP405",
    "TEMP406",
    "TEMP407",
    "TEMP408",
    "TEMP409",
    "TEMP410",
    "TEMP411",
    "TEMP412",
    "TEMP413",
    "TEMP414",
    "TEMP415",
    "TEMP416",
    "TEMP417",
    "TEMP418",
    "TEMP501",
    "TEMP502",
    "TEMP503",
    "TEMP504",
    "TEMP505",
    "TEMP506",
    "TEMP507",
    "TEMP508",
    "TEMP509",
    "TEMP510",
    "TEMP511",
    "TEMP512",
]

temps = [0] * 90
averages: list[list[float]] = [[] for _ in range(len(LABELS))]
times: list[list[float]] = [[] for _ in range(len(LABELS))]
running_sums = [0.0] * len(LABELS)
running_counts = [0] * len(LABELS)


def send_cmd(ser, cmd: str, delay=0.1):
    ser.write((cmd + "\r").encode())
    time.sleep(delay)
    resp = ser.read(ser.in_waiting or 1)
    if resp and resp[-1:] == BELL:
        raise RuntimeError(f"Command '{cmd}' was rejected (BELL)")
    return resp


# TODO: average temps, plot vs time, seperate based on channel
def parse_frame(line: str):
    """
    Standard frame format from CANDapter:
      tIIILDD...DD   (standard 11-bit)
      TIIIIIIIILDD..  (extended 29-bit)
    t/T = frame type, III = ID hex, L = DLC, DD = data bytes hex
    """
    line = line.strip()
    if not line:
        return None

    try:
        if line[0] != "t":
            can_id = int(line[1:9], 16)
            if can_id > 100:
                return None

            dlc = int(line[9], 16)
            data = bytes.fromhex(line[10 : 10 + dlc * 2])

            idx = (can_id - 1) // 7
            base = idx * 7
            if base < 0 or base >= len(temps):
                return None

            timestamp = time.time() - START_TIME
            updated_indices: list[int] = []

            for offset, value in enumerate(data[:7]):
                sensor_idx = base + offset
                if sensor_idx < 0 or sensor_idx >= len(temps):
                    continue

                temps[sensor_idx] = value

                if sensor_idx < len(LABELS):
                    running_sums[sensor_idx] += value
                    running_counts[sensor_idx] += 1
                    averages[sensor_idx].append(
                        running_sums[sensor_idx] / running_counts[sensor_idx]
                    )
                    times[sensor_idx].append(timestamp)
                    updated_indices.append(sensor_idx)

            return {
                "timestamp": timestamp,
                "id": can_id,
                "updated_indices": updated_indices,
            }
    except (ValueError, IndexError):
        return None


def parse_daq_frame(line: str):
    line = line.strip().upper()
    if not line or not line.startswith("X18FF5"):
        return None

    try:
        can_id = int(line[1:9], 16)
        if can_id < DAQ_BASE_ID or can_id > DAQ_LAST_ID:
            return None

        dlc = int(line[9], 16)
        data = bytes.fromhex(line[10 : 10 + dlc * 2])
        timestamp = time.time() - START_TIME

        voltages = [value / DAQ_SCALE for value in data[:7]]
        return {
            "timestamp": timestamp,
            "id": can_id,
            "voltages": voltages,
        }
    except (ValueError, IndexError):
        return None


def parse_faults_frame(line: str):
    line = line.strip().upper()
    if not line or not line.startswith("X1839F380"):
        return None

    try:
        can_id = int(line[1:9], 16)
        dlc = int(line[9], 16)
        data = bytes.fromhex(line[10 : 10 + dlc * 2])
        timestamp = time.time() - START_TIME
        fault_counts = data[3]
        high_temp = data[2]
        low_temp = data[1]
        return {
            "timestamp": timestamp,
            "id": can_id,
            "faults": fault_counts,
            "high": high_temp,
            "low": low_temp,
        }
    except (ValueError, IndexError):
        return None

def parse_inv_frame(line: str):
    line = line.strip().upper()
    if not line or not line.startswith("t"):
        return None

    try:
        can_id = int(line[1:4], 16)
        if can_id != 0xA4:
            return None

        dlc = int(line[4], 16)
        data = bytes.fromhex(line[5 : 5 + dlc * 2])
        timestamp = time.time() - START_TIME
        inv_byte0 = data[0] if data else None
        return {
            "timestamp": timestamp,
            "id": can_id,
            "inv_byte0": inv_byte0,
            "dlc": dlc,
        }
    except (ValueError, IndexError):
        return None


def prompt_save_data() -> bool:
    while True:
        choice = input("\nSave captured data before quitting? (y/n): ").strip().lower()
        if choice in {"y", "yes"}:
            return True
        if choice in {"n", "no"}:
            return False
        print("Please enter 'y' or 'n'.")


def delete_logs(paths: list[str]) -> None:
    for path in paths:
        try:
            if os.path.exists(path):
                os.remove(path)
                print(f"Deleted {path}")
        except OSError as exc:
            print(f"Could not delete {path}: {exc}")


def main():
    os.makedirs(RUN_DIR, exist_ok=True)
    log_paths = [LOG_FILE, DAQ_LOG_FILE, FAULT_LOG_FILE, INV_LOG_FILE]
    save_data = True
    with (
        serial.Serial(PORT, SERIAL_BAUD, timeout=1) as ser,
        open(LOG_FILE, "w", newline="") as csvfile,
        open(DAQ_LOG_FILE, "w", newline="") as daq_csvfile,
        open(FAULT_LOG_FILE, "w", newline="") as fault_csvfile,
        open(INV_LOG_FILE, "w", newline="") as inv_csvfile,
    ):

        writer = csv.writer(csvfile)
        daq_writer = csv.writer(daq_csvfile)
        fault_writer = csv.writer(fault_csvfile)
        inv_writer = csv.writer(inv_csvfile)
        writer.writerow(
            [
                "timestamp",
                "channel",
                "temp",
            ]
        )
        daq_writer.writerow(["timestamp", "voltage"])
        fault_writer.writerow(["timestamp", "high", "low", "faults"])
        inv_writer.writerow(["timestamp", "dlc", "inv_byte0"])

        # send_cmd(ser, "C")  # close if already open
        send_cmd(ser, f"S{CAN_SPEED}")  # set CAN baud rate
        send_cmd(ser, "Z1")
        send_cmd(ser, "O")  # open CAN channel
        print(
            f"CAN open on {PORT}. Logging to {LOG_FILE}, {DAQ_LOG_FILE}, {FAULT_LOG_FILE}, and {INV_LOG_FILE} — Ctrl+C to stop.\n"
        )

        buf = ""
        try:
            while True:
                raw = ser.read(ser.in_waiting or 1)
                if not raw:
                    continue

                buf += raw.decode("ascii", errors="ignore")

                while "\r" in buf:
                    line, buf = buf.split("\r", 1)
                    frame = parse_frame(line)
                    if frame:
                        for sensor_idx in frame["updated_indices"]:
                            writer.writerow(
                                [
                                    frame["timestamp"],
                                    LABELS[sensor_idx],
                                    temps[sensor_idx],
                                ]
                            )
                        csvfile.flush()

                    daq_frame = parse_daq_frame(line)
                    if daq_frame:
                        for voltage in daq_frame["voltages"]:
                            daq_writer.writerow([daq_frame["timestamp"], voltage])
                        daq_csvfile.flush()

                    fault_frame = parse_faults_frame(line)
                    if fault_frame:
                        fault_writer.writerow(
                            [
                                fault_frame["timestamp"],
                                fault_frame["high"],
                                fault_frame["low"],
                                fault_frame["faults"],
                            ]
                        )
                        fault_csvfile.flush()

                    inv_frame = parse_inv_frame(line)
                    if inv_frame:
                        inv_writer.writerow(
                            [inv_frame["timestamp"], inv_frame["dlc"], inv_frame["inv_byte0"]]
                        )
                        inv_csvfile.flush()

        except KeyboardInterrupt:
            save_data = prompt_save_data()
            print("\nStopping...")
        finally:
            send_cmd(ser, "C")
            print("CAN channel closed.")

    if not save_data:
        delete_logs(log_paths)
        try:
            if os.path.isdir(RUN_DIR) and not os.listdir(RUN_DIR):
                os.rmdir(RUN_DIR)
                print(f"Deleted empty run directory {RUN_DIR}")
        except OSError as exc:
            print(f"Could not remove run directory {RUN_DIR}: {exc}")
    else:
        print(f"Data saved in {RUN_DIR}")


if __name__ == "__main__":
    main()
