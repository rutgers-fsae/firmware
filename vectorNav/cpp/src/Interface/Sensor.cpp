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

#include "vectornav/Interface/Sensor.hpp"

#include "vectornav/Debug.hpp"
#include "vectornav/Interface/Commands.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/GenericCommand.hpp"

namespace VN
{

// ------------------------------------------
// Constructor and Desctructor
// ------------------------------------------

Sensor::Sensor(MeasQueueMode mode) : _measQueueMode{mode}, _parseToCD{mode != MeasQueueMode::Off}
{
    // Set up packet synchronizer
    _packetSynchronizer.addDispatcher(&_faPacketDispatcher);
    _packetSynchronizer.addDispatcher(&_asciiPacketDispatcher);
    _packetSynchronizer.addDispatcher(&_fbPacketDispatcher);
    if (_parseToCD) { _measurementQueue.setPutMode(static_cast<MeasurementQueue::PutMode>(_measQueueMode)); }
}

Sensor::Sensor(ByteBuffer& mainBuffer, ByteBuffer& fbBuffer, MeasQueueMode mode)
    : _mainByteBuffer(mainBuffer),
      _measQueueMode{mode},
      _parseToCD{mode != MeasQueueMode::Off},
      _fbPacketDispatcher(&_faPacketDispatcher, fbBuffer.data(), fbBuffer.size())
{
    // Set up packet synchronizer
    _packetSynchronizer.addDispatcher(&_faPacketDispatcher);
    _packetSynchronizer.addDispatcher(&_asciiPacketDispatcher);
    _packetSynchronizer.addDispatcher(&_fbPacketDispatcher);
    if (_parseToCD) { _measurementQueue.setPutMode(static_cast<MeasurementQueue::PutMode>(_measQueueMode)); }
}

Sensor::~Sensor()
{
#if (THREADING_ENABLE)
    if (_listening)
    {  // Happens if exited by Error, and ensures that secondary thread closes out.
        _stopListening();
    }
#endif
}

// -------------------
// Serial Connectivity
// -------------------

Error Sensor::connect(const Serial_Base::PortName& portName, const BaudRate baudRate) noexcept
{
    if (_connectionType != ConnectionType::None) { return Error::AlreadyConnected; }
    Error lastError = _serial.open(portName, static_cast<uint32_t>(baudRate));
    if (lastError != Error::None) { return lastError; }
    _connectionType = ConnectionType::Serial;
#if (THREADING_ENABLE)
    _startListening();
#endif
    return Error::None;
}

Error Sensor::connect(const Filesystem::FilePath& fileName) noexcept
{
    if (_connectionType != ConnectionType::None) { return Error::AlreadyConnected; }
    Errored lastError = _file.open(fileName);
    if (lastError) { return Error::FileOpenFailed; }
    _connectionType = ConnectionType::File;
#if (THREADING_ENABLE)
    _startListening();
#endif
    return Error::None;
}

Error Sensor::autoConnect(const Serial_Base::PortName& portName) noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }

    Error error = connect(portName, BaudRate::Baud115200);
    if (error != Error::None) { return error; }

    return autoBaud();
}

bool Sensor::verifySensorConnectivity() noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return false; }
    Registers::System::Model modelRegister;
    modelRegister.model = "";

    Error readRegError = readRegister(&modelRegister, Config::Sensor::retryVerifyConnectivity);
    if (readRegError != Error::None) { return false; }
    return (modelRegister.model != "");
}

Error Sensor::changeBaudRate(const BaudRate newBaudRate, Registers::System::BaudRate::SerialPort serialPort) noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }
    if (!_serial.isSupportedBaudRate(static_cast<uint32_t>(newBaudRate))) { return Error::UnsupportedBaudRate; }

    // Check to see if the serial port selected is the active serial port
    bool isActiveSerial{true};
    Error latestError;
    if (serialPort == Registers::System::BaudRate::SerialPort::ActiveSerial) { isActiveSerial = true; }
    else
    {
        Registers::System::BaudRate baudRateRegister;
        baudRateRegister.serialPort = Registers::System::BaudRate::SerialPort::Poll;
        latestError = readRegister(&baudRateRegister);
        if (latestError != VN::Error::None) { return latestError; }
        if (baudRateRegister.serialPort == Registers::System::BaudRate::SerialPort::Poll) { return Error::SerialReadFailed; }
        isActiveSerial = (serialPort == baudRateRegister.serialPort);
    }

    // Change the sensor baud rate on the selected serial line
    Registers::System::BaudRate reg5;
    reg5.baudRate = newBaudRate;
    reg5.serialPort = serialPort;
    latestError = writeRegister(&reg5, true);
    if (latestError != Error::None) { return latestError; }

    // Change the host baud rate if connected to active serial
    if (isActiveSerial)
    {
        // Host baud rate may switch quicker than unit can switch its baud rate, so we need to give time to the unit before sending any subsequent verify calls
        thisThread::sleepFor(50ms);  // Appears that the sensor may take time to correctly configure, even if changing from and to the same baud rate. Tested by
                                     // changing baud rate in loop, and occasionally received invalid checksum.
        latestError = changeHostBaudRate(newBaudRate);
    }
    return latestError;
}

