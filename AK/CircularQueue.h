#pragma once

#include <AK/Assertions.h>
#include <AK/Types.h>

namespace AK {

template<typename T, int Capacity>
class CircularQueue {
public:
    CircularQueue()
    {
    }

    ~CircularQueue()
    {
        clear();
    }

    void clear()
    {
        for (int i = 0; i < m_size; ++i)
            elements()[(m_head + i) % Capacity].~T();

        m_head = 0;
        m_size = 0;
    }

    bool is_empty() const { return !m_size; }
    int size() const { return m_size; }

    int capacity() const { return Capacity; }

    void enqueue(T&& value)
    {
        auto& slot = elements()[(m_head + m_size) % Capacity];
        if (m_size == Capacity)
            slot.~T();

        new (&slot) T(value);
        if (m_size == Capacity)
            m_head = (m_head + 1) % Capacity;
        else
            ++m_size;
    }

    void enqueue(const T& value)
    {
        enqueue(T(value));
    }

    T dequeue()
    {
        ASSERT(!is_empty());
        auto& slot = elements()[m_head];
        T value = move(slot);
        slot.~T();
        m_head = (m_head + 1) % Capacity;
        --m_size;
        return value;
    }

    const T& at(int index) const { return elements()[(m_head + index) % Capacity]; }

    const T& first() const { return at(0); }
    const T& last() const { return at(size() - 1); }

    class ConstIterator {
    public:
        bool operator!=(const ConstIterator& other) { return m_index != other.m_index; }
        ConstIterator& operator++()
        {
            m_index = (m_index + 1) % Capacity;
            if (m_index == m_queue.m_head)
                m_index = m_queue.m_size;
            return *this;
        }

        const T& operator*() const { return m_queue.elements()[m_index]; }

    private:
        friend class CircularQueue;
        ConstIterator(const CircularQueue& queue, const int index)
            : m_queue(queue)
            , m_index(index)
        {
        }
        const CircularQueue& m_queue;
        int m_index { 0 };
    };

    ConstIterator begin() const { return ConstIterator(*this, m_head); }
    ConstIterator end() const { return ConstIterator(*this, size()); }

    int head_index() const { return m_head; }

protected:
    T* elements() { return reinterpret_cast<T*>(m_storage); }
    const T* elements() const { return reinterpret_cast<const T*>(m_storage); }

    friend class ConstIterator;
    alignas(T) u8 m_storage[sizeof(T) * Capacity];
    int m_size { 0 };
    int m_head { 0 };
};

}

using AK::CircularQueue;
