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


namespace LoadConfiguration
{
    class Program
    {
        static void Usage()
        {
            Console.WriteLine("Usage: LoadConfiguration [port] [fileLocation or \"generic\"]");
        }

        public enum ReadType
        {
            Xml,
            Ascii,
            Generic
        }

        static int Main(string[] args)
        {
            /*
            This load configuration example walks through the C# usage of the SDK to load a configuration from an XML or ASCII file onto a VectorNav unit.

            This example will achieve the following:
            1. Handle input arguments
            2. Instantiate a Sensor object and use it to connect to the VectorNav unit
            3. Load the configuration onto the VectorNav unit
            4. Disconnect from the VectorNav unit
            */

            // 1. Handle input arguments
            if (args.Length > 2)
            {
                Usage();
                return 1;
            }
            string portName = "COM1";
            if (args.Length > 0) { portName = args[0]; }
            string sourceDir = Directory.GetParent(Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location)).Parent.FullName;
            string filePath = Path.Combine(sourceDir, "configSettings.xml");
            if (args.Length > 1) { filePath = args[1]; }
            ReadType readType;
            if (filePath == "generic") { readType = ReadType.Generic; }
            else
            {
                if (!System.IO.File.Exists(filePath))
                {
                    Console.WriteLine($"\nError: File does not exist: {filePath}");
                    return 1;
                }
                string extension = System.IO.Path.GetExtension(filePath);
                if (string.IsNullOrEmpty(extension))
                {
                    Console.WriteLine($"\nError: File path extension was not specified.");
                    return 1;
                }
                else if (extension == ".xml") { readType = ReadType.Xml; }
                else { readType = ReadType.Ascii; }
            }

            // 2. Instantiate a Sensor object and use it to connect to the VectorNav unit
            Sensor sensor = new Sensor();
            try { sensor.AutoConnect(portName, true); } // enables throwing from async error monitoring thread  
            catch (Exception latestError)
            {
                Console.WriteLine($"Error: {latestError.Message} encountered when connecting to {portName}.");
                return 1;
            }
            Console.WriteLine($"\nConnected to {portName} at {sensor.ConnectedBaudRate()}");

            // 3. Load the configuration onto the VectorNav unit - There are a few different file types that can be used to upload
            // a previously saved configuration file onto a VectorNav unit, such as an XML file or text file. When using
            // the RegisterScan.LoadConfiguration method, a Restore Factory Settings command will be issued to the unit
            // prior to attempting to load the user-defined configuration settings onto the unit. Following the upload, a
            // Write Settings command and a Reset command will also be issued to the unit. If performing a Restore Factory
            // Settings command is not desired, the RegisterScan.SetConfigurationRegisters() method should instead be used
            // to load the user-defined settings onto the unit.
            if (readType == ReadType.Xml)
            {
                RegisterScan.XmlConfigReader configReader = new RegisterScan.XmlConfigReader(filePath);
                try { RegisterScan.LoadConfiguration(sensor, configReader); }
                catch (Exception latestError)
                {
                    Console.Write($"Error: {latestError} occured when loading configuration.");
                    return 1;
                }
            }
            else if (readType == ReadType.Ascii)
            {
                RegisterScan.AsciiConfigReader configReader = new RegisterScan.AsciiConfigReader(filePath);
                try { RegisterScan.LoadConfiguration(sensor, configReader); }
                catch (Exception latestError)
                {
                    Console.Write($"Error: {latestError} occured when loading configuration.");
                    return 1;
                }
            }
            else
            {
                Console.WriteLine("Generic LoadConfiguration example unimplemented.");
                sensor.Disconnect();
                return 1;
            }
            Console.Write("Configuration has been loaded to the sensor.");

            // 4. Disconnect from the VectorNav unit
            sensor.Disconnect();
            Console.WriteLine("RegisterScan load config example complete.");
            return 0;
        }
    }
}

