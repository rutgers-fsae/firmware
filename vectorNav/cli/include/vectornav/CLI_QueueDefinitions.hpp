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

#ifndef VN_CLI_QUEUE_DEFINITIONS_HPP_
#define VN_CLI_QUEUE_DEFINITIONS_HPP_

#pragma managed(push, off)
#include "vectornav/Implementation/QueueDefinitions.hpp"
#pragma managed(pop)

namespace VNSDK
{

public ref class ManagedQueuePointer
{
    private:
        VN::PacketQueue_Interface* _queueInterface;
        bool _isOwner;
    
    public:
        ManagedQueuePointer(VN::PacketQueue_Interface* queueInterface)
        {
            _queueInterface = queueInterface;
            _isOwner = false;
        }

        ~ManagedQueuePointer()
        {
            this->!ManagedQueuePointer();
        }

        !ManagedQueuePointer()
        {
            if (_isOwner)
            {
                delete _queueInterface;
            }
            _queueInterface = nullptr;
        }

        VN::PacketQueue_Interface* ToNativePointer()
        {
            return _queueInterface;
        }
};

} // namespace VNSDK


#endif  // VN_CLI_QUEUE_DEFINITIONS_HPP_