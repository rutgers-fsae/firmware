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

#include "vectornav/Implementation/AsciiPacketProtocol.hpp"

#include <cstdint>

#include "vectornav/Config.hpp"
#include "vectornav/Debug.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Implementation/CoreUtils.hpp"
#include "vectornav/Interface/CompositeData.hpp"
#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{
namespace AsciiPacketProtocol
{
// namespace {
struct AsciiMeasurementIndices
{
    AsciiMeasurementIndices() {};
    AsciiMeasurementIndices(uint8_t MeasGroupIndex, uint8_t MeasTypeIndex) : measGroupIndex(MeasGroupIndex), measTypeIndex(MeasTypeIndex) {};
    uint8_t measGroupIndex;
    uint8_t measTypeIndex;
};

std::optional<Vector<AsciiMeasurementIndices, 9>> _getAsciiMeasurementIndices(const AsciiMeasurementHeader asciiHeader);
bool _parseAsciiMessage(const AsciiMeasurementHeader asciiHeader, const ByteBuffer& buffer, const size_t syncByteIdx, const Metadata& metadata,
                        CompositeData& compositeData);

using HeaderChars = std::array<char, 5>;

constexpr std::array<std::tuple<HeaderChars, AsciiMeasurementHeader>, 22> headerMapping = {

    std::make_tuple(HeaderChars{"YPR"}, AsciiMeasurementHeader::YPR), std::make_tuple(HeaderChars{"QTN"}, AsciiMeasurementHeader::QTN),
    std::make_tuple(HeaderChars{"QMR"}, AsciiMeasurementHeader::QMR), std::make_tuple(HeaderChars{"MAG"}, AsciiMeasurementHeader::MAG),
    std::make_tuple(HeaderChars{"ACC"}, AsciiMeasurementHeader::ACC), std::make_tuple(HeaderChars{"GYR"}, AsciiMeasurementHeader::GYR),
    std::make_tuple(HeaderChars{"MAR"}, AsciiMeasurementHeader::MAR), std::make_tuple(HeaderChars{"YMR"}, AsciiMeasurementHeader::YMR),
    std::make_tuple(HeaderChars{"YBA"}, AsciiMeasurementHeader::YBA), std::make_tuple(HeaderChars{"YIA"}, AsciiMeasurementHeader::YIA),
    std::make_tuple(HeaderChars{"IMU"}, AsciiMeasurementHeader::IMU), std::make_tuple(HeaderChars{"GPS"}, AsciiMeasurementHeader::GPS),
    std::make_tuple(HeaderChars{"GPE"}, AsciiMeasurementHeader::GPE), std::make_tuple(HeaderChars{"INS"}, AsciiMeasurementHeader::INS),
    std::make_tuple(HeaderChars{"INE"}, AsciiMeasurementHeader::INE), std::make_tuple(HeaderChars{"ISL"}, AsciiMeasurementHeader::ISL),
    std::make_tuple(HeaderChars{"ISE"}, AsciiMeasurementHeader::ISE), std::make_tuple(HeaderChars{"DTV"}, AsciiMeasurementHeader::DTV),
    std::make_tuple(HeaderChars{"G2S"}, AsciiMeasurementHeader::G2S), std::make_tuple(HeaderChars{"G2E"}, AsciiMeasurementHeader::G2E),
    std::make_tuple(HeaderChars{"HVE"}, AsciiMeasurementHeader::HVE), std::make_tuple(HeaderChars{"RTK"}, AsciiMeasurementHeader::RTK)};

uint8_t _getNumAsciiParameters(const AsciiMeasurementHeader header)
{
    switch (header)
    {
        case AsciiMeasurementHeader::QTN:
        {
            return 4;
        }
        case AsciiMeasurementHeader::QMR:
        {
            return 13;
        }
        case AsciiMeasurementHeader::YPR:
            [[fallthrough]];
        case AsciiMeasurementHeader::MAG:
            [[fallthrough]];
        case AsciiMeasurementHeader::ACC:
            [[fallthrough]];
        case AsciiMeasurementHeader::HVE:
            [[fallthrough]];
        case AsciiMeasurementHeader::GYR:
        {
            return 3;
        }
        case AsciiMeasurementHeader::YMR:
        {
            return 12;
        }
        case AsciiMeasurementHeader::MAR:
            [[fallthrough]];
        case AsciiMeasurementHeader::YBA:
            [[fallthrough]];
        case AsciiMeasurementHeader::YIA:
        {
            return 9;
        }
        case AsciiMeasurementHeader::IMU:
        {
            return 11;
        }
        case AsciiMeasurementHeader::GPS:
            [[fallthrough]];
        case AsciiMeasurementHeader::GPE:
            [[fallthrough]];
        case AsciiMeasurementHeader::G2S:
            [[fallthrough]];
        case AsciiMeasurementHeader::G2E:
            [[fallthrough]];
        case AsciiMeasurementHeader::INS:
            [[fallthrough]];
        case AsciiMeasurementHeader::INE:
            [[fallthrough]];
        case AsciiMeasurementHeader::ISL:
            [[fallthrough]];
        case AsciiMeasurementHeader::ISE:
        {
            return 15;
        }
        case AsciiMeasurementHeader::DTV:
        {
            return 7;
        }
        case AsciiMeasurementHeader::RTK:
            [[fallthrough]];
        case AsciiMeasurementHeader::None:
        {
            return 0;
        }
        default:
            return 0;
    }
}

AsciiMeasurementHeader getMeasHeader(AsciiHeader headerChars)
{
    for (const auto& mapEntry : headerMapping)
    {
        if (std::memcmp(headerChars.data() + 2, std::get<0>(mapEntry).data(), sizeof(char) * 3) == 0) { return std::get<1>(mapEntry); }
    }
    return AsciiMeasurementHeader::None;
}

std::optional<EnabledMeasurements> asciiHeaderToMeasHeader(const AsciiMeasurementHeader header) noexcept
{
    EnabledMeasurements presentMeasurements = {};
    if (!asciiIsParsable(header)) { return std::nullopt; }
    auto asciiMeasurementIndices = (*_getAsciiMeasurementIndices(header));
    for (const auto& currentMeasField : asciiMeasurementIndices)
    {
        uint8_t ind = static_cast<uint8_t>(currentMeasField.measGroupIndex - 1);  // Subtracting 1 because of Common group offset
        if (ind >= presentMeasurements.size()) { return std::nullopt; }
        presentMeasurements[ind] |= 1 << currentMeasField.measTypeIndex;
    }
    return presentMeasurements;
};

bool allDataIsEnabled(const AsciiMeasurementHeader header, const EnabledMeasurements& measurementsToCheck) noexcept
{
    const std::optional<EnabledMeasurements> measHeader = asciiHeaderToMeasHeader(header);
    if (!measHeader.has_value()) { return false; }
    else { return VN::allDataIsEnabled(measHeader.value(), measurementsToCheck); }
}

bool asciiIsMeasurement(const AsciiMeasurementHeader header) noexcept { return _getAsciiMeasurementIndices(header).has_value(); }

bool asciiIsParsable(const AsciiPacketProtocol::AsciiMeasurementHeader header) noexcept
{
    return (_getAsciiMeasurementIndices(header).has_value() && !_getAsciiMeasurementIndices(header).value().empty());
}

bool anyDataIsEnabled(const AsciiMeasurementHeader header, const EnabledMeasurements& measurementsToCheck) noexcept
{
    const std::optional<EnabledMeasurements> measHeader = asciiHeaderToMeasHeader(header);
    if (!measHeader.has_value()) { return false; }
    else { return VN::anyDataIsEnabled(measHeader.value(), measurementsToCheck); }
}

FindPacketReturn findPacket(const ByteBuffer& byteBuffer) noexcept
{
    auto syncByteIndex = byteBuffer.find('$');
    if (!syncByteIndex.has_value()) { return {AsciiPacketProtocol::Validity::Invalid, Metadata{}}; }
    return findPacket(byteBuffer, syncByteIndex.value());
}

FindPacketReturn findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
{
    VN_PROFILER_TIME_CURRENT_SCOPE();
    Metadata details;
    details.timestamp = now();

    auto tmpByte = byteBuffer.peek_unchecked(syncByteIndex);
    if (tmpByte != static_cast<uint8_t>('$')) { return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}}; }  // It was a mistake to come here.

    // Search for the end of the packet (\n)
    const auto newlineCharacterIndex = byteBuffer.find('\n', syncByteIndex);
    if (!newlineCharacterIndex.has_value())
    {
        if (byteBuffer.size() - syncByteIndex > Config::PacketFinders::asciiPacketMaxLength)
        {
            return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}};
        }
        else { return {PacketDispatcher::FindPacketRetVal::Validity::Incomplete, Metadata{}}; }
    }

    const bool isMissingCarriageReturn = byteBuffer.peek_unchecked(newlineCharacterIndex.value() - 1) != '\r';

    details.length = newlineCharacterIndex.value() - syncByteIndex + 1;

    if (details.length > Config::PacketFinders::asciiPacketMaxLength) { return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}}; }

    bool processingHeader = true;
    uint8_t checksum8 = 0;
    uint16_t crc16 = 0;
    for (uint16_t fromSyncByteIndex = 1; fromSyncByteIndex < details.length; ++fromSyncByteIndex)
    {                                                              // Beginning one after the sync byte, but mark it as checked
        size_t fromTailIndex = syncByteIndex + fromSyncByteIndex;  // Should be zero-based, but is relative to current absolute tail.
        tmpByte = byteBuffer.peek_unchecked(fromTailIndex);
        if (tmpByte == ',')
        {
            details.delimiterIndices.push_back(fromSyncByteIndex);
            processingHeader = false;
        }
        else if (tmpByte == '*')
        {
            details.delimiterIndices.push_back(fromSyncByteIndex);
            processingHeader = false;
            break;
        }
        else if (((tmpByte < ' ') || (tmpByte > '~') || (tmpByte == '$')) && (tmpByte != '\r'))
        {
            // Contains a non-ascii character
            return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}};
        }
        if (processingHeader)
        {
            if (fromSyncByteIndex > Config::PacketFinders::asciiHeaderMaxLength) { return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}}; }
            details.header.push_back(tmpByte);
        }
        _calculateChecksum(&checksum8, tmpByte);
        _calculateCRC(&crc16, tmpByte);
    }

    const uint16_t bytesBetweenAstereskAndNewline = details.length - details.delimiterIndices.back();
    uint8_t crcLength;
    uint16_t calculatedChecksum;
    if (bytesBetweenAstereskAndNewline == static_cast<size_t>(2 + 2 + 1 - isMissingCarriageReturn))
    {  // *, CRC1, CRC2, \r (if isMissingCarriageReturn = false) , \n
        crcLength = 2;
        calculatedChecksum = checksum8;
    }
    else if (bytesBetweenAstereskAndNewline == static_cast<size_t>(4 + 2 + 1 - isMissingCarriageReturn))
    {  // *, CRC1, CRC2, CRC3, CRC4, \r (if isMissingCarriageReturn = false), \n
        crcLength = 4;
        calculatedChecksum = crc16;
    }
    else if (bytesBetweenAstereskAndNewline == (0 + 2 + 1))
    {  // *, \r, \n

        // No crc check necessary
        return {PacketDispatcher::FindPacketRetVal::Validity::Valid, details};
    }
    else { return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}}; }

    uint16_t reportedChecksum;

    // Verify Check Sum
    char crcStr[4] = {0, 0, 0, 0};
    switch (crcLength)
    {
        case 2:
        {
            const size_t crcBeginIndex = (details.length - 1) - (crcLength + 1 - isMissingCarriageReturn);
            byteBuffer.peek_unchecked(reinterpret_cast<uint8_t*>(crcStr), 2, syncByteIndex + crcBeginIndex);
            const std::optional<uint8_t> reportedCrc8 = StringUtils::fromStringHex<uint8_t>(crcStr, crcStr + 2);
            if (!reportedCrc8.has_value()) { return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}}; }
            reportedChecksum = reportedCrc8.value();
            break;
        }
        case 4:
        {
            const size_t crcBeginIndex = (details.length - 1) - (crcLength + 1 - isMissingCarriageReturn);
            byteBuffer.peek_unchecked(reinterpret_cast<uint8_t*>(crcStr), 4, syncByteIndex + crcBeginIndex);
            const std::optional<uint16_t> reportedCrc16 = StringUtils::fromStringHex<uint16_t>(crcStr, crcStr + 4);
            if (!reportedCrc16.has_value()) { return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}}; }
            reportedChecksum = reportedCrc16.value();
            break;
        }
        default:
            abort();
    }
    if (reportedChecksum == calculatedChecksum) { return {PacketDispatcher::FindPacketRetVal::Validity::Valid, details}; }
    else { return {PacketDispatcher::FindPacketRetVal::Validity::Invalid, Metadata{}}; }
}

