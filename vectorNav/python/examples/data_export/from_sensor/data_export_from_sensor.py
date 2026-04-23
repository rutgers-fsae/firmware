# VectorNav SDK (v1.0.0)
# Copyright (c) 2024 VectorNav Technologies, LLC
# 
# WARNING â€“ This software contains Controlled Technical Data, export of which
# is restricted by the Export Administration Regulations ("EAR") (ECCN 7D994). 
# Disclosure to foreign persons contrary to the EAR is prohibited. Violations
# of these export laws and regulations are subject to severe civil and criminal
# penalties.
# 
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import os
import sys
import time

from vectornav import Sensor
from vectornav.Plugins import DataExport


def main(argv):
    """
    This data export example walks through the Python usage of the SDK to export data from a VectorNav unit to a CSV file.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Create an ExportCsv object
    3. Add a subscriber to all binary and ASCII packets
    4. Log data from the VectorNav unit
    5. Disconnect from the VectorNav unit
    """

    # Parse command line arguments
    portName = argv[0] if argv else "COM1"
    outputDirectory = argv[1] if len(argv) == 2 else os.path.dirname(os.path.abspath(__file__))
    print(outputDirectory)

    # 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    sensor = Sensor()
    try:
        sensor.autoConnect(portName)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when connecting to {portName}.\n")
        return
    print(f"Connected to {portName} at {sensor.connectedBaudRate()}")

    # 2. Create an ExportCsv object
    csvExporter = DataExport.ExporterCsv(outputDirectory)

    # 3. Add a subscriber to all binary and ASCII packets
    try:
        sensor.subscribeToMessage(
            csvExporter.getQueuePtr(),
            Sensor.BinaryOutputMeasurements(),
            Sensor.FaSubscriberFilterType.AnyMatch
        )
    except Exception as latestError:
        print(f"Error: {latestError} encountered when subscribing.\n")
        return

    try:
        sensor.subscribeToMessage(
            csvExporter.getQueuePtr(), "VN", Sensor.AsciiSubscriberFilterType.StartsWith
        )
    except Exception as latestError:
        print(f"Error: {latestError} encountered when subscribing.\n")
        return

    # 4. Log data from the VectorNav unit
    csvExporter.start()
    print("Logging to ", outputDirectory)

    start_time = time.time()

    while (time.time() - start_time) < 5:
        time.sleep(0.001)

        # Handle asynchronous errors
        try:
            sensor.throwIfAsyncError()
        except Exception as asyncError:
            print(f"Received async error: {asyncError}");

    csvExporter.stop()

    # 5. Disconnect from the VectorNav unit
    sensor.disconnect()
    print("DataExportFromSensor example complete.")


if __name__ == "__main__":
    main(sys.argv[1:])
