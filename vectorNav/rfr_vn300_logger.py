# RFR VN-300 Full Data Logger — Rutgers Formula Racing
# Run: py -3.11 rfr_vn300_logger.py
# Logs GPS, IMU, INS, and attitude data to a timestamped CSV file

import sys  # sys lets us exit the program if connection fails
import csv  # csv is python's built-in library for writing CSV files
from datetime import datetime  # gives us current date and time
# gives us 2 things we need from VectorNav SDK library
from vectornav import Sensor, Registers

# ── CONFIG ───────────────────────────────────────────────────────────
# Serial port (Windows: COM3 | Raspberry Pi: /dev/ttyUSB0) serial port of VN-300 connected to!
PORT = "COM3"
RATE = 40          # Output rate divisor — 400/40 = 10Hz VN 300 internally runs at 400Hz, divide the rate 40 to get actual rate
# creates a new filename everytime the script runs
FILENAME = f"rfr_vn300_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"


def parse_vec3(obj):
    """Parse a VectorNav 3D vector object into a tuple of 3 floats using str()"""
    try:
        # Converts C++ object to string, remove parentheses, split by comma
        s = str(obj).strip('()').split(',')
        # converts each string peice to a decimal number
        return float(s[0]), float(s[1]), float(s[2])
    except:
        return None, None, None


# ── CONNECT ──────────────────────────────────────────────────────────
sensor = Sensor()  # Creates Sensor object
try:
    # autocconnect tries all standard baud rates automatically until it finds the VN-300
    sensor.autoConnect(PORT)
# contains specific error message explaining what went wrong (e.g. wrong port, no permission, etc.)
except Exception as e:
    # explaining what went wrong (e.g. wrong port, no permission, etc.)
    print(f"Connection failed: {e}")
    sys.exit(1)  # stops the entire script

# confirms connection worked and prints baud rate
print(f"Connected: {sensor.connectedBaudRate()} baud")
m = Registers.System.Model()  # creates an empty container for the model number data
# prints model number to confirm we're talking to the sensor(VN-300)
sensor.readRegister(m)
# confirms we are talking to the right device and prints the model number(VN-300T-CR)
print(f"Sensor: {m.model}\n")

# BINARY OUTPUT 1 — IMU + ATTITUDE    1 MEANS INCLUDE IT IN THE PACKET, 0 MEANS DON'T INCLUDE IT
# Creates a configuration object for Binary Output 1
b1 = Registers.System.BinaryOutput1()
# Tells the sensor to stream data out on the serial port 1(FTDI cable)
b1.asyncMode.serial1 = 1
# RATE is already defined as 40, which divides the internal 400Hz rate to get 10Hz output
b1.rateDivisor = RATE
b1.common.timeStartup = 1  # Time since boot in nanoseconds
b1.common.timeGps = 1      # GPS UTC time
b1.common.ypr = 1          # Yaw Pitch Roll in degrees: Orientation of the car in degrees
b1.common.accel = 1        # Accelerometer forces X/Y/Z m/s²
b1.common.angularRate = 1  # Gyroscope(rotation rates) X/Y/Z in rad/s
b1.common.imu = 1          # Full IMU packet
b1.common.magPres = 1      # Magnetometer (heading) and barometric pressure
# Delta velocity and delta theta (Change in velocities and angles between samples)
b1.common.deltas = 1
# Sends the completed config form to the physical sensor
sensor.writeRegister(b1)
print("Binary Output 1 configured (IMU + Attitude)")

# BINARY OUTPUT 2 — GNSS
# Creates configuration object for teh raw GPS data. The data straight from the satellite receiver
b2 = Registers.System.BinaryOutput2()
b2.asyncMode.serial1 = 1  # Stream on serial port 1
b2.rateDivisor = RATE  # same 10Hz rate
# Fix type: 3 = 3D fix(fix type 3 means full 3D fix, need this before driving)
b2.gnss.gnss1Fix = 1
# Satellite count (Number of satellites in view, more is better for accuracy)
b2.gnss.gnss1NumSats = 1
# Raw GPS position lat/lon/alt (Latitude, longitude, and altitude from the GPS)
b2.gnss.gnss1PosLla = 1
# GPS velocity North/East/Down (Velocity in north/east/down coordinates from the GPS)
b2.gnss.gnss1VelNed = 1
# Position accuracy estimate (how accurate the GPS position is in meters)
b2.gnss.gnss1PosUncertainty = 1
# Sends the completed config form to the physical sensor
sensor.writeRegister(b2)
print("Binary Output 2 configured (GNSS)")