std::optional<CompositeData> parsePacket(const ByteBuffer& buffer, const size_t syncByteIndex, const Metadata& metadata,
                                         AsciiPacketProtocol::AsciiMeasurementHeader measEnum) noexcept
{
    VN_PROFILER_TIME_CURRENT_SCOPE();

    const uint8_t numExpectedDelimeters = _getNumAsciiParameters(measEnum) + 1;
    // delimeters are wrong or there are too many appended messages
    if (!(numExpectedDelimeters <= metadata.delimiterIndices.size() && metadata.delimiterIndices.size() - numExpectedDelimeters < 3)) { return std::nullopt; }

    CompositeData compositeData{metadata.header};
    AsciiPacketExtractor extractor(buffer, metadata, syncByteIndex);

    auto asciiParsingData = _getAsciiMeasurementIndices(measEnum);
    if (!asciiParsingData.has_value()) { return std::nullopt; }

    for (const auto& measIndex : asciiParsingData.value())
    {
        if (compositeData.copyFromBuffer(extractor, measIndex.measGroupIndex, measIndex.measTypeIndex)) { return std::nullopt; }
    }

    // append amount
    for (int i = 0; i < 2; i++)
    {
        auto appendParam = extractor.nextAsciiParameter();
        if (!appendParam.has_value()) { break; }
        if (appendParam.value()[0] == 'S')
        {
            compositeData.asciiAppendStatus = StringUtils::fromStringHex<uint16_t>(appendParam.value().begin() + 1, appendParam.value().end());
            if (!compositeData.asciiAppendStatus.has_value()) { return std::nullopt; }
            extractor.discard(1);
        }
        else if (appendParam.value()[0] == 'T')
        {
            compositeData.asciiAppendCount = StringUtils::fromString<uint32_t>(appendParam.value().begin() + 1, appendParam.value().end());
            if (!compositeData.asciiAppendCount.has_value()) { return std::nullopt; }
            extractor.discard(1);
        }
        else { return std::nullopt; }
    }

    return std::make_optional(compositeData);
}

