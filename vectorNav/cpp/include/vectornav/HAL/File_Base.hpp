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

#ifndef VN_FILE_BASE_HPP_
#define VN_FILE_BASE_HPP_

#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{
namespace Filesystem
{
using FilePath = String<256>;
bool exists(const FilePath& filePath) noexcept;
}  // namespace Filesystem

/// @brief SDK object of a file to read from.
class InputFile_Base
{
public:
    virtual ~InputFile_Base() {};

    /// @brief Opens the specified file for reading.
    /// @param filePath
    /// @return An error occurred.
    virtual Errored open(const Filesystem::FilePath& filePath) = 0;

    /// @brief Closes the file.
    virtual void close() = 0;

    /// @brief Checks if the file is open by the SDK.
    virtual bool is_open() const = 0;

    /// @brief Resets the file head to the beginning of the file, clearing any error flags.
    virtual void reset() = 0;

    /// @brief Reads the next bytes from the file into the buffer, advancing file head.
    /// @param buffer Output buffer to receive the bytes.
    /// @param count Number of bytes to read from the file.
    /// @return Number of bytes read from file.
    virtual size_t read(char* buffer, const size_t count) = 0;

    /// @brief Reads the next bytes from the file into the buffer, advancing file head, until endChar is reached.
    /// @param buffer Output buffer to receive the bytes.
    /// @param bufferCapacity Capacity of output buffer.
    /// @param endChar The character at which to stop.
    /// @return An error occurred.
    virtual Errored read(char* buffer, const size_t bufferCapacity, const char endChar) = 0;

    /// @brief Gets the next line (until \\n) from the file into the buffer. Strips off \\r if present. Null-terminates output.
    /// @param buffer Output buffer to receive the bytes.
    /// @param capacity Capacity of output buffer, serving as max line length.
    /// @return An error occurred.
    virtual Errored getLine(char* buffer, const size_t capacity) = 0;
};

/// @brief SDK object of a file to write to.
class OutputFile_Base
{
public:
    virtual ~OutputFile_Base() = default;

    /// @brief Opens the specified file for writing.
    /// @param filePath The file to open.
    /// @return An error occurred.
    virtual Errored open(const Filesystem::FilePath& filePath) = 0;

    /// @brief Closes the file.
    virtual void close() = 0;

    /// @brief Checks if the file is open by the SDK.
    virtual bool is_open() const = 0;

    /// @brief Resets the file head to the beginning of the file, clearing any error flags.
    virtual void reset() = 0;

    /// @brief Forces buffered data to be written to the underlying file system.
    virtual void flush() = 0;

    /// @brief Write bytes to the file.
    /// @param buffer Input buffer head to write to the file.
    /// @param count Number of bytes to write to the file.
    /// @return An error occurred.
    virtual Errored write(const char* buffer, const size_t count) = 0;

    /// @brief Writes full buffer to the file.
    /// @param buffer Input buffer head to write to the file. Must be null-terminated.
    /// @return A error occurred.
    virtual Errored write(const char* buffer) = 0;

    /// @brief Writes a line to the file, terminating with \\n.
    /// @param buffer Input buffer head to write to the file.
    /// @param count Number of bytes from the buffer to write to the file (\\n character is additional, not counted in count)
    /// @return An error occurred.
    virtual Errored writeLine(const char* buffer, const size_t count) = 0;

    /// @brief Writes a line to the file, terminating with \\n.
    /// @param buffer Input buffer head to write to the file. Must be null-terminated.
    /// @return An error occurred.
    virtual Errored writeLine(const char* buffer) = 0;
};

}  // namespace VN

#endif  // VN_FILE_BASE_HPP_
