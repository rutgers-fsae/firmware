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

#ifndef VN_FAPACKETPROTOCOL_HPP_
#define VN_FAPACKETPROTOCOL_HPP_

#include <optional>

#include "vectornav/Config.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Implementation/BinaryHeader.hpp"
#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#include "vectornav/Implementation/PacketDispatcher.hpp"
#include "vectornav/Interface/CompositeData.hpp"
#include "vectornav/TemplateLibrary/ByteBuffer.hpp"

namespace VN
{
namespace FaPacketProtocol
{

using Validity = PacketDispatcher::FindPacketRetVal::Validity;

using Metadata = PacketMetadata<BinaryHeader>;

struct FindPacketReturn
{
    Validity validity;
    Metadata metadata;
};

FindPacketReturn findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept;

std::optional<CompositeData> parsePacket(const ByteBuffer& buffer, const size_t syncByteIndex, const Metadata& metadata,
                                         const EnabledMeasurements& measurementsToParse) noexcept;

}  // namespace FaPacketProtocol

class FaPacketExtractor
{
public:
    // Passed buffer must contain bytes enough for metadata.length
    FaPacketExtractor(const ByteBuffer& buffer, const FaPacketProtocol::Metadata& metadata, uint16_t offset = 0) : _buffer(buffer, offset), _metadata(metadata)
    {
    }

    FaPacketExtractor(uint8_t* buffer, const FaPacketProtocol::Metadata& metadata) : _buffer(buffer, metadata.length, metadata.length), _metadata(metadata) {}

    template <class T>
    T extract_unchecked() noexcept
    {
        uint16_t tmp = _index;
        _index += static_cast<uint16_t>(sizeof(T));
        return *reinterpret_cast<const T*>(_buffer.peek_ptr_unchecked(tmp));
    }

    template <class T>
    Errored extract(std::optional<T>& value) noexcept;

    template <uint16_t N, uint16_t M, class T>
    Errored extract(std::optional<Matrix<N, M, T>>& value) noexcept;

    Errored discard(const uint16_t numDiscard) noexcept
    {
        if ((_index + numDiscard) > _metadata.length) { return true; }
        _index += numDiscard;
        return false;
    }

    Errored discard(const uint8_t group, const uint8_t field) noexcept
    {
        uint16_t numDiscard = 0;
        if ((group == 3 || group == 6 || group == 12) && (field == 14 || field == 16))
        {
            if (field == 14)
            {
                const uint8_t numSats = _buffer.peek_unchecked(_index);
                numDiscard = 2 + 8 * numSats;
            }
            else
            {
                const uint8_t numMeas = _buffer.peek_unchecked(_index + 10);
                numDiscard = 12 + 28 * numMeas;
            }
        }
        else
        {
            const auto size = getStaticBinaryTypeSize(group, field);
            if (!size.has_value()) { return true; }
            numDiscard = size.value();
        }
        return discard(numDiscard);
    }

    BinaryHeader header() const noexcept { return _metadata.header; };
    uint16_t length() const noexcept { return _metadata.length; };

