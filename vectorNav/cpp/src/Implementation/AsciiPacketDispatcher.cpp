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

#include "vectornav/Implementation/AsciiPacketDispatcher.hpp"

#include "vectornav/Implementation/AsciiPacketProtocol.hpp"
#include "vectornav/Interface/CompositeData.hpp"

namespace VN
{
PacketDispatcher::FindPacketRetVal AsciiPacketDispatcher::findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
{
    const AsciiPacketProtocol::FindPacketReturn findPacketRetVal = AsciiPacketProtocol::findPacket(byteBuffer, syncByteIndex);
    if (findPacketRetVal.validity == AsciiPacketProtocol::Validity::Valid) { _latestPacketMetadata = findPacketRetVal.metadata; }
    return {findPacketRetVal.validity, findPacketRetVal.metadata.length};
}

Error AsciiPacketDispatcher::dispatchPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
{
    VN_PROFILER_TIME_CURRENT_SCOPE();
    Error error{Error::None};
    if (StringUtils::startsWith(_latestPacketMetadata.header, "VN"))
    {
        AsciiPacketProtocol::AsciiMeasurementHeader asciiHeader = AsciiPacketProtocol::getMeasHeader(_latestPacketMetadata.header);
        if (asciiHeader != AsciiPacketProtocol::AsciiMeasurementHeader::None)
        {
            error = _invokeSubscribers(byteBuffer, syncByteIndex, _latestPacketMetadata);
            if constexpr (Config::PacketDispatchers::compositeDataQueueCapacity > 0)
            {
                if (_parseToCD && AsciiPacketProtocol::asciiIsParsable(asciiHeader))
                {
                    Error latestError = _tryPushToCompositeDataQueue(byteBuffer, syncByteIndex, _latestPacketMetadata, asciiHeader);
                    if (latestError != Error::None) { error = latestError; }
                }
            }
        }
        else
        {  // Data is not a measurement. This includes vnerrs.
            VN_DEBUG_1("Passing command response.");
            AsciiMessage packet{};
            for (uint16_t idx = 0; idx < _latestPacketMetadata.length; idx++) { packet.push_back(byteBuffer.peek_unchecked(syncByteIndex + idx)); }
            _commandProcessor->matchResponse(packet, _latestPacketMetadata);
        }
    }
    else
    {  // Data does not begin with "VN"
        error = _invokeSubscribers(byteBuffer, syncByteIndex, _latestPacketMetadata);
    }

    return error;
}

Error AsciiPacketDispatcher::addSubscriber(PacketQueue_Interface* subscriber, const AsciiHeader& headerToUse, SubscriberFilterType filterType) noexcept
{
    if (subscriber == nullptr) { return Error::PacketQueueNull; }
    if (headerToUse.empty()) { filterType = SubscriberFilterType::StartsWith; }
    if (_subscribers.push_back(Subscriber{subscriber, headerToUse, filterType})) { return Error::MessageSubscriberCapacityReached; }
    else { return Error::None; }
}

void AsciiPacketDispatcher::removeSubscriber(PacketQueue_Interface* subscriberToRemove) noexcept
{
    for (auto itr = _subscribers.begin(); itr != _subscribers.end();)
    {
        if (subscriberToRemove == itr->queueToPush) { itr = _subscribers.erase(itr); }
        else { ++itr; }
    }
}

void AsciiPacketDispatcher::removeSubscriber(PacketQueue_Interface* subscriberToRemove, const AsciiHeader& headerToUse) noexcept
{
    for (auto itr = _subscribers.begin(); itr != _subscribers.end();)
    {
        if (subscriberToRemove == itr->queueToPush && headerToUse == itr->headerFilter) { itr = _subscribers.erase(itr); }
        else { ++itr; }
    }
}

Error AsciiPacketDispatcher::_tryPushToCompositeDataQueue(const ByteBuffer& byteBuffer, const size_t syncByteIndex,
                                                          const AsciiPacketProtocol::Metadata& metadata,
                                                          AsciiPacketProtocol::AsciiMeasurementHeader measEnum) noexcept
{
    // if (!AsciiPacketProtocol::anyDataIsEnabled(metadata.header, _enabledMeasurements)) { return false; }
    auto compositeData = AsciiPacketProtocol::parsePacket(byteBuffer, syncByteIndex, metadata, measEnum);
    if (!compositeData.has_value()) { return Error::ParsingFailed; }

    // Copy to the output queue
    auto pCompositeData = _compositeDataQueue->put();
    if (!pCompositeData) { return Error::MeasurementQueueFull; }
    *pCompositeData = compositeData.value();  // Todo 477: INvestigate passing pointer into the parser, rather than returning and copying it. Will that
                                              // be more efficient than calling "reset" and assigning values?
    return Error::None;
}

Error AsciiPacketDispatcher::_invokeSubscribers(const ByteBuffer& byteBuffer, const size_t syncByteIndex,
                                                const AsciiPacketProtocol::Metadata& metadata) noexcept
{
    Error error{Error::None};
    for (auto& subscriber : _subscribers)
    {
        if (StringUtils::startsWith(metadata.header, subscriber.headerFilter))
        {
            if (subscriber.filterType == SubscriberFilterType::StartsWith)
            {
                const Error latestError = _tryPushToSubscriber(byteBuffer, syncByteIndex, metadata, subscriber);
                if (latestError != Error::None) { error = latestError; }
            }
        }
        else
        {
            if (subscriber.filterType == SubscriberFilterType::DoesNotStartWith)
            {
                const Error latestError = _tryPushToSubscriber(byteBuffer, syncByteIndex, metadata, subscriber);
                if (latestError != Error::None) { error = latestError; }
            }
        }
    }

    return error;
}

Error AsciiPacketDispatcher::_tryPushToSubscriber(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const AsciiPacketProtocol::Metadata& metadata,
                                                  Subscriber& subscriber) noexcept
{
    auto putSlot = subscriber.queueToPush->put();
    if (putSlot)
    {
        if (putSlot->capacity >= metadata.length)
        {
            putSlot->details.syncByte = PacketDetails::SyncByte::Ascii;
            putSlot->details.asciiMetadata = metadata;
            byteBuffer.peek_unchecked(putSlot->buffer, metadata.length, syncByteIndex);
        }
        else
        {
            putSlot->details = PacketDetails();
            return Error::PacketQueueOverrun;
        }
    }
    else { return Error::PacketQueueFull; }
    return Error::None;
}

}  // namespace VN
