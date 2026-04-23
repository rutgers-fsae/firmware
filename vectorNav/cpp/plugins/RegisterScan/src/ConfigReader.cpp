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

#include "vectornav/ConfigReader.hpp"

namespace VN
{

namespace RegisterScan
{

Error AsciiConfigReader::next(AsciiMessage& msg)
{
    Error error{Error::None};
    if (file.read(msg.data(), msg.capacity(), '\n')) { error = Error::FileReadFailed; }
    return error;
}

Error XmlConfigReader::next(AsciiMessage& msg)
{
    Error error{Error::None};
    if (_doc.document_element().root().empty())
    {
        if (!_doc.load_file(_path.c_str()))
        {
            error = Error::FileReadFailed;
            return error;
        }
        _next_node = _doc.child("VectorNav_Sensor").child("Register");
    }

    _curr_node = _next_node;

    if (!_curr_node)
    {
        error = Error::FileReadFailed;
        return error;
    }

    if (double_config)
    {
        const auto delimeter = std::strchr(_curr_node.child("Values").first_child().value(), ';');
        std::snprintf(msg.data(), msg.capacity(), "$VNWRG,%s,%s,2*XX", _curr_node.child("Id").first_child().value(), delimeter + 1);
        double_config = false;
    }
    else
    {
        if (const auto delimieter = std::strchr(_curr_node.child("Values").first_child().value(), ';'); delimieter != nullptr)
        {
            double_config = true;
            const int length = static_cast<int>(delimieter - _curr_node.child("Values").first_child().value());
            std::snprintf(msg.data(), msg.capacity(), "$VNWRG,%s,%.*s,1*XX", _curr_node.child("Id").first_child().value(), length,
                          _curr_node.child("Values").first_child().value());
            return error;
        }
        else
        {
            snprintf(msg.data(), msg.capacity(), "$VNWRG,%s,%s*XX", _curr_node.child("Id").first_child().value(),
                     _curr_node.child("Values").first_child().value());
        }
    }

    _next_node = _curr_node.next_sibling();

    return error;
}

}  // namespace RegisterScan
}  // namespace VN
