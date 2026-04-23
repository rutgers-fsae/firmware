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

#ifndef VN_TIMER_PC_HPP_
#define VN_TIMER_PC_HPP_

#include "vectornav/HAL/Timer_Base.hpp"

namespace VN
{

using time_point = std::chrono::steady_clock::time_point;
inline time_point now() { return std::chrono::steady_clock::now(); }

inline std::ostream& operator<<(std::ostream& os, const time_point& tp) noexcept
{
    auto duration = tp.time_since_epoch();
    os << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms";
    return os;
}

class Timer : Timer_Base
{
public:
    Timer() {}
    Timer(Microseconds timerLength) : _timerLength(timerLength) {}

    void setTimerLength(Microseconds timerLength) noexcept override final
    {
        _timerLength = timerLength;
        _hasStarted = false;
    }

    void start() noexcept override final
    {
        _timeStarted = Clock::now();
        _hasStarted = true;
    }

    bool hasTimedOut() const noexcept override final
    {
        if (!_hasStarted) { return false; }
        return (Clock::now() - _timeStarted) >= _timerLength;
    }

    Nanoseconds timeElapsed() const noexcept override final
    {
        // Used only for profiling
        if (!_hasStarted) { return 0ns; }
        return Clock::now() - _timeStarted;
    }

private:
    using Clock = std::chrono::steady_clock;

    Clock::time_point _timeStarted;
    Microseconds _timerLength{0};
    bool _hasStarted = false;
};

}  // namespace VN
#endif  // VN_TIMER_PC_HPP_
