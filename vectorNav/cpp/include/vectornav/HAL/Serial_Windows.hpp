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

#ifndef VN_SERIAL_WINDOWS_HPP_
#define VN_SERIAL_WINDOWS_HPP_

#include <string>

#define NOMINMAX 1
#include <windows.h>

#include <exception>

#include "vectornav/Config.hpp"
#include "vectornav/HAL/Serial_Base.hpp"
#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{

class Serial : public Serial_Base
{
public:
    using Serial_Base::Serial_Base;

    // ***********
    // Port access
    // ***********
    Error open(const PortName& portName, const uint32_t baudRate) noexcept override final;
    void close() noexcept override;
    Error changeBaudRate(const uint32_t baudRate) noexcept override final;

    // ***************
    // Port read/write
    // ***************
    Error getData() noexcept override final;
    Error send(const char* buffer, const size_t len) noexcept override final;

private:
    // ***********
    // Port access
    // ***********
    HANDLE _serialPortHandle = NULL;
    Errored _reconfigurePort();

    // Mutex _mutex;

    // ***************
    // Port read/write
    // ***************
    Errored _flush();
};

// ######################
//     Implementation
// ######################

inline Error Serial::open(const PortName& portName, const uint32_t baudRate) noexcept
{
    if (_isOpen) { close(); }

    PortName port_with_prefix = "\\\\.\\" + portName;

    std::wstring wide_port(port_with_prefix.begin(), port_with_prefix.end());

    LPCWSTR lp_port = wide_port.c_str();
    _serialPortHandle = CreateFileW(lp_port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (_serialPortHandle == INVALID_HANDLE_VALUE)
    {
        DWORD create_file_err = GetLastError();
        switch (create_file_err)
        {
            case ERROR_FILE_NOT_FOUND:
            case ERROR_PATH_NOT_FOUND:
                return Error::InvalidPortName;
            case ERROR_ACCESS_DENIED:
                return Error::AccessDenied;
            default:
                VN_DEBUG_1("Error getting the serial port state: " + std::to_string(create_file_err));
                return Error::UnexpectedSerialError;
        }
    }

    _portName = portName;
    _baudRate = baudRate;
    if (_reconfigurePort()) { return Error::UnexpectedSerialError; }
    _isOpen = true;
    return Error::None;
}

inline void Serial::close() noexcept
{
    if (_serialPortHandle != NULL) CloseHandle(_serialPortHandle);
    _serialPortHandle = NULL;
    _isOpen = false;
}

inline Error Serial::changeBaudRate(const uint32_t baudRate) noexcept
{
    if (!_isOpen) { return Error::SerialPortClosed; }
    if (_flush()) { return Error::UnexpectedSerialError; }
    close();
    const Error openError = open(_portName, baudRate);
    if (openError != Error::None) { return openError; }
    _baudRate = baudRate;
    return Error::None;
}

// Add ability to block on getting data
inline Error Serial::getData() noexcept
{
    if (!_isOpen) { return Error::SerialPortClosed; }
    DWORD bytes_read;
    COMSTAT stats;
    DWORD errors;
    ClearCommError(_serialPortHandle, &errors, &stats);

    auto bytesRemaining = stats.cbInQue;
    size_t linearBytes = _byteBuffer.numLinearBytesToPut();
    while (bytesRemaining > 0 && linearBytes > 0)
    {
        if (!ReadFile(_serialPortHandle, const_cast<uint8_t*>(_byteBuffer.tail()), std::min(bytesRemaining, static_cast<DWORD>(linearBytes)), &bytes_read,
                      NULL))
        {
            VN_DEBUG_1("Error while reading from the serial port: " + std::to_string(GetLastError()));
            return Error::SerialReadFailed;
        }
        _byteBuffer.put(static_cast<size_t>(bytes_read));
        linearBytes = _byteBuffer.numLinearBytesToPut();
        bytesRemaining -= bytes_read;
    }

    if (bytesRemaining > 0) { return Error::PrimaryBufferFull; }

    return Error::None;
}

inline Error Serial::send(const char* buffer, const size_t len) noexcept
{
    if (!_isOpen) { return Error::SerialPortClosed; }
    DWORD bytes_written;
    if (!WriteFile(_serialPortHandle, buffer, static_cast<DWORD>(len), &bytes_written, NULL))
    {
        VN_DEBUG_1("Error while writing to the serial port: " + std::to_string(GetLastError()));
        return Error::SerialWriteFailed;
    }
    return Error::None;
}

inline Errored Serial::_flush() { return !PurgeComm(_serialPortHandle, PURGE_TXCLEAR | PURGE_RXCLEAR); }

inline Errored Serial::_reconfigurePort()
{
    DCB dcbSerialParams{};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(_serialPortHandle, &dcbSerialParams))
    {
        VN_DEBUG_1("Error getting the serial port state.");
        return true;
    }
    dcbSerialParams.BaudRate = _baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fOutxCtsFlow = false;
    dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
    dcbSerialParams.fOutX = false;
    dcbSerialParams.fInX = false;

    // activate settings
    if (!SetCommState(_serialPortHandle, &dcbSerialParams))
    {
        CloseHandle(_serialPortHandle);
        VN_DEBUG_1("Error setting serial port settings.");
        return true;
    }

    // Setup timeouts
    COMMTIMEOUTS comTimeout{};
    comTimeout.ReadIntervalTimeout = 0;
    comTimeout.ReadTotalTimeoutMultiplier = 0;
    comTimeout.ReadTotalTimeoutConstant = 1;
    comTimeout.WriteTotalTimeoutMultiplier = 3;
    comTimeout.WriteTotalTimeoutConstant = 2;
    if (!SetCommTimeouts(_serialPortHandle, &comTimeout))
    {
        VN_DEBUG_1("Error setting timeouts.");
        return true;
    }
    return false;
}

}  // namespace VN

#endif  // VN_SERIAL_WINDOWS_HPP_
