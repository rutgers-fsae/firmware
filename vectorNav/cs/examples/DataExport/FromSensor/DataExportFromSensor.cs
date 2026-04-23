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
using System.Reflection;
using VNSDK;
using DataExport = VNSDK.DataExport;

namespace DataExportFromSensor
{
    class Program
    {
        static int Main(string[] args)
        {
            /* This data export example walks through the C# usage of the SDK to export data from a VectorNav unit to a CSV file.

            This example will achieve the following:
            1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            2. Create an ExportCsv object
            3. Add a subscriber to all binary and ASCII packets
            4. Log data from the VectorNav unit
            5. Disconnect from the VectorNav unit
            */

            // Parse command line arguments
            String portName = "COM1";
            String outputDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            if (args.Length > 0)
            {
                portName = args[0];
            }
            if (args.Length > 1)
            {
                outputDirectory = args[1];
            }


            // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            Sensor sensor = new Sensor();
            try { sensor.AutoConnect(portName); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when connecting to {portName}.");
                return 1;
            }
            Console.WriteLine($"Connected to {portName} at {sensor.ConnectedBaudRate()}");

            // 2. Create an ExportCsv object
            DataExport.ExporterCsv csvExporter = new DataExport.ExporterCsv(outputDirectory);

            // 3. Add a subscriber to all binary and ASCII packets
            try { sensor.RegisterDataExporter(csvExporter); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered  when subscribing.");
                return 1;
            }

            // 4. Log data from the VectorNav unit
            if (csvExporter.Start())
            {
                Console.WriteLine("Error: Failed to start logging to CSV file.");
                return 1;
            }
            Console.WriteLine($"Logging to {outputDirectory}");

            DateTime endTime = DateTime.Now.AddSeconds(5);
            while (DateTime.Now < endTime)
            {
                System.Threading.Thread.Sleep(1);

                // Handle asynchronous errors
                try { sensor.ThrowIfAsyncError(); }
                catch (Exception asyncError)
                {
                    Console.WriteLine($"Received async error: {asyncError.Message}");
                }
            }

            csvExporter.Stop();

            // 5. Disconnect from the VectorNav unit
            sensor.Disconnect();
            Console.WriteLine($"DataExportFromSensor example complete.");
            return 0;
        }
    }
}

