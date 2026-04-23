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

#ifndef VN_EXPORTER_HPP_
#define VN_EXPORTER_HPP_

#include <atomic>
#include <cstdio>

#include "vectornav/Config.hpp"
#if THREADING_ENABLE
#include "vectornav/HAL/Thread.hpp"
#endif
#include "vectornav/Implementation/QueueDefinitions.hpp"

namespace VN
{
namespace DataExport
{

class Exporter
{
private:
    static constexpr uint16_t PACKET_QUEUE_SIZE = 256;

public:
    using PacketQueueMode = PacketQueue<PACKET_QUEUE_SIZE>::PutMode;

    Exporter(const size_t& packetCapacity, const PacketQueueMode& mode) : _queue{mode, packetCapacity} {}

    virtual ~Exporter() = default;

    virtual void exportToFile() = 0;

#if THREADING_ENABLE
    Errored start()
    {
        if (_thread == nullptr) { _logging = true; }
        else { return true; }
        _thread = std::make_unique<Thread>(&Exporter::_export, this);
        return false;
    }

    void stop()
    {
        _logging = false;
        _thread->join();
    }

    bool isLogging() const { return _logging; }
#endif

    PacketQueue_Interface* getQueuePtr() { return &_queue; }

protected:
    PacketQueue<PACKET_QUEUE_SIZE> _queue;
#if THREADING_ENABLE
    std::atomic<bool> _logging = false;
    std::unique_ptr<Thread> _thread = nullptr;

private:
    void _export()
    {
        while (_logging)
        {
            thisThread::sleepFor(1ms);
            exportToFile();
        }
        exportToFile();
    }
#endif
};

}  // namespace DataExport
}  // namespace VN

#endif  // VN_EXPORTER_HPP_
