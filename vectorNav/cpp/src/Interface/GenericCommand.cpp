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


#include "vectornav/Interface/GenericCommand.hpp"

#include "vectornav/Debug.hpp"
#include "vectornav/TemplateLibrary/String.hpp"

namespace VN
{
bool GenericCommand::isAwaitingResponse() const noexcept
{
    LockGuard lock(_mutex);
    return _awaitingResponse;
}

bool GenericCommand::hasValidResponse() const noexcept
{
    LockGuard lock(_mutex);
    return _hasValidResponse();
}

AsciiMessage GenericCommand::getResponse() const noexcept
{
    LockGuard lock(_mutex);
    if (!_hasValidResponse()) { return ""; }
    else { return _commandString; }
}

std::optional<Error> GenericCommand::getError() const noexcept
{
    LockGuard lock(_mutex);
    if (!_hasValidResponse()) { return std::nullopt; }
    if (isMatchingError(_commandString))
    {
        auto errorValue = _getErrorValue(_commandString);
        if (!errorValue.has_value()) { return std::nullopt; }
        return std::make_optional(static_cast<Error>(errorValue.value()));
    }
    else { return std::nullopt; }
}

AsciiMessage GenericCommand::getCommandString() noexcept
{
    LockGuard lock(_mutex);
    if (_hasValidResponse()) { return ""; }
    return _commandString;
}

std::optional<uint16_t> GenericCommand::_getErrorValue(const AsciiMessage& errIn) noexcept
{
    const uint8_t commaIdx = 6;  //$VNERR,
    if ((errIn.length() < 8) || errIn[commaIdx] != ',') { return false; }

    const size_t astereskIndex = errIn.find('*', commaIdx);
    if (astereskIndex == AsciiMessage::npos) { return false; }

    AsciiMessage errorNumString = StringUtils::extractBetween(errIn, ',', '*');
    //  errIn.begin() + commaIdx, errIn.begin() + astereskIndex;
    return StringUtils::fromStringHex<uint16_t>(errorNumString.begin(), errorNumString.end());
}

bool GenericCommand::isMatchingError(const AsciiMessage& errIn) noexcept
{
    if (!StringUtils::startsWith(errIn, "$VNERR,")) { return false; }
    auto errorValue = _getErrorValue(errIn);
    if (!errorValue.has_value()) { return false; }
    return VnErr_Synchronous::is_value(static_cast<Error>(errorValue.value()));
}

bool GenericCommand::isMatchingResponse(const AsciiMessage& responseToCheck, const time_point timestamp) noexcept
{
    LockGuard lock(_mutex);
    _awaitingResponse = false;
    _responseMatched = false;
    AsciiMessage stringToMatch{};
    std::snprintf(stringToMatch.begin(), 3 + 1 + _numCharToMatch, "$VN%s", _commandString.c_str());
    if (StringUtils::startsWith(responseToCheck, stringToMatch)) { _responseMatched = true; }
    else
    {
        if (isMatchingError(responseToCheck)) { _responseMatched = true; }
        else { VN_DEBUG_1("response NOT matched.\n Expected response:\t" + stringToMatch + "\nReceived response:\t" + responseToCheck); }
    }
    if (_responseMatched)
    {
        _commandString = responseToCheck;
        _responseTime = timestamp;
    }
    return _responseMatched;
}

void GenericCommand::prepareToSend() noexcept
{
    LockGuard lock(_mutex);
    _awaitingResponse = true;
    _responseMatched = false;
    _sentTime = now();
}

bool GenericCommand::_hasValidResponse() const noexcept
{
    // We shouldn't need to check awaiting response because if it is awaiting, the response should always be empty.
    return (!_awaitingResponse && _responseMatched);
}

}  // namespace VN
