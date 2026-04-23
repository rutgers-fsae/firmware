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

%% Make .NET assembly visible to MATLAB
vnsdkAssembly = NET.addAssembly([pwd '\..\..\..\net\VnSdk_Net.dll']); % change to file path
import VNSDK.* % Get rid of 'VNSDK' namespace qualifications

%{
This getting started example walks through the MATLAB usage of the SDK to connect to and interact with a VectorNav unit.

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
%}

% Define the port connection parameters to be used later
if ~exist('portName', 'var')
    portName = 'COM1'; % Change the sensor port name to the COM port of your local machine
end

%% 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
if exist('sensor','var')
    sensor.Disconnect();  % We don't want to replace an object that already exists, and may keep the serial port locked
else
    sensor = Sensor();
end

try
    sensor.AutoConnect(portName);
    if ~sensor.VerifySensorConnectivity()
        throw(MException('VNSDK:Error','VerificationFailure'));
    end
catch latestError
    error('Error encountered when connecting to %s.\n%s\n', portName, latestError.message);
end
fprintf(['Connected to %s at %i\n'],portName, sensor.ConnectedBaudRate());

%% 2. Poll and print the model number using a read register command
% Create an empty register object of the necessary type, where the data member will be populated when the sensor responds to the "read register" request
modelRegister = Registers.System.Model();
try
    sensor.ReadRegister(modelRegister);
catch latestError
    error('Error encountered when reading register 1 (Model).\n%s\n', latestError.message);
end
fprintf('Sensor model number: %s\n', modelRegister.model)

%% 3. Poll and print the current yaw, pitch, and roll using a read register command
yprRegister = Registers.Attitude.YawPitchRoll();
try
    sensor.ReadRegister(yprRegister);
catch latestError
    error('Error encountered when reading register reading 8 (YawPitchRoll).\n%s\n', latestError.message);
end
fprintf(['Current reading:\n\tYaw: %f\t, Pitch: %f\t, Roll: %f\n'], yprRegister.yaw, yprRegister.pitch, yprRegister.roll);

%% 4. Configure the asynchronous ASCII output to YPR at 2Hz
availableEnums = vnsdkAssembly.Enums;  % Get available enums
getEnumNames('VNSDK.Registers.System.AsyncOutputType+Ador', vnsdkAssembly)  % display enum names
asyncDataOutputType = Registers.System.AsyncOutputType();
asyncDataOutputType.ador = Registers.System.('AsyncOutputType+Ador').YPR;
asyncDataOutputType.serialPort = Registers.System.('AsyncOutputType+SerialPort').Serial1;
try
    sensor.WriteRegister(asyncDataOutputType);
catch latestError
    error('Error encountered when writing register 6 (AsyncOutputType).\n%s\n', latestError.message);
end
disp("ADOR configured.");

asyncDataOutputFrequency = Registers.System.AsyncOutputFreq();
asyncDataOutputFrequency.adof = Registers.System.('AsyncOutputFreq+Adof').Rate2Hz;
asyncDataOutputFrequency.serialPort = Registers.System.('AsyncOutputFreq+SerialPort').Serial1;
try
    sensor.WriteRegister(asyncDataOutputFrequency);
catch latestError
    error('Error encountered when writing register 7 (AsyncOutputFreq).\n%s\n', latestError.message);
end
disp("ADOF configured.")

%% 5. Configure the first binary output to output timeStartup, accel, and angRate, all from common group, with a 400 rate divisor
binaryOutput1Register = Registers.System.BinaryOutput1();
binaryOutput1Register.rateDivisor = 400;
binaryOutput1Register.asyncMode.serial1 = true;
binaryOutput1Register.common.timeStartup = true;
binaryOutput1Register.common.angularRate = true;
binaryOutput1Register.common.accel = true;
binaryOutput1Register.common.imu = true;
try
    sensor.WriteRegister(binaryOutput1Register);
catch latestError
    error('Error encountered when writing register 75 (BinaryOutput1).\n%s\n', latestError.message);
end
disp("Binary output 1 message configured.")

%% 6. Enter a loop for 5 seconds where it:
%      Determines which measurement it received(VNYPR or the necessary binary header)
%      Prints out the relevant measurement from the CompositeData struct
secondsToRun = 5;
tic;
while toc < secondsToRun
    compositeData = sensor.GetNextMeasurement();
    % Check to make sure that a measurement is available
    if ~compositeData.HasValue
        continue;
    end
    if compositeData.Value.MatchesMessage(binaryOutput1Register)
        fprintf("Found binary 1 measurment.\n")

        timeStartup = compositeData.Value.time.timeStartup.Value;
        fprintf("\tTime: %f\t", timeStartup.nanoseconds());
        accel = compositeData.Value.imu.accel.Value;
        fprintf("\n\tAccel X: %f\n\tAccel Y: %f\n\tAccel Z: %f\n", accel.x, accel.y, accel.z);
    elseif compositeData.Value.MatchesMessage('VNYPR')
        fprintf("Found Ascii ypr measurment.\n")

        ypr = compositeData.Value.attitude.ypr;
        fprintf('\tYaw: %f\n\tPitch: %f\n\tRoll: %f\n',ypr.Value.yaw, ypr.Value.pitch, ypr.Value.roll);
    else
        fprintf("Unrecognized asynchronous message received.\n");
    end

    % Handle async errors
    try
        sensor.ThrowIfAsyncError();
    catch asyncError
        fprintf('Received async error: %s\n',  asyncError.message);
    end
end

%% 7. Disconnect from the VectorNav unit
sensor.Disconnect();
fprintf('Sensor disconnected.\n')
fprintf('GettingStarted example complete')
