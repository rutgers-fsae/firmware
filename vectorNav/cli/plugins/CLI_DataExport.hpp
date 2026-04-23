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

#ifndef VN_CLI_DATAEXPORT_HPP_
#define VN_CLI_DATAEXPORT_HPP_

#include "vectornav/CLI_QueueDefinitions.hpp"

#pragma managed(push, off)
#include "vectornav/Exporter.hpp"
#include "vectornav/ExporterAscii.hpp"
#include "vectornav/ExporterCsv.hpp"
// #include "vectornav/ExporterImr.hpp"
// #include "vectornav/ExporterRinex.hpp"
#include "vectornav/ExporterSkippedByte.hpp"
#pragma managed(pop)

using namespace System;

namespace VNSDK
{
namespace DataExport
{

public enum class PacketQueueMode
{
    Force = static_cast<int>(VN::DataExport::Exporter::PacketQueueMode::Force),
    Try = static_cast<int>(VN::DataExport::Exporter::PacketQueueMode::Try),
#if THREADING_ENABLE
    Retry = static_cast<int>(VN::DataExport::Exporter::PacketQueueMode::Retry)
#endif
};  

inline VN::DataExport::Exporter::PacketQueueMode ToNativeInstance(PacketQueueMode mode)
{
    return static_cast<VN::DataExport::Exporter::PacketQueueMode>(static_cast<int>(mode));
}

inline PacketQueueMode ToManagedInstance(VN::DataExport::Exporter::PacketQueueMode mode)
{
    return static_cast<PacketQueueMode>(static_cast<int>(mode));
}

public ref class Exporter
{
protected:
    VN::DataExport::Exporter *_exporter;
    VN::Filesystem::FilePath* _filePath;

public:
    Exporter(String^ filePath)
    {
        marshal_context^ context = gcnew marshal_context();
        _filePath = new VN::Filesystem::FilePath(context->marshal_as<const char*>(filePath));
        delete context;
    }

    virtual ~Exporter()
    {
        if (_exporter != nullptr)
        {
            delete _exporter;
        }

        delete _filePath;
    };

    VN::DataExport::Exporter* GetRawExporter()
    {
        return _exporter;
    }

    VN::PacketQueue_Interface* GetQueuePointer()
    {
        return _exporter->getQueuePtr();
    }

    ManagedQueuePointer^ GetManagedQueuePointer()
    {
        VN::PacketQueue_Interface* nativePtr = _exporter->getQueuePtr();
        ManagedQueuePointer^ managed_queue = gcnew ManagedQueuePointer(nativePtr);
        return managed_queue;
    }

    /// @brief start logging
    bool Start()
    {
        return _exporter->start();
    }

    /// @brief stop logging
    void Stop()
    {
        _exporter->stop();
    }

    /// @brief is logging
    bool IsLogging()
    {
        return _exporter->isLogging();
    }

    /// @brief export data to a file
    void ExportToFile()
    {
        _exporter->exportToFile();
    }

    /// @brief finalize the file before exporting
    virtual void FinalizeFile()
    {
    }

};

public ref class ExporterAscii : Exporter
{
    public:
    
    ExporterAscii(String ^filePath) : Exporter(filePath)
    {
        _exporter = new VN::DataExport::ExporterAscii(*_filePath);
    }

    ExporterAscii(String ^filePath, PacketQueueMode mode) : Exporter(filePath)
    {
        _exporter = new VN::DataExport::ExporterAscii(*_filePath, ToNativeInstance(mode));
    }

    ExporterAscii(String ^filePath, PacketQueueMode mode, bool enableSystemTimeStamps) : Exporter(filePath)
    {
        _exporter = new VN::DataExport::ExporterAscii(*_filePath, ToNativeInstance(mode), enableSystemTimeStamps);
    }

}; // public ref class ExporterAscii

public ref class ExporterCsv : Exporter
{
public:
    
    ExporterCsv(String^ filePath) : Exporter(filePath)
    {
        _exporter = new VN::DataExport::ExporterCsv(*_filePath);
    }

    ExporterCsv(String^ filePath, PacketQueueMode mode) : Exporter(filePath)
    {
        _exporter = new VN::DataExport::ExporterCsv(*_filePath, ToNativeInstance(mode));
    }

}; // public ref class ExporterCsv


public ref class ExporterSkippedByte : Exporter
{
public:
    ExporterSkippedByte(String^ filePath) : Exporter(filePath)
    {
        _exporter = new VN::DataExport::ExporterSkippedByte(*_filePath);
    }

    ExporterSkippedByte(String^ filePath, PacketQueueMode mode) : Exporter(filePath)
    {
        _exporter = new VN::DataExport::ExporterSkippedByte(*_filePath, ToNativeInstance(mode));
    }

};

} // namespace DataExport
} // namespace VNSDK
#endif  // VN_CLI_DATAEXPORT_HPP_
