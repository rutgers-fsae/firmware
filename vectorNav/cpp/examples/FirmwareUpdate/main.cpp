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

#include <optional>
#include <variant>

#include "vectornav/FirmwareUpdater.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/Sensor.hpp"

using namespace VN;

std::string usage =
    "[--PortName={port_name}] [--{Processor}={file_path}... | --vnXml={file_path}] [--firmwareBaudRate={baudRate}] [--bootloaderBaudRate={baudRate}]";

struct ParsedArgs
{
    std::optional<Serial_Base::PortName> portName;
    std::optional<std::variant<FirmwareProgrammer::FirmwareUpdater::FilePaths, Filesystem::FilePath>> filePaths;
    std::optional<Sensor::BaudRate> firmwareBaudRate;
    std::optional<Sensor::BaudRate> bootloaderBaudRate;
};

std::optional<ParsedArgs> parseArgs(int argc, char** argv);

int main(int argc, char** argv)
{
    /*
    This firmware update example walks through the C++ usage of the SDK to connect to and update the firmware on a VectorNav unit.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Create a FirmwareUpdater object
    3. Update the firmware based on the file type
    4. Disconnect from the VectorNav unit
    */

    // Parse command line arguments
    std::optional<ParsedArgs> parsedArgs = parseArgs(argc, argv);
    if (!parsedArgs.has_value())
    {
        std::cerr << "Error: Invalid arguments." << std::endl;
        return 1;
    }

    // Define the port connection parameters to be used later
    const std::string portName =
        parsedArgs->portName.has_value() ? parsedArgs->portName.value() : "COM1";  // Change the sensor port name to the COM port of your local machine
    const Sensor::BaudRate firmwareBaudRate = parsedArgs->firmwareBaudRate.has_value() ? parsedArgs->firmwareBaudRate.value() : Sensor::BaudRate::Baud115200;
    const Sensor::BaudRate bootloaderBaudRate =
        parsedArgs->bootloaderBaudRate.has_value() ? parsedArgs->bootloaderBaudRate.value() : Sensor::BaudRate::Baud115200;

    // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit.
    // We are not autoconnecting or verifying connectivity because we cannot assume the sensor has a valid firmware
    Sensor sensor;
    Error latestError = sensor.connect(portName, firmwareBaudRate);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " encountered when connecting to " + portName << std::endl;
        return static_cast<int>(latestError);
    }
    std::cout << "Connected to " << portName << " at " << sensor.connectedBaudRate().value() << std::endl;

    // 2. Create a FirmwareUpdater object
    FirmwareProgrammer::FirmwareUpdater firmwareUpdater;

    // 3. Update firmware based on the file type - There are two file types that can be used to update the firmware: VNX or VNXML
    ErrorAll firmwareUpdateError{Error::None};

    if (parsedArgs->filePaths.has_value())
    {
        if (std::holds_alternative<FirmwareProgrammer::FirmwareUpdater::FilePaths>(*parsedArgs->filePaths))
        {
            // Run loading individual VNX files
            firmwareUpdateError = firmwareUpdater.updateFirmware(&sensor, std::get<FirmwareProgrammer::FirmwareUpdater::FilePaths>(*parsedArgs->filePaths),
                                                                 {firmwareBaudRate, bootloaderBaudRate});
        }
        else if (std::holds_alternative<Filesystem::FilePath>(*parsedArgs->filePaths))
        {
            // Run using a VNXML file
            firmwareUpdateError =
                firmwareUpdater.updateFirmware(&sensor, std::get<Filesystem::FilePath>(*parsedArgs->filePaths), {firmwareBaudRate, bootloaderBaudRate});
        }
        else { VN_ABORT(); }
    }
    else
    {
        // No paths were provided via command line, so run loading individual packaged reference model VNX files
        const auto dirPath = std::filesystem::path(__FILE__).parent_path();
        firmwareUpdateError =
            firmwareUpdater.updateFirmware(&sensor,
                                           FirmwareProgrammer::FirmwareUpdater::FilePaths{
                                               {(dirPath / "ReferenceModels_v3.vn100.vnx").string(), FirmwareProgrammer::FirmwareUpdater::Processor::Nav}},
                                           {firmwareBaudRate, bootloaderBaudRate});  // Change this to your desired VNX file
    }

    if (firmwareUpdateError != Error::None)
    {
        std::cerr << firmwareUpdateError << " : firmware update failed." << std::endl;
        return 1;
    }

    // 4. Disconnect from the VectorNav unit
    sensor.disconnect();
    std::cout << "FirmwareUpdate example complete" << std::endl;
}

