#pragma once

#include <stdexcept>

namespace naive {

template <typename T>
class Vector
{
public:
    class ConstIterator;

    class Iterator
    {
    public:
        Iterator() = default;
        Iterator(T * ptr);
        Iterator(const Iterator & it);

    public:
        const T & operator*() const
        { return *m_ptr; }

        T & operator*()
        { return *m_ptr; }

        Iterator & operator++();
        Iterator operator++(int);

        Iterator & operator--();
        Iterator operator--(int);

        bool operator==(const Iterator & it);
        bool operator==(const ConstIterator & it);
        bool operator!=(const Iterator & it);
        bool operator!=(const ConstIterator & it);

    private:
        friend class ConstIterator;

    private:
        T * m_ptr = nullptr;
    };

    class ConstIterator
    {
    public:
        ConstIterator() = default;
        ConstIterator(const T * ptr);
        ConstIterator(const ConstIterator & it);
        ConstIterator(const Iterator & it);

    public:
        const T & operator*() const
        { return *m_ptr; }

        ConstIterator & operator++();
        ConstIterator operator++(int);

        ConstIterator & operator--();
        ConstIterator operator--(int);

        bool operator==(const ConstIterator & it);
        bool operator!=(const ConstIterator & it);

    private:
        friend class Iterator;

    private:
        const T * m_ptr = nullptr;
    };

public:
    Vector() = default;
    explicit Vector(size_t size);
    Vector(size_t size, const T & value);
    Vector(const Vector & vector);
    Vector(Vector && vector);

    ~Vector();

public:
    Vector & operator=(const Vector & vector);
    Vector & operator=(Vector && vector);

public:
    T & operator[](size_t pos)
    { return m_data[pos]; }

    const T & operator[](size_t pos) const
    { return m_data[pos]; }

    T & at(size_t pos);
    const T & at(size_t pos) const;

    T & front();
    const T & front() const;

    T & back();
    const T & back() const;

public:
    Iterator begin();
    ConstIterator cbegin() const;

    Iterator end();
    ConstIterator cend() const;

public:
    bool empty()
    { return m_size == 0; }

    size_t size()
    { return m_size; }

    size_t capacity()
    { return m_capacity; }

    void reserve(size_t capacity);

public:
    void clear();

    Iterator insert(ConstIterator pos, const T & value);
    Iterator insert(ConstIterator pos, T && value);

    template <typename ... Args>
    Iterator emplace(ConstIterator pos, Args && ... args);

    Iterator erase(Iterator pos);
    Iterator erase(Iterator first, Iterator last);

    void push_back(const T & value);
    void push_back(T && value);

    template <typename ... Args>
    void emplace_back(Args && ... args);

    void pop_back();

    void resize(size_t size);

private:
    T * m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;
};

}

#include "NaiveVector.inc"
