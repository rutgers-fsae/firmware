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

using System;
using System.IO;
using VNSDK;
using DataExport = VNSDK.DataExport;

namespace DataExportFromFile
{
    class Program
    {
        static int Main(string[] args)
        {
            /* This data export example walks through the C# usage of the SDK to export a binary file to a CSV or ASCII file.

            This example will achieve the following:
            1. Create a FileExporter object
            2. Add exporters for CSV, ASCII, and skipped bytes
            3. Process file
            4. Print parsing stats
            */

            // Parse command line arguments
            String outputDirectory = Directory.GetParent(Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location)).Parent.FullName;
            String filePath = Path.Combine(outputDirectory, "DataExportFromFile.bin");

            if (args.Length > 0)
            {
                filePath = args[0];
                outputDirectory = Path.GetDirectoryName(Path.GetFullPath(filePath));
            }

            Console.WriteLine($"Exporting {filePath}");
            Console.WriteLine($"Outputting to {outputDirectory}\n");

            // 1. Create a Sensor object for file processing
            MeasQueueMode pushToCompositeData = MeasQueueMode.Off;  // do not parse to CompositeData because that is not needed by exporters

            ByteBuffer mainBuffer = new ByteBuffer(65536);
            ByteBuffer fbBuffer = new ByteBuffer(2048);

            Sensor sensor = new Sensor(mainBuffer, fbBuffer, pushToCompositeData);

            // 2. Add exporters for CSV, ASCII, and skipped bytes and subscribe to sensor messages
            DataExport.ExporterAscii asciiExporter = new DataExport.ExporterAscii(outputDirectory, DataExport.PacketQueueMode.Retry);
            sensor.SubscribeToMessage(asciiExporter.GetManagedQueuePointer(), "VN");

            DataExport.ExporterCsv csvExporter = new DataExport.ExporterCsv(outputDirectory, DataExport.PacketQueueMode.Retry);
            sensor.SubscribeToMessage(csvExporter.GetManagedQueuePointer(), "VN");

            BinaryOutputMeasurements binMeas = new BinaryOutputMeasurements();
            sensor.SubscribeToMessage(csvExporter.GetManagedQueuePointer(), binMeas, FaSubscriberFilterType.AnyMatch);

            DataExport.ExporterSkippedByte skippedByteExporter = new DataExport.ExporterSkippedByte(outputDirectory);
            sensor.SubscribeToMessage(skippedByteExporter.GetManagedQueuePointer(), SyncByte.None);

            // 3. Kick off exporter threads 
            asciiExporter.Start();
            csvExporter.Start();
            skippedByteExporter.Start();

            // 4. Connect to file, monitoring AsyncError queue for FileReadFailed error to indicate end of file reached
            sensor.Connect(filePath);
            while (true)
            {
                System.Threading.Thread.Sleep(1);

                // Handle asynchronous errors
                try { sensor.ThrowIfAsyncError(); }
                catch (Exception asyncError)
                {
                    if (asyncError.Message == "FileReadFailed")
                    {
                        break;
                    } else {
                        Console.WriteLine($"Received async error: {asyncError.Message}");
                    }
                }
            }

            sensor.Disconnect();

            // 5. Stop Exporters
            asciiExporter.Stop();
            csvExporter.Stop();
            skippedByteExporter.Stop();


            Console.WriteLine("DataExportFromFile example complete.");
            return 0;
        }
    }
}
