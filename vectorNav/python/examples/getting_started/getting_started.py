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
import time

from vectornav import Registers, Sensor


def main(argv):
    """
    This getting started example walks through the Python usage of the SDK to connect to and interact with a VectorNav unit.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Poll and print the model number using a read register command
    3. Poll and print the current yaw, pitch, and roll using a read register command
    4. Configure the asynchronous ASCII output to YPR at 2Hz
    5. Configure the first binary output message to output timeStartup, accel, and angRate, all from common group, at a 2 Hz output rate (1 Hz if VN-300) through both serial ports
    6. Enter a loop for 5 seconds where it:
       Determines which measurement it received (VNYPR or the necessary binary header)
       Prints out the relevant measurement from the CompositeData struct
    7. Disconnect from the VectorNav unit
    """

    # Define the port connection parameters
    portName = argv[0] if argv else "COM1"  # Change the sensor port name to the com port of your local machine

    # 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    sensor = Sensor()
    try:
        sensor.autoConnect(portName)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when connecting to {portName}.\n")
        return
    print(f"Connected to {portName} at {sensor.connectedBaudRate()}")

    # 2. Poll and print the model number using a read register command
    # Create an empty register object of the necessary type, where the data member will be populated when the sensor responds to the "read register" request
    modelRegister = Registers.System.Model()
    try:
        sensor.readRegister(modelRegister)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when reading register 1 (Model).\n")
        return
    print(f"Sensor Model Number: {modelRegister.model}")

    # 3. Poll and print the current yaw, pitch, and roll using a read register command
    yprRegister = Registers.Attitude.YawPitchRoll()
    try:
        sensor.readRegister(yprRegister)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when reading 8 (YawPitchRoll).\n")
        return
    print(f"Current Reading:\n\tYaw: {yprRegister.yaw} , Pitch: {yprRegister.pitch} , Roll: {yprRegister.roll} ")

    # 4. Configure the asynchronous ASCII output to YPR at 2Hz
    asyncDataOutputType = Registers.System.AsyncOutputType()
    asyncDataOutputType.ador = Registers.System.AsyncOutputType.Ador.YPR
    asyncDataOutputType.serialPort = Registers.System.AsyncOutputType.SerialPort.Serial1
    try:
        sensor.writeRegister(asyncDataOutputType)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when writing register 6 (AsyncOutputType).\n")
        return
    print("ADOR Configured")

    asyncDataOutputFrequency = Registers.System.AsyncOutputFreq()
    asyncDataOutputFrequency.adof = Registers.System.AsyncOutputFreq.Adof.Rate2Hz
    asyncDataOutputFrequency.serialPort = Registers.System.AsyncOutputFreq.SerialPort.Serial1
    try:
        sensor.writeRegister(asyncDataOutputFrequency)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when writing register 7 (AsyncOutputFreq).\n")
        return
    print("ADOF Configured")

    # 5. Configure the first binary output message to output timeStartup, accel, and angRate, all from common group, at a 2 Hz output rate (1 Hz if VN-300) through both serial ports
    binaryOutput1Register = Registers.System.BinaryOutput1()
    binaryOutput1Register.asyncMode.serial1 = 1
    binaryOutput1Register.asyncMode.serial2 = 1
    binaryOutput1Register.rateDivisor = 400
    binaryOutput1Register.common.timeStartup = 1
    binaryOutput1Register.common.accel = 1
    binaryOutput1Register.common.angularRate = 1
    binaryOutput1Register.common.imu = 1

    try:
        sensor.writeRegister(binaryOutput1Register)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when writing register 75 (BinaryOutput1).\n")
        return
    print("Binary output 1 message configured")

    # 6. Enter a loop for 5 seconds where it:
    #      Determines which measurement it received (VNYPR or the necessary binary header)
    #      Prints out the relevant measurement from the CompositeData struct
    t0 = time.time()
    while time.time() - t0 < 5:
        compositeData = sensor.getNextMeasurement()
        # Check to make sure that a measurement is available
        if not compositeData:
            continue

        if compositeData.matchesMessage(binaryOutput1Register):
            print("Found binary 1 measurement.")

            timeStartup = compositeData.time.timeStartup.nanoseconds()
            print(f"\tTime: {timeStartup}")
            accel = compositeData.imu.accel
            print(f"\tAccel X: {accel[0]}\n\tAccel Y: {accel[1]}\n\tAccel Z: {accel[2]}")
        elif compositeData.matchesMessage("VNYPR"):
            print("Found ASCII YPR measurement.")

            ypr = compositeData.attitude.ypr
            print(f"\tYaw: {ypr.yaw}\n\tPitch: {ypr.pitch}\n\tRoll: {ypr.roll}")
        else:
            print("Unrecognized asynchronous message received")

        # Handle asynchronous errors
        try:
            sensor.throwIfAsyncError()
        except Exception as asyncError:
            print(f"Received async error: {asyncError}");
  
    # 7. Disconnect from the VectorNav unit
    sensor.disconnect()
    print("Sensor disconnected")
    print("GettingStarted example complete.")


if __name__ == "__main__":
    main(sys.argv[1:])
