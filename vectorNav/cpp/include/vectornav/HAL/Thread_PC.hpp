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

#ifndef VN_THREAD_PC_HPP_
#define VN_THREAD_PC_HPP_

#include <thread>
#if (_WIN32)
#define NOMINMAX 1
#include "vectornav/Debug.hpp"
#include "windows.h"
#endif
#include "vectornav/HAL/Thread_Base.hpp"
// #include <Windows.h> // Necessary for priority

namespace VN
{

class Thread : public Thread_Base
{
public:
    Thread() {};
    Thread(const Thread&) = delete;

    template <typename Callable, typename... Args>
    explicit Thread(Callable&& f, Args&&... args) : _thread(std::forward<Callable>(f), std::forward<Args>(args)...){};

    ~Thread() override
    {
        if (_thread.joinable()) { _thread.join(); }
    }

    void join() override final { _thread.join(); }

    void detach() override final { _thread.detach(); }

    bool joinable() const override final { return _thread.joinable(); }

    // void setHighestPriority() {
    //     if (_thread.joinable()) {
    //         std::cout << "joinable.\n";
    //     } else {
    //         std::cout << "Not joinable.\n";
    //         // Should be exception?
    //     }
    //     // HANDLE threadHandle = static_cast<HANDLE>(_thread.native_handle());
    //     // ThreadHandle_t threadHandle;
    //     // HANDLE threadHandle = (HANDLE*)(_thread.native_handle());
    //     auto threadHandle = _thread.native_handle();
    //     // HANDLE myHandle = OpenThread(THREAD_ALL_ACCESS, true, threadHandle);
    //     BOOL success = SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST);
    //     //https://stackoverflow.com/questions/62751283/windowswhy-does-stdthreadnative-handle-return-a-value-of-type-long-long-u if (!success) {
    //         DWORD lastError = GetLastError();
    //         // 6 is ERROR_INVALID_HANDLE
    //         throw(std::system_error(lastError, std::system_category(), "Error elevating thread priority"));
    //     }
    // }

private:
    std::thread _thread;
};

namespace thisThread
{
inline void sleepFor(const Microseconds sleepDuration) noexcept
{
    static bool _init = true;
    if (_init)
    {
#if (_WIN32)
        // Set the global Windows timer resolution to 1ms (max resolution). Windows resets this at the close of the application
        // It doesn't guarantee a 1ms sleep will actually be 1ms, but seems to limit it to ~5ms.
        MMRESULT result = timeBeginPeriod(1);
        VN_ASSERT(result == TIMERR_NOERROR);
#endif
        _init = false;
    }
    std::this_thread::sleep_for(sleepDuration);
};
}  // namespace thisThread

}  // namespace VN
#endif  // VN_THREAD_PC_HPP_
