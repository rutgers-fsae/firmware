import serial
import time
import csv
from datetime import datetime
from rich import print
import numpy as np
import matplotlib.pyplot as plt

PORT = "/dev/cu.usbserial-DN8FHRI7"
SERIAL_BAUD = 115200
CAN_SPEED = 6
LOG_FILE = "log.csv"
PLOT_UPDATE_INTERVAL = 0.2

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
def parse_frame(line: str, start_time: float):
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

            dlc = int(line[9])
            data = bytes.fromhex(line[10 : 10 + dlc * 2])
            ext = True

            idx = int((can_id - 1) / 7)
            if idx < 0 or idx >= len(temps):
                return None
            temps[idx] = list(data)
            t = time.time()
            averages[idx].append(np.average(temps[idx]))
            times[idx].append(t - start_time)

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
    plt.ion()
    fig, ax = plt.subplots(figsize=(12, 7))
    lines = []
    for idx in range(len(averages)):
        (line_plot,) = ax.plot([], [], label=f"Channel {idx}")
        lines.append(line_plot)
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Average Temperature")
    ax.set_title("Average Temperature vs Time")
    ax.legend(loc="upper left", ncols=2)
    ax.grid(True, alpha=0.3)
    plt.show(block=False)

    start_time = time.time()
    last_plot_update = 0.0

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
                    frame = parse_frame(line, start_time)
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

                now = time.time()
                if now - last_plot_update >= PLOT_UPDATE_INTERVAL:
                    for idx, line_plot in enumerate(lines):
                        line_plot.set_data(times[idx], averages[idx])
                    ax.relim()
                    ax.autoscale_view()
                    plt.pause(0.001)
                    last_plot_update = now

        except KeyboardInterrupt:
            print("\nStopping...")
        finally:
            send_cmd(ser, "C")
            plt.ioff()
            print("CAN channel closed.")


if __name__ == "__main__":
    main()
