import serial
import time
import csv
from datetime import datetime
from rich import print
import numpy as np

PORT = "/dev/cu.usbserial-DN8FHRI7"
SERIAL_BAUD = 115200
CAN_SPEED = 6
LOG_FILE = "log.csv"

ACK = b"\x06"
BELL = b"\x07"

temps = [[], [], [], [], [], [], [], [], [], [], [], []]
averages: list[list[int | float]] = [[], [], [], [], [], [], [], [], [], [], [], []]
times: list[list[int | float]] = [[], [], [], [], [], [], [], [], [], [], [], []]


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
            t0 = time.time()

            can_id = int(line[1:9], 16)
            if can_id > 100:
                return None

            dlc = int(line[9])
            data = bytes.fromhex(line[10 : 10 + dlc * 2])
            ext = True

            idx = int((can_id - 1) / 7)
            if idx < 0 or idx >= len(temps):
                return None
            temps[idx] = list(data)
            t = time.time()
            averages[idx].append(np.average(temps[idx]))
            times[idx].append(t - t0)

            return {
                "timestamp": datetime.now().isoformat(timespec="milliseconds"),
                "id": can_id,
                "id_hex": f"0x{can_id:03X}",
                "extended": ext,
                "dlc": dlc,
                "data": temps[idx],
                "data_hex": " ".join(str(b) for b in data),
            }
    except (ValueError, IndexError):
        return None


def main():
    with (
        serial.Serial(PORT, SERIAL_BAUD, timeout=1) as ser,
        open(LOG_FILE, "w", newline="") as csvfile,
    ):

        writer = csv.writer(csvfile)
        writer.writerow(["timestamp", "id_hex", "id_base_10", "dlc", "data_hex"])

        # send_cmd(ser, "C")  # close if already open
        send_cmd(ser, f"S{CAN_SPEED}")  # set CAN baud rate
        send_cmd(ser, "Z1")
        send_cmd(ser, "O")  # open CAN channel
        print(f"CAN open on {PORT}. Logging to {LOG_FILE} — Ctrl+C to stop.\n")

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
                        # print(
                        #     f"{frame['timestamp']}  {frame['id_hex']}  {int(frame['id_hex'], 0)}  "
                        #     f"[{frame['dlc']}]  {frame['data_hex']}"
                        # )
                        print(temps)
                        writer.writerow(
                            [
                                frame["timestamp"],
                                frame["id_hex"],
                                int(frame["id_hex"], 0),
                                frame["dlc"],
                                frame["data_hex"],
                            ]
                        )
                        csvfile.flush()

        except KeyboardInterrupt:
            print("\nStopping...")
        finally:
            send_cmd(ser, "C")
            print("CAN channel closed.")


if __name__ == "__main__":
    main()
