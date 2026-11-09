#!/usr/bin/env python3
"""Read bytes from an Arduino Nano (I2C peripheral) through an Adafruit FT232H and print them.

Setup (macOS):
    brew install libusb
    pip3 install pyftdi adafruit-blinka
    export BLINKA_FT232H=1

Wiring: FT232H D0 -> Nano A5 (SCL), FT232H D1 -> Nano A4 (SDA), GND -> GND.
Flip the FT232H "I2C" switch ON (older boards without the switch: jumper D1 to D2).

Nano side (Wire peripheral at 0x08):
    #include <Wire.h>
    void onReq() { Wire.write("hello"); }
    void setup() { Wire.begin(0x08); Wire.onRequest(onReq); }
    void loop() {}

Usage: python3 ft232h_i2c_read.py [--addr 0x08] [--len 5] [--period 0.5]
"""

import argparse
import os
import time

os.environ.setdefault("BLINKA_FT232H", "1")  # must be set before importing board
import board  # noqa: E402

p = argparse.ArgumentParser()
p.add_argument("--addr", type=lambda s: int(s, 0), default=0x08)
p.add_argument("--len", type=int, default=5)
p.add_argument("--period", type=float, default=0.5)
args = p.parse_args()

i2c = board.I2C()
while not i2c.try_lock():
    pass
print("devices found:", [hex(a) for a in i2c.scan()])

buf = bytearray(args.len)
try:
    while True:
        try:
            i2c.readfrom_into(args.addr, buf)
            print(buf.hex(" "), "|", buf.decode("ascii", "replace"), flush=True)
        except OSError as e:  # NACK / Nano not ready; keep going
            print("read failed:", e, flush=True)
        time.sleep(args.period)
except KeyboardInterrupt:
    pass
finally:
    i2c.unlock()
