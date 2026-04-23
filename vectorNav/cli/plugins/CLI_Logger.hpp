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

#ifndef VN_CLI_LOGGER_HPP_
#define VN_CLI_LOGGER_HPP_

#include "vectornav/CLI_ByteBuffer.hpp"

#pragma managed(push, off)
#include "vectornav/SimpleLogger.hpp"
#pragma managed(pop)

using namespace System;

namespace VNSDK
{
    
public ref class Logger
{
public:
    
    ref class SimpleLogger
    {
    private:
        VN::Logger::SimpleLogger *_simpleLogger = nullptr;
        VN::Filesystem::FilePath *_filePath;

    public:
    
        SimpleLogger(ByteBuffer^ bufferToLog, String ^filePath)
        {
            marshal_context^ context = gcnew marshal_context();
            _filePath = new VN::Filesystem::FilePath(context->marshal_as<const char*>(filePath));
            delete context;

            _simpleLogger = new VN::Logger::SimpleLogger(*(bufferToLog->GetNativePointer()), *_filePath);
        }

        ~SimpleLogger()
        {
            delete _simpleLogger;
            delete _filePath;
        }

        /// @brief start logging
        bool Start()
        {
            return _simpleLogger->start();
        }

        /// @brief stop logging
        void Stop()
        {
            _simpleLogger->stop();
        }

        /// @brief is logging
        bool IsLogging()
        {
            return _simpleLogger->isLogging();
        }

        /// @brief get the number of bytes logged
        size_t NumBytesLogged()
        {
            return _simpleLogger->numBytesLogged();
        }
    }; // public ref class SimpleLogger

}; // public ref class Logger
} // namespace VNSDK
#endif  // VN_CLI_LOGGER_HPP_
