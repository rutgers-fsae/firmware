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

#include "vectornav/Bootloader.hpp"

#include <algorithm>
#include <cstdint>

#include "vectornav/Config.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Interface/Commands.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/GenericCommand.hpp"
#include "vectornav/Interface/Registers.hpp"
#include "vectornav/Interface/Sensor.hpp"
#include "vectornav/TemplateLibrary/String.hpp"
#include "vectornav/TemplateLibrary/Vector.hpp"

namespace VN
{
namespace FirmwareProgrammer
{
namespace Bootloader
{
#if VN_DEBUG_LEVEL >= 0
char DEBUGprogress[100];
#define PRINT_PROGRESS(progressBar, percentComplete)                                            \
    std::snprintf(DEBUGprogress, 100, "\r[%10s] %03u%%", progressBar.c_str(), percentComplete); \
    VN_DEBUG_0(DEBUGprogress << std::flush);
#else
#define PRINT_PROGRESS(progressBar, percentComplete)
#endif

class BootloaderDispatcher : public PacketDispatcher
{
public:
    BootloaderDispatcher() : PacketDispatcher{Vector<uint8_t, SYNC_BYTE_CAPACITY>{'V'}} {};
    virtual FindPacketRetVal findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
    {
        const String<23> blMsgMatch = "VectorNav Bootloader v.";

        if (byteBuffer.size() < blMsgMatch.length())
        {
            // There's not enough data to have the full message, don't bother checking
            return {FindPacketRetVal::Validity::Incomplete, blMsgMatch.length()};
        }
        for (uint8_t i = 0; i < blMsgMatch.length(); ++i)
        {
            if (byteBuffer.peek_unchecked(syncByteIndex + i) != blMsgMatch[i]) { return {FindPacketRetVal::Validity::Invalid, 0}; }
        }
        return {FindPacketRetVal::Validity::Valid, blMsgMatch.length()};
    }