Error Sensor::changeHostBaudRate(const BaudRate newBaudRate) noexcept
{
    if (connectedBaudRate().has_value() && newBaudRate == connectedBaudRate().value()) { return Error::None; }
#if (THREADING_ENABLE)
    _stopListening();
#endif
    Error lastError = _serial.changeBaudRate(static_cast<uint32_t>(newBaudRate));
    if (lastError != Error::None) { return lastError; }
#if (THREADING_ENABLE)
    _startListening();
#endif
    return Error::None;
}

void Sensor::disconnect() noexcept
{
#if (THREADING_ENABLE)
    _stopListening();
#endif
    if (_connectionType == ConnectionType::Serial) { _serial.close(); }
    else if (_connectionType == ConnectionType::File) { _file.close(); }
    _connectionType = ConnectionType::None;
}

// ----------------------
// Accessing Measurements
// ----------------------

Sensor::CompositeDataQueueReturn Sensor::getNextMeasurement(const bool block) noexcept
{
    if constexpr (Config::PacketDispatchers::compositeDataQueueCapacity == 0) { return nullptr; }
    Timer timer(Config::Sensor::getMeasurementTimeoutLength);
    timer.start();
    CompositeDataQueueReturn queueReturn = _measurementQueue.get();
    if (!queueReturn)
    {
        if (block) { queueReturn = _blockOnMeasurement(timer, Config::Sensor::getMeasurementSleepDuration); }
    }
    return queueReturn;
}

Sensor::CompositeDataQueueReturn Sensor::getMostRecentMeasurement(const bool block) noexcept
{
    Timer timer(Config::Sensor::getMeasurementTimeoutLength);
    timer.start();
    CompositeDataQueueReturn queueReturn = _measurementQueue.getBack();
    if (!queueReturn)
    {
        if (block) { queueReturn = _blockOnMeasurement(timer, Config::Sensor::getMeasurementSleepDuration); }
    }
    return queueReturn;
}

Sensor::CompositeDataQueueReturn Sensor::_blockOnMeasurement(Timer& timer, [[maybe_unused]] const Microseconds sleepLength) noexcept
{
    bool hasTimedOut = false;
    bool retValHasValue = false;
    CompositeDataQueueReturn queueReturn;
    while (!retValHasValue && !hasTimedOut)
    {
#if (THREADING_ENABLE)
        thisThread::sleepFor(sleepLength);
#else
        bool needsMoreData = processNextPacket();
        if (needsMoreData)
        {
            Error lastError = loadMainBufferFromSerial();
            if (lastError != Error::None) { _asyncErrorQueue.put(AsyncError(lastError, now())); }
        }
#endif
        queueReturn = _measurementQueue.get();
        retValHasValue = queueReturn != nullptr;
        hasTimedOut = timer.hasTimedOut();
    }
    return queueReturn;
}

Error Sensor::_blockOnCommand(GenericCommand* command, Timer& timer) noexcept
{
    bool hasTimedOut = false;
#if (!THREADING_ENABLE)
    Error lastError = loadMainBufferFromSerial();
    if (lastError != Error::None) { _asyncErrorQueue.put(AsyncError(lastError, now())); }
#endif
    while (command->isAwaitingResponse())
    {
#if (THREADING_ENABLE)
        thisThread::sleepFor(Config::Sensor::commandSendSleepDuration);
#else
        bool needsMoreData = processNextPacket();
        if (needsMoreData)
        {
            lastError = loadMainBufferFromSerial();
            if (lastError != Error::None) { _asyncErrorQueue.put(AsyncError(lastError, now())); }
        }
#endif
        hasTimedOut = timer.hasTimedOut();
        if (hasTimedOut)
        {
            _commandProcessor.popCommandFromQueueBack();        // Since we're not tracking the command, let's remove it from the queue so that we can resend it
            command->isMatchingResponse("FAIL", time_point());  // Ensure awaiting flag is set false
            VN_DEBUG_1("Command timed out.");
            return Error::ResponseTimeout;
        }
    };

    if (!command->hasValidResponse())
    {
        // The command was popped by another command's response
        VN_DEBUG_1("Command removed from queue.");
        return Error::ResponseTimeout;
    }

    auto maybeError = command->getError();
    if (maybeError.has_value()) { return maybeError.value(); }
    return Error::None;
}

