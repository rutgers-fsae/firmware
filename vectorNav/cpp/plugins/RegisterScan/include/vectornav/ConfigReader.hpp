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

#ifndef VN_CONFIGREADER_HPP_
#define VN_CONFIGREADER_HPP_

#include <cstdint>
#include <functional>

#include "pugixml/pugixml.hpp"
#include "vectornav/HAL/File.hpp"
#include "vectornav/Interface/Sensor.hpp"
#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{

namespace RegisterScan
{

template <typename T>
/**
 * @struct ConfigReader
 * @brief A base template interface for reading VectorNav configuration messages through the CRTP pattern.
 * @tparam T The derived reader class implementing the specific reading mechanism.
 */
struct ConfigReader
{
    /**
     * @brief Gets the next configuration message using the derived class implementation.
     * @param[out] msg Reference to an AsciiMessage object that will be populated with the next configuration.
     * @return Error code indicating success or failure of the operation.
     */
    VN::Error getNextConfig(AsciiMessage& msg) { return static_cast<T*>(this)->next(msg); }
};

/**
 * @class GenericConfigReader
 * @brief A configurable reader that uses a function callback to retrieve VectorNav ASCII configuration messages.
 * @details Allows for custom configuration reading logic through a user-provided callback function. Should
 * return an error if the read operation fails.
 */
class GenericConfigReader : public ConfigReader<GenericConfigReader>
{
public:
    /**
     * @brief Constructs a GenericConfigReader with a custom callback function.
     * @param getNextConfig Function object that implements the configuration reading logic.
     */
    GenericConfigReader(std::function<VN::Error(AsciiMessage&)> getNextConfig) : _next(getNextConfig) {}

    /**
     * @brief Retrieves the next configuration message using the provided callback function.
     * @param[out] msg Reference to an AsciiMessage object that will be populated with the next configuration.
     * @return Error code indicating success or failure of the operation.
     */
    VN::Error next(AsciiMessage& msg)
    {
        VN::Error error = _next(msg);
        return error;
    }

private:
    std::function<VN::Error(AsciiMessage&)> _next = nullptr;
};

/**
 * @class AsciiConfigReader
 * @brief A reader that reads VectorNav configurations from ASCII text files line by line.
 * @details Reads one line at a time from the specified file into the provided message buffer,
 *          using newline characters as line delimiters. Returns an error if the read operation fails.
 */
class AsciiConfigReader : public ConfigReader<AsciiConfigReader>
{
public:
    /**
     * @brief Constructs an AsciiConfigReader that reads from the specified file path.
     * @param path The file path containing ASCII configuration commands.
     */
    AsciiConfigReader(const Filesystem::FilePath& path) : file(path) {}

    /**
     * @brief Reads the next line from the ASCII configuration file into the provided message buffer.
     * @param[out] msg Reference to an AsciiMessage object that will be populated with the next line.
     * @return Error code indicating success or failure of the read operation.
     */
    VN::Error next(AsciiMessage& msg);

private:
    InputFile file;
};

/**
 * @class XmlConfigReader
 * @brief A reader that converts VectorNav configurations from XML format into ASCII messages.
 * @details Parses Register elements from an XML document and converts them to VNWRG ASCII messages.
 * Returns an error if the read operation fails.
 */
class XmlConfigReader : public ConfigReader<XmlConfigReader>
{
public:
    /**
     * @brief Constructs an XmlConfigReader that reads from the specified XML file path.
     * @param path The file path to the XML configuration document.
     */
    XmlConfigReader(const Filesystem::FilePath& path) : _path(path) {}

    /**
     * @brief Reads the next register configuration from the XML document and formats it as a VNWRG ASCII message.
     * @param[out] msg Reference to an AsciiMessage object that will be populated with the formatted configuration command.
     * @return Error code indicating success or failure of the operation.
     * @details Handles special cases where register values contain delimiters that require splitting into multiple commands.
     */
    VN::Error next(AsciiMessage& msg);

private:
    bool double_config = false;
    pugi::xml_node _curr_node;
    pugi::xml_node _next_node;
    Filesystem::FilePath _path;
    pugi::xml_document _doc;
};

}  // namespace RegisterScan
}  // namespace VN

#endif  // VN_CONFIGREADER_HPP_
