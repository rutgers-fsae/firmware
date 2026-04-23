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

vnsdkAssembly = NET.addAssembly([pwd '\..\..\..\net\VnSdk_Net.dll']); % change to file path
import VNSDK.* % Get rid of 'VNSDK' namespace qualifications
import System.Collections.Generic.*;

%{
This firmware update example walks through the MATLAB usage of the SDK to connect to and update the firmware on a VectorNav unit.

This example will achieve the following:
1. Instantiate a Sensor object and use it to connect to the VectorNav unit
2. Create a FirmwareUpdater object
3. Update the firmware based on the file type
4. Disconnect from the VectorNav unit
%}

% Define the port connection parameters to be used later
if ~exist('portName', 'var')
    portName = 'COM1'; % Change the sensor port name to the COM port of your local machine
end

files = NET.createGeneric('System.Collections.Generic.List', {'VNSDK.FirmwareProgrammer+FirmwareUpdater+FirmwareFile'}, 0);
if ~exist('Nav', 'var')
    Nav = fullfile(pwd, 'ReferenceModels_v3.vn100.vnx'); % Change this to your sensor
    file = VNSDK.('FirmwareProgrammer+FirmwareUpdater+FirmwareFile')(VNSDK.('FirmwareProgrammer+FirmwareUpdater+Processor').Nav, Nav);
    files.Add(Nav)
else
    file = VNSDK.('FirmwareProgrammer+FirmwareUpdater+FirmwareFile')(VNSDK.('FirmwareProgrammer+FirmwareUpdater+Processor').Nav, Nav);
    files.Add(file)
    fprintf('Loading Nav firmware from %s\n', Nav);
end
if ~exist('Gnss', 'var')
    Gnss = '';
else
    file = VNSDK.('FirmwareProgrammer+FirmwareUpdater+FirmwareFile')(VNSDK.('FirmwareProgrammer+FirmwareUpdater+Processor').Gnss, Gnss);
    files.Add(file);
    fprintf('Loading Gnss firmware from %s\n', Gnss);
end
if ~exist('Imu', 'var')
    Imu = '';
else
    file = VNSDK.('FirmwareProgrammer+FirmwareUpdater+FirmwareFile')(VNSDK.('FirmwareProgrammer+FirmwareUpdater+Processor').Imu, Imu);
    files.Add(file);
    fprintf('Loading Imu firmware from %s\n', Imu);
end
if ~exist('vnXml', 'var')
    vnXml = '';
else
    fprintf('Loading VnXml from %s\n', vnXml);
end
if ~exist('firmwareBaudRate', 'var')
    firmwareBaudRate = 115200;
end
if ~exist('bootloaderBaudRate', 'var')
    bootloaderBaudRate = 115200;
end

%% 1. Instantiate a Sensor object and use it to connect to the VectorNav unit.
% We are not autoconnecting or verifying connectivity because we cannot assume the sensor has a valid firmware
if exist('sensor','var')
    sensor.Disconnect();  % We don't want to replace an object that already exists, and may keep the serial port locked
else
    sensor = Sensor();
end

try
    sensor.Connect(portName, firmwareBaudRate);
catch latestError
    error('Error encountered when connecting to %s.\n%s\n', portName, latestError.message);
end

% 2. Create a FirmwareUpdater object
firmwareUpdater = VNSDK.('FirmwareProgrammer+FirmwareUpdater')();
params = VNSDK.('FirmwareProgrammer+FirmwareUpdater+Params')(firmwareBaudRate, bootloaderBaudRate);
% 3. Update the firmware based on the file type - There are two file types that can be used to update the firmware: VNX or VNXML
if ~isempty(vnXml)
    try
        firmwareUpdater.UpdateFirmware(sensor, vnXml, params);
    catch firmwareUpdateError
        error('%s\nFirmware update failed.', firmwareUpdateError.message);
    end
else
    try
        firmwareUpdater.UpdateFirmware(sensor, files, params);
    catch firmwareUpdateError
        error('%s\nFirmware update failed.', firmwareUpdateError.message);
    end
end


% Handle asynchronous errors that occurred during firmware upgrade
while (true)
    try
        sensor.ThrowIfAsyncError();
        break;
    catch asyncError
        fprintf('Received async error: %s\n',  asyncError.message);
    end
end

% 4. Disconnect from the VectorNav unit
sensor.Disconnect();
fprintf('FirmwareUpdate example complete.');
