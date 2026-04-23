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


namespace NonBlockingCommands
{
    class Program
    {
        static int Main(string[] args)
        {
            /*
            This example demonstrates how to send commands without blocking, validating each command was received correctly
            with different types of commands.

            This example will achieve the following:
            1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            2. Configure the asynchronous ASCII output to YPR at 100 Hz
            3. Send generic command (Known Magnetic Disturbance command)
            4. Wait and check response from the unit
            5. Output VNYPR at 100 Hz and send velocity aiding commands at 10 Hz for 5 seconds:
                5.1. Check if a valid response has been received from the velocity aiding command
                5.2. Print response and send new command
            6. Disconnect from the VectorNav unit
            */

            // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            String portName = "COM1";  // Change the sensor port name to the COM port of your local machine
            if (args.Length > 0)
            {
                portName = args[0];
            }
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

            // 2. Configure the asynchronous ASCII output to YPR at 100Hz
            VNSDK.Registers.System.AsyncOutputType asyncDataOutputType = new VNSDK.Registers.System.AsyncOutputType();
            asyncDataOutputType.ador = VNSDK.Registers.System.AsyncOutputType.Ador.YPR;  // Configure for Yaw, Pitch, Roll data.
            asyncDataOutputType.serialPort = VNSDK.Registers.System.AsyncOutputType.SerialPort.Serial1;
            try { sensor.WriteRegister(asyncDataOutputType); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when writing register 6 (AsyncOutputType).");
                return 1;
            }
            Console.WriteLine("ADOR configured.");

            VNSDK.Registers.System.AsyncOutputFreq asyncDataOutputFrequency = new VNSDK.Registers.System.AsyncOutputFreq();
            asyncDataOutputFrequency.adof = VNSDK.Registers.System.AsyncOutputFreq.Adof.Rate100Hz;  // Set output rate to 100Hz
            asyncDataOutputFrequency.serialPort = VNSDK.Registers.System.AsyncOutputFreq.SerialPort.Serial1;
            try { sensor.WriteRegister(asyncDataOutputFrequency); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when writing register 7 (AsyncOutputFreq).");
                return 1;
            }
            Console.WriteLine("ADOF configured\n");

            // 3. Send generic command (Known Magnetic Disturbance command)
            GenericCommand kmd = new KnownMagneticDisturbance(KnownMagneticDisturbance.State.Present);
            try { sensor.SendCommand(ref kmd, Sensor.SendCommandBlockMode.None); } // Non-blocking
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} received while sending the Known Magnetic Disturbance Command..");
                return 1;
            }

            // 4. Wait and check response from the unit
            System.Threading.Thread.Sleep(250);

            // We could check kmd.awaitingResponse() but it will be removed from the command queue (setting kmd.awaitingResponse to false) after commandRemovalTimeoutLength
            // (default 200ms), if any command has been sent or received since.
            if (kmd.HasValidResponse())
            {
                Console.WriteLine($"KMD Response: {kmd.GetResponse()}");
                int? error_maybe = kmd.GetError();
                if (error_maybe.HasValue)
                {
                    Console.WriteLine($"\tError: {error_maybe.Value}");
                    return error_maybe.Value;
                }
            }
            else { Console.WriteLine("Error: KMD did not receive a valid response"); }

            // 5. Output VNYPR at 100 Hz and send velocity aiding commands at 10 Hz for 5 seconds

            // Initialize velocity aiding register, command, counters, flags, and timers
            VNSDK.Registers.VelocityAiding.VelAidingMeas velAidRegister = new VNSDK.Registers.VelocityAiding.VelAidingMeas();
            velAidRegister.velocityX = 0;
            velAidRegister.velocityY = 0;
            velAidRegister.velocityZ = 0;
            VNSDK.GenericCommand velAidWRGCommand = velAidRegister.ToWriteCommand();
            if (velAidWRGCommand == null)
            {
                Console.WriteLine($"Error: Failed to create velocity aiding command.");
                return 1;
            }

            int asciiCount = 0;
            int velAidSentCount = 0;
            bool validResponseReceived = false;

            Random random = new Random();
            TimeSpan timeout = TimeSpan.FromSeconds(5);
            TimeSpan resendTimeout = TimeSpan.FromMilliseconds(500);
            System.Diagnostics.Stopwatch timer = System.Diagnostics.Stopwatch.StartNew();
            System.Diagnostics.Stopwatch resendTimer = System.Diagnostics.Stopwatch.StartNew();

            while (timer.Elapsed < timeout)
            {
                Nullable<CompositeData> compositeData = sensor.GetNextMeasurement();
                if (compositeData.HasValue && compositeData.Value.MatchesMessage("VNYPR"))
                {
                    Nullable<Ypr> ypr = compositeData.Value.attitude.ypr.Value;
                    Console.WriteLine($"YPR: {ypr.Value.yaw}, {ypr.Value.pitch}, {ypr.Value.roll}");
                    asciiCount++;
                }

                // 5.1. Check if a valid response has been received from the velocity aiding command
                if (!validResponseReceived && !velAidWRGCommand.IsAwaitingResponse())
                {
                    int? error_maybe = velAidWRGCommand.GetError();
                    if (velAidWRGCommand.HasValidResponse())
                    {
                        Console.WriteLine($"\nWRG Response {velAidWRGCommand.GetResponse()}");
                        validResponseReceived = true;
                    }
                    else if (error_maybe.HasValue)
                    {
                        Console.WriteLine($"Error: {error_maybe.Value}");
                        return error_maybe.Value;
                    }
                }

                // 5.2. Print response and send new command
                if (resendTimer.Elapsed > resendTimeout)
                {
                    if (!validResponseReceived && velAidSentCount > 0) { Console.WriteLine($"\nError: Response Timeout\n"); }

                    velAidRegister = new VNSDK.Registers.VelocityAiding.VelAidingMeas();
                    velAidRegister.velocityX = (float)(random.NextDouble()); // random to simulate different velocities
                    velAidRegister.velocityY = (float)(random.NextDouble());
                    velAidRegister.velocityZ = (float)(random.NextDouble());
                    velAidWRGCommand = velAidRegister.ToWriteCommand(); // The GenericCommand object was instantiated before the loop.
                    if (velAidWRGCommand == null)
                    {
                        Console.WriteLine($"Error: Expected a valid instance of GenericCommand^, but received null");
                        return 1;
                    }
                    try { sensor.SendCommand(ref velAidWRGCommand, Sensor.SendCommandBlockMode.None); } // Non-blocking
                    catch (Exception latestError)
                    {
                        Console.WriteLine($"Error: {latestError} encountered when writing to register");
                    }

                    validResponseReceived = false;
                    velAidSentCount++;
                    resendTimer.Restart(); // Restart send timer
                }

                // Handle asynchronous errors
                try { sensor.ThrowIfAsyncError(); }
                catch (Exception asyncError)
                {
                    Console.WriteLine($"Received async error: {asyncError.Message}");
                }
            }

            Console.WriteLine($"\nTotal ASCII YPR Packets Received: {asciiCount}");
            Console.WriteLine($"Total VelAid Commands Sent: {velAidSentCount}");
            Console.WriteLine($"\nNonBlockingCommands example complete");

            // 6. Disconnect from the VectorNav unit
            sensor.Disconnect();
            return 0;
        }
    }
}

