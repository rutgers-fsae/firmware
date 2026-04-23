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

#ifndef VN_FAPACKETDISPATCHER_HPP_
#define VN_FAPACKETDISPATCHER_HPP_

#include "vectornav/Config.hpp"
#include "vectornav/Implementation/BinaryHeader.hpp"
#include "vectornav/Implementation/FaPacketProtocol.hpp"
#include "vectornav/Implementation/PacketDispatcher.hpp"
#include "vectornav/Implementation/QueueDefinitions.hpp"
#include "vectornav/TemplateLibrary/ByteBuffer.hpp"
#include "vectornav/TemplateLibrary/Vector.hpp"

namespace VN
{

class FaPacketDispatcher : public PacketDispatcher
{
public:
    FaPacketDispatcher(MeasurementQueue* measurementQueue, EnabledMeasurements enabledMeasurements, bool parseToCD = true)
        : PacketDispatcher({0xFA}), _compositeDataQueue(measurementQueue), _enabledMeasurements(enabledMeasurements), _parseToCD{parseToCD}
    {
    }

    PacketDispatcher::FindPacketRetVal findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept override;

    FaPacketProtocol::Metadata getLatestPacketMetadata() const noexcept { return _latestPacketMetadata; }

    Error dispatchPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept override;

    enum class SubscriberFilterType
    {
        ExactMatch,
        AnyMatch,
        NotExactMatch
    };

    Error addSubscriber(PacketQueue_Interface* subscriber, EnabledMeasurements headerToUse, SubscriberFilterType filterType) noexcept;

    void removeSubscriber(PacketQueue_Interface* subscriberToRemove) noexcept;
    void removeSubscriber(PacketQueue_Interface* subscriberToRemove, const EnabledMeasurements& headerToUse) noexcept;

protected:
    struct Subscriber
    {
        PacketQueue_Interface* queueToPush;
        EnabledMeasurements headerFilter;
        SubscriberFilterType filterType;
    };

    static const auto SUBSCRIBER_CAPACITY = Config::PacketDispatchers::faPacketSubscriberCapacity;
    using Subscribers = Vector<Subscriber, SUBSCRIBER_CAPACITY>;
    Subscribers _subscribers;

    MeasurementQueue* _compositeDataQueue;
    EnabledMeasurements _enabledMeasurements;
    FaPacketProtocol::Metadata _latestPacketMetadata;
    bool _parseToCD;

    Error _tryPushToCompositeDataQueue(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const FaPacketProtocol::Metadata& packetDetails) noexcept;
    Error _invokeSubscribers(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const FaPacketProtocol::Metadata& packetDetails) noexcept;
    Error _tryPushToSubscriber(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const FaPacketProtocol::Metadata& metadata,
                               Subscriber& subscriber) noexcept;
};

}  // namespace VN

#endif  // VN_FAPACKETDISPATCHER_HPP_