bool startsWith(const char* string, const char* pattern)
{
    size_t stringLength = std::strlen(string);
    size_t patternLength = std::strlen(pattern);

    if (patternLength > stringLength) { return false; }

    return std::strncmp(string, pattern, patternLength) == 0;
}

std::string extractAfter(const char* string, const char delimiter)
{
    if (string == nullptr) { return ""; }

    const char* delimiterPos = std::strchr(string, delimiter);

    if (delimiterPos == nullptr) { return ""; }

    size_t length = std::strlen(delimiterPos + 1);
    return std::string(delimiterPos + 1, length);
}

std::optional<ParsedArgs> parseArgs(int argc, char** argv)
{
    ParsedArgs retVal;
    uint8_t invalidArg = 0;
    for (uint8_t i = 1; i < argc; ++i)
    {
        std::string rhs = extractAfter(argv[i], '=');
        if (rhs.empty())
        {
            invalidArg = i;
            break;
        }
        if (startsWith(argv[i], "--PortName="))
        {
            if (retVal.portName.has_value())
            {
                invalidArg = i;
                break;
            }
            retVal.portName = rhs;
        }
        else if (startsWith(argv[i], "--Nav="))
        {
            if (!retVal.filePaths.has_value()) { retVal.filePaths = FirmwareProgrammer::FirmwareUpdater::FilePaths{}; }
            if (std::holds_alternative<FirmwareProgrammer::FirmwareUpdater::FilePaths>(retVal.filePaths.value()))
            {
                std::get<FirmwareProgrammer::FirmwareUpdater::FilePaths>(*retVal.filePaths)
                    .push_back({rhs, FirmwareProgrammer::FirmwareUpdater::Processor::Nav});
            }
            else if (std::holds_alternative<Filesystem::FilePath>(retVal.filePaths.value()))
            {
                invalidArg = i;
                break;
            }
            else { VN_ABORT(); }
        }
        else if (startsWith(argv[i], "--Gnss="))
        {
            if (!retVal.filePaths.has_value()) { retVal.filePaths = FirmwareProgrammer::FirmwareUpdater::FilePaths{}; }
            if (std::holds_alternative<FirmwareProgrammer::FirmwareUpdater::FilePaths>(retVal.filePaths.value()))
            {
                std::get<FirmwareProgrammer::FirmwareUpdater::FilePaths>(*retVal.filePaths)
                    .push_back({rhs, FirmwareProgrammer::FirmwareUpdater::Processor::Gnss});
            }
            else if (std::holds_alternative<Filesystem::FilePath>(retVal.filePaths.value()))
            {
                invalidArg = i;
                break;
            }
            else { VN_ABORT(); }
        }
        else if (startsWith(argv[i], "--Imu="))
        {
            if (!retVal.filePaths.has_value()) { retVal.filePaths = FirmwareProgrammer::FirmwareUpdater::FilePaths{}; }
            if (std::holds_alternative<FirmwareProgrammer::FirmwareUpdater::FilePaths>(retVal.filePaths.value()))
            {
                std::get<FirmwareProgrammer::FirmwareUpdater::FilePaths>(*retVal.filePaths)
                    .push_back({rhs, FirmwareProgrammer::FirmwareUpdater::Processor::Imu});
            }
            else if (std::holds_alternative<Filesystem::FilePath>(retVal.filePaths.value()))
            {
                invalidArg = i;
                break;
            }
            else { VN_ABORT(); }
        }
        else if (startsWith(argv[i], "--vnXml="))
        {
            if (retVal.filePaths.has_value())
            {
                invalidArg = i;
                break;
            }
            retVal.filePaths = rhs;
        }
        else if (startsWith(argv[i], "--firmwareBaudRate="))
        {
            if (retVal.firmwareBaudRate.has_value())
            {
                invalidArg = i;
                break;
            }
            retVal.firmwareBaudRate = static_cast<Sensor::BaudRate>(std::stoi(rhs));
        }
        else if (startsWith(argv[i], "--bootloaderBaudRate="))
        {
            if (retVal.bootloaderBaudRate.has_value())
            {
                invalidArg = i;
                break;
            }
            retVal.bootloaderBaudRate = static_cast<Sensor::BaudRate>(std::stoi(rhs));
        }
        else
        {
            invalidArg = i;
            break;
        }
    }
    if (invalidArg != 0)
    {
        std::cout << "Invalid argument: " << argv[invalidArg] << std::endl;
        std::cerr << "Usage: " << argv[0] << " " << usage << std::endl;
        return std::nullopt;
    }
    return std::make_optional(retVal);
}
