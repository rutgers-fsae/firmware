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
This load configuration example walks through the MATLAB usage of the SDK to load a configuration from an XML or ASCII file onto a VectorNav unit.

This example will achieve the following:
1. Handle input arguments
2. Instantiate a Sensor object and use it to connect to the VectorNav unit
3. Load the configuration onto the VectorNav unit
4. Disconnect from the VectorNav unit
%}

%% 1. Handle input arguments
if ~exist('portName', 'var')
    portName = 'COM1';
end

if ~exist('path', 'var')
    outputDirectory = fileparts(mfilename('fullpath'));
    path = fullfile(outputDirectory, 'configSettings.xml');
end

if strcmpi(path, 'generic')
    readType = 'generic';
else
    if ~isfile(path)
        fprintf('\nError: File does not exist: %s\n', path);
        return;
    end
    [~, ~, extension] = fileparts(path);
    if isempty(extension)
        fprintf('\nError: File path extension was not specified.\n');
        return;
    elseif strcmpi(extension, '.xml')
        readType = 'xml';
    else
        readType = 'ascii';
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
fprintf('\nConnected to %s at %d\n\n', portName, sensor.ConnectedBaudRate());

%% 3. Load the configuration onto the VectorNav unit - There are a few different file types that can be used to upload
%% a previously saved configuration file onto a VectorNav unit, such as an XML file or text file. When using
%% the RegisterScan.LoadConfiguration method, a Restore Factory Settings command will be issued to the unit
%% prior to attempting to load the user-defined configuration settings onto the unit. Following the upload, a
%% Write Settings command and a Reset command will also be issued to the unit. If performing a Restore Factory
%% Settings command is not desired, the RegisterScan.SetConfigurationRegisters() method should instead be used
%% to load the user-defined settings onto the unit.
if strcmpi(readType, 'xml')
    configReader = VNSDK.('RegisterScan+XmlConfigReader')(path);
elseif strcmpi(readType, 'ascii')
    configReader = VNSDK.('RegisterScan+AsciiConfigReader')(path);
else
    fprintf('Generic LoadConfiguration example unimplemented.\n');
    sensor.Disconnect();
    return;
end

try
    RegisterScan.LoadConfiguration(sensor, configReader);
catch latestError
    error('Error occured when loading configuration. \n%s\n', latestError.message);
end

disp('Configuration has been loaded to the sensor.')

%% 4. Disconnect from the VectorNav unit
sensor.Disconnect();
fprintf('LoadConfiguration example complete.');
