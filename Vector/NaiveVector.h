#pragma once

#include <stdexcept>

namespace naive {

template <typename T>
class Vector
{
public:
    class VectorIterator
    {
    public:
        VectorIterator() = default;
        VectorIterator(T * ptr);
        VectorIterator(const VectorIterator & it);

    public:
        const T & operator*() const
        { return *m_ptr; }

        T & operator*()
        { return *m_ptr; }

        VectorIterator & operator++();
        VectorIterator operator++(int);

        VectorIterator & operator--();
        VectorIterator operator--(int);

    private:
        T * m_ptr = nullptr;
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

    T & front()
    { return *begin(); }

    T & back();

public:
    // TODO: cbegin, const front etc
    VectorIterator begin();
    VectorIterator end();

public:
    bool empty()
    { return m_size == 0; }

    size_t size()
    { return m_size; }

    size_t capacity()
    { return m_capacity; }

public:
    void push_back(const T & value);
    void push_back(T && value);

public:
    void clear();

private:
    T * m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;
};

template <typename T>
Vector<T>::VectorIterator::VectorIterator(T * ptr) :
    m_ptr(ptr)
{
}

template <typename T>
Vector<T>::VectorIterator::VectorIterator(const VectorIterator & it) :
    m_ptr(it.m_ptr)
{
}

template <typename T>
typename Vector<T>::VectorIterator & Vector<T>::VectorIterator::operator++()
{
    ++m_ptr;
    return *this;
}

template <typename T>
typename Vector<T>::VectorIterator Vector<T>::VectorIterator::operator++(int)
{
    VectorIterator it = *this;
    ++m_ptr;
    return it;
}

template <typename T>
typename Vector<T>::VectorIterator & Vector<T>::VectorIterator::operator--()
{
    --m_ptr;
    return *this;
}

template <typename T>
typename Vector<T>::VectorIterator Vector<T>::VectorIterator::operator--(int)
{
    VectorIterator it = *this;
    --m_ptr;
    return it;
}

template <typename T>
Vector<T>::Vector(size_t size) :
    m_size(0),
    m_capacity(size)
{
    m_data = new T[size];
}

template <typename T>
Vector<T>::Vector(size_t size, const T & value) :
    m_size(size),
    m_capacity(size)
{
    m_data = new T[size];

    for (size_t i = 0; i < size; ++i) {
        m_data[i] = value;
    }
}

template <typename T>
Vector<T>::Vector(const Vector & vector) :
    m_size(vector.m_size),
    m_capacity(vector.m_capacity)
{
    m_data = new T[m_capacity];
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = vector.m_data[i];
    }
}

template <typename T>
Vector<T>::Vector(Vector && vector) :
    m_size(vector.m_size),
    m_capacity(vector.m_capacity)
{
    m_data = vector.m_data;
    vector.m_data = nullptr;

    vector.m_size = 0;
    vector.m_capacity = 0;
}

template <typename T>
Vector<T>::~Vector()
{
    delete []m_data;
}

template <typename T>
Vector<T> & Vector<T>::operator=(const Vector<T> & vector)
{
    m_size = vector.m_size;

    if (m_capacity < m_size) {
        m_capacity = vector.m_capacity;
        delete []m_data;
        m_data = new T[m_capacity];
    }

    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = vector.m_data[i];
    }

    return *this;
}

template <typename T>
Vector<T> & Vector<T>::operator=(Vector<T> && vector)
{
    m_size = vector.m_size;
    m_capacity = vector.m_capacity;

    delete []m_data;
    m_data = vector.m_data;

    vector.m_capacity = vector.m_size = 0;
    vector.m_data = nullptr;

    return *this;
}

template <typename T>
T & Vector<T>::at(size_t pos)
{
    if (pos >= m_size) {
        throw std::out_of_range("");
    }

    return m_data[pos];
}

template <typename T>
const T & Vector<T>::at(size_t pos) const
{
    if (pos >= m_size) {
        throw std::out_of_range("");
    }

    return m_data[pos];
}

template <typename T>
T & Vector<T>::back()
{
    auto tmp = end();
    --tmp;
    return *tmp;
}

template <typename T>
typename Vector<T>::VectorIterator Vector<T>::begin()
{
    return Vector<T>::VectorIterator(m_data);
}

template <typename T>
typename Vector<T>::VectorIterator Vector<T>::end()
{
    if (m_size == 0) {
        return VectorIterator();
    }

    return VectorIterator(m_data + m_size);
}

template <typename T>
void Vector<T>::push_back(const T & value)
{
    if (m_capacity == m_size) {
        if (m_capacity == 0) {
            m_capacity = 1;
        } else {
            m_capacity *= 2;
        }

        T * tmp = m_data;
        m_data = new T[m_capacity];

        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = tmp[i];
        }
    }

    m_data[m_size] = value;
    ++m_size;
}

template <typename T>
void Vector<T>::push_back(T && value)
{
    if (m_capacity == m_size) {
        if (m_capacity == 0) {
            m_capacity = 1;
        }
        else {
            m_capacity *= 2;
        }

        T * tmp = m_data;
        m_data = new T[m_capacity];

        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = tmp[i];
        }
    }

    m_data[m_size] = std::move(value);
    ++m_size;
}

template <typename T>
void Vector<T>::clear()
{
    m_size = 0;
}

}
