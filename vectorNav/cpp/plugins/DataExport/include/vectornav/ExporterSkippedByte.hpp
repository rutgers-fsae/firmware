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

#ifndef VN_EXPORTERSKIPPEDBYTES_HPP_
#define VN_EXPORTERSKIPPEDBYTES_HPP_

#include "vectornav/Debug.hpp"
#include "vectornav/Exporter.hpp"
#include "vectornav/HAL/File.hpp"
#include "vectornav/Implementation/Packet.hpp"
#include "vectornav/Interface/GenericCommand.hpp"

namespace VN
{
namespace DataExport
{

class ExporterSkippedByte : public Exporter
{
private:
    static constexpr uint16_t EXPORTER_PACKET_CAPACITY = 2048;

public:
    ExporterSkippedByte(const Filesystem::FilePath& outputDir, PacketQueueMode mode = PacketQueueMode::Force)
        : Exporter(EXPORTER_PACKET_CAPACITY, mode), _filePath(outputDir)
    {
        if (!_filePath.empty() && _filePath.back() != std::filesystem::path::preferred_separator)
        {
            _filePath = _filePath + std::filesystem::path::preferred_separator;
        }
    }

    void exportToFile() override
    {
        if (!_file.is_open() && _init_file()) { return; }
        while (!_queue.isEmpty())
        {
            const auto p = _queue.get();
            if (!p || (p->details.syncByte != PacketDetails::SyncByte::None)) { continue; }

            _file.write(reinterpret_cast<const char*>(p->buffer), p->length());
        }
    }

private:
    Filesystem::FilePath _filePath;
    OutputFile _file;

    Errored _init_file()
    {
        Filesystem::FilePath fileName;
        std::snprintf(fileName.begin(), fileName.capacity(), "%sskippedBytes.bin", _filePath.c_str());
        _file = OutputFile(fileName);
        return _file.is_open();
    }
};

}  // namespace DataExport
}  // namespace VN

#endif  // VN_EXPORTERSKIPPEDBYTES_HPP_
