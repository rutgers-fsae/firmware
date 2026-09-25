# Nano R4 airspeed recorder

Reads a Matek ASPD-4525 differential-pressure sensor with an Arduino Nano R4 and records it over USB in a Python/Tk desktop application. The GUI supports named runs, 1–1,000 requested reads/s, zero calibration, live pressure/airspeed plots, and a saved-run dropdown.

This is a bench acquisition and analysis tool. It has not been validated as a vehicle safety/control system or calibrated against a reference airspeed instrument. The GUI calculates standard-density airspeed; it does not measure atmospheric density or ground speed.

## Hardware

| ASPD-4525 PCB label | Nano R4 pin |
| --- | --- |
| 5V | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

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

| Command | Response/action |
| --- | --- |
| `INFO` | `HELLO,2,1000,<current_rate>` |
| `RATE,100` | `RATE_OK,100` or `RATE_ERROR,...` |
| `r` | Explicit I2C recovery attempt |

Sample: `D,sequence,device_micros,raw,status,requested_hz,total_skipped_slots`

Read failure: `F,sequence,device_micros,bytes_received,requested_hz,total_skipped_slots`

After three consecutive failed reads, firmware attempts bounded bus recovery, with at least 500 ms between attempts. It releases the lines through pull-ups rather than driving them HIGH, applies up to nine clock pulses if SDA is held low, and reports `I2C_RECOVERY` line states. Recovery cannot repair faulty wiring or establish the physical cause of a dropout.

## Files and ignored data

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
