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

#ifndef VN_PACKET_HPP_
#define VN_PACKET_HPP_

#include <array>
#include <cstdint>

#include "AsciiPacketProtocol.hpp"
#include "FaPacketProtocol.hpp"
#include "FbPacketProtocol.hpp"
#include "PacketDispatcher.hpp"

namespace VN
{

struct PacketDetails
{
    enum class SyncByte : uint8_t
    {
        FA = 0xFA,
        Ascii = '$',
        FB = 0xFB,
        None = 0,
    } syncByte;

    union
    {
        AsciiPacketProtocol::Metadata asciiMetadata;
        FaPacketProtocol::Metadata faMetadata;
        FbPacketProtocol::Metadata fbMetadata;
        PacketMetadata<uint8_t> defaultMetadata;
    };
    PacketDetails() : syncByte(SyncByte::None), defaultMetadata() {}
};

struct Packet
{
    Packet(uint16_t capacity) : buffer(new uint8_t[capacity]), capacity(capacity) {}

    template <uint16_t Capacity>
    Packet(std::array<uint8_t, Capacity>& externalBuffer) : buffer(externalBuffer.data()), capacity(Capacity), _autoAllocated(false)
    {
    }
    ~Packet()
    {
        if (_autoAllocated) { delete[] buffer; }
    }

    Packet(const Packet&) = delete;
    Packet& operator=(const Packet&) = delete;

    Packet(Packet&& other) noexcept : details(std::move(other.details)), _autoAllocated(true)
    {
        if (other._autoAllocated) { std::swap(buffer, other.buffer); }
        else
        {
            buffer = new uint8_t[other.capacity];
            std::memcpy(buffer, other.buffer, other.capacity);
        }
        std::swap(capacity, other.capacity);
    }

    Packet& operator=(Packet&&) = delete;

    PacketDetails details{};
    uint8_t* buffer = nullptr;
    uint16_t capacity = 0;
    uint16_t length() const noexcept
    {
        switch (details.syncByte)
        {
            case PacketDetails::SyncByte::FA:
                return details.faMetadata.length;
            case PacketDetails::SyncByte::Ascii:
                return details.asciiMetadata.length;
            case PacketDetails::SyncByte::FB:
                return details.fbMetadata.length;
            case PacketDetails::SyncByte::None:
                return details.defaultMetadata.length;
            default:
                return 0;
        }
    }

    time_point timestamp() const noexcept
    {
        switch (details.syncByte)
        {
            case PacketDetails::SyncByte::FA:
                return details.faMetadata.timestamp;
            case PacketDetails::SyncByte::Ascii:
                return details.asciiMetadata.timestamp;
            case PacketDetails::SyncByte::FB:
                return details.fbMetadata.timestamp;
            case PacketDetails::SyncByte::None:
                return details.defaultMetadata.timestamp;
            default:
                return time_point();
        }
    }

private:
    const bool _autoAllocated = true;
};

}  // namespace VN

#endif  // VN_PACKET_HPP_
