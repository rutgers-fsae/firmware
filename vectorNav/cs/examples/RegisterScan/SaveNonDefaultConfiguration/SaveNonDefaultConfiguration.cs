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


namespace SaveNonDefaultConfiguration
{
    class Program
    {
        static void Usage()
        {
            Console.WriteLine("Usage: SaveConfiguration [port] [fileLocation or \"generic\"]");
        }

        public enum WriteType
        {
            Xml,
            Ascii,
            Generic
        }

        static int Main(string[] args)
        {
            /*
            This save configuration example walks through the C# usage of the SDK to save the current non-default configuration of a VectorNav unit into an XML file or ASCII file.

            This example will achieve the following:
            1. Handle input arguments
            2. Instantiate a Sensor object and use it to connect to the VectorNav unit
            3. Save all non-default values from the VectorNav unit as specified
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
            WriteType writeType;
            if (filePath == "generic") { writeType = WriteType.Generic; }
            else
            {
                string extension = System.IO.Path.GetExtension(filePath);
                if (string.IsNullOrEmpty(extension))
                {
                    Console.WriteLine($"\nError: File path extension was not specified.");
                    return 1;
                }
                else if (extension == ".xml") { writeType = WriteType.Xml; }
                else { writeType = WriteType.Ascii; }
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

            // 3. Save all non-default values from the VectorNav unit as specified  - There are a few different file types
            // that can be used to save the current non-default configuration settings from a VectorNav unit into
            // a configuration file, such as an XML file or text file. As part of the
            // RegisterScan.SaveNonDefaultConfiguration() method, a Restore Factory Settings command will be issued
            // to the VectorNav unit. When using this method, it is highly recommended to monitor the progress of
            // this call as any errors encountered could mean that the settings previously configured were not able
            // to be reapplied onto the unit. If performing a Restore Factory Settings command is not desired, the
            // RegisterScan.SaveConfiguration() method should instead be used to save the current configuration of
            // a VectorNav unit.
            if (writeType == WriteType.Xml)
            {
                RegisterScan.XmlConfigWriter configWriter = new RegisterScan.XmlConfigWriter(filePath, sensor);
                try { RegisterScan.SaveNonDefaultConfiguration(sensor, configWriter); }
                catch (Exception latestError)
                {
                    Console.Write($"Error: {latestError} occured when saving configuration.");
                    return 1;
                }
            }
            else if (writeType == WriteType.Ascii)
            {
                RegisterScan.AsciiConfigWriter configWriter = new RegisterScan.AsciiConfigWriter(filePath);
                try { RegisterScan.SaveNonDefaultConfiguration(sensor, configWriter); }
                catch (Exception latestError)
                {
                    Console.Write($"Error: {latestError} occured when saving configuration.");
                    return 1;
                }
            }
            else
            {
                Console.WriteLine("Generic SaveNonDefualtConfiguration example unimplemented.");
                sensor.Disconnect();
                return 1;
            }

            // 4. Disconnect from the VectorNav unit
            sensor.Disconnect();
            Console.WriteLine("SaveNonDefaultConfiguration example complete.");
            return 0;
        }
    }
}