std::optional<Vector<AsciiMeasurementIndices, 9>> _getAsciiMeasurementIndices(AsciiMeasurementHeader asciiHeader)
{
    if (asciiHeader == AsciiMeasurementHeader::YPR)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 1),  // Ypr
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::QTN)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 2),  // Quaternion
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::QMR)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 2),   // Quaternion
            AsciiMeasurementIndices(2, 8),   // Mag
            AsciiMeasurementIndices(2, 9),   // Accel
            AsciiMeasurementIndices(2, 10),  // AngularRate
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::MAG)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(2, 8),  // Mag
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::ACC)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{

            AsciiMeasurementIndices(2, 9),  // Accel
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::GYR)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(2, 10),  // AngularRate
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::MAR)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(2, 8),   // Mag
            AsciiMeasurementIndices(2, 9),   // Accel
            AsciiMeasurementIndices(2, 10),  // AngularRate
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::YMR)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 1),   // Ypr
            AsciiMeasurementIndices(2, 8),   // Mag
            AsciiMeasurementIndices(2, 9),   // Accel
            AsciiMeasurementIndices(2, 10),  // AngularRate
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::YBA)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 1),   // Ypr
            AsciiMeasurementIndices(4, 6),   // LinBodyAcc
            AsciiMeasurementIndices(2, 10),  // AngularRate
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::YIA)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 1),   // Ypr
            AsciiMeasurementIndices(4, 7),   // LinAccelNed
            AsciiMeasurementIndices(2, 10),  // AngularRate
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::IMU)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(2, 1),  // UncompMag
            AsciiMeasurementIndices(2, 2),  // UncompAccel
            AsciiMeasurementIndices(2, 3),  // UncompGyro
            AsciiMeasurementIndices(2, 4),  // Temperature
            AsciiMeasurementIndices(2, 5),  // Pressure
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::GPS)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(3, 1),   // GpsTow
            AsciiMeasurementIndices(3, 2),   // GpsWeek
            AsciiMeasurementIndices(3, 4),   // GnssFix
            AsciiMeasurementIndices(3, 3),   // NumSats
            AsciiMeasurementIndices(3, 5),   // GnssPosLla
            AsciiMeasurementIndices(3, 7),   // GnssVelNed
            AsciiMeasurementIndices(3, 9),   // GnssPosUncertainty
            AsciiMeasurementIndices(3, 10),  // GnssVelUncertainty
            AsciiMeasurementIndices(3, 11),  // GnssTimeUncertainty
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::GPE)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(3, 1),   // GpsTow
            AsciiMeasurementIndices(3, 2),   // GpsWeek
            AsciiMeasurementIndices(3, 4),   // GnssFix
            AsciiMeasurementIndices(3, 3),   // NumSats
            AsciiMeasurementIndices(3, 6),   // GnssPosEcef
            AsciiMeasurementIndices(3, 8),   // GnssVelEcef
            AsciiMeasurementIndices(3, 9),   // GnssPosUncertaintyEcef
            AsciiMeasurementIndices(3, 10),  // GnssVelUncertainty
            AsciiMeasurementIndices(3, 11),  // GnssTimeUncertainty
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::INS)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(1, 2),   // GpsTow
            AsciiMeasurementIndices(1, 3),   // GpsWeek
            AsciiMeasurementIndices(5, 0),   // InsStatus
            AsciiMeasurementIndices(4, 1),   // Ypr
            AsciiMeasurementIndices(5, 1),   // PosLla
            AsciiMeasurementIndices(5, 4),   // VelNed
            AsciiMeasurementIndices(4, 13),  // AttU
            AsciiMeasurementIndices(5, 9),   // PosU
            AsciiMeasurementIndices(5, 10),  // VelU
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::INE)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(1, 2),   // GpsTow
            AsciiMeasurementIndices(1, 3),   // GpsWeek
            AsciiMeasurementIndices(5, 0),   // InsStatus
            AsciiMeasurementIndices(4, 1),   // Ypr
            AsciiMeasurementIndices(5, 2),   // PosEcef
            AsciiMeasurementIndices(5, 5),   // VelEcef
            AsciiMeasurementIndices(4, 13),  // AttU
            AsciiMeasurementIndices(5, 9),   // PosU
            AsciiMeasurementIndices(5, 10),  // VelU
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::ISL)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 1),   // Ypr
            AsciiMeasurementIndices(5, 1),   // PosLla
            AsciiMeasurementIndices(5, 4),   // VelNed
            AsciiMeasurementIndices(2, 9),   // Accel
            AsciiMeasurementIndices(2, 10),  // AngularRate
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::ISE)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 1),   // Ypr
            AsciiMeasurementIndices(5, 2),   // PosEcef
            AsciiMeasurementIndices(5, 5),   // VelEcef
            AsciiMeasurementIndices(2, 9),   // Accel
            AsciiMeasurementIndices(2, 10),  // AngularRate
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::DTV)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(2, 6),  // DeltaTheta
            AsciiMeasurementIndices(2, 7),  // DeltaVel
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::G2S)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(6, 1),   // GpsTow
            AsciiMeasurementIndices(6, 2),   // GpsWeek
            AsciiMeasurementIndices(6, 4),   // GnssFix
            AsciiMeasurementIndices(6, 3),   // NumSats
            AsciiMeasurementIndices(6, 5),   // GnssPosLla
            AsciiMeasurementIndices(6, 7),   // GnssVelNed
            AsciiMeasurementIndices(6, 9),   // GnssPosUncertainty
            AsciiMeasurementIndices(6, 10),  // GnssVelUncertainty
            AsciiMeasurementIndices(6, 11),  // GnssTimeUncertainty
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::G2E)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(6, 1),   // GpsTow
            AsciiMeasurementIndices(6, 2),   // GpsWeek
            AsciiMeasurementIndices(6, 4),   // GnssFix
            AsciiMeasurementIndices(6, 3),   // NumSats
            AsciiMeasurementIndices(6, 6),   // GnssPosEcef
            AsciiMeasurementIndices(6, 8),   // GnssVelEcef
            AsciiMeasurementIndices(6, 9),   // GnssPosUncertaintyEcef
            AsciiMeasurementIndices(6, 10),  // GnssVelUncertainty
            AsciiMeasurementIndices(6, 11),  // GnssTimeUncertainty
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::HVE)
    {
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{
            AsciiMeasurementIndices(4, 12),  // Heave
        });
    }
    else if (asciiHeader == AsciiMeasurementHeader::RTK)
    {
        // Deprecated or unused measurements
        return std::make_optional(Vector<AsciiMeasurementIndices, 9>{});
    }
    else { return std::nullopt; }
};

uint8_t _getNumAppendedFields(const uint8_t numFieldsPresent, const uint8_t numFieldsExpected) { return numFieldsPresent - numFieldsExpected; }

}  // namespace AsciiPacketProtocol
}  // namespace VN
