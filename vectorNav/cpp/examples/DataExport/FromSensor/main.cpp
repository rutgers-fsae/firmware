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
#include <thread>

#include "vectornav/Config.hpp"
#include "vectornav/ExporterCsv.hpp"
#include "vectornav/Implementation/AsciiPacketDispatcher.hpp"
#include "vectornav/Implementation/FaPacketDispatcher.hpp"
#include "vectornav/Implementation/FaPacketProtocol.hpp"
#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#include "vectornav/Interface/Sensor.hpp"

using namespace VN;
namespace fs = std::filesystem;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    /*
    This data export example walks through the C++ usage of the SDK to export data from a VectorNav unit to a CSV file.

    This example will achieve the following:
    1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    2. Create an ExportCsv object
    3. Add a subscriber to all binary and ASCII packets
    4. Log data from the VectorNav unit
    5. Disconnect from the VectorNav unit
    */

    // Parse command line arguments
    const std::string portName = (argc > 1) ? argv[1] : "COM1";
    const std::string outputDirectory = (argc > 2) ? argv[2] : (fs::path(__FILE__).parent_path()).string();

    // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
    Sensor sensor;
    Error latestError = sensor.autoConnect(portName);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " : encountered when connecting to " << sensor.connectedPortName().value() << std::endl;
        return static_cast<int>(latestError);
    }
    std::cout << "Connected to " << portName << " at " << sensor.connectedBaudRate().value() << std::endl;

    // 2. Create an ExportCsv object
    DataExport::ExporterCsv csvExporter(outputDirectory);

    // 3. Add a subscriber to all binary and ASCII packets
    latestError = sensor.subscribeToMessage(csvExporter.getQueuePtr(), Sensor::BinaryOutputMeasurements{}, Sensor::FaSubscriberFilterType::AnyMatch);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " : encountered when subscribing." << std::endl;
        return static_cast<int>(latestError);
    }
    latestError = sensor.subscribeToMessage(csvExporter.getQueuePtr(), "VN", Sensor::AsciiSubscriberFilterType::StartsWith);
    if (latestError != Error::None)
    {
        std::cerr << latestError << " : encountered when subscribing." << std::endl;
        return static_cast<int>(latestError);
    }

    // 4. Log data from the VectorNav unit
    csvExporter.start();
    std::cout << "Logging to " << outputDirectory << std::endl;

    std::this_thread::sleep_for(5s);
    csvExporter.stop();

    // 5. Disconnect from the VectorNav unit
    sensor.disconnect();
    std::cout << "DataExportFromSensor example complete." << std::endl;
}
