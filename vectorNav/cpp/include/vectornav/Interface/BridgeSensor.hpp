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

#ifndef VN_BRIDGESENSOR_HPP_
#define VN_BRIDGESENSOR_HPP_

#include "vectornav/Interface/Sensor.hpp"

namespace VN
{
class BridgeSensor : public Sensor
{
public:
    using Sensor::Sensor;

    ~BridgeSensor() { disableMonitor(); }

    using Sensor::connect;
    Error connect(const Serial_Base::PortName& portName, const BaudRate baudRate, const bool monitorAsyncErrors)
    {
        Error error = connect(portName, baudRate);
        if (error != Error::None) { return error; }
        _initMonitor(monitorAsyncErrors);
        return error;
    }

    using Sensor::autoConnect;
    Error autoConnect(const Serial_Base::PortName& portName, const bool monitorAsyncErrors)
    {
        Error error = autoConnect(portName);
        if (error != Error::None) { return error; }
        _initMonitor(monitorAsyncErrors);
        return error;
    }

    using Sensor::reset;
    Error reset(const bool monitorAsyncErrors)
    {
        disableMonitor();
        Error error = reset();
        if (error != Error::None) { return error; }
        _initMonitor(monitorAsyncErrors);
        return error;
    }

    bool asyncErrorThrowingEnabled() { return _asyncErrorQueueThrowingEnabled; }

    void disableMonitor()
    {
        _asyncErrorQueueThrowingEnabled = false;
        if (_asyncErrorThread && _asyncErrorThread->joinable()) { _asyncErrorThread->join(); }
    }

private:
    std::atomic<bool> _asyncErrorQueueThrowingEnabled = false;
    std::unique_ptr<Thread> _asyncErrorThread = nullptr;

    void _initMonitor(const bool monitorAsyncErrors)
    {
        if (monitorAsyncErrors)
        {
            _asyncErrorQueueThrowingEnabled = true;
            _asyncErrorThread = std::make_unique<Thread>(&BridgeSensor::_threadMonitor, this);
        }
    }

    void _threadMonitor()
    {
        while (_asyncErrorQueueThrowingEnabled)
        {
            VN::thisThread::sleepFor(1ms);
            auto asyncError = Sensor::getNextAsyncError();
            if (asyncError)
            {
                std::cerr << *asyncError << std::endl;
                throw std::runtime_error(errorCodeToString(asyncError->error));
            }
        }
    }
};
}  // namespace VN

#endif  // VN_BRIDGESENSOR_HPP_
