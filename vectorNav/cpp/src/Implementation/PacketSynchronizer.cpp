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

#include "vectornav/Implementation/PacketSynchronizer.hpp"

#include <algorithm>

#include "vectornav/Debug.hpp"
#include "vectornav/HAL/Timer.hpp"

namespace VN
{

Errored PacketSynchronizer::addDispatcher(PacketDispatcher* packetParser) noexcept
{
    return _dispatchers.push_back({packetParser, packetParser->getSyncBytes(), PacketDispatcher::FindPacketRetVal()});
}

Errored PacketSynchronizer::dispatchNextPacket() noexcept
{
    Error latestError{Error::None};
    bool needMoreData = true;
    size_t byteBufferSize = _primaryByteBuffer.size();
    if (byteBufferSize == 0 || ((_prevValidity == PacketDispatcher::FindPacketRetVal::Validity::Incomplete) && (byteBufferSize < _prevBytesRequested)))
    {
        // Early return if there's no new data
        return needMoreData;
    }
    _prevByteBufferSize = byteBufferSize;
    VN_PROFILER_TIME_CURRENT_SCOPE();
    for (size_t fromHeadIndex = 0; fromHeadIndex < byteBufferSize; ++fromHeadIndex)
    {
        for (const auto& currentDispatcher : this->_dispatchers)
        {
            // TODO 133: Modify to handle multi-size sync bytes
            if (currentDispatcher.syncBytes.front() == _primaryByteBuffer.peek_unchecked(fromHeadIndex))
            {
                auto retVal = currentDispatcher.packetDispatcher->findPacket(_primaryByteBuffer, fromHeadIndex);
                switch (retVal.validity)
                {
                    case (PacketDispatcher::FindPacketRetVal::Validity::Valid):
                    {
                        needMoreData = false;
                        ++currentDispatcher.numValidPackets;
                        VN_DEBUG_2("Packet found: " + std::to_string(currentDispatcher.syncBytes.front()) + " length: " + std::to_string(retVal.length));
                        latestError = currentDispatcher.packetDispatcher->dispatchPacket(_primaryByteBuffer, fromHeadIndex);
                        if (latestError != Error::None && _asyncErrorQueuePush)
                        {
                            _asyncErrorQueuePush(AsyncError{latestError, errorCodeToString(latestError), now()});
                        }

                        // Require that at least the sync bytes are discarded, to prevent locking due to a bad dispatcher
                        size_t numPacketBytesToDiscard = std::max(currentDispatcher.syncBytes.size(), retVal.length);
                        latestError = _copyToSkippedByteQueueIfEnabled(fromHeadIndex);
                        if (latestError != Error::None && _asyncErrorQueuePush)
                        {
                            _asyncErrorQueuePush(AsyncError{latestError, errorCodeToString(latestError), now()});
                        }
                        _receivedByteCount += (fromHeadIndex + numPacketBytesToDiscard);
                        _primaryByteBuffer.discard(fromHeadIndex + numPacketBytesToDiscard);

                        // We are returning so that we can pull data off the serial queue after each packet. This way we don't blow through the whole buffer
                        // at a time while the serial queue overflows.
                        _prevValidity = PacketDispatcher::FindPacketRetVal::Validity::Valid;
                        _prevByteBufferSize -= fromHeadIndex + numPacketBytesToDiscard;
                        return needMoreData;
                    }
                    case (PacketDispatcher::FindPacketRetVal::Validity::Invalid):
                    {
                        // Keep searching, might have just been a random sync byte.
                        ++currentDispatcher.numInvalidPackets;
                        continue;
                    }
                    case (PacketDispatcher::FindPacketRetVal::Validity::Incomplete):
                    {
                        // Let's trust that this is probably a packet of this type, so we'll wait for more data and start searching again.
                        // We might as well discard all of the bytes so far, because clearly no one wanted it.
                        VN_DEBUG_2("Found possible packet: " + std::to_string(currentDispatcher.syncBytes.front()) +
                                   " bytes available: " + std::to_string(_primaryByteBuffer.size()));

                        // If dispatcher reports "incomplete" despite number of bytes exceeding global packet max length, then it is being too greedy and we
                        // should continue and let the other dispatchers search for packets.
                        if ((byteBufferSize - fromHeadIndex) > _packetMaxLength) { continue; }

                        latestError = _copyToSkippedByteQueueIfEnabled(fromHeadIndex);
                        if (latestError != Error::None && _asyncErrorQueuePush)
                        {
                            _asyncErrorQueuePush(AsyncError{latestError, errorCodeToString(latestError), now()});
                        }
                        _receivedByteCount += fromHeadIndex;
                        _primaryByteBuffer.discard(fromHeadIndex);
                        _prevByteBufferSize -= fromHeadIndex;
                        _prevValidity = PacketDispatcher::FindPacketRetVal::Validity::Incomplete;
                        _prevBytesRequested = retVal.length;
                        return needMoreData;
                    }
                    default:
                    {
                        // Should never happen.  Treat it as Invalid if it does.
                        VN_DEBUG_1("Unknown packet validity.");
                        ++currentDispatcher.numInvalidPackets;
                        continue;
                    }
                }
            }
        }
    }
    // At this point, we can flush the buffer, because no one is interested in any of the data.
    latestError = _copyToSkippedByteQueueIfEnabled(byteBufferSize);
    if (latestError != Error::None && _asyncErrorQueuePush) { _asyncErrorQueuePush(AsyncError{latestError, errorCodeToString(latestError), now()}); }
    _receivedByteCount += byteBufferSize;
    _primaryByteBuffer.discard(byteBufferSize);
    _prevByteBufferSize -= byteBufferSize;
    _prevValidity = PacketDispatcher::FindPacketRetVal::Validity::Invalid;
    return needMoreData;
}

size_t PacketSynchronizer::getValidPacketCount(const SyncBytes& syncBytes) const noexcept
{
    for (auto dispatcher : _dispatchers)
    {
        if (syncBytes == dispatcher.syncBytes) { return dispatcher.numValidPackets; }
    }
    return 0;
}

size_t PacketSynchronizer::getInvalidPacketCount(const SyncBytes& syncBytes) const noexcept
{
    for (auto dispatcher : _dispatchers)
    {
        if (syncBytes == dispatcher.syncBytes) { return dispatcher.numInvalidPackets; }
    }
    return 0;
}

Error PacketSynchronizer::_copyToSkippedByteQueueIfEnabled(const size_t numBytesToCopy) const noexcept
{
    if (numBytesToCopy == 0) { return Error::None; }
    _skippedByteCount += numBytesToCopy;
    VN_DEBUG_2("Discovered skipped bytes: " + std::to_string(numBytesToCopy));
    if (_pSkippedByteQueue)
    {
        uint16_t bytesRemaining = numBytesToCopy;
        while (bytesRemaining > 0)
        {
            auto putSlot = _pSkippedByteQueue->put();
            if (putSlot)
            {
                putSlot->details.syncByte = PacketDetails::SyncByte::None;
                putSlot->details.defaultMetadata.length = numBytesToCopy;
                putSlot->details.defaultMetadata.timestamp = now();
                size_t putBytes = std::min(numBytesToCopy, static_cast<size_t>(putSlot->capacity));
                _primaryByteBuffer.peek_unchecked(putSlot->buffer, putBytes, 0);
                putSlot->details.defaultMetadata.header = putSlot->buffer[0];
                bytesRemaining -= putBytes;
            }
            else { return Error::PacketQueueFull; }
        }
    }
    return Error::None;
}

}  // namespace VN
