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

#include <filesystem>
#include <string>

#include "vectornav/HAL/File.hpp"
#include "vectornav/Interface/Registers.hpp"
#include "vectornav/Interface/Sensor.hpp"
#include "vectornav/RegisterScan.hpp"

using namespace VN;

std::string usage = "[port] [fileLocation or \"generic\")";

enum class ReadType
{
    Xml,
    Ascii,
    Generic
};

int main(int argc, char* argv[])
{
    /*
    This load configuration example walks through the C++ usage of the SDK to load a configuration from an XML or ASCII file onto a VectorNav unit.

    This example will achieve the following:
    1. Handle input arguments
    2. Instantiate a Sensor object and use it to connect to the VectorNav unit
    3. Load the configuration onto the VectorNav unit
    4. Disconnect from the VectorNav unit
    */

    // 1. Handle input arguments
    if (argc > 3) { std::cerr << "Usage: LoadConfiguration " << usage << std::endl; };

    const std::string portName = (argc > 1) ? argv[1] : "COM1";  // Pass in port name as a positional argument or edit here
    std::filesystem::path filePath = (argc > 2) ? argv[2] : (std::filesystem::path(__FILE__).parent_path() / "configSettings.xml");
    // std::filesystem::path filePath = "generic";

    ReadType readType{ReadType::Generic};
    if (filePath.string() == std::string("generic")) { readType = ReadType::Generic; }
    else
    {
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "Error: File does not exist: " << filePath.string() << std::endl;
            return 1;
        }
        std::filesystem::path extension = filePath.extension();
        if (extension.empty())
        {
            std::cerr << "Error: File path extension was not specified." << std::endl;
            return 1;
        }
        else if (extension == ".xml") { readType = ReadType::Xml; }
        else { readType = ReadType::Ascii; }
    }

    // 2. Instantiate a Sensor object and use it to connect to the VectorNav unit
    Sensor sensor;
    Error latestError = sensor.autoConnect(portName);
    if (latestError != Error::None)
    {
        std::cerr << "Error " << latestError << " encountered when connecting to " + portName << std::endl;
        if (argc > 1) { std::cout << "Usage: ./LoadConfiguration " << usage << std::endl; };
        return static_cast<int>(latestError);
    }
    std::cout << "Connected to " << portName << " at " << sensor.connectedBaudRate().value() << std::endl;

    // 3. Load the configuration onto the VectorNav unit - There are a few different file types that can be used to upload
    // a previously saved configuration file onto a VectorNav unit, such as an XML file or text file. When using
    // the RegisterScan::loadConfiguration method, a Restore Factory Settings command will be issued to the unit
    // prior to attempting to load the user-defined configuration settings onto the unit. Following the upload, a
    // Write Settings command and a Reset command will also be issued to the unit. If performing a Restore Factory
    // Settings command is not desired, the RegisterScan::setConfigurationRegisters() method should instead be used
    // to load the user-defined settings onto the unit.
    Error err;
    if (readType == ReadType::Xml)
    {
        RegisterScan::XmlConfigReader configReader(filePath.string());
        err = RegisterScan::loadConfiguration(sensor, configReader);
    }
    else if (readType == ReadType::Ascii)
    {
        RegisterScan::AsciiConfigReader configReader(filePath.string());
        err = RegisterScan::loadConfiguration(sensor, configReader);
    }
    else  // readType == ReadType::Generic
    {
        uint16_t messageCount{0};
        std::vector<AsciiMessage> messages{"$VNRRG,06,1,1*0865", "$VNRRG,07,1,1*A234", "$VNRRG,75,3,400,01,0321*3B31"};
        RegisterScan::GenericConfigReader configReader(
            [&messages, &messageCount](AsciiMessage& msg)
            {
                if (messageCount < messages.size())
                {
                    msg = messages[messageCount];
                    messageCount++;
                    return Error::None;
                }
                else { return Error::FileReadFailed; }
            });
        err = RegisterScan::loadConfiguration(sensor, configReader);
    }
    if (err != Error::None)
    {
        std::cerr << "Error " << err << " occured when loading configuration." << std::endl;
        return 1;
    }
    std::cout << "Configuration has been loaded to the sensor." << std::endl;

    // 4. Disconnect from the VectorNav unit
    sensor.disconnect();
    std::cout << "LoadConfiguration example complete." << std::endl;
}
