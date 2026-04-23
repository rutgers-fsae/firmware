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

import random
import sys
import time

from vectornav import GenericCommand, KnownMagneticDisturbance, Registers, Sensor


def main(argv):
    """
    This example demonstrates how to send commands without blocking, validating each command was received correctly
    with different types of commands. For more information on non-blocking commands, please refer to the 'Advanced Functionality'
    section of the documentation.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Configure the asynchronous ASCII output to YPR at 100 Hz
    3. Send generic command (Known Magnetic Disturbance command)
    4. Wait and check response from the unit
    5. Output VNYPR at 100 Hz and send velocity aiding commands at 10 Hz for 5 seconds:
        5.1. Check if a valid response has been received from the velocity aiding command
        5.2. Print response and send new command
    6. Disconnect from the VectorNav unit'
    """

    # 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    portName = argv[0] if argv else "COM1"  # Change the sensor port name to the com port of your local machine
    sensor = Sensor()
    try:
        sensor.autoConnect(portName)
        if not sensor.verifySensorConnectivity():
            raise RuntimeError("VerificationFailure")
    except Exception as latestError:
        print(f"Error: {latestError} encountered when connecting to {portName}.\n")
        return
    print(f"Connected to {portName} at {sensor.connectedBaudRate()}")

    modelRegister = Registers.System.Model()
    try:
        sensor.readRegister(modelRegister)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when reading register 1 (Model).\n")
        return
    print(f"Sensor Model Number: {modelRegister.model}")

    # 2. Configure the asynchronous ASCII output to YPR at 100 Hz
    asyncDataOutputType = Registers.System.AsyncOutputType()
    asyncDataOutputType.ador = asyncDataOutputType.Ador.YPR  # Configure for Yaw, Pitch, Roll data
    asyncDataOutputType.serialPort = asyncDataOutputType.SerialPort.Serial1
    try:
        sensor.writeRegister(asyncDataOutputType)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when writing register 6 (AsyncOutputType).\n")
        return
    print("ADOR Configured")

    asyncDataOutputFrequency = Registers.System.AsyncOutputFreq()
    asyncDataOutputFrequency.adof = asyncDataOutputFrequency.Adof.Rate100Hz  # Set output rate to 100Hz
    asyncDataOutputFrequency.serialPort = asyncDataOutputFrequency.SerialPort.Serial1
    try:
        sensor.writeRegister(asyncDataOutputFrequency)
    except Exception as latestError:
        print(f"Error: {latestError} encountered when writing register 7 (AsyncOutputFreq).\n")
        return
    print("ADOF Configured")

    # 3. Send generic command (Known Magnetic Disturbance command)
    kmd = KnownMagneticDisturbance(KnownMagneticDisturbance.State.Present)
    try:
        sensor.sendCommand(kmd, Sensor.SendCommandBlockMode.none)  # Non-blocking
    except Exception as latestError:
        print(f"Error: {latestError} received while sending the Known Magnetic Disturbance Command.\n")
        return
    # 4. Wait and check response from the unit
    time.sleep(0.25)

    # We could check kmd.awaitingResponse() but it will be removed from the command queue (setting kmd.awaitingResponse to false) after
    # commandRemovalTimeoutLength (default 200ms), if any command has been sent or received since.
    if kmd.hasValidResponse():
        print(f"KMD Response: {kmd.getResponse()}")
        error_maybe = kmd.getError()
        if error_maybe is not None:
            print(f"\tError: {error_maybe}")
            return
    else:
        print("Error: KMD did not receive a valid response.")

    # 5. Output VNYPR at 100 Hz and send velocity aiding commands at 10 Hz for 5 seconds

    # Initialize velocity aiding register, command, counters, flags, and timers
    velAidRegister = Registers.VelocityAiding.VelAidingMeas()
    velAidRegister.velocityX = 0.0
    velAidRegister.velocityY = 0.0
    velAidRegister.velocityZ = 0.0
    velAidWRGCmdOpt = velAidRegister.toWriteCommand()
    if velAidWRGCmdOpt is None:
        print("Error: Failed to create velocity aiding command.")
        return
    else:
        velAidWRGCommand = velAidWRGCmdOpt

    asciiCount = 0
    velAidSentCount = 0
    validResponseReceived = False

    start_time = time.time()
    resend_time = time.time()

    while time.time() - start_time < 5:
        compositeData = sensor.getNextMeasurement(True)
        if compositeData and compositeData.matchesMessage("VNYPR"):
            ypr = compositeData.attitude.ypr
            print(f"YPR: {ypr.yaw}, {ypr.pitch}, {ypr.roll}")
            asciiCount += 1

        # 5.1. Check if a valid response has been received from the velocity aiding command
        if not validResponseReceived and not velAidWRGCommand.isAwaitingResponse():
            error_maybe = velAidWRGCommand.getError()
            if velAidWRGCommand.hasValidResponse():
                print(f"\nWRG Response: {velAidWRGCommand.getResponse()}")
                validResponseReceived = True
            elif error_maybe is not None:
                print(f"Error: {error_maybe}")
                return

        # 5.2. Print response and send new command
        if (time.time() - resend_time) > 0.5:
            if not validResponseReceived and velAidSentCount > 0:
                print("Error: Response Timeout")
                return

            velAidRegister.velocityX = random.random()  # random.random() to simulate different velocities
            velAidRegister.velocityY = random.random()
            velAidRegister.velocityZ = random.random()
            velAidWRGCmdOpt = velAidRegister.toWriteCommand()
            if velAidWRGCmdOpt is None:
                print("Error: Failed to create velocity aiding command.")
                return
            else:
                velAidWRGCommand = velAidWRGCmdOpt
            try:
                sensor.sendCommand(velAidWRGCommand, Sensor.SendCommandBlockMode.none)  # Non-blocking
            except Exception as latestError:
                print(f"Error: {latestError} encountered when writing to register.\n")

            validResponseReceived = False
            velAidSentCount += 1
            resend_time = time.time()  # Restart send timer
      
        # Handle asynchronous errors
        try:
            sensor.throwIfAsyncError()
        except Exception as asyncError:
            print(f"Received async error: {asyncError}");

    print(f"\nTotal ASCII YPR Packets Received: {asciiCount}")
    print(f"Total Velocity Aiding Commands Sent: {velAidSentCount}")
    print("\nNonBlockingCommands example complete")

    # 6. Disconnect from the VectorNav unit
    sensor.disconnect()


if __name__ == "__main__":
    main(sys.argv[1:])
