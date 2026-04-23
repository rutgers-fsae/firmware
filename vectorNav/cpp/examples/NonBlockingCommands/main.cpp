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

#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include "vectornav/HAL/Duration.hpp"
#include "vectornav/HAL/Thread.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Interface/Commands.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/Registers.hpp"
#include "vectornav/Interface/Sensor.hpp"

using namespace VN;

int main(int argc, char* argv[])
{
    /*
    This example demonstrates how to send commands without blocking, validating each command was received correctly
    with different types of commands. For more information on non-blocking commands, please refer to the 'Advanced Functionality'
    section of the documentation.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Configure the asychronous ASCII output to YPR at 100 Hz
    3. Send generic command (Known Magnetic Disturbance command)
    4. Wait and check response from the unit
    5. Output VNYPR at 100 Hz and send velocity aiding commands at 10 Hz for 5 seconds:
        5.1. Check if a valid response has been received from the velocity aiding command
        5.2. Print response and send new command
    6. Disconnect from the VectorNav unit
    */

    // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    const std::string portName = (argc > 1) ? argv[1] : "COM1";  // Change the sensor port name to the COM port of your local machine
    Sensor sensor;
    Error latestError = static_cast<Error>(sensor.autoConnect(portName));
    if (latestError != Error::None)
    {
        std::cerr << "Error: " << latestError << " encountered when connecting to " + portName << std::endl;
        return static_cast<int>(latestError);
    }
    std::cout << "\nConnected to " << portName << " at " << sensor.connectedBaudRate().value() << std::endl;

    Registers::System::Model modelRegister;
    latestError = sensor.readRegister(&modelRegister);
    if (latestError != Error::None)
    {
        std::cout << "Error " << latestError << " encountered when reading register " << std::to_string(modelRegister.id()) << " (" << modelRegister.name()
                  << ")" << std::endl;
        return static_cast<int>(latestError);
    }
    std::string modelNumber = modelRegister.model;
    std::cout << "Sensor Model Number: " << modelNumber << std::endl;

    // 2. Configure the asynchronous ASCII output to YPR at 100 Hz
    Registers::System::AsyncOutputType asyncDataOutputType;
    asyncDataOutputType.ador = Registers::System::AsyncOutputType::Ador::YPR;  // Configure for Yaw, Pitch, Roll data
    asyncDataOutputType.serialPort = Registers::System::AsyncOutputType::SerialPort::Serial1;
    latestError = sensor.writeRegister(&asyncDataOutputType);
    if (latestError != Error::None)
    {
        std::cerr << "Error: " << latestError << " encountered when configuring ADOR." << std::endl;
        return static_cast<int>(latestError);
    }
    else { std::cout << "ADOR configured\n"; }

    Registers::System::AsyncOutputFreq asyncDataOutputFrequency;
    asyncDataOutputFrequency.adof = Registers::System::AsyncOutputFreq::Adof::Rate100Hz;  // Set output rate to 100Hz
    asyncDataOutputFrequency.serialPort = Registers::System::AsyncOutputFreq::SerialPort::Serial1;
    latestError = sensor.writeRegister(&asyncDataOutputFrequency);
    if (latestError != Error::None)
    {
        std::cerr << "Error: " << latestError << " encountered when configuring ADOF." << std::endl;
        return static_cast<int>(latestError);
    }
    else { std::cout << "ADOF configured\n\n"; }

    // 3. Send generic command (Known Magnetic Disturbance command)
    KnownMagneticDisturbance kmd(KnownMagneticDisturbance::State::Present);
    latestError = sensor.sendCommand(&kmd, Sensor::SendCommandBlockMode::None);  // Non-blocking
    if (latestError != Error::None)
    {
        std::cerr << "Error: " << latestError << " received while sending the Known Magnetic Disturbance Command." << std::endl;
        return static_cast<int>(latestError);
    }

    // 4. Wait and check response from the unit
#if (THREADING_ENABLE)
    thisThread::sleepFor(250ms);

#else  // Threading disabled
    // Without a blocking call, serial polling and triggering of packet identification/routing must be handled explicitly.
    // For more information on non-blocking commands, please refer to the 'Non-Os Usage' section of documentation/Documentation.html.
    Timer kmdTimer{250ms};
    kmdTimer.start();

    while (!kmdTimer.hasTimedOut())
    {
        const bool needsMoreData = sensor.processNextPacket();
        if (needsMoreData) { sensor.loadMainBufferFromSerial(); }
        else
        {
            if (!kmd.isAwaitingResponse()) { break; }
            std::this_thread::sleep_for(1ms);
        }
    }
#endif
    // We could check kmd.awaitingResponse() but it will be removed from the command queue (setting kmd.awaitingResponse to false) after
    // commandRemovalTimeoutLength (default 200ms), if any command has been sent or received since.
    if (kmd.hasValidResponse())
    {
        std::cout << "KMD Response: " << kmd.getResponse() << std::endl;
        std::optional<Error> error_maybe = kmd.getError();
        if (error_maybe.has_value())
        {
            std::cerr << "\tError: " << error_maybe.value() << std::endl;
            return static_cast<int>(error_maybe.value());
        }
    }
    else { std::cerr << "Error: KMD did not receive a valid response." << std::endl; }

    // 5. Output VNYPR at 100 Hz and send velocity aiding commands at 10 Hz for 5 seconds

    // Initialize velocity aiding register, command, counters, flags, and timers
    Registers::VelocityAiding::VelAidingMeas velAidRegister;
    velAidRegister.velocityX = 0.0f;
    velAidRegister.velocityY = 0.0f;
    velAidRegister.velocityZ = 0.0f;
    GenericCommand velAidWRGCommand;
    std::optional<GenericCommand> velAidWRGCmdOpt = velAidRegister.toWriteCommand();
    if (!velAidWRGCmdOpt.has_value())
    {
        std::cerr << "Error: Failed to create velocity aiding command." << std::endl;
        return static_cast<int>(Error::NotEnoughParameters);
    }
    else { velAidWRGCommand = velAidWRGCmdOpt.value(); }

    uint16_t asciiCount = 0;
    uint16_t velAidSentCount = 0;
    bool validResponseReceived = false;

    Timer mainTimer{5s};
    Timer resendTimer{500ms};
    mainTimer.start();
    resendTimer.start();

    while (!mainTimer.hasTimedOut())
    {
        // [if not THREADING_ENABLE] Because we're blocking on our high-rate measurement, packets (including commands)
        // will continue to be processed. We therefore don't need to do the manual while loop we did above
        Sensor::CompositeDataQueueReturn compositeData = sensor.getNextMeasurement(true);
        if (compositeData && compositeData->matchesMessage("VNYPR"))
        {
            const Ypr ypr = compositeData->attitude.ypr.value();
            std::cout << "YPR: " << ypr.yaw << ", " << ypr.pitch << ", " << ypr.roll << std::endl;
            asciiCount++;
        }

        // 5.1. Check if a valid response has been received from the velocity aiding command
        if (!validResponseReceived && !velAidWRGCommand.isAwaitingResponse())
        {
            const std::optional<Error> error_maybe = velAidWRGCommand.getError();
            if (velAidWRGCommand.hasValidResponse())
            {
                std::cout << "\nWRG Response: " << velAidWRGCommand.getResponse() << std::endl;
                validResponseReceived = true;
            }
            else if (error_maybe.has_value())
            {
                std::cerr << "Error: " << error_maybe.value() << std::endl;
                return static_cast<int>(error_maybe.value());
            }
        }

        // 5.2. Print response and send new command
        if (resendTimer.hasTimedOut())
        {
            if (!validResponseReceived && velAidSentCount > 0) { std::cerr << "\nError: Response Timeout\n" << std::endl; }
            velAidRegister.velocityX = static_cast<float>(std::rand());  // std::rand() to simulate different velocities
            velAidRegister.velocityY = static_cast<float>(std::rand());
            velAidRegister.velocityZ = static_cast<float>(std::rand());
            velAidWRGCmdOpt = velAidRegister.toWriteCommand();  // The GenericCommand object was instantiated before the loop.
            if (!velAidWRGCmdOpt.has_value())
            {
                std::cerr << "\tError: Failed to create velocity aiding command." << std::endl;
                return static_cast<int>(Error::NotEnoughParameters);
            }
            else { velAidWRGCommand = velAidWRGCmdOpt.value(); }
            latestError = sensor.sendCommand(&velAidWRGCommand, Sensor::SendCommandBlockMode::None);  // Non-blocking
            if (latestError != Error::None) { std::cerr << "Error " << latestError << " encountered when writing to register." << std::endl; }

            validResponseReceived = false;
            velAidSentCount++;
            resendTimer.start();  // Restart send timer
        }
    }

    std::cout << "\nTotal ASCII YPR Packets Received: " << asciiCount << std::endl;
    std::cout << "Total VelAid Commands Sent: " << velAidSentCount << std::endl;
    std::cout << "\nNonBlockingCommands example complete" << std::endl;

    // 6. Disconnect from the VectorNav unit
    sensor.disconnect();

    return 0;
}
