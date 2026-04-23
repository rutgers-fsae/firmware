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

#ifndef VN_FIRMWAREPROGRAMMERERRORS_HPP_
#define VN_FIRMWAREPROGRAMMERERRORS_HPP_

namespace VN
{
namespace FirmwareProgrammer
{

enum class ErrorBL : uint16_t
{
    // Bootloader errors from sensor
    None = 0,
    InvalidCommand = 0x01,
    InvalidRecordType = 0x02,
    InvalidByteCount = 0x03,
    InvalidMemoryAddress = 0x04,
    CommError = 0x05,
    InvalidHexFile = 0x06,
    DecryptionError = 0x07,
    InvalidBlockCRC = 0x08,
    InvalidProgramCRC = 0x09,
    InvalidProgramSize = 0x0A,
    MaxRetryCount = 0x0B,
    Timeout = 0x0C,
    Reserved = 0x0D,

    // Firmware update errors
    EnterBootloaderFailed = 301,
    IncompatibleVNX = 302
};

}  // namespace FirmwareProgrammer

inline static const char* errorCodeToString(FirmwareProgrammer::ErrorBL error) noexcept
{
    switch (error)
    {
        case FirmwareProgrammer::ErrorBL::None:
            return "BL:None";
        case FirmwareProgrammer::ErrorBL::InvalidCommand:
            return "BL:InvalidCommand";
        case FirmwareProgrammer::ErrorBL::InvalidRecordType:
            return "BL:InvalidRecordType";
        case FirmwareProgrammer::ErrorBL::InvalidByteCount:
            return "BL:InvalidByteCount";
        case FirmwareProgrammer::ErrorBL::InvalidMemoryAddress:
            return "BL:InvalidMemoryAddress";
        case FirmwareProgrammer::ErrorBL::CommError:
            return "BL:CommError";
        case FirmwareProgrammer::ErrorBL::InvalidHexFile:
            return "BL:InvalidHexFile";
        case FirmwareProgrammer::ErrorBL::DecryptionError:
            return "BL:DecryptionError";
        case FirmwareProgrammer::ErrorBL::InvalidBlockCRC:
            return "BL:InvalidBlockCRC";
        case FirmwareProgrammer::ErrorBL::InvalidProgramCRC:
            return "BL:InvalidProgramCRC";
        case FirmwareProgrammer::ErrorBL::InvalidProgramSize:
            return "BL:InvalidProgramSize";
        case FirmwareProgrammer::ErrorBL::MaxRetryCount:
            return "BL:MaxRetryCount";
        case FirmwareProgrammer::ErrorBL::Timeout:
            return "BL:Timeout";
        case FirmwareProgrammer::ErrorBL::Reserved:
            return "BL:Reserved";
        case FirmwareProgrammer::ErrorBL::EnterBootloaderFailed:
            return "BL:EnterBootloaderFailed";
        case FirmwareProgrammer::ErrorBL::IncompatibleVNX:
            return "BL:IncompatibleVNX";
        default:
            return "BL:Unknown error code.";
    }
}

}  // namespace VN
#endif  // VN_FIRMWAREPROGRAMMERERRORS_HPP_
