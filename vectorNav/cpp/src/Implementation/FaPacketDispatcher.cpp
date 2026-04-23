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

#include "vectornav/Implementation/FaPacketDispatcher.hpp"

namespace VN
{
PacketDispatcher::FindPacketRetVal FaPacketDispatcher::findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
{
    FaPacketProtocol::FindPacketReturn findPacketRetVal = FaPacketProtocol::findPacket(byteBuffer, syncByteIndex);
    if (findPacketRetVal.validity == FaPacketProtocol::Validity::Valid) { _latestPacketMetadata = findPacketRetVal.metadata; }
    return {findPacketRetVal.validity, findPacketRetVal.metadata.length};
}

Error FaPacketDispatcher::dispatchPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
{
    VN_PROFILER_TIME_CURRENT_SCOPE();
    Error error = _invokeSubscribers(byteBuffer, syncByteIndex, _latestPacketMetadata);
    if constexpr (Config::PacketDispatchers::compositeDataQueueCapacity > 0)
    {
        std::optional<EnabledMeasurements> measFound = _latestPacketMetadata.header.toMeasurementHeader();
        if (_parseToCD && measFound.has_value() && anyDataIsEnabled(measFound.value(), _enabledMeasurements))
        {
            Error latestError = _tryPushToCompositeDataQueue(byteBuffer, syncByteIndex, _latestPacketMetadata);
            if (latestError != Error::None) { error = latestError; }
        }
    }

    return error;
}

Error FaPacketDispatcher::addSubscriber(PacketQueue_Interface* subscriber, EnabledMeasurements headerToUse, SubscriberFilterType filterType) noexcept
{
    if (subscriber == nullptr) { return Error::PacketQueueNull; }
    if (headerToUse == EnabledMeasurements{0})
    {
        // If they pass no header filter, we should match on any message
        for (auto& group : headerToUse) { group = std::numeric_limits<uint32_t>::max(); }
        filterType = SubscriberFilterType::AnyMatch;
    }
    if (_subscribers.push_back(Subscriber{subscriber, headerToUse, filterType})) { return Error::MessageSubscriberCapacityReached; }
    else { return Error::None; }
}

void FaPacketDispatcher::removeSubscriber(PacketQueue_Interface* subscriberToRemove) noexcept
{
    for (auto itr = _subscribers.begin(); itr != _subscribers.end();)
    {
        auto& subscriber = *itr;
        if (subscriberToRemove == subscriber.queueToPush) { itr = _subscribers.erase(itr); }
        else { ++itr; }
    }
}

void FaPacketDispatcher::removeSubscriber(PacketQueue_Interface* subscriberToRemove, const EnabledMeasurements& headerToUse) noexcept
{
    for (auto itr = _subscribers.begin(); itr != _subscribers.end();)
    {
        auto& subscriber = *itr;
        if (subscriberToRemove == subscriber.queueToPush && headerToUse == subscriber.headerFilter) { itr = _subscribers.erase(itr); }
        else { ++itr; }
    }
}

Error FaPacketDispatcher::_tryPushToCompositeDataQueue(const ByteBuffer& byteBuffer, const size_t syncByteIndex,
                                                       const FaPacketProtocol::Metadata& packetDetails) noexcept
{
    VN_PROFILER_TIME_CURRENT_SCOPE();
    // TODO: Currently we fail to parse if no data is enabled. This is expected but will be fixed in the future
    auto compositeData = FaPacketProtocol::parsePacket(byteBuffer, syncByteIndex, packetDetails, _enabledMeasurements);
    if (!compositeData.has_value()) { return Error::ParsingFailed; }

    // Copy to the output queue
    auto pCompositeData = _compositeDataQueue->put();
    if (!pCompositeData) { return Error::MeasurementQueueFull; }
    *pCompositeData = compositeData.value();  // Todo 477: INvestigate passing pointer into the parser, rather than returning and copying it. Will that
                                              // be more efficient than calling "reset" and assigning values?
    return Error::None;
}

Error FaPacketDispatcher::_invokeSubscribers(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const FaPacketProtocol::Metadata& packetDetails) noexcept
{
    Error error{Error::None};
    VN_PROFILER_TIME_CURRENT_SCOPE();
    const auto packetHeader = packetDetails.header.toMeasurementHeader();
    if (!packetHeader.has_value()) { return Error::ParsingFailed; }
    for (auto& subscriber : _subscribers)
    {
        const auto filterHeader = subscriber.headerFilter;
        bool pushToSub;
        switch (subscriber.filterType)
        {
            case (SubscriberFilterType::AnyMatch):
            {
                pushToSub = anyDataIsEnabled(filterHeader, packetHeader.value());
                break;
            }
            case (SubscriberFilterType::ExactMatch):
            {
                pushToSub = filterHeader == packetHeader.value();
                break;
            }
            case (SubscriberFilterType::NotExactMatch):
            {
                pushToSub = filterHeader != packetHeader.value();
                break;
            }
            default:
                pushToSub = false;
        }
        if (pushToSub)
        {
            const Error latestError = _tryPushToSubscriber(byteBuffer, syncByteIndex, packetDetails, subscriber);
            if (latestError != Error::None) { error = latestError; }
        }
    }
    return error;
}

Error FaPacketDispatcher::_tryPushToSubscriber(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const FaPacketProtocol::Metadata& metadata,
                                               Subscriber& subscriber) noexcept
{
    auto putSlot = subscriber.queueToPush->put();
    if (putSlot)
    {
        if (putSlot->capacity >= metadata.length)
        {
            putSlot->details.syncByte = PacketDetails::SyncByte::FA;
            putSlot->details.faMetadata = metadata;
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
