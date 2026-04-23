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

import vectornav
from vectornav import Sensor
from vectornav.Plugins import DataExport

def main(argv):
    """
    This data export example walks through the Python usage of the SDK to export a binary file to a CSV or ASCII file.

    This example will achieve the following:
    1. Create a FileExporter object
    2. Add exporters for CSV, ASCII, and skipped bytes
    3. Process file
    4. Print parsing stats
    """

    # Parse command line arguments
    parent_dir = os.path.dirname(os.path.abspath(__file__))
    filePath = argv[0] if argv else os.path.join(parent_dir, "data_export_from_file.bin")
    outputPath = argv[1] if len(argv) == 2 else parent_dir

    print(f"Exporting {filePath}")
    print(f"Outputting to {outputPath}\n")

    # 1. Create a Sensor object for file processing
    pushToCompositeData = Sensor.MeasQueueMode.Off
    mainBuffer = vectornav.ByteBuffer(65536)
    fbBuffer   = vectornav.ByteBuffer(2048)

    sensor = Sensor(mainBuffer, fbBuffer, pushToCompositeData)

    # 2. Add exporters for CSV, ASCII, and skipped bytes and subscribe to sensor messages
    asciiExporter = DataExport.ExporterAscii(outputPath, DataExport.PacketQueueMode.Retry)
    sensor.subscribeToMessage(
        asciiExporter.getQueuePtr(), "VN", Sensor.AsciiSubscriberFilterType.StartsWith
    )
    
    csvExporter = DataExport.ExporterCsv(outputPath, DataExport.PacketQueueMode.Retry)
    sensor.subscribeToMessage(
        csvExporter.getQueuePtr(),
        Sensor.BinaryOutputMeasurements(),
        Sensor.FaSubscriberFilterType.AnyMatch
    )
    sensor.subscribeToMessage(
        csvExporter.getQueuePtr(), "VN", Sensor.AsciiSubscriberFilterType.StartsWith
    )

    skippedByteExporter = DataExport.ExporterSkippedByte(outputPath, DataExport.PacketQueueMode.Retry)
    sensor.subscribeToMessage(skippedByteExporter.getQueuePtr(), Sensor.SyncByte.none)
    
    # 3. Kick off exporter threads
    asciiExporter.start()
    csvExporter.start()
    skippedByteExporter.start()

    # 4. Connect to file, monitoring AsyncError queue for FileReadFailed error to indicate end of file reached
    sensor.connect(filePath)
    while True:
        time.sleep(0.001)

        # Handle asynchronous errors
        try:
            sensor.throwIfAsyncError()
        except vectornav.FileReadFailed:
            break
        except Exception as asyncError:
            print(f"Received async error: {asyncError}")
    
    sensor.disconnect()
    
    # 5. Stop exporters
    asciiExporter.stop()
    csvExporter.stop()
    skippedByteExporter.stop()
    
    print("DataExportFromFile example complete.")
    
if __name__ == "__main__":
    main(sys.argv[1:])
