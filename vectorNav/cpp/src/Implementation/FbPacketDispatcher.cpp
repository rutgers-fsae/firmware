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

#include "vectornav/Implementation/FbPacketDispatcher.hpp"

#include "vectornav/Implementation/CoreUtils.hpp"
namespace VN
{
PacketDispatcher::FindPacketRetVal FbPacketDispatcher::findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
{
    FbPacketProtocol::FindPacketReturn findPacketRetVal = FbPacketProtocol::findPacket(byteBuffer, syncByteIndex);
    if (findPacketRetVal.validity == FbPacketProtocol::Validity::Valid) { _latestPacketMetadata = findPacketRetVal.metadata; }
    return {findPacketRetVal.validity, findPacketRetVal.metadata.length};
}

Error FbPacketDispatcher::dispatchPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
{
    // We must assume that _latestPacketMetadata is correctly set.
    for (auto& subscriber : _subscribers)
    {
        if (subscriber.queueToPush && subscriber.filter.packet)
        {
            auto putSlot = subscriber.queueToPush->put();
            if (putSlot)
            {
                if (putSlot->capacity >= _latestPacketMetadata.length)
                {
                    putSlot->details.syncByte = PacketDetails::SyncByte::FB;
                    putSlot->details.fbMetadata = _latestPacketMetadata;
                    byteBuffer.peek_unchecked(putSlot->buffer, _latestPacketMetadata.length, syncByteIndex);
                }
                else
                {
                    putSlot->details = PacketDetails();
                    // TODO: Error::PacketQueueOverrun
                }
            }
            else
            {
                // TODO: Error::PacketQueueFull
            }
        }
    }

    Error error{Error::None};
    const bool packetIsInvalid = (_latestPacketMetadata.header.currentPacketCount == 1)
                                     ? false
                                     : (_latestPacketMetadata.header.messageId != _previousPacketMetadata.header.messageId) ||
                                           ((_latestPacketMetadata.header.messageId == _previousPacketMetadata.header.messageId) &&
                                            (_latestPacketMetadata.header.currentPacketCount != (_previousPacketMetadata.header.currentPacketCount + 1)));
    if (packetIsInvalid)
    {
        // Unless this is the first packet of a message, the packet is invalid if it's either:
        //   1. Isn't the same messageId as the previous
        //   2. The same message as previous, but not next in order (nor the first of a new)
        // We should wipe the copied buffer and move on.
        _fbByteBuffer.reset();
        return Error::ReceivedUnexpectedMessage;
    }

    if (_latestPacketMetadata.header.currentPacketCount == 1) { _resetFbBuffer(); }

    error = _moveBytesFromMainBufferToFbBuffer(_latestPacketMetadata.header, byteBuffer, _latestPacketMetadata.header.payloadLength,
                                               syncByteIndex + 1 + 5);  // Add after FB header
    if (error != Error::None) { return error; }

    const bool packetIsFinalOfMessage = _latestPacketMetadata.header.currentPacketCount == _latestPacketMetadata.header.totalPacketCount;
    if (packetIsFinalOfMessage)
    {
        _addFaPacketCrc();

        const auto retVal = _faPacketDispatcher->findPacket(_fbByteBuffer, 0);

        if (retVal.validity == PacketDispatcher::FindPacketRetVal::Validity::Valid)
        {
            const auto duplicateRetVal = FaPacketProtocol::findPacket(_fbByteBuffer, 0);
            if (duplicateRetVal.validity == FaPacketProtocol::Validity::Valid)  // should always pass
            {
                for (auto& subscriber : _subscribers)
                {
                    if (subscriber.queueToPush && subscriber.filter.completedFaMessage)
                    {
                        auto putSlot = subscriber.queueToPush->put();
                        if (putSlot)
                        {
                            putSlot->details.syncByte = PacketDetails::SyncByte::FA;
                            putSlot->details.faMetadata = duplicateRetVal.metadata;
                            _fbByteBuffer.peek_unchecked(putSlot->buffer, putSlot->details.faMetadata.length);
                        }
                        else {}  // Todo**Raise exception of buffer overflow
                    }
                }
                _faPacketDispatcher->dispatchPacket(_fbByteBuffer, 0);
            }
        }
        _fbByteBuffer.reset();
    }
    _previousPacketMetadata = _latestPacketMetadata;

    return error;
}

Error FbPacketDispatcher::_moveBytesFromMainBufferToFbBuffer(FbPacketProtocol::Header splitPacketDetails, const ByteBuffer& byteBuffer,
                                                             const uint16_t numOfBytesToMove, const size_t startingIndex) noexcept
{
    if (splitPacketDetails.payloadLength > byteBuffer.size()) { return Error::ReceivedByteBufferFull; }
    for (uint16_t i = 0; i < numOfBytesToMove; i++)
    {
        auto tmpByte = byteBuffer.peek_unchecked(startingIndex + i);
        _fbByteBuffer.put(&tmpByte, 1);
    }
    return Error::None;
}

void FbPacketDispatcher::_resetFbBuffer() noexcept
{
    _fbByteBuffer.reset();
    const uint8_t faSyncByte = 0xFA;
    _fbByteBuffer.put(&faSyncByte, 1);
}

void FbPacketDispatcher::_addFaPacketCrc() noexcept
{
    uint16_t crc = 0;

    // Calculate a CRC over everything but the sync byte
    for (size_t i = 1; i < _fbByteBuffer.size(); ++i) { _calculateCRC(&crc, (*_fbByteBuffer.peek(i))); }

    // Crc is put in big endian
    uint8_t data = 0;
    data = static_cast<uint8_t>(crc >> 8);
    _fbByteBuffer.put(&data, 1);  // CRC
    data = static_cast<uint8_t>(crc & 0xFF);
    _fbByteBuffer.put(&data, 1);  // CRC
}

}  // namespace VN
