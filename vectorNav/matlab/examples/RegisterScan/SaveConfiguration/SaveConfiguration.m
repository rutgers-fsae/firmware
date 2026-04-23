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
This save configuration example walks through the MATLAB usage of the SDK to save the current configuration of a VectorNav unit into an XML file or ASCII file.

This example will achieve the following:
1. Handle input arguments
2. Instantiate a Sensor object and use it to connect to the VectorNav unit
3. Save the configuration from the VectorNav unit
4. Disconnect from the VectorNav unit
%}

%% 1. Handle input arguments
if ~exist('portName', 'var')
    portName = 'COM1';
end

if ~exist('path', 'var')
    outputDirectory = fileparts(mfilename('fullpath'));
    path = fullfile(outputDirectory, 'configSettings.txt');
end

if strcmpi(path, 'generic')
    writeType = 'generic';
else
    [~, ~, extension] = fileparts(path);
    if isempty(extension)
        fprintf('\nError: File path extension was not specified.\n');
        return;
    elseif strcmpi(extension, '.xml')
        writeType = 'xml';
    else
        writeType = 'ascii';
    end
end

%% 2. Instantiate a Sensor object and use it to connect to the VectorNav unit
if exist('sensor','var')
    sensor.Disconnect();
else
    sensor = Sensor();
end
try
    sensor.AutoConnect(portName, true); % enables throwing from async error monitoring thread
catch latestError
    error('Error encountered when connecting to %s.\n%s\n', portName, latestError.message);
end
fprintf('Connected to %s at %d\n\n', portName, sensor.ConnectedBaudRate());

%% 3. Save the configuration from the VectorNav unit - There are a few different file types that can be used to save
% the current configuration settings from a VectorNav unit into a configuration file, such as an XML file or text file.

% filter = VNSDK.('RegisterScan+SaveConfigurationFilter')();
% filter.type = VNSDK.('RegisterScan+SaveConfigurationFilter+Type').Include;
% filter.list = RegisterScan.GetDefaultConfigRegisters();

if strcmpi(writeType, 'xml')
    configWriter = VNSDK.('RegisterScan+XmlConfigWriter')(path, sensor);
elseif strcmpi(writeType, 'ascii')
    configWriter = VNSDK.('RegisterScan+AsciiConfigWriter')(path);
else
    fprintf('Generic SaveConfiguration example unimplemented.\n');
    sensor.Disconnect();
    return;
end

try
    % RegisterScan.SaveConfiguration(sensor, configWriter, filter);
    RegisterScan.SaveConfiguration(sensor, configWriter);
catch latestError
    error('Error occured when saving configuration. \n%s\n', latestError.message);
end

%% 4. Disconnect from the VectorNav unit
sensor.Disconnect();
fprintf('SaveConfiguration example complete.');
