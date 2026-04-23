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

disp('Starting Simple Logger example.')

vnsdkAssembly = NET.addAssembly([pwd '\..\..\..\net\VnSdk_Net.dll']); % change to file path
import VNSDK.* % Get rid of 'VNSDK' namespace qualifications

%{
This simple logger example walks through the MATlAB usage of the SDK to log data from a VectorNav unit.

This example will achieve the following:
1. Instantiate a Sensor object and use it to connect to the VectorNav unit
2. Set up resources needed for data logging
3. Register the logger's input buffer to receive all bytes from the unit
4. Log data from the VectorNav unit
    4.1. Check for any asynchronous errors
5. Disconnect from the VectorNav unit
%}

if ~exist('portName', 'var')
    portName = 'COM1'; % Pass in port name and path as positional arguments, or edit them here
end

if ~exist('path', 'var')
    path = fullfile(pwd, 'log.bin');
end

%% 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
if exist('sensor','var')
    sensor.Disconnect();  % We don't want to replace an object that already exists, and may keep the serial port locked
else
    sensor = Sensor();
end

try
    sensor.AutoConnect(portName);
catch latestError
    error('Error encountered when connecting to %s.\n%s\n', portName, latestError.message);
end
fprintf('Connected to %s at %s\n', sensor.ConnectedPortName(), sensor.ConnectedBaudRate())

modelRegister= Registers.System.Model();
try
    sensor.ReadRegister(modelRegister);
catch latestError
    error('Error encountered when reading register 1 (Model).\n%s\n', latestError.message);
end
fprintf('Sensor Model Number: %s\n', modelRegister.model)

%% 2. Set up resources needed for data logging
bufferToLog = ByteBuffer(1024*3);
logger = VNSDK.('Logger+SimpleLogger')(bufferToLog, path);

%% 3. Register the logger's input buffer to receive all bytes from the unit
sensor.RegisterReceivedByteBuffer(bufferToLog);

%% 4. Log data from the VectorNav unit
if logger.Start()
    error('Failed to write to file.');
end
fprintf('Logging to %s\n', path);

secondsToRun = 5;
tic;
while toc < secondsToRun
    pause(0.001);
    %% 4.1. Check for any asynchronous errors - Any buffer overruns means data has been dropped.
    try
        sensor.ThrowIfAsyncError();
    catch asyncError
        fprintf('Received async error: %s\n',  asyncError.message);
    end
end

logger.Stop();
sensor.DeregisterReceivedByteBuffer();

fprintf('Logged %d bytes.', logger.NumBytesLogged());

%% 5. Disconnect from the VectorNav unit
sensor.Disconnect();
disp('SimpleLogger example complete.')
