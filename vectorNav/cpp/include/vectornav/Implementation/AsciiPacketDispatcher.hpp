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

#ifndef VN_ASCIIPACKETDISPATCHER_HPP_
#define VN_ASCIIPACKETDISPATCHER_HPP_

#include "vectornav/Config.hpp"
#include "vectornav/Implementation/AsciiPacketProtocol.hpp"
#include "vectornav/Implementation/CommandProcessor.hpp"
#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#include "vectornav/Implementation/PacketDispatcher.hpp"
#include "vectornav/Implementation/QueueDefinitions.hpp"

namespace VN
{
class AsciiPacketDispatcher : public PacketDispatcher
{
public:
    AsciiPacketDispatcher(MeasurementQueue* measurementQueue, EnabledMeasurements enabledMeasurements, CommandProcessor* commandProcessor,
                          bool parseToCD = true)
        : PacketDispatcher{{'$'}},
          _compositeDataQueue(measurementQueue),
          _enabledMeasurements(enabledMeasurements),
          _commandProcessor(commandProcessor),
          _parseToCD{parseToCD}
    {
    }

    PacketDispatcher::FindPacketRetVal findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept override;

    Error dispatchPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept override;

    enum class SubscriberFilterType
    {
        StartsWith,
        DoesNotStartWith
    };

    Error addSubscriber(PacketQueue_Interface* subscriber, const AsciiHeader& headerToUse, SubscriberFilterType filterType) noexcept;

    void removeSubscriber(PacketQueue_Interface* subscriberToRemove) noexcept;
    void removeSubscriber(PacketQueue_Interface* subscriberToRemove, const AsciiHeader& headerToUse) noexcept;

private:
    MeasurementQueue* _compositeDataQueue;
    [[maybe_unused]] EnabledMeasurements _enabledMeasurements;

    AsciiPacketProtocol::Metadata _latestPacketMetadata;
    CommandProcessor* _commandProcessor;
    struct Subscriber
    {
        PacketQueue_Interface* queueToPush;
        AsciiHeader headerFilter;
        SubscriberFilterType filterType;
    };

    static const auto SUBSCRIBER_CAPACITY = Config::PacketDispatchers::asciiPacketSubscriberCapacity;
    using Subscribers = Vector<Subscriber, SUBSCRIBER_CAPACITY>;
    Subscribers _subscribers;
    bool _parseToCD;

    Error _tryPushToCompositeDataQueue(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const AsciiPacketProtocol::Metadata& metadata,
                                       AsciiPacketProtocol::AsciiMeasurementHeader measEnum) noexcept;
    Error _invokeSubscribers(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const AsciiPacketProtocol::Metadata& metadata) noexcept;
    Error _tryPushToSubscriber(const ByteBuffer& byteBuffer, const size_t syncByteIndex, const AsciiPacketProtocol::Metadata& metadata,
                               Subscriber& subscriber) noexcept;
};
}  // namespace VN

#endif  // VN_ASCIIPACKETDISPATCHER_HPP_