# ── BINARY OUTPUT 3 — INS FUSED (Inertial Navigation system) GPS + IMU COMBINED, more accurate than raw GPS
# Creates configuration object for the INS fused solution
b3 = Registers.System.BinaryOutput3()
b3.asyncMode.serial1 = 1             # Stream on serial port 1
b3.rateDivisor = RATE                # same 10Hz rate
b3.ins.posLla = 1          # Fused lat/lon/altitude position — most accurate
b3.ins.velNed = 1          # Fused velocity North/East/Down - smoother than raw GPS
b3.ins.posU = 1          # Position uncertainty- How confident the INS is in meters
b3.ins.velU = 1          # Velocity uncertainty- How confident the INS is in m/s
# Sends the completed INS config form to the physical sensor
sensor.writeRegister(b3)
print("Binary Output 3 configured (INS)\n")

# CSV SETUP COLS is the list that defines every coloumns in the CSV file, order = The order coloumns appear in the Excel
# Each string is a coloumns header name
COLS = [
    "timestamp",                           # Local system time
    "startup_ns", "gps_utc_ns",           # Sensor timing
    "yaw", "pitch", "roll",               # Attitude degrees
    "ax", "ay", "az",                     # Acceleration m/s² (az = downforce)
    "gx", "gy", "gz",                     # Gyro rad/s (gz = yaw rate)
    "dvx", "dvy", "dvz",                  # Delta velocity
    "dtx", "dty", "dtz", "dt",           # Delta theta + interval
    "mx", "my", "mz",                     # Magnetometer Gauss
    "temp_c", "pressure_pa",              # Temperature and pressure
    "gps_fix", "gps_sats",               # GPS quality
    "gps_lat", "gps_lon", "gps_alt",    # Raw GPS position
    "gps_vn", "gps_ve", "gps_vd",       # GPS velocity
    "gps_speed",                          # Ground speed m/s
    "ins_lat", "ins_lon", "ins_alt",    # INS fused position
    "ins_vn", "ins_ve", "ins_vd",       # INS fused velocity
]

# Opens CSV file for writing "w"= Write mode(creates news file), newline=prevents extra blank lines in Windows
f = open(FILENAME, "w", newline="")
# Dictwriter lets us write rows as dictionaires, fieldnames=COLS tells it the order of coloumns, ignore keys that are not in COLS
w = csv.DictWriter(f, fieldnames=COLS, extrasaction="ignore")
w.writeheader()  # Coloumns names as the first row in the CSV
# Confirms to the terminal that logging has started
print(f"Logging → {FILENAME}")
print("Ctrl+C to stop\n")

