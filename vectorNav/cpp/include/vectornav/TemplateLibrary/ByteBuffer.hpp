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

#ifndef VN_BYTEBUFFER_HPP_
#define VN_BYTEBUFFER_HPP_

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>

#include "vectornav/Debug.hpp"
#include "vectornav/Interface/Errors.hpp"
#if (VN_DEBUG_LEVEL > 0)
#include <array>
#include <iostream>
#include <string>
#endif

namespace VN
{

class ByteBuffer
{
public:
    // ------------------------------------------
    /*! \name Initialization */  //@{
    // ------------------------------------------
    ByteBuffer(const size_t capacity) : _buffer(new uint8_t[capacity]), _capacity(capacity) {}

    ByteBuffer(uint8_t* buffer, const size_t capacity, const size_t size = 0)
        : _buffer(buffer), _capacity(capacity), _tail(size), _size(size), _autoAllocated(false)
    {
    }

    ByteBuffer(const ByteBuffer& other, size_t offset)
        : _buffer(other._buffer),
          _capacity(other._capacity),
          _tail(other._tail),
          _head((other._head + offset) % other._capacity),
          _size(other._size.load() - offset),
          _autoAllocated(false) {};

    ~ByteBuffer()
    {
        if (_autoAllocated) { delete[] _buffer; }
    }

    ByteBuffer(const ByteBuffer& other) : ByteBuffer(other, 0) {};
    ByteBuffer& operator=(const ByteBuffer& other) = delete;
    ByteBuffer& operator=(ByteBuffer&& other) = delete;
    ByteBuffer(ByteBuffer&& other) = delete;

    bool operator==(const ByteBuffer& other) const noexcept
    {
        if (this->size() != other.size()) { return false; }
        for (size_t i = 0; i < this->size(); i++)
        {
            if (this->peek_unchecked(i) != other.peek_unchecked(i)) { return false; }
        }
        return true;
    }

    // ------------------------------------------
    /*! \name Byte Manipulation */  //@{
    // ------------------------------------------

    Errored get(uint8_t* outputBufferHead, const size_t numBytes) noexcept
    {
        if (peek(outputBufferHead, numBytes, 0) || discard(numBytes)) { return true; }
        else { return false; }
    }

    Errored peek(uint8_t* outputBufferHead, const size_t numBytesToPeek, const size_t startingIndex = 0) const noexcept
    {
        if (startingIndex + numBytesToPeek > _size) { return true; }
        peek_unchecked(outputBufferHead, numBytesToPeek, startingIndex);
        return false;
    }

    std::optional<uint8_t> peek(const size_t index = 0) const noexcept
    {
        if (_size < (index + 1)) { return std::nullopt; }
        return std::make_optional(peek_unchecked(index));
    }

    void peek_unchecked(uint8_t* outputBufferHead, const size_t numBytesToPeek, const size_t startingIndex = 0) const noexcept
    {
        const size_t numLinearBytesAvail = numLinearBytesToPeek(startingIndex);
        if (numLinearBytesAvail >= numBytesToPeek) { memcpy(outputBufferHead, _buffer + ((_head + startingIndex) % _capacity), numBytesToPeek); }
        else
        {
            memcpy(outputBufferHead, _buffer + ((_head + startingIndex) % _capacity), numLinearBytesAvail);
            size_t numBytesLeftToPeek = numBytesToPeek - numLinearBytesAvail;
            memcpy(outputBufferHead + numLinearBytesAvail, _buffer, numBytesLeftToPeek);
        }
    }

    void reset() noexcept { discard(_size); }

    uint8_t peek_unchecked(const size_t index = 0) const noexcept { return _buffer[(_head + index) % _capacity]; }

    const uint8_t* peek_ptr_unchecked(size_t index = 0) const noexcept { return &_buffer[(_head + index) % _capacity]; }

