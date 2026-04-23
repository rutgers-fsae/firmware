% VectorNav SDK (v1.0.0)
% Copyright (c) 2024 VectorNav Technologies, LLC
% 
% WARNING â€“ This software contains Controlled Technical Data, export of which
% is restricted by the Export Administration Regulations ("EAR") (ECCN 7D994). 
% Disclosure to foreign persons contrary to the EAR is prohibited. Violations
% of these export laws and regulations are subject to severe civil and criminal
% penalties.
% 
% The MIT License (MIT)
% 
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
% 
% The above copyright notice and this permission notice shall be included in
% all copies or substantial portions of the Software.
% 
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
% THE SOFTWARE.

vnsdkAssembly = NET.addAssembly([pwd '\..\..\..\..\net\VnSdk_Net.dll']); % change to file path
import VNSDK.* % Get rid of 'VNSDK' namespace qualifications

%{
This data export example walks through the MATLAB usage of the SDK to export data from a VectorNav unit to a CSV file.

This example will achieve the following:
1. Instantiate a Sensor object and use it to connect to the VectorNav unit
2. Create an ExportCsv object
3. Add a subscriber to all binary and ASCII packets
4. Log data from the VectorNav unit
5. Disconnect from the VectorNav unit
%}


if ~exist('portName', 'var')
    portName = 'COM1';
end

if ~exist('path', 'var')
    outputDirectory = fileparts(mfilename('fullpath'));
else
    outputDirectory = fileparts(path);
end

% 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
if exist('sensor','var')
    sensor.Disconnect();
else
    sensor = Sensor();
end
try
    sensor.AutoConnect(portName);
catch latestError
    error('Error encountered when connecting to %s.\n%s\n', portName, latestError.message);
end
fprintf('Connected to %s at %s\n', sensor.ConnectedPortName(), sensor.ConnectedBaudRate())

%% 2. Create an ExportCsv object
csvExporter = DataExport.ExporterCsv(outputDirectory);

%% 3. Add a subscriber to all binary and ASCII packets
try
    sensor.RegisterDataExporter(csvExporter);
catch latestError
    error('Error encountered when subscribing.\n%s\n', latestError.message);
end

%% 4. Log data from the VectorNav unit
if csvExporter.Start()
    error('Error: Failed to start logging to CSV file.');
end
fprintf('Logging to %s\n', outputDirectory);

secondsToRun = 5;
tic;
while toc < secondsToRun
    pause(0.001);

    % Handle async errors
    try
        sensor.ThrowIfAsyncError();
    catch asyncError
        fprintf('Received async error: %s\n',  asyncError.message);
    end
end

csvExporter.Stop();

%% 5. Disconnect from the VectorNav unit
sensor.Disconnect();
disp('DataExportFromSensor example complete.');