// ----------------
// Sending Commands
// ----------------

Error Sensor::readRegister(Register* registerToRead, const bool retryOnFailure) noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }
    GenericCommand readCommand = registerToRead->toReadCommand();
    SendCommandBlockMode waitMode;
    if (retryOnFailure) { waitMode = SendCommandBlockMode::BlockWithRetry; }
    else { waitMode = SendCommandBlockMode::Block; }
    Error sendCommandError = sendCommand(&readCommand, waitMode);
    if (sendCommandError != Error::None) { return sendCommandError; }

    if (registerToRead->fromCommand(readCommand)) { return Error::ReceivedInvalidResponse; }
    return Error::None;
}

Error Sensor::writeRegister(ConfigurationRegister* registerToWrite, const bool retryOnFailure) noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }
    std::optional<GenericCommand> writeCommand = registerToWrite->toWriteCommand();
    if (!writeCommand.has_value()) { return Error::NotEnoughParameters; }
    SendCommandBlockMode waitMode;
    if (retryOnFailure) { waitMode = SendCommandBlockMode::BlockWithRetry; }
    else { waitMode = SendCommandBlockMode::Block; }
    const Error sendCommandError = sendCommand(&writeCommand.value(), waitMode);
    if (sendCommandError != Error::None) { return sendCommandError; }
    if (registerToWrite->fromCommand(writeCommand.value())) { return Error::ReceivedInvalidResponse; };
    return Error::None;
}

Error Sensor::writeSettings() noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }
    WriteSettings wnv{};
    return sendCommand(&wnv, SendCommandBlockMode::Block, Config::Sensor::wnvSendTimeoutLength, Config::Sensor::wnvSendTimeoutLength * 2);
}

Error Sensor::reset() noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }
    uint16_t errQueueSize0 = asyncErrorQueueSize();
    Reset rst{};
    const Error sendCommandError = sendCommand(&rst, SendCommandBlockMode::BlockWithRetry);
    if (sendCommandError != Error::None) { return sendCommandError; }
    thisThread::sleepFor(Config::Sensor::resetSleepDuration);  // Give sensor time to start up
    if (!verifySensorConnectivity())
    {
        Error latestError = autoBaud();
        if (latestError != Error::None) { return latestError; }
    }

    // Remove any async errors since we connected successfully
    uint16_t errQueueSizef = asyncErrorQueueSize();
    for (uint16_t i{0}; i < errQueueSizef - errQueueSize0; ++i) { _asyncErrorQueue.popBack(); }

    return Error::None;
}

Error Sensor::restoreFactorySettings() noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }
    RestoreFactorySettings rfs{};
    auto baudRate = _serial.connectedBaudRate();
    if (!baudRate) { return Error::SerialPortClosed; }

    Error sendCommandRetVal = sendCommand(&rfs, SendCommandBlockMode::Block, Config::Sensor::wnvSendTimeoutLength, Config::Sensor::wnvSendTimeoutLength * 2);
    if (*baudRate == 115200)
    {
        if (sendCommandRetVal != Error::None) { return sendCommandRetVal; }
    }
    else
    {
        // Allow the unit not to reply if not on default baud rate
        if (sendCommandRetVal != Error::None && sendCommandRetVal != Error::ResponseTimeout) { return sendCommandRetVal; }
    }
#if (THREADING_ENABLE)
    _stopListening();
#endif
    const Error changeBaudRateError = _serial.changeBaudRate(115200);
    if (changeBaudRateError != Error::None) { return changeBaudRateError; }
    const auto cachedMeasQSize = _measurementQueue.size();
#if (THREADING_ENABLE)
    _startListening();
