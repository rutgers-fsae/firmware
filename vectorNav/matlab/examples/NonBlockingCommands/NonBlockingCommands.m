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
%}


%% 1. Instantiate a Sensor object and use it to connect to the VectorNav unit
if ~exist('portName', 'var')
    portName = 'COM1'; % Change the sensor port name to the comm port of your local machine
end
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
fprintf('\nConnected to %s at %d\n', portName, sensor.ConnectedBaudRate());

modelRegister = Registers.System.Model();
try
    sensor.ReadRegister(modelRegister);
catch latestError
    error('Error encountered when reading register 1 (Model).\n%s\n', latestError.message);
end
fprintf('Sensor model number: %s\n', modelRegister.model)

%% 2. Configure the asynchronous ASCII output to YPR at 100 Hz
asyncDataOutputType = Registers.System.AsyncOutputType();
asyncDataOutputType.ador = Registers.System.('AsyncOutputType+Ador').YPR; % Configure for Yaw, Pitch, Roll data.
asyncDataOutputType.serialPort = Registers.System.('AsyncOutputType+SerialPort').Serial1;
try
    sensor.WriteRegister(asyncDataOutputType);
catch latestError
    error('Error encountered when writing register 6 (AsyncOutputType).\n%s\n', latestError.message);
end
disp("ADOR configured.");

asyncDataOutputFrequency = Registers.System.AsyncOutputFreq();
asyncDataOutputFrequency.adof = Registers.System.('AsyncOutputFreq+Adof').Rate100Hz; % Set output rate to 100Hz.
asyncDataOutputFrequency.serialPort = Registers.System.('AsyncOutputFreq+SerialPort').Serial1;
try
    sensor.WriteRegister(asyncDataOutputFrequency);
catch latestError
    error('Error encountered when writing register 7 (AsyncOutputFreq).\n%s\n', latestError.message);
end
disp("ADOF configured.")

%% 3. Send generic command (Known Magnetic Disturbance command)
kmd = KnownMagneticDisturbance(VNSDK.('KnownMagneticDisturbance+State').Present);
try
    sensor.SendCommand(kmd, VNSDK.('Sensor+SendCommandBlockMode').None); % Non-blocking
catch latestError
    error('Error received while sending the Known Magnetic Disturbance Command.\n%s\n', latestError.message)
end

%% 4. Wait and check response
pause(0.25);
% We could check kmd.awaitingResponse() but it will be removed from the command queue (setting kmd.awaitingResponse to false) after commandRemovalTimeoutLength
% (default 200ms), if any command has been sent or received since.
if kmd.HasValidResponse()
    fprintf('\nKMD Response: %s\n', kmd.GetResponse());
    error_maybe = kmd.GetError();
    if error_maybe.HasValue
        error('Error: %s\n', error_maybe.Value);
    end
else
    error('Error: KMD did not receive a valid response.');
end

%% 5. Output VNYPR at 100 Hz and send velocity aiding commands at 10 Hz for 5 seconds

% Initialize velocity aiding register, command, counters, flags, and timers
velAidRegister = Registers.VelocityAiding.VelAidingMeas();
velAidRegister.velocityX = 0;
velAidRegister.velocityY = 0;
velAidRegister.velocityZ = 0;
velAidWRGCommand = velAidRegister.ToWriteCommand();
if isempty(velAidWRGCommand)
    error('Error: Failed to create velocity aiding command.');
end

asciiCount = 0;
velAidSentCount = 0;
validResponseReceived = false;

totalTimeLimit = 5;
resendInterval = 0.5;
timer = tic;
resendTimer = tic;

while toc(timer) < totalTimeLimit
    measurement = sensor.GetNextMeasurement();
    if measurement.HasValue && measurement.Value.MatchesMessage("VNYPR")
        ypr = measurement.Value.attitude.ypr;
        fprintf('YPR: %f, %f, %f\n', ypr.Value.yaw, ypr.Value.pitch, ypr.Value.roll);
        asciiCount = asciiCount + 1;
    end

    %% 5.1. Check if a valid response has been received from the velocity aiding command
    if (~validResponseReceived && ~velAidWRGCommand.IsAwaitingResponse())
        error_maybe = velAidWRGCommand.GetError();
        if (velAidWRGCommand.HasValidResponse())
            fprintf('\nWRG Response: %s\n', velAidWRGCommand.GetResponse())
            validResponseReceived = true;
        elseif error_maybe.HasValue
            error('Error: %s\n', error_maybe.Value);
        end
    end

    %% 5.2. Print response and send new command
    if toc(resendTimer) > resendInterval
        if (~validResponseReceived && velAidSentCount > 0)
            error('\nError: Response Timeout\n');
        end

        velAidRegister = Registers.VelocityAiding.VelAidingMeas();
        velAidRegister.velocityX = rand(); % rand() to simulate different velocities
        velAidRegister.velocityY = rand();
        velAidRegister.velocityZ = rand();
        velAidWRGCommand = velAidRegister.ToWriteCommand();
        if isempty(velAidWRGCommand)
            error('Error: Failed to create velocity aiding command.');
        end
        try
            sensor.SendCommand(velAidWRGCommand, VNSDK.('Sensor+SendCommandBlockMode').None); % Non-blocking
        catch latestError
            fprintf('Error encountered when writing to register.\n%s', latestError.message);
        end

        validResponseReceived = false;
        velAidSentCount = velAidSentCount + 1;
        resendTimer = tic;  % Restart send timer
    end

    % Handle async errors
    try
        sensor.ThrowIfAsyncError();
    catch asyncError
        fprintf('Received async error: %s\n',  asyncError.message);
    end
end

fprintf('\nTotal ASCII YPR Packets Received: %d\n', asciiCount);
fprintf('Total VelAid Commands Sent: %d\n', velAidSentCount);
fprintf('\nNonBlockingCommands example complete\n');

%% 6. Disconnect from the VectorNav unit
sensor.Disconnect();
