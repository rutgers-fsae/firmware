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

#ifndef VN_LOGGER_HPP_
#define VN_LOGGER_HPP_

#include <cstdint>

#include "vectornav/HAL/File.hpp"
#include "vectornav/HAL/Thread.hpp"
#include "vectornav/TemplateLibrary/ByteBuffer.hpp"

namespace VN
{
namespace Logger
{

/**
 * @class SimpleLogger
 * @brief Class responsible for logging data from a ByteBuffer to a file.
 *
 * The SimpleLogger class writes the contents of a ByteBuffer to a specified file.
 * It runs in a separate thread and logs data continuously until stopped.
 */
class SimpleLogger
{
public:
    /**
     * @brief Constructs a SimpleLogger instance and opens the file for logging.
     *
     * @param bufferToLog The ByteBuffer to log.
     * @param filePath The file path where the log will be stored.
     */
    SimpleLogger(ByteBuffer& bufferToLog, const Filesystem::FilePath& filePath) : _bufferToLog(bufferToLog) { _logFile.open(filePath); }

    /**
     * @brief Destructor that stops logging and closes the file.
     */
    ~SimpleLogger() { stop(); }

    SimpleLogger(const SimpleLogger&) = delete;
    SimpleLogger& operator=(const SimpleLogger&) = delete;
    SimpleLogger(SimpleLogger&&) = delete;
    SimpleLogger& operator=(SimpleLogger&&) = delete;

    /**
     * @brief Writes the contents of the buffer to the specified output file.
     *
     * @param outputFile The file to write the data to.
     * @param buffer The ByteBuffer to log.
     * @return The number of bytes successfully logged, or a negative value on error.
     */
    static int32_t logBuffer(OutputFile& outputFile, ByteBuffer& buffer)
    {
        if (!outputFile.is_open()) { return -1; }
        const size_t bufferSize = buffer.size();
        if (bufferSize == 0) { return 0; }
        size_t numLinearBytes = buffer.numLinearBytesToPeek();

        const size_t bytesToLog = std::min(bufferSize, numLinearBytes);
        if (outputFile.write(reinterpret_cast<const char*>(buffer.head()), bytesToLog)) { return -1; }
        size_t bytesLogged = bytesToLog;
        buffer.discard(bytesLogged);
        if (bufferSize > numLinearBytes)
        {
            numLinearBytes = buffer.numLinearBytesToPeek();
            bytesLogged += numLinearBytes;
            if (outputFile.write(reinterpret_cast<const char*>(buffer.head()), numLinearBytes)) { return -1; }  // Write the second section
            buffer.discard(numLinearBytes);
        }
        return bytesLogged;
    }

    /**
     * @brief Starts logging in a new thread.
     *
     * @return True if the file is not open, false otherwise.
     */
    Errored start()
    {
        if (!_logFile.is_open()) { return true; }
        _logging = true;
        _loggingThread = std::make_unique<Thread>(&SimpleLogger::_log, this);
        return false;
    }

    /**
     * @brief Stops logging and closes the file.
     */
    void stop()
    {
        if (_logging)
        {
            _logging = false;
            _loggingThread->join();
        }
    }

    /**
     * @brief Checks if the logger is currently logging.
     *
     * @return true if logging, false otherwise.
     */
    bool isLogging() { return _logging; }

    /**
     * @brief Gets the total number of bytes logged so far.
     *
     * @return The number of bytes logged.
     */
    size_t numBytesLogged() { return _numBytesLogged; }

    /**
     * @brief Gets the total number write to file errors encountered.
     *
     * @return The number of write to file errors.
     */
    size_t numWriteErrors() { return _writeErrorCount; }

protected:
    /**
     * @brief The main logging loop that writes data from the ByteBuffer to the log file.
     * Runs continuously as long as logging is enabled.
     */
    void _log()
    {
        while (_logging)
        {
            int32_t numBytes = logBuffer(_logFile, _bufferToLog);
            if (numBytes < 0) { _writeErrorCount++; }
            else { _numBytesLogged += numBytes; }
            thisThread::sleepFor(sleepDuration);
        }
        int32_t numBytes = logBuffer(_logFile, _bufferToLog);
        if (numBytes < 0) { _writeErrorCount++; }
        else { _numBytesLogged += numBytes; }
        _logFile.close();
    }

    Microseconds sleepDuration = 1ms;                  ///< Duration between log attempts.
    std::atomic<bool> _logging = false;                ///< Flag indicating whether logging is active.
    OutputFile _logFile;                               ///< File to which the buffer will be logged.
    ByteBuffer& _bufferToLog;                          ///< Reference to the ByteBuffer containing the data to log.
    std::unique_ptr<Thread> _loggingThread = nullptr;  ///< Pointer to the logging thread.
    std::atomic<size_t> _numBytesLogged = 0;           ///< Number of bytes logged.
    std::atomic<size_t> _writeErrorCount = 0;          ///< Number of write to file errors.
};

}  // namespace Logger
}  // namespace VN
#endif  // VN_LOGGER_HPP_
