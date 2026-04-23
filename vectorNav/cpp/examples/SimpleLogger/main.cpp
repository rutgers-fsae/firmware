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

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/Registers.hpp"
#include "vectornav/Interface/Sensor.hpp"
#include "vectornav/SimpleLogger.hpp"
#include "vectornav/TemplateLibrary/ByteBuffer.hpp"

using namespace VN;
namespace fs = std::filesystem;

std::string usage = "[port] [path]\n";

int main(int argc, char* argv[])
{
    /*
    This simple logger example walks through the C++ usage of the SDK to log data from a VectorNav unit.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Set up resources needed for data logging
    3. Register the logger's input buffer to receive all bytes from the unit
    4. Log data from the VectorNav unit
        4.1. Check for any asynchronous errors
    5. Disconnect from the VectorNav unit
    */

    // Pass in port name and path as positional arguments, or edit them here
    const std::string portName = (argc > 1) ? argv[1] : "COM1";
    const std::string filePath = (argc > 2) ? argv[2] : (fs::path(__FILE__).parent_path() / "log.bin").string();

    // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    Sensor sensor;
    Error latestError = sensor.autoConnect(portName);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " encountered when connecting to " + portName << std::endl;
        return static_cast<int>(latestError);
    }
    std::cout << "Connected to " << portName << " at " << sensor.connectedBaudRate().value() << std::endl;

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

    // 2. Set up resources needed for data logging
    ByteBuffer bufferToLog{1024 * 3};
    Logger::SimpleLogger logger{bufferToLog, filePath};

    // 3. Register the logger's input buffer to receive all bytes from the sensor
    sensor.registerReceivedByteBuffer(&bufferToLog);

    // 4. Log data from the VectorNav unit
    if (logger.start())
    {
        std::cout << "Error: Failed to write to file." << std::endl;
        return 1;
    }
    std::cout << "Logging to " << filePath << std::endl;

    Timer timer{5s};
    timer.start();
    std::optional<AsyncError> asyncError;
    while (!timer.hasTimedOut())
    {
        std::this_thread::sleep_for(1ms);
        // 4.1. Check for any asynchronous errors - Any buffer overruns means data has been dropped.
        asyncError = sensor.getNextAsyncError();
        if (asyncError) { std::cout << "Received async error: " << asyncError->error << std::endl; }
    }

    logger.stop();
    sensor.deregisterReceivedByteBuffer();

    std::cout << "Logged " << std::to_string(logger.numBytesLogged()) << " bytes." << std::endl;

    // 5. Disconnect from the VectorNav unit
    sensor.disconnect();
    std::cout << "SimpleLogger example complete.";
}
