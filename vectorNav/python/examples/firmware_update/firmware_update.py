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

import sys

import vectornav
from vectornav import Sensor
from vectornav.Plugins import FirmwareProgrammer

usage = "[--PortName={port_name}] [--{Processor}={file_path}... | --VnXml={file_path}] [--firmwareBaudRate={baudRate}] [--bootloaderBaudRate={baudRate}]"


class FirmwareUpdateArgs:
    portName: str
    filePaths: str
    params: FirmwareProgrammer.FirmwareUpdater.Params = FirmwareProgrammer.FirmwareUpdater.Params()


def getIndexOfArg(needle, haystack):
    for i, arg in enumerate(haystack):
        if needle in arg:
            return i + 1
    return 0


def main(argv):
    """
    This firmware update example walks through the Python usage of the SDK to connect to and update the firmware on a VectorNav unit.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Create a FirmwareUpdater object
    3. Update the firmware based on the file type
    4. Disconnect from the VectorNav unit
    """

    # Parse command line arguments
    fwArgs = FirmwareUpdateArgs()

    # Define the port connection parameters to be used later
    if idx := getIndexOfArg("--PortName=", argv):
        fwArgs.portName = argv[idx - 1].split("=")[-1]
    else:
        print(f"PortName Required: \n usage -> {usage}")
        return

    # Get path to vnXml file if using .vnXml to update firmware
    if idx := getIndexOfArg("--vnXml=", argv):
        fwArgs.filePaths = argv[idx - 1].split("=")[-1]
    else:
        fwArgs.filePaths = vectornav.FilePaths()

    # Parse list of processors and paths to .vnx files to update different processors
    if idx := getIndexOfArg("--Nav=", argv):
        fwArgs.filePaths.push_back(
            FirmwareProgrammer.FirmwareUpdater.File(
                argv[idx - 1].split("=")[-1], FirmwareProgrammer.FirmwareUpdater.Processor.Nav
            )
        )
    if idx := getIndexOfArg("--Gnss=", argv):
        fwArgs.filePaths.push_back(
            FirmwareProgrammer.FirmwareUpdater.File(
                argv[idx - 1].split("=")[-1], FirmwareProgrammer.FirmwareUpdater.Processor.Gnss
            )
        )

    if idx := getIndexOfArg("--Imu=", argv):
        fwArgs.filePaths.push_back(
            FirmwareProgrammer.FirmwareUpdater.File(
                argv[idx - 1].split("=")[-1], FirmwareProgrammer.FirmwareUpdater.Processor.Imu
            )
        )

    # Parse optional arguments for firmware and bootloader baud rates
    if idx := getIndexOfArg("--firmwareBaudRate=", argv):
        fwArgs.params.firmwareBaudRate = int(argv[idx - 1].split("=")[-1])

    if idx := getIndexOfArg("--bootloaderBaudRate=", argv):
        fwArgs.params.bootloaderBaudRate = int(argv[idx - 1].split("=")[-1])

    # 1. Instantiate a Sensor object and use it to connect to the VectorNav unit.
    #  We are not autoconnecting or verifying connectivity because we cannot assume the sensor has a valid firmware
    sensor = Sensor()
    try:
        sensor.connect(fwArgs.portName, fwArgs.params.firmwareBaudRate)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when connecting to {fwArgs.portName}.\n")
        return
    print(f"Connected to {sensor.connectedPortName()} at {sensor.connectedBaudRate()}")

    # 2. Create a FirmwareUpdater object
    firmwareUpdater = FirmwareProgrammer.FirmwareUpdater()

    # 3. Update firmware based on the file type - There are two file types that can be used to update the firmware: VNX or VNXML
    try:
        firmwareUpdater.updateFirmware(sensor, fwArgs.filePaths, fwArgs.params)
    except Exception as firmwareUpdateError:
        print(f"Error: {firmwareUpdateError} firmware update failed.")
        return

    # Handle asynchronous errors that occurred during firmware upgrade
    while True:
        try:
            sensor.throwIfAsyncError()
            break
        except Exception as asyncError:
            print(f"Received async error: {asyncError}")

    # 4. Disconnect from the VectorNav unit
    sensor.disconnect()
    print("FirmwareUpdate example complete")


if __name__ == "__main__":
    main(sys.argv[1:])
