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

#include "vectornav/Implementation/CommandProcessor.hpp"

#include <cstdint>
#include <cstring>
#include <optional>

#include "vectornav/Debug.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Implementation/CoreUtils.hpp"
#include "vectornav/Interface/GenericCommand.hpp"
#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{

CommandProcessor::RegisterCommandReturn CommandProcessor::registerCommand(GenericCommand* pCommand, const Microseconds timeoutThreshold) noexcept
{  // Should only be called on main thread
    if (pCommand->isAwaitingResponse()) { return RegisterCommandReturn{Error::CommandResent, AsciiMessage{}}; }

    {
        LockGuard guard{_mutex};
        auto curr_time = now();
        while (!_cmdQueue.isEmpty())
        {
            const auto item = _cmdQueue.peek();
            VN_ASSERT(item.has_value());  // while loop confirms value exists
            if ((curr_time - item.value().cmd->getSentTime()) > item.value().timeoutThreshold) { _cmdQueue.get().value().cmd->setStale(); }
            else { break; }
        }

        if (_cmdQueue.isFull()) { return RegisterCommandReturn{Error::CommandQueueFull, AsciiMessage{}}; }
    }

    pCommand->prepareToSend();
    AsciiMessage messageToSend;
    if (frameVnAsciiString(pCommand->getCommandString().c_str(), messageToSend.data(), messageToSend.capacity()))
    {
        // Todo: Handle overflow and alert user
    };

    VN_DEBUG_1("TX: " + messageToSend);

    {
        LockGuard guard{_mutex};
        _cmdQueue.put({pCommand, timeoutThreshold});
    }
    return RegisterCommandReturn{Error::None, messageToSend};
}

Errored CommandProcessor::matchResponse(const AsciiMessage& response, const AsciiPacketProtocol::Metadata& metadata) noexcept
{  // Should be called on high-priority thread
    LockGuard guard{_mutex};
    while (!_cmdQueue.isEmpty())
    {
        const auto item = _cmdQueue.peek();
        VN_ASSERT(item.has_value());  // while loop confirms value exists
        if ((metadata.timestamp - item.value().cmd->getSentTime()) > item.value().timeoutThreshold) { _cmdQueue.get().value().cmd->setStale(); }
        else { break; }
    }

    bool responseHasBeenMatched = false;
    VN_DEBUG_1("RX: " + response + "\t queue size: " + std::to_string(_cmdQueue.size()));
    if (StringUtils::startsWith(response, AsciiMessage("$VNERR,")))
    {
        if (GenericCommand::isMatchingError(response))
        {
            auto frontCommand = _cmdQueue.get();
            if (!frontCommand.has_value() || !frontCommand.value().cmd->isMatchingResponse(response, metadata.timestamp))
            {
                _asyncErrorQueuePush(AsyncError(Error::ReceivedUnexpectedMessage, response, now()));
            }
        }
        else
        {
            // Is an async error or is ill-formed
            AsciiMessage errorNumString = StringUtils::extractBetween(response, ',', '*');
            const auto errorNum = StringUtils::fromStringHex<uint8_t>(errorNumString.begin(), errorNumString.end());
            if (errorNum.has_value()) { _asyncErrorQueuePush(AsyncError(static_cast<Error>(errorNum.value()), response, now())); }
            else { _asyncErrorQueuePush(AsyncError(Error::ReceivedUnexpectedMessage, response, now())); }
        }
        return true;
    }
    else
    {
        while (!responseHasBeenMatched && !_cmdQueue.isEmpty())
        {
            bool validResponse = false;
            auto frontCommand = _cmdQueue.get();
            VN_ASSERT(frontCommand.has_value());  // The while loop validates that the command queue is not empty
            validResponse = (*frontCommand).cmd->isMatchingResponse(response, metadata.timestamp);
            if (validResponse)
            {
                responseHasBeenMatched = true;
                VN_DEBUG_1("response matched.");
            }  // We don't need an else. Caller should be monitoring cmd object, and we want to limit errors thrown on high-priority thread.
            else { VN_DEBUG_1("response not matched."); }
        }
    }
    if (!responseHasBeenMatched)
    {
        _asyncErrorQueuePush(AsyncError(Error::ReceivedUnexpectedMessage, response, now()));
        return true;
    }
    return false;
}

int CommandProcessor::queueSize() const noexcept
{
    LockGuard guard{_mutex};
    return _cmdQueue.size();
}

void CommandProcessor::popCommandFromQueueBack() noexcept
{
    LockGuard guard{_mutex};
    _cmdQueue.popBack();
}

std::optional<CommandProcessor::QueueItem> CommandProcessor::getFrontCommand() noexcept
{
    LockGuard guard{_mutex};
    return _cmdQueue.get();
}

}  // namespace VN