    Errored put(const uint8_t* inputBufferHead, size_t inputBufferSize) noexcept
    {
        if (inputBufferSize == 0) { return false; }

        if (_size + inputBufferSize > _capacity)
        {
            VN_DEBUG_1("Buffer overflow.");
            return true;
        }

        const size_t numBytesLinearlyAvailable = numLinearBytesToPut();
        if (numBytesLinearlyAvailable >= inputBufferSize) { memcpy(_buffer + _tail, inputBufferHead, inputBufferSize); }
        else
        {
            memcpy(_buffer + _tail, inputBufferHead, numBytesLinearlyAvailable);
            memcpy(_buffer, inputBufferHead + numBytesLinearlyAvailable, inputBufferSize - numBytesLinearlyAvailable);
        }

        VN_DEBUG_2("putting bytes: " + std::to_string(inputBufferSize));

        return put(inputBufferSize);
    }

    Errored put(size_t numBytes)  // allows updating tail when bytes directly copied to buffer
    {
        if (numBytes == 0) { return false; }
        if (_size + numBytes > _capacity) { return true; }
        _tail = (_tail + numBytes) % _capacity;
        _size += numBytes;
        return false;
    }

    Errored discard(const size_t numBytes) noexcept
    {
        if (numBytes == 0) { return false; }
        if (numBytes > _size) { return true; }
        _head = (_head + numBytes) % _capacity;
        _size -= numBytes;
        return false;
    }

    size_t numLinearBytesToPeek(const size_t startingIndex = 0) const noexcept
    {
        if (startingIndex >= _size) { return 0; }
        return std::min<size_t>(_capacity - ((_head + startingIndex) % _capacity), _size - startingIndex);
    }

    size_t numLinearBytesToPut() const noexcept
    {
        size_t emptyBytes = _capacity - _size;
        if (_tail + emptyBytes <= _capacity) { return emptyBytes; }
        else { return (_capacity - _tail); }
    }

    std::optional<size_t> find(const uint8_t byteToFind, const size_t idxToBegin = 0) const noexcept
    {
        if (_size <= idxToBegin) { return std::nullopt; }

        const size_t tail = (_head + _size) % _capacity;
        const size_t startIndex = (_head + idxToBegin) % _capacity;

        const_iterator bufferBegin = _begin();
        const_iterator foundIterator;
        if (tail <= startIndex)  // wraps around
        {
            foundIterator = std::find(bufferBegin + startIndex, _end(), byteToFind);

            bool valueHasBeenFound = foundIterator != _end();
            if (!valueHasBeenFound)
            {
                foundIterator = std::find(bufferBegin, bufferBegin + tail, byteToFind);

                valueHasBeenFound = foundIterator != (bufferBegin + tail);
                if (!valueHasBeenFound) { return std::nullopt; }
            }
        }
        else  // tail > startIndex
        {
            foundIterator = std::find(bufferBegin + startIndex, bufferBegin + tail, byteToFind);
            bool valueHasBeenFound = foundIterator != (bufferBegin + tail);
            if (!valueHasBeenFound) { return std::nullopt; }
        }

        size_t foundIndex = static_cast<size_t>((_end() - (bufferBegin + _head)) + (foundIterator - bufferBegin)) % _capacity;

        return std::make_optional(foundIndex);
    }

    // ------------------------------------------
    /*! \name State Checking */  //@{
    // ------------------------------------------

    bool isEmpty() const noexcept { return (_size == 0); }
    bool isFull() const noexcept { return (_size == _capacity); }
    size_t capacity() const noexcept { return _capacity; }
    size_t size() const noexcept { return _size; }
    uint8_t* data() const noexcept { return _buffer; }
    const uint8_t* head() const noexcept { return &_buffer[_head]; }
    const uint8_t* tail() const noexcept { return &_buffer[_tail]; }

#if (VN_DEBUG_LEVEL > 0)
    void printBuffer() const
    {
        for (size_t i = 0; i < size(); i++) { std::cout << std::hex << peek(i).value() << " "; }
        std::cout << std::endl;
    }
#endif
private:
    using const_iterator = const uint8_t*;

    uint8_t* _buffer;
    size_t _capacity;
    size_t _tail = 0;
    size_t _head = 0;
    std::atomic<size_t> _size = 0;
    bool _autoAllocated = true;

    constexpr const_iterator _begin() const noexcept { return _buffer; }
    const_iterator _end() const noexcept { return _begin() + _capacity; }
};

}  // namespace VN

#endif  // VN_BYTEBUFFER_HPP_
