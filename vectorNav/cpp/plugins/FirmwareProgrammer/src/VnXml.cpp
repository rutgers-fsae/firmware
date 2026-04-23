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

#include "vectornav/VnXml.hpp"

#include "vectornav/Interface/GenericCommand.hpp"

namespace VN
{
namespace FirmwareProgrammer
{
namespace VnXml
{
namespace
{
struct ExtractComponentReturn
{
    enum class Status
    {
        Success,
        Failed,
        NotFound
    } status = Status::NotFound;
    Component component{};
    uint16_t linesConsumed = 0;
};
ExtractComponentReturn extractComponent(InputFile& vnXmlFile, AsciiMessage beginningLine);
}  // namespace

Metadata extractMetadata(InputFile& vnXmlFile)
{
    // The only part of the structure that I care about is:
    //   Component [HwId="##""] [Type="#"]
    //     ...
    //     Data

    size_t lineNumber = 0;
    AsciiMessage line;
    Metadata components;

    while (!vnXmlFile.getLine(line.begin(), line.capacity()))
    {
        ++lineNumber;

        // Enter the "Components" node
        if (line.find("<Components") == AsciiMessage::npos) { continue; }

        bool processingComponents = true;
        while (processingComponents)
        {
            // Continue to the next line, discarding "Components" tag
            vnXmlFile.getLine(line.begin(), line.capacity());
            ++lineNumber;
            if (line.find("</Components>") != AsciiMessage::npos) { break; }

            ExtractComponentReturn componentReturn = extractComponent(vnXmlFile, line);
            switch (componentReturn.status)
            {
                case (ExtractComponentReturn::Status::Success):
                {
                    break;
                }
                case (ExtractComponentReturn::Status::Failed):
                {
                    VN_ABORT();
                }
                case (ExtractComponentReturn::Status::NotFound):
                {
                    lineNumber += componentReturn.linesConsumed;
                    continue;
                }
                default:
                    VN_ABORT();
            }
            if (componentReturn.status != ExtractComponentReturn::Status::Success) { VN_ABORT(); }

            componentReturn.component.dataLineBegin += lineNumber;
            componentReturn.component.dataLineEnd += lineNumber;

            components.push_back(componentReturn.component);
            lineNumber += componentReturn.linesConsumed;
        }
    }

    return components;
}

namespace
{
ExtractComponentReturn extractComponent(InputFile& vnXmlFile, AsciiMessage line)
{  // Check if the line contains a Component definition
    ExtractComponentReturn retVal;

    if (line.find("<Component") == AsciiMessage::npos) { return retVal; }  // If we didn't find a component tag, return right away.

    // Set status to failed for early returns
    retVal.status = ExtractComponentReturn::Status::Failed;

    // Parse the Component attributes
    size_t attribPosBegin;
    if ((attribPosBegin = line.find("Type=")) != AsciiMessage::npos)
    {
        uint8_t memType;
        auto [ptr, ec] = std::from_chars(&line[attribPosBegin + 6], &line[line.find('"', attribPosBegin + 6)], memType);
        if (ec == std::errc()) { retVal.component.memoryType = static_cast<MemoryType>(memType); }
        else { return retVal; }
    }
    else
    {
        // component missing Type...We won't try to get a default right now
        return retVal;
    }

    if ((attribPosBegin = line.find("HwId=")) != AsciiMessage::npos)
    {
        uint8_t hwId;
        auto [ptr, ec] = std::from_chars(&line[attribPosBegin + 6], &line[line.find('"', attribPosBegin + 6)], hwId);
        if (ec == std::errc()) { retVal.component.hardwareId = static_cast<HardwareId>(hwId); }
        else { return retVal; }
    }
    else
    {
        // component missing HwId...We won't try to get a default right now
        return retVal;
    }

    // Find the data block
    while (!vnXmlFile.getLine(line.begin(), line.capacity()))
    {
        ++retVal.linesConsumed;

        if (line.find("</Data>") != AsciiMessage::npos)
        {
            retVal.component.dataLineEnd = retVal.linesConsumed - 1;
            break;
        }

        if (line.find(':') != AsciiMessage::npos)
        {
            if (retVal.component.dataLineBegin == 0) { retVal.component.dataLineBegin = retVal.linesConsumed; }
        }
    }

    // Exit the "Component" node
    bool foundEndOfComponent = false;
    while (!vnXmlFile.getLine(line.begin(), line.capacity()))
    {
        ++retVal.linesConsumed;

        if (line.find("</Component>") != AsciiMessage::npos)
        {
            foundEndOfComponent = true;
            break;
        }
    }
    if (!foundEndOfComponent) { return retVal; }

    // Make sure we got everything we expected. Otherwise we won't do any recovery
    if (retVal.component.dataLineBegin == 0 || retVal.component.dataLineEnd == 0) { return retVal; }
    if (!HardwareIdCheck::is_value(retVal.component.hardwareId) || retVal.component.hardwareId == HardwareId::Undefined) { return retVal; }
    if (!MemoryTypeCheck::is_value(retVal.component.memoryType) || retVal.component.memoryType == MemoryType::Undefined) { return retVal; }

    retVal.status = ExtractComponentReturn::Status::Success;
    return retVal;
}
}  // namespace

}  // namespace VnXml
}  // namespace FirmwareProgrammer
}  // namespace VN
