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
using System.Collections.Generic;
using VNSDK;

namespace FirmwareUpdate
{
    class Program
    {
        static int Main(string[] args)
        {
            /*
            This firmware update example walks through the C# usage of the SDK to connect to and update the firmware on a VectorNav unit.

            This example will achieve the following:
            1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            2. Create a FirmwareUpdater object
            3. Update the firmware based on the file type
            4. Disconnect from the VectorNav unit
            */

            Parameters parameters = new Parameters(args);
            if (!parameters.valid)
            {
                Console.WriteLine(("Usage: " + parameters.usage));
                return 1;
            }

            // 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
            // We are not autoconnecting or verifying connectivity because we cannot assume the sensor has a valid firmware
            Sensor sensor = new Sensor();
            try { sensor.Connect(parameters.portName, parameters.firmwareBaudRate); }
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when connecting to {parameters.portName}.");
                return 1;
            }

            // 2. Create FirmwareUpdater object
            FirmwareProgrammer.FirmwareUpdater firmwareUpdater = new FirmwareProgrammer.FirmwareUpdater();

            // 3. Update firmware based on the file type - There are two file types that can be used to update the firmware: VNX or VNXML
            FirmwareProgrammer.FirmwareUpdater.Params fwUpdateParams = new FirmwareProgrammer.FirmwareUpdater.Params(parameters.firmwareBaudRate, parameters.bootloaderBaudRate);
            Console.WriteLine($"Begin updating.");

            if (!string.IsNullOrEmpty(parameters.vnxmlPath))
            {
                // Run using a VNXML file
                Console.WriteLine($"Update firmware with vnxml: {parameters.vnxmlPath}");
                try
                {
                    firmwareUpdater.UpdateFirmware(ref sensor, parameters.vnxmlPath, fwUpdateParams);
                }
                catch (Exception latestError)
                {
                    Console.WriteLine($"{latestError.Message}: firmware update failed.");
                    return 1;
                }

            }
            else
            {
                // Run loading individual VNX files
                try
                {
                    firmwareUpdater.UpdateFirmware(ref sensor, parameters.files, fwUpdateParams);
                }
                catch (Exception latestError)
                {
                    Console.WriteLine($"{latestError.Message}: firmware update failed.");
                    return 1;
                }

            }

            // Handle asynchronous errors that occurred during firmware upgrade
            while (true)
            {
                try
                {
                    sensor.ThrowIfAsyncError();
                    break;
                }
                catch (Exception asyncError)
                {
                    Console.WriteLine($"Received async error: {asyncError.Message}");
                }
            }

            // 4. Disconnect from the VectorNav unit
            sensor.Disconnect();
            return 0;
        }
    }

    public class Parameters
    {
        public string usage { get; }
        public bool valid { get; }
        public string portName { get; }
        public string vnxmlPath { get; }
        public string navPath { get; }
        public string gnssPath { get; }
        public string imuPath { get; }
        public uint firmwareBaudRate { get; }
        public uint bootloaderBaudRate { get; }

        public List<FirmwareProgrammer.FirmwareUpdater.FirmwareFile> files { get; } = new List<FirmwareProgrammer.FirmwareUpdater.FirmwareFile>();

        public Parameters(string[] args)
        {
            usage = "[--PortName={port_name}] [--{Processor}={file_path}... | --vnXml={file_path}] --firmwareBaudRate={firmware_baudrate} --bootloaderBaudRate={bootloader_baudrate}";
            portName = string.Empty;
            vnxmlPath = string.Empty;
            navPath = string.Empty;
            gnssPath = string.Empty;
            imuPath = string.Empty;
            firmwareBaudRate = 115200;
            bootloaderBaudRate = 115200;
            valid = true;

            // Define the port connection parameters to be used later
            if (args.Length == 0)
            {
                // No paths were provided via command line, so run loading individual packaged reference model VNX files
                portName = "COM1"; // Change the sensor port name to the COM port of your local machine              
                navPath = "ReferenceModels_v3.vn100.vnx"; // Change this to your desired VNX file
            }

            int index = 0;
            uint tempBaudRate;

            foreach (string parameter in args)
            {
                index = parameter.IndexOf('=');
                if (index == -1)
                {
                    valid = false;
                    return;
                }
                string value = parameter.Substring(index + 1);

                // [--PortName={port_name}]
                if (parameter.StartsWith("--PortName"))
                {
                    portName = value;
                }

                // [--{Processor}={file_path}...]
                if (parameter.StartsWith("--Nav"))
                {
                    Console.WriteLine($"Update NAV Firmware : {value}");
                    FirmwareProgrammer.FirmwareUpdater.FirmwareFile file = new FirmwareProgrammer.FirmwareUpdater.FirmwareFile(FirmwareProgrammer.FirmwareUpdater.Processor.Nav, value);
                    files.Add(file);
                }
                if (parameter.StartsWith("--Gnss"))
                {
                    Console.WriteLine($"Update GNSS Firmware: {value}");
                    FirmwareProgrammer.FirmwareUpdater.FirmwareFile file = new FirmwareProgrammer.FirmwareUpdater.FirmwareFile(FirmwareProgrammer.FirmwareUpdater.Processor.Gnss, value);
                    files.Add(file);
                }
                if (parameter.StartsWith("--Imu"))
                {
                    Console.WriteLine($"Update IMU Firmware : {value}");
                    FirmwareProgrammer.FirmwareUpdater.FirmwareFile file = new FirmwareProgrammer.FirmwareUpdater.FirmwareFile(FirmwareProgrammer.FirmwareUpdater.Processor.Imu, value);
                    files.Add(file);
                }

                // [--vnXml={file_path}]
                if (parameter.StartsWith("--vnXml"))
                {
                    vnxmlPath = value;
                }

                // [--firmwareBaudRate={firmware_baudrate}]
                if (parameter.StartsWith("--firmwareBaudRate"))
                {
                    if (uint.TryParse(value, out tempBaudRate))
                    {
                        firmwareBaudRate = tempBaudRate;
                    }
                }

                // [--bootloaderBaudRate={bootloader_baudrate}]
                if (parameter.StartsWith("--bootloaderBaudRate"))
                {
                    if (uint.TryParse(value, out tempBaudRate))
                    {
                        bootloaderBaudRate = tempBaudRate;
                    }
                }

            }
        }
    }
}

