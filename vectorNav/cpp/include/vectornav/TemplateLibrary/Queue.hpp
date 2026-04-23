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

#ifndef VN_QUEUE_HPP_
#define VN_QUEUE_HPP_

#include <array>
#include <optional>

#include "vectornav/Config.hpp"
#include "vectornav/HAL/Mutex.hpp"

namespace VN
{

template <class ItemType, uint16_t Capacity>
class Queue
{
public:
    Queue() = default;

    Queue(const Queue& other) = delete;
    Queue& operator=(const Queue& other) = delete;
    Queue(Queue&& other) noexcept = default;
    Queue& operator=(Queue&& other) noexcept = delete;

    void put(const ItemType& item) noexcept
    {  // This will fail to put() if the buffer is full
        if (!_full)
        {
            _buffer[_tail] = item;
            _tail = (_tail + 1) % Capacity;
            _full = _tail == _head;
        }
    }

    void put(ItemType&& item) noexcept
    {  // This will fail to put() if the buffer is full
        if (!_full)
        {
            _buffer[_tail] = std::move(item);
            _tail = (_tail + 1) % Capacity;
            _full = _tail == _head;
        }
    }

    std::optional<ItemType> get() noexcept
    {
        if (_isEmpty()) { return std::nullopt; }
        // Read data and advance the head (we now have a free space)
        auto item = std::move(_buffer[_head]);
        _full = false;
        _head = (_head + 1) % Capacity;
        return item;
    }

    std::optional<ItemType> peek() const noexcept
    {
        if (_isEmpty()) { return std::nullopt; }
        // Read data
        auto item = _buffer[_head];
        return item;
    }

    void reset() noexcept
    {
        _tail = _head;
        _full = false;
    }

    bool isEmpty() const noexcept { return _isEmpty(); }

    bool isFull() const noexcept { return _full; }

    constexpr uint16_t capacity() const noexcept { return Capacity; }

    uint16_t size() const noexcept { return _full ? Capacity : (_tail + Capacity - _head) % Capacity; }

    void popBack() noexcept
    {
        if (!_isEmpty())
        {
            _full = false;
            _tail = (_tail + Capacity - 1) % Capacity;
        }
    }
    using value_type = std::optional<ItemType>;  // Used to be able to arbitrate away implementation in Sensor
private:
    bool _isEmpty() const noexcept { return (!_full && (_tail == _head)); }
    std::array<ItemType, Capacity> _buffer;
    uint16_t _tail = 0;
    uint16_t _head = 0;
    bool _full = false;
};

template <class ItemType, uint16_t Capacity>
class Queue_Mutexed : public Queue<ItemType, Capacity>
{
public:
    using Queue<ItemType, Capacity>::Queue;
    using Base = Queue<ItemType, Capacity>;

    void put(const ItemType& item) noexcept
    {  // This will fail to put() if the buffer is full
        LockGuard lock(_mutex);
        Base::put(item);
    }

    void put(ItemType&& item) noexcept
    {  // This will fail to put() if the buffer is full
        LockGuard lock(_mutex);
        Base::put(std::move(item));
    }

    std::optional<ItemType> get() noexcept
    {
        LockGuard lock(_mutex);
        return Base::get();
    }

    std::optional<ItemType> peek() const noexcept
    {
        LockGuard lock(_mutex);
        return Base::peek();
    }

    void reset() noexcept
    {
        LockGuard lock(_mutex);
        Base::reset();
    }

    bool isEmpty() const noexcept
    {
        LockGuard lock(_mutex);
        return Base::isEmpty();
    }

    bool isFull() const noexcept
    {
        LockGuard lock(_mutex);
        return Base::isFull();
    }

    uint16_t capacity() const noexcept { return Base::capacity(); }

    uint16_t size() const noexcept
    {
        LockGuard lock(_mutex);
        return Base::size();
    }

    void popBack() noexcept
    {
        LockGuard lock(_mutex);
        Base::popBack();
    }
    using value_type = std::optional<ItemType>;  // Used to be able to arbitrate away implementation in Sensor
private:
    mutable Mutex _mutex;
};

}  // namespace VN

#endif  // VN_QUEUE_HPP_
