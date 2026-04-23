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
using VNSDK;

namespace SimpleLogger
{
    class Program
    {
        static int Main(string[] args)
        {
            /*
            This simple logger example walks through the C# usage of the SDK to log data from a VectorNav unit.

            This example will achieve the following:
            1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            2. Set up resources needed for data logging
            3. Register the logger's input buffer to receive all bytes from the unit
            4. Log data from the VectorNav unit
                4.1. Check for any asynchronous errors
            5. Disconnect from the VectorNav unit
            */

            // Pass in port name and path as positional arguments, or edit them here
            String portName = (args.Length > 0) ? args[0] : "COM1";
            String filePath = (args.Length > 1) ? args[1] : System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "log.bin");

            // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            Sensor sensor = new Sensor();
            try { sensor.AutoConnect(portName); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when connecting to {portName}.");
                return 1;
            }
            Console.WriteLine($"\nConnected to {portName} at {sensor.ConnectedBaudRate()}");

            VNSDK.Registers.System.Model modelRegister = new VNSDK.Registers.System.Model();
            try { sensor.ReadRegister(modelRegister); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when reading register 1 (Model).");
                return 1;
            }
            Console.WriteLine($"Sensor Model Number: {modelRegister.model}");

            // 2. Set up resources needed for data logging
            ByteBuffer bufferToLog = new ByteBuffer(1024 * 3);
            Logger.SimpleLogger logger = new Logger.SimpleLogger(bufferToLog, filePath);

            // 3. Register the logger's input buffer to receive all bytes from the unit
            sensor.RegisterReceivedByteBuffer(bufferToLog);

            // 4. Log data from the VectorNav unit
            if (logger.Start())
            {
                Console.WriteLine("Error: Failed to write to file.");
                return 1;
            }

            Console.WriteLine($"Logging to {filePath}");

            DateTime endTime = DateTime.Now.AddSeconds(5);
            while (DateTime.Now < endTime)
            {
                System.Threading.Thread.Sleep(1);

                // 4.1. Check for any asynchronous errors - Any buffer overruns means data has been dropped.
                try { sensor.ThrowIfAsyncError(); }
                catch (Exception asyncError)
                {
                    Console.WriteLine($"Received async error: {asyncError.Message}");
                }
            }

            logger.Stop();
            sensor.DeregisterReceivedByteBuffer();

            Console.WriteLine($"Logged {logger.NumBytesLogged()} bytes.");

            // 5. Disconnect from the VectorNav unit
            sensor.Disconnect();
            Console.WriteLine($"SimpleLogger example complete.");

            return 0;
        }
    }
}