# MAIN LOGGING LOOP
n = 0  # n is a row counter
try:
    while True:
        # Get next data packet (asks the sensor for the next avaiable data packet)
        cd = sensor.getNextMeasurement()
        if not cd:
            continue                # Skip if nothing ready

        # start a new row dictionary with current local time isoformat gives a clean readable timestamp
        r = {"timestamp": datetime.now().isoformat()}

        # Timing
        if cd.time.timeStartup:  # check if the packet contains startup time before trying to read it
            # nanaseconds converts the time object to a plain number
            r["startup_ns"] = cd.time.timeStartup.nanoseconds()
        if cd.time.timeGps:  # more accurate than system clock time probably useful for syncing with other car sensors
            r["gps_utc_ns"] = cd.time.timeGps.nanoseconds()

        # Attitude — yaw/pitch/roll
        if cd.attitude.ypr:
            # Yaw= car's heading angle, 0-360 degrees where 0/360 is north, 90 is east, 180 is south, 270 is west
            r["yaw"] = cd.attitude.ypr.yaw
            # pitch = nose up/down angle, positive is nose up, negative is nose down
            r["pitch"] = cd.attitude.ypr.pitch
            # roll = lean left/right angle, positive is leaning right, negative is leaning left
            r["roll"] = cd.attitude.ypr.roll

        # Accelerometer — az is vertical g-force for aero downforce analysis
        # accel is subscriptable - [o] is x, [1] is y, [2] is z
        # ax = forward/backward force, ay = left/right cornering force, az = vertical force (KEY for aero team, increases with downforce)
        if cd.imu.accel:
            r["ax"], r["ay"], r["az"] = cd.imu.accel[0], cd.imu.accel[1], cd.imu.accel[2]

        # Gyroscope — gz is yaw rate, how fast car is turning
        # rotation rates ins rad/s on each axis, gz = yaw rate how fast the car is turning, higher means faster turn
        if cd.imu.angularRate:
            r["gx"], r["gy"], r["gz"] = cd.imu.angularRate[0], cd.imu.angularRate[1], cd.imu.angularRate[2]

        # Delta theta — integrated rotation between samples
        if cd.imu.deltaTheta:
            # Change in rotation angle, used to reconstruct trajectory
            r["dtx"] = cd.imu.deltaTheta.deltaTheta[0]
            r["dty"] = cd.imu.deltaTheta.deltaTheta[1]
            r["dtz"] = cd.imu.deltaTheta.deltaTheta[2]
            # dt = time interval between this sample and last in seconds
            r["dt"] = cd.imu.deltaTheta.deltaTime

        # Delta velocity — integrated acceleration between samples
        if cd.imu.deltaVel:
            # change in velocity since last sample in m/s, integrated from accelrometer between samples
            r["dvx"], r["dvy"], r["dvz"] = cd.imu.deltaVel[0], cd.imu.deltaVel[1], cd.imu.deltaVel[2]

        # Magnetometer — used for heading calibration
        # Magnetic feild strength in guass on each axis, used internally by VN-300 for heading
        if cd.imu.mag:
            r["mx"], r["my"], r["mz"] = cd.imu.mag[0], cd.imu.mag[1], cd.imu.mag[2]

        # Temperature and pressure
        if cd.imu.temperature:  # Internal sensor temperature in Celciius
            r["temp_c"] = cd.imu.temperature
        if cd.imu.pressure:  # air pressure in pascals - changes with altitude and weather
            r["pressure_pa"] = cd.imu.pressure

        # Raw GNSS — parse position and velocity from string
        if cd.gnss.gnss1Fix:  # fix type 3 = full  3D lock - car cannot move until this is 3, int() converts the object to a plain number
            r["gps_fix"] = int(cd.gnss.gnss1Fix)
        if cd.gnss.gnss1NumSats:  # number of satellites in view
            r["gps_sats"] = cd.gnss.gnss1NumSats
        if cd.gnss.gnss1PosLla:  # posLla has named attributes lat/lon/alt raw GPS position from satellites
            r["gps_lat"] = cd.gnss.gnss1PosLla.lat
            r["gps_lon"] = cd.gnss.gnss1PosLla.lon
            r["gps_alt"] = cd.gnss.gnss1PosLla.alt
        if cd.gnss.gnss1VelNed:  # raw GPS velocity in North East Down in m/s
            r["gps_vn"] = cd.gnss.gnss1VelNed[0]  # northward
            r["gps_ve"] = cd.gnss.gnss1VelNed[1]  # eastward
            r["gps_vd"] = cd.gnss.gnss1VelNed[2]  # downward
            # ground speed = 3D magnitude of velocity vector using pythagoras
            # sqrt(vn² + ve² + vd²) gives total speed in m/s
            r["gps_speed"] = (r["gps_vn"]**2 + r["gps_ve"] **
                              2 + r["gps_vd"]**2)**0.5  # Ground speed

            # Ground speed

        # INS fused solution — GPS + IMU combined, most accurate
        if cd.ins.posLla:
            # posLla has named attributes .lat .lon .alt
            # this is GPS + IMU combined
            # more accurate and smoother than raw gps_lat/lon above
            r["ins_lat"] = cd.ins.posLla.lat
            r["ins_lon"] = cd.ins.posLla.lon
            r["ins_alt"] = cd.ins.posLla.alt
        if cd.ins.velNed:
            # fused velocity North East Down in m/s
            # smoother than raw GPS velocity because IMU fills the gaps
            r["ins_vn"] = cd.ins.velNed[0]
            r["ins_ve"] = cd.ins.velNed[1]
            r["ins_vd"] = cd.ins.velNed[2]

        w.writerow(r)   # Write row to CSV
        f.flush()       # Flush so no data lost on crash (forces the data to physically write to the SD card immediately)
        n += 1          # Increment row counter

        # Print live status every 10 rows
        if n % 10 == 0:
            # conerts gps fix  number to readable word
            # 0 = no fix, 1 = time only, 2 = 2D fix, 3 = 3D fix(what we want while collecting the data)
            fix = {0: "NoFix", 1: "TimeOnly", 2: "2D",
                   3: "3D"}.get(r.get("gps_fix"), "?")
            # ins_lat is more accurate than gps_lat so we prefer it
            # if ins_lat is empty in this row, use gps_lat instead
            # if both are empty just show "?" so the print doesnt crash
            lat = r.get("ins_lat", r.get("gps_lat", "?"))
            # prints a one line summary to the terminal every 10 rows
            # so you can see the logger is alive and working during a run
            # shows: row number, GPS fix status, satellite count, latitude, heading, downforce
            print(f"[{n:5d}] {fix} | Sats:{r.get('gps_sats', '?')} | "
                  f"Lat:{lat} | Yaw:{r.get('yaw', '?')} | Az:{r.get('az', '?')}")

# instead of crashing with an error it exits cleanly and tells you how many rows were saved
except KeyboardInterrupt:
    print(f"\nStopped. {n} rows saved → {FILENAME}")
# closes the CSV file properly so no data is corrupted or lost
f.close()
# releases the serial port so the next time you run the script it can reconnect
# without this the port stays locked and you get "port already in use" error
sensor.disconnect()
print("Done.")
