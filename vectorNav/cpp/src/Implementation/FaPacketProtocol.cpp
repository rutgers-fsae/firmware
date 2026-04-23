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

#include "vectornav/Implementation/FaPacketProtocol.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "vectornav/Debug.hpp"
#include "vectornav/HAL/Timer.hpp"
#include "vectornav/Implementation/BinaryHeader.hpp"
#include "vectornav/Implementation/CoreUtils.hpp"
#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#include "vectornav/TemplateLibrary/ByteBuffer.hpp"

namespace VN
{
namespace FaPacketProtocol
{
namespace
{
bool _isValidBinaryCrc(const ByteBuffer& buffer, const size_t syncByteIndex, const uint16_t packetLength) noexcept
{
    uint16_t calculatedCrc = 0;
    // Crc validation does not include sync byte
    for (size_t fromHeadIndex = syncByteIndex + 1; fromHeadIndex < (syncByteIndex + packetLength); ++fromHeadIndex)
    {
        _calculateCRC(&calculatedCrc, buffer.peek_unchecked(fromHeadIndex));
    }
    return calculatedCrc == 0;
}

PacketDispatcher::FindPacketRetVal::Validity _calculateBinaryMeasurementTypeSize(const ByteBuffer& buffer, const size_t typeDataStartIndex,
                                                                                 const uint16_t binaryGroupOffset, const uint16_t binaryTypeOffset,
                                                                                 uint16_t& binaryTypeSize) noexcept
{
    if (binaryGroupOffset == 3 || binaryGroupOffset == 6 || binaryGroupOffset == 12)
    {  // Is a GNSS group
        if (binaryTypeOffset == 14)
        {  // Is Sat Info
            auto satCount = buffer.peek(typeDataStartIndex);
            if (!satCount.has_value()) { return Validity::Incomplete; }
            if (satCount.value() > Config::PacketFinders::gnssSatInfoMaxCount) { return Validity::Invalid; }
            binaryTypeSize = 2 + 8 * satCount.value();
        }
        else if (binaryTypeOffset == 16)
        {  // Is Raw Meas
            auto numSats = buffer.peek(typeDataStartIndex + 10);
            if (!numSats.has_value()) { return Validity::Incomplete; }
            if (numSats.value() > Config::PacketFinders::gnssRawMeasMaxCount) { return Validity::Invalid; }
            binaryTypeSize = 12 + 28 * numSats.value();
        }
        else
        {
            auto staticBinaryTypeSizeRetVal = getStaticBinaryTypeSize(binaryGroupOffset, binaryTypeOffset);
            if (!staticBinaryTypeSizeRetVal.has_value()) { return Validity::Invalid; }
            binaryTypeSize = staticBinaryTypeSizeRetVal.value();
        }
    }
    else
    {
        auto staticBinaryTypeSizeRetVal = getStaticBinaryTypeSize(binaryGroupOffset, binaryTypeOffset);
        if (!staticBinaryTypeSizeRetVal.has_value()) { return Validity::Invalid; }
        binaryTypeSize = staticBinaryTypeSizeRetVal.value();
    }
    return Validity::Valid;
}

PacketDispatcher::FindPacketRetVal::Validity _calculteExpectedPayloadSize(const ByteBuffer& buffer, BinaryHeader& header, const size_t syncByteIndex,
                                                                          uint16_t& expectedPayloadSize) noexcept
{
    uint8_t headerSize = header.outputGroups.size() + header.outputTypes.size() * 2;
    uint8_t groupByteNumber = 0;
    uint8_t typeWordIndex = -1;  // Initialize below zero because we preincrement
    expectedPayloadSize = 0;
    // Loop through both the normal and extension group byte
    for (const auto groupByte : header.outputGroups)
    {
        // Loop through each bit of the group byte, besides extension bit
        for (auto groupBitOffset = 0; groupBitOffset < 7; ++groupBitOffset)
        {
            bool groupIsEnabled = groupByte & (1 << groupBitOffset);
            if (groupIsEnabled)
            {
                uint8_t typeWordNumber = 0;
                // Loop through both the normal and extension type word
                uint16_t typeWord;
                do {
                    // This shouldn't need to be checked, because we are assuming we are getting a valid header.
                    typeWord = header.outputTypes[++typeWordIndex];

                    // Loop through each bit of the type word, besides extension bit
                    for (auto typeBitOffset = 0; typeBitOffset < 15; ++typeBitOffset)
                    {
                        if (!(typeWord & (1 << typeBitOffset))) { continue; }
                        size_t typeDataStartIdx = syncByteIndex + headerSize + expectedPayloadSize + 1;  // Necessary for dynamic-sized packets
                        uint8_t binaryGroupNumber = groupByteNumber * 8 + groupBitOffset;
                        uint8_t binaryTypeNumber = typeWordNumber * 16 + typeBitOffset;

                        uint16_t measurementTypeSize = 0;
                        Validity measurementTypeSizeRetVal =
                            _calculateBinaryMeasurementTypeSize(buffer, typeDataStartIdx, binaryGroupNumber, binaryTypeNumber, measurementTypeSize);
                        expectedPayloadSize += measurementTypeSize;
                        switch (measurementTypeSizeRetVal)
                        {
                            case (Validity::Invalid):
                                // Fall through
                            case (Validity::Incomplete):
                            {
                                if (expectedPayloadSize > Config::PacketFinders::faPacketMaxLength) { return Validity::Invalid; }
                                else { return measurementTypeSizeRetVal; }
                            }
                            case (Validity::Valid):
                                // Do nothing
                                break;
                            default:
                                return measurementTypeSizeRetVal;
                        }
                    }
                    ++typeWordNumber;
                } while (typeWord & 0x8000);
            }
        }
        ++groupByteNumber;
    }
    if (expectedPayloadSize > Config::PacketFinders::faPacketMaxLength) { return Validity::Invalid; }
    return Validity::Valid;
}

PacketDispatcher::FindPacketRetVal::Validity _populateHeader(const ByteBuffer& buffer, const size_t syncByteIdx, BinaryHeader& header) noexcept
{
    const size_t numPacketBytesInBuffer = buffer.size() - syncByteIdx;
    size_t currentFromHeadIndex = syncByteIdx;

    bool processingExtensionGroups;
    do {
        const auto byte = buffer.peek(++currentFromHeadIndex);
        if (!byte.has_value()) { return Validity::Incomplete; }

        if (header.outputGroups.push_back(byte.value())) { return Validity::Invalid; }

        processingExtensionGroups = header.outputGroups.back() & 0x80;

    } while (processingExtensionGroups);

    // Add the type words to the header.
    // Iterate through both the regular and extension group byte.
    for (const auto currGroupByte : header.outputGroups)
    {
        // Iterate through each group bit, except for extension bit.
        for (uint8_t currGroupBitOffset = 0; currGroupBitOffset < 7; ++currGroupBitOffset)
        {
            if (currGroupByte & (1 << currGroupBitOffset))
            {
                bool processingMeasurementTypes;
                do {
                    auto tmpRet1 = buffer.peek(++currentFromHeadIndex);
                    if (!tmpRet1.has_value()) { return Validity::Incomplete; }
                    auto tmpRet2 = buffer.peek(++currentFromHeadIndex);
                    if (!tmpRet2.has_value()) { return Validity::Incomplete; }
                    uint16_t currentTypeWord = tmpRet1.value() + (tmpRet2.value() << 8);
                    // Type is too long  (too many bits set or extensions) for what can fit in our data structure
                    if (header.outputTypes.push_back(currentTypeWord)) { return Validity::Invalid; }
                    processingMeasurementTypes = (currentTypeWord & 0x8000);
                } while (processingMeasurementTypes);
            }
        }
    }

    // Now that we know the number of type bytes, we can check for a sufficient amount of data one last time.
    uint8_t minimumRequiredPacketSize =
        1 + header.outputGroups.size() + header.outputTypes.size() * 3 + 2;  // The * 3 is because there must be one byte in payload for each type
    if (numPacketBytesInBuffer < minimumRequiredPacketSize) { return Validity::Incomplete; }
    else { return Validity::Valid; }
}

}  // namespace

FindPacketReturn findPacket(const ByteBuffer& byteBuffer, const size_t syncByteIndex) noexcept
{
    VN_PROFILER_TIME_CURRENT_SCOPE();
    Metadata metadata;
    metadata.timestamp = now();

    uint8_t tmpByte = byteBuffer.peek_unchecked(syncByteIndex);

    if (tmpByte != 0xFA) { return {Validity::Invalid, Metadata{BinaryHeader{}, 1, time_point{}}}; }

    size_t numPacketBytesInBuffer = byteBuffer.size() - syncByteIndex;
    if (numPacketBytesInBuffer < 7)
    {  // Minimum requirement is 1 sync, 3 header, 1 payload, 2 crc
        return {Validity::Incomplete, Metadata{BinaryHeader{}, 7, time_point{}}};
    }

    BinaryHeader header{};
    PacketDispatcher::FindPacketRetVal::Validity headerValidity = _populateHeader(byteBuffer, syncByteIndex, header);
    switch (headerValidity)
    {
        case (Validity::Invalid):
            // Fall through
        case (Validity::Incomplete):
        {
            return {headerValidity, Metadata{BinaryHeader{}, 7, time_point{}}};
        }
        case (Validity::Valid):
        {
            // Everything completed fine. Proceed normally.
            break;
        }
        default:
        {
            return {headerValidity, Metadata{BinaryHeader{}, 7, time_point{}}};
        }
    }
    uint8_t headerSize = header.outputGroups.size() + header.outputTypes.size() * 2;

    uint16_t expectedPayloadSize = 0;
    PacketDispatcher::FindPacketRetVal::Validity calcExpectedPayloadSizeValidity =
        _calculteExpectedPayloadSize(byteBuffer, header, syncByteIndex, expectedPayloadSize);
    uint16_t requiredPacketLength = 1 + headerSize + expectedPayloadSize + 2;
    switch (calcExpectedPayloadSizeValidity)
    {
        case (Validity::Invalid):
            // This will happen if an invalid binary group/type was set
            // Fall through
        case (Validity::Incomplete):
            // This will happen if there isn't enough data in the buffer, which can only be for dynamic packets
            return {calcExpectedPayloadSizeValidity, Metadata{BinaryHeader{}, requiredPacketLength, time_point{}}};
        case (Validity::Valid):
            // Do nothing

            break;
        default:
            return {calcExpectedPayloadSizeValidity, Metadata{BinaryHeader{}, requiredPacketLength, time_point{}}};
    }

    // We now know the whole size of the expected payload. Check to make sure we have enough bytes one last time.
    if (numPacketBytesInBuffer < requiredPacketLength) { return {Validity::Incomplete, Metadata{BinaryHeader{}, requiredPacketLength, time_point{}}}; };

    metadata.header = header;
    metadata.length = requiredPacketLength;

    const bool isValidCrc = _isValidBinaryCrc(byteBuffer, syncByteIndex, requiredPacketLength);
    return isValidCrc ? FindPacketReturn{Validity::Valid, metadata} : FindPacketReturn{Validity::Invalid, metadata};
}

std::optional<CompositeData> parsePacket(const ByteBuffer& buffer, const size_t syncByteIndex, const Metadata& metadata,
                                         [[maybe_unused]] const EnabledMeasurements& measurementsToParse) noexcept
{
    VN_PROFILER_TIME_CURRENT_SCOPE();
    CompositeData compositeData(metadata.header);

    FaPacketExtractor extractor(buffer, metadata, syncByteIndex);
    extractor.discard(metadata.header.size() + 1);

    BinaryHeaderIterator iter(metadata.header);
    bool consumed = false;
    while (iter.next())
    {
        uint16_t fieldSize = 0;
        auto validity = _calculateBinaryMeasurementTypeSize(buffer, syncByteIndex + extractor.index(), iter.group(), iter.field(), fieldSize);
        if (validity != PacketDispatcher::FindPacketRetVal::Validity::Valid) { return std::nullopt; }
        if (compositeData.copyFromBuffer(extractor, iter.group(), iter.field())) { extractor.discard(fieldSize); }
        else { consumed = true; }
    }

    if (extractor.index() != (metadata.length - 2)) { return std::nullopt; }
    return (consumed) ? std::make_optional(compositeData) : std::nullopt;
}

}  // namespace FaPacketProtocol
}  // namespace VN
