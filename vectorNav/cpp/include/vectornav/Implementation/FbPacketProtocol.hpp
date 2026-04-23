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

#ifndef VN_FBPACKETPROTOCOL_HPP_
#define VN_FBPACKETPROTOCOL_HPP_

#include <cstdint>

#include "vectornav/Config.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Implementation/PacketDispatcher.hpp"
#include "vectornav/TemplateLibrary/ByteBuffer.hpp"

namespace VN
{

namespace FbPacketProtocol
{
struct Header
{
    uint8_t messageType = 0;
    uint8_t messageId = 0;
    uint8_t totalPacketCount = 0;
    uint8_t currentPacketCount = 0;
    uint16_t payloadLength = 0;

    bool operator==(const Header& rhs) const
    {
        if (messageType == rhs.messageType && messageId == rhs.messageId && totalPacketCount == rhs.totalPacketCount &&
            currentPacketCount == rhs.currentPacketCount && payloadLength == rhs.payloadLength)
        {
            return true;
        }
        return false;
    }
};

using Validity = PacketDispatcher::FindPacketRetVal::Validity;
constexpr uint16_t MAX_PACKET_LENGTH = VN::Config::PacketFinders::fbPacketMaxLength;

using Metadata = PacketMetadata<Header>;

struct FindPacketReturn
{
    Validity validity;
    Metadata metadata;
};

FindPacketReturn findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept;

}  // namespace FbPacketProtocol
}  // namespace VN

#endif  // VN_FBPACKETPROTOCOL_HPP_
