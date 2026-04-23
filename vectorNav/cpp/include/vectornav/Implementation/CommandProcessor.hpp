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

#ifndef VN_COMMANDPROCESSOR_HPP_
#define VN_COMMANDPROCESSOR_HPP_

#include <assert.h>

#include <functional>

#include "vectornav/Config.hpp"
#include "vectornav/HAL/Mutex.hpp"
#include "vectornav/Implementation/AsciiPacketProtocol.hpp"
#include "vectornav/Implementation/CoreUtils.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/GenericCommand.hpp"
#include "vectornav/TemplateLibrary/Queue.hpp"
#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{

/// @brief The CommandProcessor class is the handshake between the user-facing code (such as Sensor) and the communication code interfacing with the sensor
/// (such as Serial and PacketProcessor). Consequently, it exists on multiple threads (all exists on the main thread except for matchResponse, which is called
/// from the high-priority thread) with an internal queue to handle the cross-thread communication. Its chief responsibility is to facilitate the communication
/// of commands between the user and the sensor, tracking received responses (sent via matchResponse) and correlating them with known-sent commands.
class CommandProcessor
{
public:
    using AsyncErrorQueuePush = std::function<void(AsyncError&&)>;

    struct RegisterCommandReturn
    {
        Error error;
        AsciiMessage message;
    };

    struct QueueItem
    {
        GenericCommand* cmd;
        Microseconds timeoutThreshold = Config::CommandProcessor::commandRemovalTimeoutLength;
    };

    CommandProcessor(AsyncErrorQueuePush asyncErrorQueuePush) : _asyncErrorQueuePush(asyncErrorQueuePush) {}

    RegisterCommandReturn registerCommand(GenericCommand* pCommand,
                                          const Microseconds timeoutThreshold = Config::CommandProcessor::commandRemovalTimeoutLength) noexcept;

    Errored matchResponse(const AsciiMessage& response, const AsciiPacketProtocol::Metadata& metadata) noexcept;
    int queueSize() const noexcept;
    int queueCapacity() const noexcept;
    void popCommandFromQueueBack() noexcept;
    std::optional<QueueItem> getFrontCommand() noexcept;

private:
    AsyncErrorQueuePush _asyncErrorQueuePush = nullptr;

    Queue<QueueItem, Config::CommandProcessor::commandProcQueueCapacity> _cmdQueue{};
    mutable Mutex _mutex;
};

}  // namespace VN

#endif  // VN_COMMANDPROCESSOR_HPP_
