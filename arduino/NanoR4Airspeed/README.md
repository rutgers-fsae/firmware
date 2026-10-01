# Airspeed recorder: Raspberry Pi or Nano R4

Reads a Matek ASPD-4525 differential-pressure sensor directly on a Raspberry Pi Zero 2 W with a headless Python logger, or with an Arduino Nano R4 over USB in a Python/Tk desktop application. The GUI supports named runs, 1–1,000 requested reads/s, zero calibration, live pressure/airspeed plots, and a saved-run dropdown.

This is a bench acquisition and analysis tool. It has not been validated as a vehicle safety/control system or calibrated against a reference airspeed instrument. The GUI calculates standard-density airspeed; it does not measure atmospheric density or ground speed.

## Raspberry Pi Zero 2 W (alongside VectorNav)

The Pi reads the ASPD-4525 directly over `/dev/i2c-1`; no Arduino is needed.
`logger/airspeed_pi.py` follows the implementation in
`../../vectorNav/rfr_vn300_logger.py` and its `deploy/` directory: headless Python,
a dedicated systemd account, editable settings, UTC CSV timestamps, SIGINT/SIGTERM
shutdown, one-second flushes, and disk sync every five seconds and on close.
It reuses this project's pressure calculation, zero calibration, and run format.
Python 3.10+ on Raspberry Pi OS Lite works; neither Tk nor pyserial is needed.

Airspeed uses `/opt/airspeed`, `/var/lib/airspeed`, and an `airspeed` account.
VectorNav keeps its USB device, services, dashboard port 8080, and log directory.
The two loggers run independently; starting/stopping VectorNav from its dashboard
only controls VectorNav. The VectorNav dashboard can request an airspeed zero; no merged CSV is included.

### Pi wiring

**Pi GPIO is 3.3 V only. Do not reuse the Nano's SDA/SCL pull-ups to 5 V on the Pi.**
Use a bidirectional I2C level shifter between the 5 V sensor bus and Pi bus,
with pull-ups appropriate to each side. Power the ASPD-4525 from 5 V and share
ground. With power disconnected, wire:

| Connection | Pi physical header pin / destination |
| --- | --- |
| Sensor 5V and level shifter HV | Pin 2 or 4 (5 V) |
| Sensor GND and shifter GND | Pin 6 (GND) |
| Shifter LV reference | Pin 1 (3.3 V) |
| Shifter low-side SDA | Pin 3 (GPIO2 / SDA1) |
| Shifter low-side SCL | Pin 5 (GPIO3 / SCL1) |
| Sensor SDA | Shifter high-side SDA |
| Sensor SCL | Shifter high-side SCL |