    uint16_t index() const { return _index; }

private:
    const ByteBuffer _buffer;
    const FaPacketProtocol::Metadata& _metadata;
    uint16_t _index = 0;
};

template <class T>
inline Errored FaPacketExtractor::extract(std::optional<T>& value) noexcept
{
    T tmp;
    if (_buffer.peek(reinterpret_cast<uint8_t*>(&tmp), sizeof(T), _index)) { return true; }
    value = tmp;
    _index += static_cast<uint16_t>(sizeof(T));
    return false;
}

template <>
inline Errored FaPacketExtractor::extract(std::optional<Ypr>& value) noexcept
{
    Ypr ypr;
    if (_buffer.peek(reinterpret_cast<uint8_t*>(&ypr), sizeof(ypr), _index)) { return true; }
    value = ypr;
    _index += static_cast<uint16_t>(sizeof(ypr));
    return false;
}

template <>
inline Errored FaPacketExtractor::extract(std::optional<DeltaTheta>& value) noexcept
{
    DeltaTheta deltaTheta;
    if (_buffer.peek(reinterpret_cast<uint8_t*>(&deltaTheta), sizeof(deltaTheta), _index)) { return true; };
    value = deltaTheta;
    _index += static_cast<uint16_t>(sizeof(deltaTheta));
    return false;
}

template <>
inline Errored FaPacketExtractor::extract(std::optional<Lla>& value) noexcept
{
    Lla lla;
    if (_buffer.peek(reinterpret_cast<uint8_t*>(&lla), sizeof(lla), _index)) { return true; };
    value = lla;
    _index += static_cast<uint16_t>(sizeof(lla));
    return false;
}

template <uint16_t N, uint16_t M, class T>
inline Errored FaPacketExtractor::extract(std::optional<Matrix<N, M, T>>& value) noexcept
{
    Matrix<N, M, T> mat;
    if (_buffer.peek(reinterpret_cast<uint8_t*>(&mat), sizeof(T) * M * N, _index)) { return true; };
    value = mat;
    _index += static_cast<uint16_t>(sizeof(T)) * M * N;
    return false;
}

template <>
inline Errored FaPacketExtractor::extract(std::optional<GnssSatInfo>& value) noexcept
{
    GnssSatInfo satInfo;

    _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&satInfo.numSats), sizeof(satInfo.numSats), _index++);
    // Check length against resv byte, sat info & CRC bytes
    if (_index + 1 + 8 * satInfo.numSats + 2 > _metadata.length) { return true; }

    satInfo.resv = _buffer.peek_unchecked(_index++);
    for (uint16_t i = 0; i < satInfo.numSats; ++i)
    {
        satInfo.sys[i] = _buffer.peek_unchecked(_index++);
        satInfo.svId[i] = _buffer.peek_unchecked(_index++);
        satInfo.flags[i] = _buffer.peek_unchecked(_index++);
        satInfo.cno[i] = _buffer.peek_unchecked(_index++);
        satInfo.qi[i] = _buffer.peek_unchecked(_index++);
        satInfo.el[i] = static_cast<int8_t>(_buffer.peek_unchecked(_index++));
        _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&satInfo.az[i]), 2, _index);
        _index += 2;
    }

    value = satInfo;
    return false;
}

template <>
inline Errored FaPacketExtractor::extract(std::optional<GnssRawMeas>& value) noexcept
{
    GnssRawMeas rawMeas;

    _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&rawMeas.tow), sizeof(rawMeas.tow), _index);
    _index += static_cast<uint16_t>(sizeof(rawMeas.tow));
    _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&rawMeas.week), sizeof(rawMeas.week), _index);
    _index += static_cast<uint16_t>(sizeof(rawMeas.week));
    _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&rawMeas.numMeas), sizeof(rawMeas.numMeas), _index++);
    // Check length against resv byte, sat info & CRC bytes
    if (_index + 1 + 28 * rawMeas.numMeas + 2 > _metadata.length) { return true; }

    rawMeas.resv = _buffer.peek_unchecked(_index++);
    for (uint16_t i = 0; i < rawMeas.numMeas; ++i)
    {
        rawMeas.sys[i] = _buffer.peek_unchecked(_index++);
        rawMeas.svId[i] = _buffer.peek_unchecked(_index++);
        rawMeas.band[i] = _buffer.peek_unchecked(_index++);
        rawMeas.chan[i] = _buffer.peek_unchecked(_index++);
        rawMeas.freqNum[i] = _buffer.peek_unchecked(_index++);
        rawMeas.cno[i] = static_cast<int8_t>(_buffer.peek_unchecked(_index++));
        _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&rawMeas.flags[i]), sizeof(uint16_t), _index);
        _index += static_cast<uint16_t>(sizeof(uint16_t));
        _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&rawMeas.pr[i]), sizeof(double), _index);
        _index += static_cast<uint16_t>(sizeof(double));
        _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&rawMeas.cp[i]), sizeof(double), _index);
        _index += static_cast<uint16_t>(sizeof(double));
        _buffer.peek_unchecked(reinterpret_cast<uint8_t*>(&rawMeas.dp[i]), sizeof(float), _index);
        _index += static_cast<uint16_t>(sizeof(float));
    }

    value = rawMeas;
    return false;
}

}  // namespace VN

#endif  // VN_FAPACKETPROTOCOL_HPP_