    virtual Error dispatchPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
    {
        size_t byteBufferSize = byteBuffer.size();
        const size_t blMsgLength = 34;  // Enough length for "VectorNav Bootloader v.XX.XX.XX.XX"
        size_t messageToPrintSize = std::min(byteBufferSize, blMsgLength);
        for (uint8_t i = 0; i < messageToPrintSize; ++i) { VN_DEBUG_0(byteBuffer.peek_unchecked(syncByteIndex + i)); }
        VN_DEBUG_0(std::endl);

        return Error::None;
    }
};

namespace
{

ErrorBL _sendRecord(Sensor* sensor, const AsciiMessage& currentRecord);
ErrorBL _getSensorError(const AsciiMessage& inMsg);
}  // namespace

Errored tryEnterBootloader(Sensor* sensor, const Sensor::BaudRate firmwareBaudRate, const Sensor::BaudRate bootloaderBaudRate)
{
    VN_DEBUG_0("Entering bootloader...\n");
    bool enteringFailed = false;

    GenericCommand enterBootloader("FWU", 3);
    sensor->sendCommand(&enterBootloader, Sensor::SendCommandBlockMode::BlockWithRetry, 6s);
    // No need to sleep, because worst case scenario we are sending spaces too early.
    if (autoconfigureBootloader(sensor, bootloaderBaudRate) != Error::None)
    {
        sensor->changeHostBaudRate(firmwareBaudRate);  // If we didn't make it into the bootloader, assume we're talking to the firmware
        sensor->reset();
        enteringFailed = true;
    }

    if (enteringFailed) { VN_DEBUG_0("Failed to enter bootloader.\n"); }
    else { VN_DEBUG_0("Bootloader entered.\n"); }

    return enteringFailed;
}

Error autoconfigureBootloader(Sensor* sensor, const Sensor::BaudRate bootloaderBaudRate)
{
    constexpr uint16_t bufferSize = 200;
    ByteBuffer _skippedByteBuffer{bufferSize};

#if (THREADING_ENABLE)
    PacketQueue<3> _skippedPacketQueue(PacketQueue<3>::PutMode::Retry, bufferSize);
#else
    PacketQueue<3> _skippedPacketQueue(PacketQueue<3>::PutMode::Force, bufferSize);
#endif  // THREADING_ENABLE

    BootloaderDispatcher blDispatch{};
    PacketSynchronizer packetSync{_skippedByteBuffer, nullptr, bufferSize - 10};

    packetSync.addDispatcher(&blDispatch);

    const AsciiMessage autobaudSequence = "                            ";  // Only 8 needed, but it's safe to send extra
    Error latestError = sensor->changeHostBaudRate(bootloaderBaudRate);
    if (latestError != Error::None)
    {
        VN_DEBUG_0(latestError << " encountered when attempting to change the host baud rate." << std::endl);
        return latestError;
    }

    // Over 35 trials, 8 took more than 10 retries, 1 took 34 retries
    // This will only take a max of 4 seconds
    sensor->subscribeToMessage(&_skippedPacketQueue, Sensor::SyncByte::None);
    const uint8_t numAllowedRetries = 40;
    for (int attemptNumber = 0; attemptNumber <= numAllowedRetries; ++attemptNumber)
    {
        latestError = sensor->serialSend(autobaudSequence.c_str(), autobaudSequence.length());
        if (latestError != Error::None)
        {
            VN_DEBUG_0(latestError << " encountered when configuring the bootloader on attempt " << (attemptNumber + 1) << ": " << std::endl);
            continue;
        }
        Timer timer(50ms);
        timer.start();
        while (!timer.hasTimedOut())
        {
#if (!THREADING_ENABLE)
            sensor->loadMainBufferFromSerial();
            while (!sensor->processNextPacket()) {}
#endif  // THREADING_ENABLE
            auto skipPacket = _skippedPacketQueue.get();
            if (skipPacket) { _skippedByteBuffer.put(skipPacket->buffer, skipPacket->length()); }
            if (packetSync.dispatchNextPacket()) {}
            if (packetSync.getValidPacketCount({'V'}) > 0)
            {
                sensor->unsubscribeFromMessage(&_skippedPacketQueue, Sensor::SyncByte::None);
                return Error::None;
            }
        }
        latestError = Error::ResponseTimeout;
    }
    sensor->unsubscribeFromMessage(&_skippedPacketQueue, Sensor::SyncByte::None);
    return latestError;  // Timed out
}

BootloaderReturn sendRecords(Sensor* sensor, InputFile& firmwareStream, const size_t numLinesInFirmware)
{
    AsciiMessage progressBar;
    std::fill_n(progressBar.begin(), 100, '-');
    bool retry = false;
    AsciiMessage currentLine;

    size_t lineNum = 0;
    uint8_t percentComplete = 0;

    PRINT_PROGRESS(progressBar, percentComplete);
    while (lineNum < numLinesInFirmware)
    {
        if ((lineNum * 100 / numLinesInFirmware) > percentComplete)
        {
            percentComplete = lineNum * 100 / numLinesInFirmware;
            progressBar[percentComplete - 1] = '#';
            PRINT_PROGRESS(progressBar, percentComplete);
        }

        if (!retry && (firmwareStream.getLine(currentLine.begin(), currentLine.capacity())))
        {
            VN_DEBUG_0("Failed to get line.\n");
            return BootloaderReturn{ErrorAll{Error::FileReadFailed}, BootloaderReturn::FailureMode::Abort};
        }
        ErrorBL error = _sendRecord(sensor, currentLine);
        switch (error)
        {
            case (ErrorBL::None):
            {
                lineNum++;
                retry = false;
                break;
            }
            case (ErrorBL::CommError):
            {
                // Retry the same line
                VN_DEBUG_0("Warning: " << error << " encountered while loading the firmware on line " << lineNum << ".\n");
                VN_DEBUG_0("Retrying line." << std::endl);
                retry = true;
                break;
            }
            case (ErrorBL::InvalidProgramCRC):
            case (ErrorBL::InvalidProgramSize):
            {
                VN_DEBUG_0(error << " encountered while loading the firmware on line " << lineNum << "." << std::endl);
                sensor->reset();
                return BootloaderReturn{ErrorAll{error}, BootloaderReturn::FailureMode::Abort};
            }
            case (ErrorBL::Timeout):
            {
                VN_DEBUG_0("Warning: " << error << " encountered while loading the firmware on line " << lineNum << ".\n");
                VN_DEBUG_0("Retrying line." << std::endl);
                retry = true;
                break;
            }
            case (ErrorBL::InvalidCommand):
            case (ErrorBL::InvalidRecordType):
            case (ErrorBL::InvalidByteCount):
            case (ErrorBL::InvalidHexFile):
            case (ErrorBL::DecryptionError):
            case (ErrorBL::InvalidBlockCRC):
            case (ErrorBL::InvalidMemoryAddress):
            case (ErrorBL::MaxRetryCount):
            case (ErrorBL::Reserved):
            default:
            {
                VN_DEBUG_0(error << " encountered while loading the firmware on line " << lineNum << "." << std::endl);
                return BootloaderReturn{ErrorAll{error}, BootloaderReturn::FailureMode::Abort};
            }
        }
    }
    progressBar[99] = '#';
    PRINT_PROGRESS(progressBar, 100);
    return BootloaderReturn{ErrorAll{Error::None}, BootloaderReturn::FailureMode::None};
}

Error exitBootloader(Sensor* sensor, const Sensor::BaudRate firmwareBaudRate)
{
    // Almost identical to Sensor::reset except for changeHostBaudRate and sleep
    Reset rst{};
    const Error sendCommandError = sensor->sendCommand(&rst, Sensor::SendCommandBlockMode::BlockWithRetry);
    if (sendCommandError != Error::None)
    {
        VN_DEBUG_0(sendCommandError << " encountered when resetting the sensor." << std::endl);
        return sendCommandError;
    }
    sensor->changeHostBaudRate(firmwareBaudRate);
    thisThread::sleepFor(5000ms);  // Testing for the VN-300 showed it was not responsive until 4.5s when GPS and Nav firmwares mismatched
    if (!sensor->verifySensorConnectivity())
    {
        Error latestError = sensor->autoBaud();
        if (latestError != Error::None)
        {
            VN_DEBUG_0(latestError << " encountered when resetting the sensor." << std::endl);
            return latestError;
        }
    }
    return Error::None;
}

namespace
{
ErrorBL _sendRecord(Sensor* sensor, const AsciiMessage& currentLine)
{
    const auto record = StringUtils::extractAfter(currentLine, ':');
    GenericCommand programCommand("BLD," + record, 3);  // Expecting response with "BLD"
    sensor->sendCommand(&programCommand, Sensor::SendCommandBlockMode::Block, 6s);
    ErrorBL error = _getSensorError(programCommand.getResponse());
    return error;
}

ErrorBL _getSensorError(const AsciiMessage& inMsg)
{
    if (StringUtils::startsWith(inMsg, "$VNBLD,"))
    {
        auto errorString = StringUtils::extractBetween(inMsg, ',', '*');
        auto errorType = StringUtils::fromStringHex<uint8_t>(errorString.begin(), errorString.end());
        if (errorType.has_value() && errorType.value() < static_cast<uint8_t>(ErrorBL::Reserved)) { return static_cast<ErrorBL>(errorType.value()); }
        else { return ErrorBL::Reserved; }
    }
    return ErrorBL::None;
}

}  // namespace
}  // namespace Bootloader
}  // namespace FirmwareProgrammer
}  // namespace VN
