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

#ifndef VN_FILE_PC_HPP_
#define VN_FILE_PC_HPP_

#include <filesystem>
#include <fstream>
#include <ios>

#include "vectornav/HAL/File_Base.hpp"
#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{

namespace Filesystem
{
inline bool exists(const FilePath& filePath) noexcept
{
    std::error_code ec;
    return std::filesystem::exists(filePath.c_str(), ec);
}
}  // namespace Filesystem

inline Filesystem::FilePath operator/(Filesystem::FilePath lhs, const char* rhs) { return Filesystem::FilePath(lhs + "/" + rhs); }

class InputFile : public InputFile_Base
{
public:
    InputFile(const bool nullTerminateRead = true) : _nullTerminateRead(nullTerminateRead) {};
    InputFile(const Filesystem::FilePath& filePath, const bool nullTerminateRead = true)
        : _file(filePath, std::ios::binary), _nullTerminateRead(nullTerminateRead) {};

    virtual ~InputFile() {};

    InputFile& operator=(InputFile&& rhs) noexcept
    {
        if (this != &rhs) { _file = std::move(rhs._file); }
        return *this;
    }

    virtual Errored open(const Filesystem::FilePath& filePath) override final
    {
        std::error_code ec;
        if (!_file.is_open() && !std::filesystem::exists(filePath.c_str(), ec)) { return true; }

        _file.open(filePath.c_str(), std::ios_base::binary);
        return !_file.good();
    }

    virtual void close() override final { _file.close(); };

    virtual Errored read(char* buffer, const size_t bufferCapacity, const char endChar) override final
    {
        if (!_file.is_open() || !_file.good())
        {
            buffer[0] = '\0';
            return true;
        };
        size_t i = 0;
        while (1)
        {
            _file.read(&buffer[i], 1);
            if (buffer[i] == endChar) { break; }
            if (!_file.good()) { return 1; }
            ++i;
            if (i > bufferCapacity) { return 1; }
        }
        if (_nullTerminateRead) { buffer[++i] = '\0'; }
        return !_file.good();
    }

    virtual size_t read(char* buffer, const size_t count) override final
    {
        size_t bytesRead = 0;
        if (_file.is_open() && _file.good())
        {
            _file.read(buffer, count);
            bytesRead = static_cast<size_t>(_file.gcount());
        }
        if (_nullTerminateRead) { buffer[bytesRead] = '\0'; }
        return bytesRead;
    }

    virtual Errored getLine(char* buffer, const size_t capacity) override final
    {
        if (!_file.is_open() || !_file.good())
        {
            buffer[0] = '\0';
            return true;
        };
        _file.getline(buffer, capacity, '\n');

        // To make it cross-compatible, strip off \r if it is ending the file
        if (buffer[_file.gcount() - 2] == '\r') { buffer[_file.gcount() - 2] = '\0'; }

        return !_file.good();
    }

    virtual bool is_open() const override final { return _file.is_open(); };

    virtual void reset() override final
    {
        _file.clear();
        _file.seekg(0, std::ios::beg);
    }

private:
    std::ifstream _file;
    bool _nullTerminateRead;
};

class OutputFile : public OutputFile_Base
{
public:
    OutputFile() {};
    OutputFile(const Filesystem::FilePath& filePath) : _file(filePath, std::ios::binary) {};

    virtual ~OutputFile()
    {
        if (_file.is_open())
        {
            flush();
            _file.close();
        }
    };

    OutputFile(const OutputFile&) = delete;
    OutputFile operator=(const OutputFile&) = delete;

    OutputFile(OutputFile&& rhs) : _file(std::move(rhs._file)) {}

    OutputFile& operator=(OutputFile&& rhs) noexcept
    {
        if (this != &rhs) { _file = std::move(rhs._file); }
        return *this;
    }

    virtual Errored open(const Filesystem::FilePath& filePath) override final
    {
        _file.open(filePath.c_str(), std::ios_base::binary | std::ios_base::trunc);
        return !_file.good();
    }

    virtual void close() override final
    {
        if (_file.is_open())
        {
            flush();
            _file.close();
        }
    };

    virtual Errored write(const char* buffer, const size_t count) override final
    {
        if (!_file.is_open() && !_file.good()) { return true; };
        _file.write(buffer, count);
        return !_file.good();
    }

    virtual Errored write(const char* buffer) override final { return write(buffer, std::strlen(buffer)); }

    virtual Errored writeLine(const char* buffer, const size_t count) override final
    {
        if (write(buffer, count)) { return true; }
        return write("\n", 1);
    }

    virtual Errored writeLine(const char* buffer) override final { return writeLine(buffer, std::strlen(buffer)); }

    virtual bool is_open() const override final { return _file.is_open(); }

    virtual void reset() override final
    {
        _file.clear();
        _file.seekp(0, std::ios::beg);
    }

    virtual void flush() override final
    {
        if (_file.is_open() && _file.good()) { _file.flush(); }
    }

private:
    std::ofstream _file;
};

}  // namespace VN

#endif  // VN_FILE_PC_HPP_