Keep I2C wires short and use the default 100 kHz bus initially. Do not attach the
Nano as another bus master. The sensor address defaults to `0x28`. The transaction
is a plain two-byte I2C read, without a register write, matching the Arduino.
See [Pi GPIO electrical guidance](https://pip-assets.raspberrypi.com/categories/685-whitepapers-app-notes/documents/RP-006553-WP/A-history-of-GPIO-usage-on-Raspberry-Pi-devices-and-current-best-practices)
and [smbus2 plain I2C operations](https://smbus2.readthedocs.io/en/0.5.0/).

### Dashboard zero control

With the combined VectorNav setup, unlock the dashboard with its operator PIN,
then choose **Zero airspeed** with both pressure ports at equal pressure in
still air. The dashboard shows sample count, elapsed time, success, and errors.
The airspeed service keeps recording but leaves calculated speeds blank during
zeroing. It needs at least 20 fresh samples over five seconds and rejects noisy
or timed-out zeros; failure preserves the previous saved calibration.

Control uses a local Unix socket owned by the airspeed service and accessible
to the VectorNav group; the dashboard needs no root permissions. Update both
services by rerunning the combined setup script after copying updated files
to the Pi. Manual `--zero` remains available below.

### Install and zero

Copy this directory to the Pi, then run from its root:

```sh
sudo bash deploy/install-rpi.sh --no-start
```

The installer enables I2C, installs a small Python virtual environment and the
service, and preserves existing `/etc/default/airspeed-logger` settings.
Reboot if `/dev/i2c-1` is not present after enabling I2C. The service is enabled
for future boots even with `--no-start`.

Zero deliberately with both ports at equal pressure in still air. Stop the
service first, then collect at least 20 fresh samples spanning five seconds:

```sh
sudo systemctl stop airspeed-logger
sudo -u airspeed /opt/airspeed/venv/bin/python /opt/airspeed/logger/airspeed_pi.py --zero
sudo systemctl start airspeed-logger
journalctl -u airspeed-logger -f
```

Zero exits after saving `/var/lib/airspeed/zero.json`. It rejects noisy readings
and times out after 45 seconds; cancellation preserves the previous file.
The service explicitly reuses that calibration on subsequent launches. Redo it
when changing sensors/tubing or when the baseline drifts. Bus/address identify
the connection, not an individual sensor. A missing calibration records pressure
with `needs_zero` and blank speeds; a malformed or mismatched calibration stops
the process. It never assumes the vehicle is stationary at startup.

Settings in `/etc/default/airspeed-logger` include bus, address, rate, air density,
output directory and calibration file. Restart the service after edits. When
zeroing a different bus/address, supply the same `--bus`, `--address`, and
`--calibration` values manually. Use the same `--output-dir` as the service so
the instance lock prevents concurrent reads/zeroing.

Each service start creates a new named CSV and `.run.json` under
`/var/lib/airspeed/logs`. Download a CSV and use the existing desktop GUI's saved
run viewer by placing the files in its data directory. A manual recording is:

```sh
sudo systemctl stop airspeed-logger
sudo -u airspeed /opt/airspeed/venv/bin/python /opt/airspeed/logger/airspeed_pi.py --rate 100 --title "Fan test"
# Ctrl-C saves and closes the recording; restart the service when finished.
```

Requested rates remain 1–1,000 Hz, with 20 Hz as the default. Linux scheduling,
I2C, disk sync, and the simultaneous VectorNav workload can reduce the achieved
rate. Missed scheduling slots are counted; the journal reports measured rate
and CSV rows retain stale/error readings with blank speeds. Three consecutive
I2C failures close the recording and exit; systemd retries after three seconds.
No GPIO bus-clear pulses are attempted while the Linux I2C driver owns the pins.
Check sensor power/wiring if retries continue.

`sample_utc` is the Pi wall clock just before the read; `device_elapsed_s` is
monotonic elapsed time and `device_sequence` counts Pi read attempts. Arduino
`device_micros` is left blank. These timestamps share the Pi clock with
VectorNav's host timestamps but do not synchronize sensor acquisition or GNSS
time. Set the Pi clock before a run; a Zero 2 W has no battery-backed RTC.
Pi hardware timing and concurrent VectorNav load have not yet been measured.

Portable acquisition checks (no Pi required):

```sh
python3 -m unittest discover -s tests -p 'test_pi.py'
bash -n deploy/install-rpi.sh
```

## Arduino Nano R4 hardware (existing option)

| ASPD-4525 PCB label | Nano R4 pin |
| ------------------- | ----------- |
| 5V                  | 5V          |
| GND                 | GND         |
| SDA                 | A4          |
| SCL                 | A5          |

Identify wires from the sensor PCB labels, not color alone. Disconnect power before changing connections. The Nano R4 header bus uses `Wire`; its separate Qwiic connector uses `Wire1`. Ensure suitable I2C pull-ups are present on the bus. When absent, the documented Nano R4 arrangement is one 4.7 kΩ resistor from SDA to 5V and another from SCL to 5V. Do not connect either signal directly to 5V. A lit sensor power LED does not establish I2C communication.

Connect pitot total pressure and static pressure to the sensor's corresponding high/low sides. The pitot must face the airflow and have unobstructed static openings. Negative corrected pressure may indicate reversed tubing, pressure transients, or a bad zero measurement; the program does not silently take its absolute value.

## Upload firmware

Install **Arduino UNO R4 Boards** (tested with 1.6.0; Nano R4 requires 1.5.0 or later). Select **Arduino Nano R4** and its USB port. Open and upload:

```
firmware/NanoR4Airspeed/NanoR4Airspeed.ino
```

`Wire` is included in the Arduino board package; no sensor library is required. I2C runs at 100 kHz. USB serial is opened at 115200. An optional address scanner is in `tools/I2CScanner/`.

With Arduino CLI, from this directory:

```sh
arduino-cli compile --fqbn arduino:renesas_uno:nanor4 firmware/NanoR4Airspeed
arduino-cli board list
arduino-cli upload --fqbn arduino:renesas_uno:nanor4 --port YOUR_PORT firmware/NanoR4Airspeed
```

The acquisition sketch uses compact protocol v2 records instead of descriptive pressure text. The logger performs pressure conversion and airspeed calculations. Close Serial Monitor/Plotter before starting the logger; quitting Arduino IDE also stops lingering serial-monitor processes.

## Install and launch the GUI

Use Python **3.11 or newer** with Tk support on macOS or Linux. `python3 -m tkinter` can check Tk availability. Install into a local virtual environment:

```sh
cd logger
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/python airspeed_logger.py
```

On macOS, after setup, double-click `logger/Start Airspeed Logger.command`. It prefers `logger/.venv/bin/python3`; `AIRSPEED_PYTHON` can select a different interpreter. No Codex runtime or user-specific path is needed. Windows is not currently supported by the POSIX serial/instance-lock implementation.

The app opens in preview, not recording. It automatically identifies one Nano R4 by USB vendor/product ID. Use only one logger per board.

1. Choose **Reads per second**, then **Apply rate**. Default: 20/s.
2. Put both pressure ports at equal pressure in still air and click **Zero in still air**. The baseline requires at least 20 fresh samples spanning at least five seconds; at 1/s it takes about 20 seconds. No old zero is silently reused.
3. Enter a **Run title** and click **Start recording**.
4. The graph shows speed versus local time, or corrected pressure using the graph selector. The live plot covers 60 seconds; the table is paged.
5. **Stop and save** ends the recording without closing the GUI. **View run** opens a saved recording; **Back to live** returns to the sensor. Browsing history does not stop a current recording.

GUI times use 12-hour AM/PM format. CSV receipt timestamps remain UTC. New files are named `airspeed-<title>-<UTC timestamp>.csv`; a `.run.json` companion stores the display title and start/end details. Existing CSV recordings, including the previous human-readable serial format, can be loaded without rewriting them.

The default data directory is **`logger/runs/`**, which is ignored by Git. Override it with `--data-dir /path/to/local/runs`. `--calibration /path/to/zero.json` explicitly reuses a calibration from the same board; normal launches require fresh zeroing. Keep the Mac awake while recording: the Arduino does not store data missed while the host is asleep.

## Read rates and timing

The sensor datasheet specifies a typical 0.5 ms update time, approximately 2,000 internal updates/s. That is not a guaranteed full-system rate. This implementation caps requests at **1,000/s**. A short test of the final implementation saved **10,005 normal sensor readings at 1,000/s**, with no detected sequence gaps or skipped scheduling slots. This is a short test result on one setup, not a long-term reliability guarantee. Start with 20–100/s; 500–1,000/s remains experimental.

The GUI reports the acknowledged requested rate, measured packet rate, fresh-sample rate, and skipped scheduling slots. It never assumes the requested rate was achieved. Acquisition and CSV writing run separately from GUI redraws. CSV output is flushed once a second and on stop. Plot reduction preserves extrema and missing-value gaps; it does not reduce the stored data. The live preview retains 60,000 rows; saved-run views load the full CSV.

The new `sample_utc` is an approximate timestamp anchored to the first USB receipt and advanced by the Arduino microsecond counter. It improves relative timing but is not a synchronized absolute clock. Counter rollover is handled. A detected restart or USB disconnection clears zero calibration.

## Measurement calculations

Matek lists the sensor as `4525DO-DS5AI001DP`: bidirectional ±1 psi with type-A output. For raw count `C`:

```
pressure_pa = ((C - 0.10 * 16383) / (0.80 * 16383) * 2 - 1) * 6894.757
corrected_pressure_pa = pressure_pa - mean(zero_samples)
airspeed_m_s = sqrt(2 * corrected_pressure_pa / 1.225)
airspeed_km_h = airspeed_m_s * 3.6
```

Only normal status and counts within the calibrated range are used. Density is fixed at **1.225 kg/m³**. The zero-noise band is `max(3 * sample_standard_deviation, 2 * raw_count_pressure_step)`, where one count is about 1.052 Pa. Inside that band, speed is recorded as 0 with `within_zero_noise`; this is noise suppression, not proof of zero airflow. More negative pressure leaves speed blank and sets `negative_pressure_check_tubing`. Non-normal status, read failures, out-of-range values and missing zero also leave speed blank. Zeroing times out after 45 seconds or rejects standard deviation above 10 Pa.

## Serial protocol

Newline-terminated commands:

| Command    | Response/action                   |
| ---------- | --------------------------------- |
| `INFO`     | `HELLO,2,1000,<current_rate>`     |
| `RATE,100` | `RATE_OK,100` or `RATE_ERROR,...` |
| `r`        | Explicit I2C recovery attempt     |

Sample: `D,sequence,device_micros,raw,status,requested_hz,total_skipped_slots`

Read failure: `F,sequence,device_micros,bytes_received,requested_hz,total_skipped_slots`

After three consecutive failed reads, firmware attempts bounded bus recovery, with at least 500 ms between attempts. It releases the lines through pull-ups rather than driving them HIGH, applies up to nine clock pulses if SDA is held low, and reports `I2C_RECOVERY` line states. Recovery cannot repair faulty wiring or establish the physical cause of a dropout.

## Files and ignored data

- `deploy/`: Pi installer, I2C dependency, systemd service and settings.
- `firmware/`: complete Arduino acquisition sketch.
- `tools/`: optional I2C scanner.
- `logger/`: GUI, protocol/measurement helpers, serial worker, and dependency pin.
- `tests/`: portable test source; fixtures are generated at runtime.
- `logger/runs/`: local CSVs, calibration JSON, metadata, and instance lock; ignored.
- `.test-output/`, `build/`, `.venv/`, caches, CSVs, calibration files and logs: ignored.

No real recordings, local calibration values, generated plots, installed dependencies, device identifiers, or raw test captures are included in Git.

CSV columns preserve the original fields (`received_utc`, `elapsed_s`, counts/status, raw/zero/corrected pressure, density/noise, speeds, reading state, calibration ID and source line). Protocol v2 adds `run_title`, `sample_utc`, device counters, requested rate, `missing_serial_samples`, and `device_skipped_slots`. Missing serial samples are sequence gaps; device skipped slots are cumulative scheduled reads skipped because execution was late or USB lacked capacity. Neither is the same as a sensor stale/error status.

## Tests

From this directory, using the environment installed above:

```sh
logger/.venv/bin/python -m unittest discover -s tests -p 'test_*.py'
logger/.venv/bin/python tests/serial_integration.py
mkdir -p .test-output
c++ -std=c++17 -Itests/firmware tests/firmware/check.cpp -o .test-output/firmware-check
.test-output/firmware-check
```

Tk component tests use a hidden test window and skip when Tk/display support is unavailable. The serial integration test uses a synthetic POSIX pseudo-terminal, not a real Arduino; generated captures stay under ignored `.test-output/`. The firmware host tests exercise bounded recovery without hardware. These tests supplement, not replace, on-device verification.

## References

- [Matek ASPD-4525 specifications](https://www.mateksys.com/?portfolio=aspd-4525)
- [Nano R4 user manual](https://docs.arduino.cc/tutorials/nano-r4/user-manual/)
- [MS4525DO datasheet](https://docs.rs-online.com/c7b3/0900766b815f0b06.pdf)
- [Manufacturer data-fetch protocol and transfer function](https://www.avrfreaks.net/sfc/servlet.shepherd/document/download/0693l00000VHWNLAA5)
- [NASA pitot-tube airspeed relation](https://www.grc.nasa.gov/www/k-12/BGP/pitot.html)
- [I2C bus-clear procedure, UM10204](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)
