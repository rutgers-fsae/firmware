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

#include "vectornav/RegisterScan.hpp"

namespace VN
{

namespace RegisterScan
{

VN::Error _setConfigurationRegister(Sensor& sensor, const AsciiMessage& msg)
{
    Error error = Error::None;
    VN_DEBUG_0("Loading setting: " << msg.c_str() << std::endl);
    const uint16_t start = msg.find(',', 0);
    const uint16_t end = msg.find(',', start + 1);
    if (start == AsciiMessage::npos || start >= msg.length() || end == AsciiMessage::npos || end >= msg.length())
    {
        error = Error::ReceivedUnexpectedMessage;
        return error;
    }
    std::optional<uint8_t> regId = StringUtils::fromString<uint8_t>(&msg[start + 1], &msg[end]);

    const auto first_comma = msg.find(',');
    const auto asterisk = msg.find('*');

    if (!regId.has_value()) { return Error::InvalidRegister; }
    AsciiMessage cmd_str;
    std::snprintf(cmd_str.data(), cmd_str.capacity(), "WRG,%.*s", static_cast<int>(asterisk - first_comma - 1), &msg[first_comma + 1]);
    auto wrg = GenericCommand(cmd_str);

    if (regId.value() == 5)
    {
        Registers::System::BaudRate baudReg;
        baudReg.baudRate = Registers::System::BaudRate::BaudRates::Baud115200;
        baudReg.serialPort = Registers::System::BaudRate::SerialPort::ActiveSerial;
        if (baudReg.fromString(msg)) { error = Error::InvalidParameter; }
        else { error = sensor.changeBaudRate(baudReg.baudRate.value(), baudReg.serialPort); }
    }
    else { error = sensor.sendCommand(&wrg, Sensor::SendCommandBlockMode::Block); }

    return error;
}

}  // namespace RegisterScan
}  // namespace VN
