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

#ifndef VN_PACKETDISPATCHER_HPP_
#define VN_PACKETDISPATCHER_HPP_

#include <cstdint>

#include "vectornav/Config.hpp"
#include "vectornav/Interface/Errors.hpp"
#include "vectornav/TemplateLibrary/ByteBuffer.hpp"
#include "vectornav/TemplateLibrary/Vector.hpp"

namespace VN
{

template <typename T>
struct PacketMetadata
{
    T header;
    uint16_t length = 0;
    time_point timestamp;
    bool operator==(const PacketMetadata<T>& other) const noexcept { return header == other.header && length == other.length; }
};

constexpr uint8_t SYNC_BYTE_CAPACITY = 1;

class PacketDispatcher
{
public:
    PacketDispatcher(Vector<uint8_t, SYNC_BYTE_CAPACITY> syncBytes) : _syncBytes(syncBytes) {};

    Vector<uint8_t, SYNC_BYTE_CAPACITY> getSyncBytes() noexcept { return _syncBytes; }

    struct FindPacketRetVal
    {
        enum class Validity : uint8_t
        {
            Valid,
            Invalid,
            Incomplete
        } validity;
        size_t length;
    };

    virtual FindPacketRetVal findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept = 0;

    virtual Error dispatchPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept = 0;

private:
    Vector<uint8_t, SYNC_BYTE_CAPACITY> _syncBytes{};
};
}  // namespace VN

#endif  // VN_PACKETDISPATCHER_HPP_
