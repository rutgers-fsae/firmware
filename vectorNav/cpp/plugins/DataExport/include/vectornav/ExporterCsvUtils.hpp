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

#ifndef VN_EXPORTERCSVUTILS_HPP_
#define VN_EXPORTERCSVUTILS_HPP_

#include <stdint.h>

#include <string>

#include "vectornav/Implementation/AsciiPacketProtocol.hpp"
#include "vectornav/Implementation/FaPacketProtocol.hpp"

namespace VN
{
namespace DataExport
{

enum CsvType : uint8_t
{
    U8,
    U16,
    U32,
    U64,
    UTC,
    FLO,
    DUB,
    SAT,
    RAW,
    UNK,
    NON,
};

using VN::DataExport::CsvType;

struct CsvTypeInfo
{
    CsvType type = CsvType::NON;
    uint8_t len = 0;
};

CsvTypeInfo csvTypeLookup(size_t i, size_t j);

template <class T>
int extractToString(FaPacketExtractor& extractor, const size_t numToExtract, char* ptr, const uint16_t remaining) noexcept
{
    int offset = 0;
    uint8_t first_comma_skip = 1;  // Skip first comma of sentense
    for (uint8_t i = 0; i < numToExtract; i++)
    {
        constexpr const char* format = []() constexpr
        {
            if constexpr (std::is_same_v<T, float>) { return ",%f"; }
            else if constexpr (std::is_same_v<T, double>) { return ",%12.8f"; }
            else if constexpr (std::is_same_v<T, uint64_t>) { return ",%llu"; }
            else { return ",%d"; }
        }();
        offset += std::snprintf(ptr + offset, remaining - offset - first_comma_skip, format + first_comma_skip, extractor.extract_unchecked<T>());
        first_comma_skip = 0;
    }
    return offset;
}

int getMeasurementString(FaPacketExtractor& extractor, const CsvTypeInfo& typeInfo, char* ptr, const uint16_t remaining);

const char* getMeasurementString(const AsciiPacketProtocol::AsciiMeasurementHeader& msg);

const char* getMeasurementName(const size_t binaryGroup, const size_t binaryField);

}  // namespace DataExport
}  // namespace VN

#endif  // VN_EXPORTERCSVUTILS_HPP_
