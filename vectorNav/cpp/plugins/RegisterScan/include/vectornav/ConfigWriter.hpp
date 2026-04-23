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

#ifndef VN_CONFIGWRITER_HPP_
#define VN_CONFIGWRITER_HPP_

#include <cstdint>
#include <functional>
#include <map>

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
 * @struct ConfigWriter
 * @brief Base template interface for writing VectorNav configurations using the CRTP patter
 * @tparam T The derived writer class implementing the specific writing mechanism.
 */
struct ConfigWriter
{
    /**
     * @brief Writes a configuration message using the derived class implementation.
     * @param msg The ASCII message containing the configuration to write.
     * @return Error code indicating success or failure of the operation.
     */
    Error writeConfig(const AsciiMessage& msg) { return static_cast<T*>(this)->write(msg); }

    /**
     * @brief Finalizes the writing process, closing files or performing cleanup.
     */
    void close() { static_cast<T*>(this)->finalize(); }
};

/**
 * @class AsciiConfigWriter
 * @brief A writer that saves VectorNav configurations to ASCII text files.
 * @details Writes configuration commands directly to a file without formatting changes.
 */
class AsciiConfigWriter : public ConfigWriter<AsciiConfigWriter>
{
public:
    /**
     * @brief Constructs an AsciiConfigWriter that writes to the specified file path.
     * @param path The file path where configurations will be saved.
     */
    AsciiConfigWriter(const Filesystem::FilePath& path) : file(path) {}

    /**
     * @brief Writes an ASCII message to the output file.
     * @param msg The ASCII message to write.
     * @return Error code indicating success or failure of the operation.
     */
    Error write(const AsciiMessage& msg)
    {
        if (file.write(msg.c_str())) { return Error::FileWriteFailed; }
        else { return Error::None; }
    }

    /**
     * @brief Finalizes the writing process by closing the output file.
     */
    void finalize()
    {
        if (file.is_open()) { file.close(); }
    }

private:
    OutputFile file;
};

/**
 * @class GenericConfigWriter
 * @brief A configurable writer that uses custom functions for writing VectorNav configurations.
 * @details Delegates writing and finalization operations to user-provided callback functions.
 */
class GenericConfigWriter : public ConfigWriter<GenericConfigWriter>
{
public:
    /**
     * @brief Constructs a GenericConfigWriter with custom callback functions.
     * @param writeConfig Function object that implements the configuration writing logic.
     * @param finalizeConfig Optional function object that implements finalization logic. If not provided a default empty lambda is used.
     */
    GenericConfigWriter(
        std::function<Error(const AsciiMessage& msg)> writeConfig, std::function<void()> finalizeConfig = []() {})
        : _writeConfig(writeConfig), _finalizeConfig(finalizeConfig)
    {
    }

    /**
     * @brief Writes a configuration message using the provided callback function.
     * @param msg The ASCII message to write.
     * @return Error code returned by the callback function.
     */
    Error write(const AsciiMessage& msg) { return _writeConfig(msg); }

    /**
     * @brief Finalizes the writing process using the provided callback function.
     */
    void finalize() { _finalizeConfig(); }

private:
    std::function<Error(const AsciiMessage& msg)> _writeConfig;
    std::function<void()> _finalizeConfig;
};

/**
 * @class XmlConfigWriter
 * @brief A writer that converts VectorNav ASCII configuration messages to XML format.
 * @details Creates an XML document with information from a VectorNav unit and register configurations,
 *          handling special cases for certain registers that require aggregation of multiple commands.
 */
class XmlConfigWriter : public ConfigWriter<XmlConfigWriter>
{
public:
    /**
     * @brief Constructs an XmlConfigWriter for the specified sensor object and output path.
     * @param sensor The sensor object to read from.
     * @param path The file path where the XML document will be saved.
     */
    XmlConfigWriter(Sensor& sensor, const Filesystem::FilePath path) : sensor(sensor), _path(path) {}

    /**
     * @brief Processes and stores an ASCII configuration message for later XML output.
     * @param msg The ASCII message containing the configuration command.
     * @return Error code indicating success or failure of the operation.
     * @details Special handling for registers 5, 6, 7, and 99 which may require aggregation of values.
     */
    Error write(const AsciiMessage& msg);

    /**
     * @brief Finalizes the XML document and writes it to the specified file path.
     * @details Aggregates any pending register values and saves the complete XML configuration.
     */
    void finalize();

private:
    Sensor& sensor;
    Filesystem::FilePath _path;
    bool _initialized = false;
    pugi::xml_document _doc;
    pugi::xml_node _root;

    AsciiMessage _reg5 = "";
    AsciiMessage _reg6 = "";
    AsciiMessage _reg7 = "";
    AsciiMessage _reg99 = "";

    Error _initialize();
    Error _write(const AsciiMessage& msg, const uint16_t regId);
};

}  // namespace RegisterScan
}  // namespace VN

#endif  // VN_CONFIGWRITER_HPP_
