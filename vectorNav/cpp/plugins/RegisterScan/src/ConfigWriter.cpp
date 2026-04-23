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

#include "vectornav/ConfigWriter.hpp"

#include <iostream>

#include "vectornav/Interface/Errors.hpp"
#include "vectornav/Interface/Registers.hpp"

namespace VN
{

namespace RegisterScan
{

Error augmentMessage(AsciiMessage& msgOut, const AsciiMessage& msg)
{
    const auto first_comma = msg.find(',');
    const auto second_comma = msg.find(',', first_comma + 1);
    const auto last_comma = msg.rfind(',');

    AsciiMessage msgParams = msg.substr(second_comma + 1, last_comma - second_comma - 1);
    if (msgOut.empty())
    {
        msgOut = msgParams;
        return Error::None;
    }

    msgOut.push_back(';');
    for (uint16_t i{0}; i != msgParams.length(); ++i) { msgOut.push_back(msgParams[i]); }

    return Error::None;
}

Error XmlConfigWriter::write(const AsciiMessage& msg)
{
    Error error{Error::None};
    if (!_initialized)
    {
        error = _initialize();
        if (error != Error::None) { return error; }
    }

    VN_DEBUG_0(msg.c_str() << std::endl);
    // start appending where needed
    const auto first_comma = msg.find(',');
    const auto second_comma = msg.find(',', first_comma + 1);
    const auto end = msg.find('*');

    const auto regId = StringUtils::fromString<uint8_t>(&msg[first_comma + 1], &msg[second_comma]);
    if (!regId.has_value())
    {
        error = Error::FileWriteFailed;
        return error;
    }

    switch (*regId)
    {
        case 5:
            error = augmentMessage(_reg5, msg);
            break;
        case 6:
            error = augmentMessage(_reg6, msg);
            break;
        case 7:
            error = augmentMessage(_reg7, msg);
            break;
        case 99:
        {
            const uint16_t commaCount = std::count(msg.begin(), msg.end(), ',');
            if (commaCount == 9)  // single-valued register
            {
                error = _write(msg.substr(second_comma + 1, end - second_comma - 1), regId.value());
            }
            else { error = augmentMessage(_reg99, msg); }
            break;
        }
        default:
            error = _write(msg.substr(second_comma + 1, end - second_comma - 1), regId.value());
    }

    return error;
}

Error XmlConfigWriter::_initialize()
{
    Error error{Error::None};
    auto decl = _doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";
    _doc.document_element().append_attribute("xmlns:xsd") = "http://www.w3.org/2001/XMLSchema";
    _doc.document_element().append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";

    Registers::System::Model modelReg;
    Registers::System::Serial serNumReg;
    Registers::System::FwVer fwVerReg;
    Registers::System::HwVer hwVerReg;

    error = sensor.readRegister(&modelReg);
    if (error != Error::None) { return error; }
    error = sensor.readRegister(&serNumReg);
    if (error != Error::None) { return error; }
    error = sensor.readRegister(&fwVerReg);
    if (error != Error::None) { return error; }
    error = sensor.readRegister(&hwVerReg);
    if (error != Error::None) { return error; }

    _root = _doc.append_child("VectorNav_Sensor");
    _root.append_child("Model").text().set(modelReg.model.c_str());

    char tmpBuf[32];
    std::snprintf(tmpBuf, sizeof(tmpBuf), "%d", hwVerReg.hwVer);
    _root.append_child("HardwareRevision").text().set(tmpBuf);
    std::snprintf(tmpBuf, sizeof(tmpBuf), "%d", serNumReg.serialNum);
    _root.append_child("SerialNumber").text().set(tmpBuf);
    _root.append_child("FirmwareVersion").text().set(fwVerReg.fwVer.c_str());

    _initialized = true;
    return error;
}

void XmlConfigWriter::finalize()
{
    if (!_initialized) { _initialize(); };

    if (!_reg5.empty()) _write(_reg5, 5);
    if (!_reg6.empty()) _write(_reg6, 6);
    if (!_reg7.empty()) _write(_reg7, 7);
    if (!_reg99.empty()) _write(_reg99, 99);

    _doc.save_file(_path.c_str());
}

Error XmlConfigWriter::_write(const AsciiMessage& msg, const uint16_t regId)
{
    // check if we have a name
    auto child = _root.append_child("Register");
    char buffer[250];
    std::snprintf(buffer, sizeof(buffer), "%d", regId);
    child.append_child("Id").text().set(buffer);

    if (const auto nameOpt = Registers::getConfigRegisterName(regId); nameOpt.has_value()) { child.append_child("Name").text().set(nameOpt.value()); }
    else { child.append_child("Name").text().set("unknown"); }

    std::snprintf(buffer, sizeof(buffer), "%.*s", static_cast<int>(msg.length()), msg.data());
    child.append_child("Values").text().set(buffer);

    return Error::None;
}

}  // namespace RegisterScan
}  // namespace VN
