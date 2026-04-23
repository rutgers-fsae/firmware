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
using VNSDK.Registers;


namespace GettingStarted
{
    class Program
    {
        static int Main(string[] args)
        {
            /*
            This getting started example walks through the C# usage of the SDK to connect to and interact with a VectorNav unit.

            This example will achieve the following:
            1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            2. Poll and print the model number using a read register command
            3. Poll and print the current yaw, pitch, and roll using a read register command
            4. Configure the asynchronous ASCII output to YPR at 2 Hz
            5. Configure the first binary output message to output timeStartup, accel, and angRate, all from common group, at a 2 Hz output rate (1 Hz if VN-300) through both serial ports
            6. Enter a loop for 5 seconds where it:
               Determines which measurement it received (VNYPR or the necessary binary header)
               Prints out the relevant measurement from the CompositeData struct
            7. Disconnect from the VectorNav unit
            */

            // Define the port connection parameters to be used later
            String portName = "COM1"; // Change the sensor port name to the comm port of your local machine
            if (args.Length > 0)
            {
                portName = args[0];
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

            // 2. Poll and print the model number using a read register command
            // Create an empty register object of the necessary type, where the data member will be populated when the sensor responds to our "read register" request
            VNSDK.Registers.System.Model modelRegister = new VNSDK.Registers.System.Model();
            try { sensor.ReadRegister(modelRegister); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when reading register 1 (Model).");
                return 1;
            }
            Console.WriteLine($"Sensor Model Number: {modelRegister.model}");

            // 3. Poll and print the current yaw, pitch, and roll using a read register command
            VNSDK.Registers.Attitude.YawPitchRoll yprRegister = new VNSDK.Registers.Attitude.YawPitchRoll();
            try { sensor.ReadRegister(yprRegister); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when reading register 8 (YawPitchRoll).");
                return 1;
            }
            Console.WriteLine($"Current Reading:" + $"\n\tYaw: {yprRegister.yaw} , " + $"Pitch: {yprRegister.pitch} , " + $"Roll: {yprRegister.roll}");

            // 4. Configure the asynchronous ASCII output to YPR at 2 Hz
            VNSDK.Registers.System.AsyncOutputType asyncDataOutputType = new VNSDK.Registers.System.AsyncOutputType();
            asyncDataOutputType.ador = VNSDK.Registers.System.AsyncOutputType.Ador.YPR;
            asyncDataOutputType.serialPort = VNSDK.Registers.System.AsyncOutputType.SerialPort.Serial1;
            try { sensor.WriteRegister(asyncDataOutputType); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when writing register 6 (AsyncOutputType).");
                return 1;
            }
            Console.WriteLine("ADOR configured.");

            VNSDK.Registers.System.AsyncOutputFreq asyncDataOutputFrequency = new VNSDK.Registers.System.AsyncOutputFreq();
            asyncDataOutputFrequency.adof = VNSDK.Registers.System.AsyncOutputFreq.Adof.Rate2Hz;
            asyncDataOutputFrequency.serialPort = VNSDK.Registers.System.AsyncOutputFreq.SerialPort.Serial1;
            try { sensor.WriteRegister(asyncDataOutputFrequency); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when writing register 7 (AsyncOutputFreq).");
                return 1;
            }
            Console.WriteLine("ADOR configured.");
            Console.WriteLine("ADOF configured.");

            // 5. Configure the first binary output message to output timeStartup, accel, and angRate, all from common group, at a 2 Hz output rate (1 Hz if VN-300) through both serial ports
            VNSDK.Registers.System.BinaryOutput1 binaryOutput1Register = new VNSDK.Registers.System.BinaryOutput1();
            binaryOutput1Register.asyncMode.serial1 = true;
            binaryOutput1Register.asyncMode.serial2 = true;
            binaryOutput1Register.rateDivisor = 400;
            binaryOutput1Register.common.timeStartup = true;
            binaryOutput1Register.common.accel = true;
            binaryOutput1Register.common.angularRate = true;
            binaryOutput1Register.common.imu = true;
            try { sensor.WriteRegister(binaryOutput1Register); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when writing register 75 (BinaryOutput1).");
                return 1;
            }
            Console.WriteLine("Binary output 1 message configured.");

            // 6. Enter a loop for 5 seconds where it:
            //     Determines which measurement it received (VNYPR or the necessary binary header)
            //     Prints out the relevant measurement from the CompositeData struct
            System.Diagnostics.Stopwatch t0 = new System.Diagnostics.Stopwatch();
            t0.Start();
            while (t0.Elapsed < TimeSpan.FromSeconds(5))
            {
                Nullable<CompositeData> compositeData = sensor.GetNextMeasurement();
                // Check to make sure that a measurement is available
                if (!compositeData.HasValue) continue;

                if (compositeData.Value.MatchesMessage(binaryOutput1Register))
                {
                    Console.WriteLine($"Found binary 1 measurment.");

                    Console.WriteLine($"\tTime: {compositeData.Value.time.timeStartup.Value.nanoseconds()}");
                    Vec3f accel = compositeData.Value.imu.accel.Value;
                    Console.WriteLine($"\tAccel: X {accel.x}\n\tAccel Y: {accel.y}\n\tAccel Z: {accel.z}");
                }
                else if (compositeData.Value.MatchesMessage($"VNYPR"))
                {
                    Console.WriteLine($"Found Ascii ypr measurement.");

                    Ypr ypr = compositeData.Value.attitude.ypr.Value;
                    Console.WriteLine($"\tYaw: {ypr.yaw}\n\tPitch: {ypr.pitch}\n\tPitch: {ypr.roll}");
                }

                // Handle asynchronous errors
                try { sensor.ThrowIfAsyncError(); }
                catch (Exception asyncError)
                {
                    Console.WriteLine($"Received async error: {asyncError.Message}");
                }
            }
            t0.Stop();

            // 7. Disconnect from the VectorNav unit
            sensor.Disconnect();
            Console.WriteLine($"Sensor disconnected.");
            Console.WriteLine($"Getting Started example complete.");

            return 0;
        }
    }
}