#endif
    // Assume connectivity if we have received a message
    Timer timer(Config::Sensor::resetSleepDuration);
    timer.start();
    while (!timer.hasTimedOut())
    {
        if (_measurementQueue.size() > cachedMeasQSize)
        {
            // We've received a validated measurement, the sensor must have started up and is correctly communicating.
            return Error::None;
        }
        thisThread::sleepFor(Config::Sensor::getMeasurementSleepDuration);
    }
    // This shouldn't happen, because the sensor should default to outputting a 40Hz ascii message.
    return verifySensorConnectivity() ? Error::None : Error::ResponseTimeout;
}

Error Sensor::knownMagneticDisturbance(const KnownMagneticDisturbance::State state) noexcept
{
    KnownMagneticDisturbance kmd(state);
    return sendCommand(&kmd, SendCommandBlockMode::Block);
}

Error Sensor::knownAccelerationDisturbance(const KnownAccelerationDisturbance::State state) noexcept
{
    KnownAccelerationDisturbance kad(state);
    return sendCommand(&kad, SendCommandBlockMode::Block);
}

Error Sensor::setInitialHeading(const float heading) noexcept
{
    SetInitialHeading sih(heading);
    return sendCommand(&sih, SendCommandBlockMode::Block);
}

Error Sensor::setInitialHeading(const Ypr& ypr) noexcept
{
    SetInitialHeading sih(ypr);
    return sendCommand(&sih, SendCommandBlockMode::Block);
}

Error Sensor::setInitialHeading(const Quat& quat) noexcept
{
    SetInitialHeading sih(quat);
    return sendCommand(&sih, SendCommandBlockMode::Block);
}

Error Sensor::asyncOutputEnable(const AsyncOutputEnable::State state) noexcept
{
    AsyncOutputEnable asy(state);
    return sendCommand(&asy, SendCommandBlockMode::Block);
}

Error Sensor::setFilterBias() noexcept
{
    SetFilterBias sfb{};
    return sendCommand(&sfb, SendCommandBlockMode::Block);
}

Error Sensor::setBootloader(const SetBootLoader::Processor processorId) noexcept
{
    SetBootLoader sbl{processorId};
    return sendCommand(&sbl, SendCommandBlockMode::Block, 6s);
}

Error Sensor::sendCommand(GenericCommand* commandToSend, SendCommandBlockMode waitMode, const Microseconds waitLengthMs,
                          const Microseconds timeoutThreshold) noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }
    CommandProcessor::RegisterCommandReturn regCommandReturn = _commandProcessor.registerCommand(commandToSend, timeoutThreshold);
    if (regCommandReturn.error != Error::None) { return regCommandReturn.error; }
    Error lastError = _serial.send(regCommandReturn.message.c_str(), regCommandReturn.message.length());
    if (lastError != Error::None) { return lastError; }

    if (waitMode == SendCommandBlockMode::None) { return Error::None; }
    Timer timer(waitLengthMs);
    timer.start();

    lastError = _blockOnCommand(commandToSend, timer);
    // We are attempting to try catch ResponseTimeout only
    if ((lastError == Error::None) || (lastError != Error::ResponseTimeout) || (waitMode != SendCommandBlockMode::BlockWithRetry)) { return lastError; }

    uint8_t retries = 0;  // Has already tried once
    while (!commandToSend->hasValidResponse())
    {  // Don't need timeout checked because it will error inside block
        timer.start();
        // We can resend because the command string will not have been overwriten by the response
        regCommandReturn = _commandProcessor.registerCommand(commandToSend);

        if (regCommandReturn.error != Error::None) { return regCommandReturn.error; }

        lastError = _serial.send(regCommandReturn.message.c_str(), regCommandReturn.message.length());
        if (lastError != Error::None) { return lastError; }

        lastError = _blockOnCommand(commandToSend, timer);
        if (lastError != Error::None)
        {
            if (lastError != Error::ResponseTimeout) { return lastError; }
            if (++retries == Config::Sensor::commandSendRetriesAllowed)
            {  // We have run out of retries
                return Error::ResponseTimeout;
            }
        }
    }
    return Error::None;
}

Error Sensor::serialSend(const char* buffer, size_t len) noexcept
{
    Error lastError = _serial.send(buffer, len);
    if (lastError != Error::None) { return lastError; }
    return Error::None;
}

