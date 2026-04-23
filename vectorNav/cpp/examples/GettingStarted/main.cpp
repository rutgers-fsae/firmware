// VectorNav SDK (v1.0.0)
// Copyright (c) 2024 VectorNav Technologies, LLC
// 
// WARNING â€“ This software contains Controlled Technical Data, export of which
// is restricted by the Export Administration Regulations ("EAR") (ECCN 7D994). 
// Disclosure to foreign persons contrary to the EAR is prohibited. Violations
// of these export laws and regulations are subject to severe civil and criminal
// penalties.
// 
// The MIT License (MIT)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <iostream>
#include <string>

#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/Registers.hpp"
#include "vectornav/Interface/Sensor.hpp"

using namespace VN;

std::string usage = "[port]\n";

int main(int argc, char* argv[])
{
    /*
    This getting started example walks through the C++ usage of the SDK to connect to and interact with a VectorNav unit.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Poll and print the model number using a read register command
    3. Poll and print the current yaw, pitch, and roll using a read register command
    4. Configure the asynchronous ASCII output to YPR at 2 Hz
    5. Configure the first binary output message to output timeStartup, accel, and angRate, all from common group, at a 2 Hz output rate (1 Hz if VN-300)
    through both serial ports.
    6. Enter a loop for 5 seconds where it:
       Determines which measurement it received (VNYPR or the necessary binary header)
       Prints out the relevant measurement from the CompositeData struct
    7. Disconnect from the VectorNav unit
    */

    // Define the port connection parameters to be used later
    const std::string portName = (argc > 1) ? argv[1] : "COM1";  // Change the sensor port name to the COM port of your local machine

    // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    Sensor sensor;
    Error latestError = sensor.autoConnect(portName);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " encountered when connecting to " + portName << std::endl;
        return static_cast<int>(latestError);
    }
    std::cout << "Connected to " << portName << " at " << sensor.connectedBaudRate().value() << std::endl;

    // 2. Poll and print the model number using a read register command
    // Create an empty register object of the necessary type, where the data member will be populated when the sensor responds to our "read register" request
    Registers::System::Model modelRegister;
    latestError = sensor.readRegister(&modelRegister);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " encountered when reading register " << std::to_string(modelRegister.id()) << " (" << modelRegister.name() << ")"
                  << std::endl;
        return static_cast<int>(latestError);
    }
    std::string modelNumber = modelRegister.model;
    std::cout << "Sensor Model Number: " << modelNumber << std::endl;

    // 3. Poll and print the current yaw, pitch, and roll using a read register command
    Registers::Attitude::YawPitchRoll yprRegister;
    latestError = sensor.readRegister(&yprRegister);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " encountered when reading register " << std::to_string(yprRegister.id()) << " (" << yprRegister.name() << ")" << std::endl;
        return static_cast<int>(latestError);
    }
    std::cout << "Current Reading:\n\tYaw: " << yprRegister.yaw << " , Pitch: " << yprRegister.pitch << " , Roll: " << yprRegister.roll << std::endl;

    // 4. Configure the asynchronous ASCII output to YPR at 2 Hz
    Registers::System::AsyncOutputType asyncDataOutputType;
    asyncDataOutputType.ador = Registers::System::AsyncOutputType::Ador::YPR;
    asyncDataOutputType.serialPort = Registers::System::AsyncOutputType::SerialPort::Serial1;
    latestError = sensor.writeRegister(&asyncDataOutputType);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " encountered when configuring register " << std::to_string(asyncDataOutputType.id()) << " (" << asyncDataOutputType.name()
                  << ")" << std::endl;
        return static_cast<int>(latestError);
    }
    else { std::cout << "ADOR configured\n"; }

    Registers::System::AsyncOutputFreq asyncDataOutputFrequency;
    asyncDataOutputFrequency.adof = Registers::System::AsyncOutputFreq::Adof::Rate2Hz;
    asyncDataOutputFrequency.serialPort = Registers::System::AsyncOutputFreq::SerialPort::Serial1;
    latestError = sensor.writeRegister(&asyncDataOutputFrequency);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " encountered when configuring register " << std::to_string(asyncDataOutputFrequency.id()) << " ("
                  << asyncDataOutputFrequency.name() << ")" << std::endl;
        return static_cast<int>(latestError);
    }
    else { std::cout << "ADOF configured\n"; }

    // 5. Configure the first binary output message to output timeStartup, accel, and angRate, all from common group, at a 2 Hz output rate (1 Hz if VN-300)
    // through both serial ports.
    Registers::System::BinaryOutput1 binaryOutput1Register;
    binaryOutput1Register.asyncMode.emplace();
    binaryOutput1Register.asyncMode->serial1 = true;
    binaryOutput1Register.asyncMode->serial2 = true;
    binaryOutput1Register.rateDivisor = 400;
    binaryOutput1Register.common.timeStartup = true;
    binaryOutput1Register.common.accel = true;
    binaryOutput1Register.common.angularRate = true;
    binaryOutput1Register.common.imu = true;

    latestError = sensor.writeRegister(&binaryOutput1Register);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " encountered when configuring register " << std::to_string(binaryOutput1Register.id()) << " ("
                  << binaryOutput1Register.name() << ")" << std::endl;
        return static_cast<int>(latestError);
    }
    else { std::cout << "Binary output 1 message configured.\n"; }

    // 6. Enter a loop for 5 seconds where it:
    //     Determines which message it received (VNYPR or the binary output)
    //     Prints out the relevant measurement from the CompositeData struct
    Timer timer{5s};
    timer.start();
    while (!timer.hasTimedOut())
    {
        Sensor::CompositeDataQueueReturn compositeData = sensor.getNextMeasurement();
        // Check to make sure that a measurement is available
        if (!compositeData) continue;

        if (compositeData->matchesMessage(binaryOutput1Register))
        {
            std::cout << "Found binary 1 measurment.\n";

            auto timeStartup = compositeData->time.timeStartup.value().nanoseconds();
            std::cout << "\tTime: " << timeStartup << "\n";
            Vec3f accel = compositeData->imu.accel.value();
            std::cout << "\tAccel X: " << accel[0] << "\n\tAccel Y: " << accel[1] << "\n\tAccel Z: " << accel[2] << "\n";
        }
        else if (compositeData->matchesMessage("VNYPR"))
        {
            std::cout << "Found Ascii ypr measurement.\n";

            Ypr ypr = compositeData->attitude.ypr.value();
            std::cout << "\tYaw: " << ypr.yaw << "\n\tPitch: " << ypr.pitch << "\n\tRoll: " << ypr.roll << std::endl;
        }
        else { std::cout << "Unrecognized asynchronous message received.\n"; }
    }

    // 7. Disconnect from the VectorNav unit
    sensor.disconnect();
    std::cout << "Sensor disconnected.\n";
    std::cout << "GettingStarted example complete." << std::endl;
}
