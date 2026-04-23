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

#include "vectornav/ExporterAscii.hpp"
#include "vectornav/ExporterCsv.hpp"
#include "vectornav/ExporterSkippedByte.hpp"
#include "vectornav/Interface/Sensor.hpp"

using namespace VN;
namespace fs = std::filesystem;
using PacketQueueMode = DataExport::Exporter::PacketQueueMode;

std::string usage = "<path to data file>\n";

int main(int argc, char* argv[])
{
    /*
    This data export example walks through the C++ usage of the SDK to export a binary file to a CSV or ASCII file.

    This example will achieve the following:
    1. Create a FileExporter object
    2. Add exporters for CSV, ASCII, and skipped bytes
    3. Process file
    4. Print parsing stats
    */

    // Parse command line arguments
    if (argc > 2)
    {
        std::cout << "Usage: ./FromFile " << usage << std::endl;
        return 1;
    }

    std::string filePath;
    if (argc > 1) { filePath = fs::path(argv[1]).string(); }
    else { filePath = (fs::path(__FILE__).parent_path() / "DataExportFromFile.bin").string(); }

    std::string outputDirectory = fs::path(filePath).parent_path().string();
    std::cout << "Exporting " << filePath << std::endl;
    std::cout << "Outputting to " << outputDirectory << '\n' << std::endl;

    // 1. Create a Sensor object for file processing
    Sensor::MeasQueueMode pushToCompositeData = Sensor::MeasQueueMode::Off;  // do not parse to CompositeData because that is not needed by exporters
    std::array<uint8_t, 65536> byteBufferArray{};  // using statically allocated main byte buffer to be able to read more of the file at once (could
                                                   // alternatively change 'mainBufferCapacity' in Config.hpp)
    std::array<uint8_t, 2048> fbBufferArray{};

    Sensor sensor(byteBufferArray, fbBufferArray, pushToCompositeData);

    // 2. Add exporters for CSV, ASCII, and skipped bytes and subscribe to sensor messages
    DataExport::ExporterAscii asciiExporter(outputDirectory, PacketQueueMode::Retry);
    sensor.subscribeToMessage(asciiExporter.getQueuePtr(), "VN");

    DataExport::ExporterCsv csvExporter(outputDirectory, PacketQueueMode::Retry);
    sensor.subscribeToMessage(csvExporter.getQueuePtr(), "VN");
    sensor.subscribeToMessage(csvExporter.getQueuePtr(), Sensor::BinaryOutputMeasurements{}, FaPacketDispatcher::SubscriberFilterType::AnyMatch);

    DataExport::ExporterSkippedByte skippedExporter(outputDirectory, PacketQueueMode::Retry);
    sensor.subscribeToMessage(skippedExporter.getQueuePtr(), Sensor::SyncByte::None);

    // 3. Kick off exporter threads
    asciiExporter.start();
    csvExporter.start();
    skippedExporter.start();

    // 4. Connect to file, monitoring AsyncError queue for FileReadFailed error to indicate end of file reached

    sensor.connect(filePath);
    while (true)
    {
        thisThread::sleepFor(1ms);
        if (auto asyncError = sensor.getNextAsyncError())
        {
            if (asyncError.value().error == Error::FileReadFailed) { break; }
            std::cout << "Received async error: " << asyncError->error << std::endl;
        }
    }
    sensor.disconnect();

    // 5. Stop exporters
    asciiExporter.stop();
    csvExporter.stop();
    skippedExporter.stop();

    std::cout << "DataExportFromFile example complete." << std::endl;
}