Error Sensor::autoBaud() noexcept
{
    if constexpr (Config::CommandProcessor::commandProcQueueCapacity == 0) { return Error::CommandQueueFull; }

    if (_connectionType != ConnectionType::Serial) { return Error::SerialPortClosed; }

    std::array<BaudRate, 9> possibleBaudRates{
        BaudRate::Baud115200, BaudRate::Baud921600, BaudRate::Baud9600,   BaudRate::Baud19200,  BaudRate::Baud38400,
        BaudRate::Baud57600,  BaudRate::Baud128000, BaudRate::Baud230400, BaudRate::Baud460800,
    };

    for (const auto activeBaudRate : possibleBaudRates)
    {
        Error error = changeHostBaudRate(activeBaudRate);
        if (error == Error::UnsupportedBaudRate) { continue; }
        if (error != Error::None) { return error; }

        if (verifySensorConnectivity()) { return Error::None; }
    }

    disconnect();
    return Error::ResponseTimeout;
}

// ------------------
// Additional logging
// ------------------

void Sensor::registerReceivedByteBuffer(ByteBuffer* const receivedByteBuffer) noexcept
{
#if THREADING_ENABLE
    LockGuard lock(_sensorMutex);
#endif
    _receivedByteBuffer = receivedByteBuffer;
}

void Sensor::deregisterReceivedByteBuffer() noexcept
{
#if THREADING_ENABLE
    LockGuard lock(_sensorMutex);
#endif
    _receivedByteBuffer = nullptr;
}

Error Sensor::subscribeToMessage(PacketQueue_Interface* queueToSubscribe, const SyncByte syncByte) noexcept
{
    switch (syncByte)
    {
        case (SyncByte::Ascii):
        {
            return subscribeToMessage(queueToSubscribe, AsciiHeader{});
        }
        case (SyncByte::FA):
        {
            return subscribeToMessage(queueToSubscribe, BinaryOutputMeasurements{});
        }
        case (SyncByte::FB):
        {
            return subscribeToMessage(queueToSubscribe, Fb00SubscriberFilter(true, true));
        }
        case (SyncByte::None):
        {
#if THREADING_ENABLE
            LockGuard lock(_sensorMutex);
#endif
            return _packetSynchronizer.registerSkippedByteQueue(queueToSubscribe);
        }
        default:
            return Error::InvalidParameter;
    }
}

Error Sensor::subscribeToMessage(PacketQueue_Interface* queueToSubscribe, const BinaryOutputMeasurements& binaryOutputMeasurmenetFilter,
                                 const FaSubscriberFilterType filterType) noexcept
{
#if THREADING_ENABLE
    LockGuard lock(_sensorMutex);
#endif
    std::optional<EnabledMeasurements> filterMeas = binaryOutputMeasurmenetFilter.toBinaryHeader().toMeasurementHeader();
    if (!filterMeas.has_value()) { return Error::ParsingFailed; }
    return _faPacketDispatcher.addSubscriber(queueToSubscribe, filterMeas.value(), filterType);
}

Error Sensor::subscribeToMessage(PacketQueue_Interface* queueToSubscribe, const AsciiHeader& asciiHeaderFilter,
                                 const AsciiSubscriberFilterType filterType) noexcept
{
#if THREADING_ENABLE
    LockGuard lock(_sensorMutex);
#endif
    return _asciiPacketDispatcher.addSubscriber(queueToSubscribe, asciiHeaderFilter, filterType);
}

Error Sensor::subscribeToMessage(PacketQueue_Interface* queueToSubscribe, const Fb00SubscriberFilter fb00Filter) noexcept
{
#if THREADING_ENABLE
    LockGuard lock(_sensorMutex);
#endif
    return _fbPacketDispatcher.addSubscriber(queueToSubscribe, fb00Filter);
}

void Sensor::unsubscribeFromMessage(PacketQueue_Interface* queueToUnsubscribe, const SyncByte syncByte) noexcept
{
#if THREADING_ENABLE
    LockGuard lock(_sensorMutex);
#endif
    switch (syncByte)
    {
        case (SyncByte::Ascii):
        {
            _asciiPacketDispatcher.removeSubscriber(queueToUnsubscribe);
            break;
        }
        case (SyncByte::FA):
        {
            _faPacketDispatcher.removeSubscriber(queueToUnsubscribe);
            break;
        }
        case (SyncByte::FB):
        {
            _fbPacketDispatcher.removeSubscriber(queueToUnsubscribe);
            break;
        }
        case (SyncByte::None):
        {
            _packetSynchronizer.deregisterSkippedByteQueue();
            break;
        }
        default:
            break;
    }
}

