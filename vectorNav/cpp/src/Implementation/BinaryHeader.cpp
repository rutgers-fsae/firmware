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

#include "vectornav/Implementation/BinaryHeader.hpp"

#include "vectornav/Debug.hpp"
#include "vectornav/Implementation/BinaryMeasurementDefinitions.hpp"

namespace VN
{

std::optional<EnabledMeasurements> BinaryHeader::toMeasurementHeader() const noexcept
{
    EnabledMeasurements presentMeasurements = {};
    int numGroupBytes = this->outputGroups.size();
    int numTypeBytes = this->outputTypes.size();
    int currGroupByteNum = 0;
    int currOutputTypeByteNum = 0;

    // This loops through the binary header's configured output groups, storing them in currGroupByte. It:
    // 1. Checks extension bits to verify consistency (last byte can't be set, all before must be)
    // 2. Loops through each bit in the set group byte (has special handling for common group)
    //      Each time a binary group bit is set, it grabs the next outputType word from the header, setting them to presentMeasurements
    for (const auto currGroupByte : this->outputGroups)
    {
        // Validate that extension bits are properly set. If we are NOT currently on the header's last group, extension bit MUST be set. If we are on the
        // header's last group, extension bit must NOT be set.
        if (currGroupByteNum < numGroupBytes - 1)
        {
            if (!(currGroupByte & 0x80)) { return std::nullopt; }
        }
        else if (currGroupByte & 0x80) { return std::nullopt; }

        // Loop through all but the extension bit
        for (uint8_t currGroupBitNum = 0; currGroupBitNum < 7; currGroupBitNum++)
        {
            const bool groupIsEnabled = currGroupByte & (1 << currGroupBitNum);
            if (!groupIsEnabled) { continue; }

            // If we're about to work on the first group of the first group byte, and the first bit of it is true, Common group is set
            const bool isCommonGroup = (currGroupByteNum == 0) && (currGroupBitNum == 0);
            if (isCommonGroup)
            {
                if (currOutputTypeByteNum >= numTypeBytes) { return std::nullopt; }
                uint16_t currTypeWord = this->outputTypes[currOutputTypeByteNum];
                if (currTypeWord & 0x8000) { return std::nullopt; }  // For common group, extension bits should never be set

                // Don't iterate the extension group, because we already made sure it isn't set
                for (uint8_t currOutputTypeBitNum = 0; currOutputTypeBitNum < CommonGroupMapping.size(); ++currOutputTypeBitNum)
                {
                    const bool typeIsEnabled = currTypeWord & (1 << currOutputTypeBitNum);
                    if (!typeIsEnabled) { continue; }
                    auto commonGroupMappingVecToUse = CommonGroupMapping[currOutputTypeBitNum];

                    // Iterate through each listed Common measurement type, as some Common "types" map to multiple measurement types
                    for (const auto& currMeasTypeCoords : commonGroupMappingVecToUse)
                    {
                        // Add the current mapped measurement type to the presentMeasurements
                        // Subtructing 1 because of Common group offset
                        if (currMeasTypeCoords.measGroupIndex >= static_cast<uint8_t>(presentMeasurements.size() + 1u)) { return std::nullopt; }
                        presentMeasurements[currMeasTypeCoords.measGroupIndex - 1u] |= 1 << currMeasTypeCoords.measTypeIndex;
                    }
                }
                currOutputTypeByteNum++;
            }
            else
            {  // Every non-common group
                uint16_t currTypeByte = 0;
                bool isExtensionBit = false;
                const uint8_t presentMeasurementsCurrentGroupIdx = currGroupByteNum * 8 + currGroupBitNum - 1;  // -1 is because of common group
                do {
                    if (currOutputTypeByteNum >= numTypeBytes) { return std::nullopt; }
                    currTypeByte = this->outputTypes[currOutputTypeByteNum];

                    // We don't want the extension bit to be set as an enabled meas type
                    const uint16_t currTypeByteWithoutExtensionBit = currTypeByte & 0x7FFF;

                    // |= is because the common group could have already set some bits in this uint32
                    presentMeasurements[presentMeasurementsCurrentGroupIdx] |= currTypeByteWithoutExtensionBit << (isExtensionBit * 16);
                    isExtensionBit = true;
                    currOutputTypeByteNum++;
                } while (currTypeByte & 0x8000);  // extension bit is enabled
            }
        }
        currGroupByteNum++;
    }
    if (currOutputTypeByteNum != numTypeBytes) { return std::nullopt; }  // It was incremented at last iteration of do, while
    return std::make_optional<EnabledMeasurements>(presentMeasurements);
}

uint8_t BinaryHeader::_countSetBits(uint8_t n)
{
    uint8_t mask = 0x7F;  // binary: 01111111
    n &= mask;
    uint8_t count = 0;
    while (n)
    {
        count += n & 1;
        n >>= 1;
    }
    return count;
}
}  // namespace VN
