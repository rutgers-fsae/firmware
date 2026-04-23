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
from enum import Enum

from vectornav import Sensor
from vectornav.Plugins import RegisterScan

usage = '[port] [fileLocation] (or "generic")'


class WriteType(Enum):
    Xml = 1
    Ascii = 2
    Generic = 3


def main(argv):
    """
    This save configuration example walks through the Python usage of the SDK to save the current non-default configuration of a VectorNav unit into an XML file or ASCII file.

    This example will achieve the following:
    1. Handle input arguments
    2. Instantiate a Sensor object and use it to connect to the VectorNav unit
    3. Save all non-default values from the VectorNav unit as specified
    4. Disconnect from the VectorNav unit
    """

    # 1. Handle input arguments
    if len(argv) > 2:
        print("Usage: python saveNonDefaultConfiguration", usage)
        return
    if len(argv) > 0:
        portName = argv[0]
    else:
        portName = "COM1"  # Pass in the portname as a positional argument or edit here
    if len(argv) > 1:
        filePath = argv[1]
    else:
        current_folder = os.getcwd()
        filePath = os.path.join(current_folder, "configSettings.txt")

    if filePath == "generic":
        writeType = WriteType.Generic
    else:
        _, file_extension = os.path.splitext(filePath)
        if file_extension == "":
            print("Error: File path extension was not specified")
            print("Usage: python saveNonDefaultConfiguration", usage)
            return
        elif file_extension == ".xml":
            writeType = WriteType.Xml
        else:
            writeType = WriteType.Ascii

    # 2. Instantiate a Sensor object and use it to connect to the VectorNav unit
    sensor = Sensor()
    try:
        sensor.autoConnect(portName, True)  # enables throwing from async error monitoring thread
    except Exception as latestError:
        print(f"Error: {latestError} encountered when connecting to {portName}.\n")
        return
    print(f"Connected to {portName} at {sensor.connectedBaudRate()}")

    # 3. Save all non-default values from the VectorNav unit as specified  - There are a few different file types
    # that can be used to save the current non-default configuration settings from a VectorNav unit into
    # a configuration file, such as an XML file or text file. As part of the
    # RegisterScan.saveNonDefaultConfiguration() method, a Restore Factory Settings command will be issued
    # to the VectorNav unit. When using this method, it is highly recommended to monitor the progress of
    # this call as any errors encountered could mean that the settings previously configured were not able
    # to be reapplied onto the unit. If performing a Restore Factory Settings command is not desired, the
    # RegisterScan.saveConfiguration() method should instead be used to save the current configuration of
    # a VectorNav unit.
    if writeType == WriteType.Xml:
        configWriter = RegisterScan.XmlConfigWriter(sensor, filePath)
    elif writeType == WriteType.Ascii:
        configWriter = RegisterScan.AsciiConfigWriter(filePath)
    else:
        print("Generic saveNonDefaultConfiguration example unimplemented.")
        sensor.disconnect()
        return

    try:
        RegisterScan.saveNonDefaultConfiguration(sensor, configWriter)
    except Exception as latestError:
        print(f"Error: {latestError} occured when loading configuration.")
        return

    # 4. Disconnect from the VectorNav unit
    sensor.disconnect()
    print("SaveNonDefaultConfiguration example complete")


if __name__ == "__main__":
    main(sys.argv[1:])