void Sensor::unsubscribeFromMessage(PacketQueue_Interface* queueToUnsubscribe, const BinaryOutputMeasurements& filter) noexcept
{
#if THREADING_ENABLE
    LockGuard lock(_sensorMutex);
#endif
    std::optional<EnabledMeasurements> filterMeas = filter.toBinaryHeader().toMeasurementHeader();
    if (!filterMeas.has_value()) { return; }
    _faPacketDispatcher.removeSubscriber(queueToUnsubscribe, filterMeas.value());
}

void Sensor::unsubscribeFromMessage(PacketQueue_Interface* queueToUnsubscribe, const AsciiHeader& filter) noexcept
{
#if THREADING_ENABLE
    LockGuard lock(_sensorMutex);
#endif
    _asciiPacketDispatcher.removeSubscriber(queueToUnsubscribe, filter);
}

// ----------------------------
// Unthreaded Packet Processing
// ----------------------------

Error Sensor::loadMainBufferFromSerial() noexcept
{
    size_t initBufferIndex = _mainByteBuffer.size();
    Error err = _serial.getData();
    if (_receivedByteBuffer) { _copyToReceivedByteBuffer(initBufferIndex); }
    return err;
}

void Sensor::_copyToReceivedByteBuffer(size_t mainBufferIndex) noexcept
{
    size_t bytesToCopy = _mainByteBuffer.size() - mainBufferIndex;
    size_t linearBytes = _receivedByteBuffer->numLinearBytesToPut();
    while (bytesToCopy > 0 && linearBytes > 0)
    {
        const size_t peekBytes = std::min(bytesToCopy, linearBytes);
        _mainByteBuffer.peek(const_cast<uint8_t*>(_receivedByteBuffer->tail()), peekBytes, mainBufferIndex);
        _receivedByteBuffer->put(peekBytes);
        bytesToCopy -= peekBytes;
        mainBufferIndex += peekBytes;
        linearBytes = _receivedByteBuffer->numLinearBytesToPut();
    }
    if (bytesToCopy > 0) { _asyncErrorQueue.put(AsyncError(Error::ReceivedByteBufferFull, now())); }
}

Error Sensor::loadMainBufferFromFile() noexcept
{
    size_t linearBytes = _mainByteBuffer.numLinearBytesToPut();
    size_t numBytes = _file.read((char*)(const_cast<uint8_t*>(_mainByteBuffer.tail())), linearBytes);
    if (numBytes == 0) { return Error::FileReadFailed; }
    _mainByteBuffer.put(numBytes);
    if (linearBytes = _mainByteBuffer.numLinearBytesToPut(); linearBytes > 0)
    {
        numBytes = _file.read((char*)(const_cast<uint8_t*>(_mainByteBuffer.tail())), linearBytes);
        if (numBytes == 0) { return Error::FileReadFailed; }
        _mainByteBuffer.put(numBytes);
    }
    return Error::None;
}

Errored Sensor::processNextPacket() noexcept { return _packetSynchronizer.dispatchNextPacket(); }

#if (THREADING_ENABLE)

void Sensor::_listen() noexcept
{
    if (_connectionType == ConnectionType::File)
    {
        _mainByteBuffer.reset();
        while (_listening)
        {
            {
                LockGuard lock(_sensorMutex);
                Error lastError = loadMainBufferFromFile();
                if (lastError != Error::None) { _asyncErrorQueue.put(AsyncError(lastError, now())); }
                bool needsMoreData = false;
                while (!needsMoreData) { needsMoreData = processNextPacket(); }
            }
            thisThread::sleepFor(Config::Sensor::listenSleepDuration);
        }
    }
    else
    {
        _mainByteBuffer.reset();
        while (_listening)
        {
            {
                LockGuard lock(_sensorMutex);
                Error lastError = loadMainBufferFromSerial();
                if (lastError != Error::None) { _asyncErrorQueue.put(AsyncError(lastError, now())); }
                bool needsMoreData = false;
                while (!needsMoreData) { needsMoreData = processNextPacket(); }
            }
            thisThread::sleepFor(Config::Sensor::listenSleepDuration);
        }
    }
}

void Sensor::_startListening() noexcept
{
    if (_listening) { return; }
    _listening = true;
    _listeningThread = std::make_unique<Thread>(&Sensor::_listen, this);

    // _listeningThread->setHighestPriority(); // ** Commented as it is compile erroring or failing in runtime.
}

void Sensor::_stopListening() noexcept
{
    if (!_listening) { return; }
    _listening = false;
    _listeningThread->join();
}
#endif  // THREADING_ENABLE

}  // namespace VN
