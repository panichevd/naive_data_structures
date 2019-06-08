namespace naive {

template <typename T>
Vector<T>::Iterator::Iterator(T * ptr) :
    m_ptr(ptr)
{
}

template <typename T>
Vector<T>::Iterator::Iterator(const Iterator & it) :
    m_ptr(it.m_ptr)
{
}

template <typename T>
typename Vector<T>::Iterator & Vector<T>::Iterator::operator++()
{
    ++m_ptr;
    return *this;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::Iterator::operator++(int)
{
    Iterator it = *this;
    ++m_ptr;
    return it;
}

template <typename T>
typename Vector<T>::Iterator & Vector<T>::Iterator::operator--()
{
    --m_ptr;
    return *this;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::Iterator::operator--(int)
{
    Iterator it = *this;
    --m_ptr;
    return it;
}

template <typename T>
bool Vector<T>::Iterator::operator==(const Iterator & it)
{
    return m_ptr == it.m_ptr;
}

template <typename T>
bool Vector<T>::Iterator::operator==(const ConstIterator & it)
{
    return m_ptr == it.m_ptr;
}

template <typename T>
bool Vector<T>::Iterator::operator!=(const Iterator & it)
{
    return m_ptr != it.m_ptr;
}

template <typename T>
bool Vector<T>::Iterator::operator!=(const ConstIterator & it)
{
    return m_ptr != it.m_ptr;
}

template <typename T>
Vector<T>::ConstIterator::ConstIterator(const T * ptr) :
    m_ptr(ptr)
{
}

template <typename T>
Vector<T>::ConstIterator::ConstIterator(const ConstIterator & it) :
    m_ptr(it.m_ptr)
{
}

template <typename T>
Vector<T>::ConstIterator::ConstIterator(const Iterator & it) :
    m_ptr(it.m_ptr)
{
}

template <typename T>
typename Vector<T>::ConstIterator & Vector<T>::ConstIterator::operator++()
{
    ++m_ptr;
    return *this;
}

template <typename T>
typename Vector<T>::ConstIterator Vector<T>::ConstIterator::operator++(int)
{
    ConstIterator it = *this;
    ++m_ptr;
    return it;
}

template <typename T>
typename Vector<T>::ConstIterator & Vector<T>::ConstIterator::operator--()
{
    --m_ptr;
    return *this;
}

template <typename T>
typename Vector<T>::ConstIterator Vector<T>::ConstIterator::operator--(int)
{
    ConstIterator it = *this;
    --m_ptr;
    return it;
}

template <typename T>
bool Vector<T>::ConstIterator::operator==(const ConstIterator & it)
{
    return m_ptr == it.m_ptr;
}

template <typename T>
bool Vector<T>::ConstIterator::operator!=(const ConstIterator & it)
{
    return m_ptr != it.m_ptr;
}

template <typename T>
Vector<T>::Vector(size_t size) :
    m_size(0),
    m_capacity(size)
{
    m_data = new T[m_capacity];
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
T & Vector<T>::front()
{
    return *begin();
}

template <typename T>
const T & Vector<T>::front() const
{
    return *cbegin();
}

template <typename T>
T & Vector<T>::back()
{
    auto tmp = end();
    --tmp;
    return *tmp;
}

template <typename T>
const T & Vector<T>::back() const
{
    auto tmp = cend();
    --tmp;
    return *tmp;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::begin()
{
    return Vector<T>::Iterator(m_data);
}

template <typename T>
typename Vector<T>::ConstIterator Vector<T>::cbegin() const
{
    return Vector<T>::ConstIterator(m_data);
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::end()
{
    return Iterator(m_data + m_size);
}

template <typename T>
typename Vector<T>::ConstIterator Vector<T>::cend() const
{
    return ConstIterator(m_data + m_size);
}

template <typename T>
void Vector<T>::reserve(size_t capacity)
{
    if (capacity > m_capacity) {
        T * tmp = m_data;
        m_data = new T[capacity];
        for (unsigned i = 0; i < m_size; ++i) {
            m_data[i] = std::move(tmp[i]);
        }
        delete []tmp;

        m_capacity = capacity;
    }
}

template <typename T>
void Vector<T>::clear()
{
    m_size = 0;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::insert(Vector<T>::ConstIterator pos, const T & value)
{
    if (m_capacity == m_size) {
        size_t capacity = 0;
        if (m_capacity == 0) {
            capacity = 1;
        } else {
            capacity = 2 * m_capacity;
        }

        auto it = begin();
        auto end_it = end();

        T * tmp = m_data;
        m_data = new T[capacity];
        
        size_t i = 0;
        for (; it != pos; ++i, ++it) {
            m_data[i] = std::move(*it);
        }

        m_data[i] = value;

        Iterator ret(m_data + i);
        ++i;

        for (; it != end_it; ++i, ++it) {
            m_data[i] = std::move(*it);
        }
       
        delete []tmp;

        m_capacity = capacity;
        ++m_size;
        return ret;
    }

    auto it = end();
    for (; it != pos; --it) {
        auto prev = it;
        --prev;

        *it = std::move(*prev);
    }

    *it = value;
    ++m_size;
    return it;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::insert(Vector<T>::ConstIterator pos, T && value)
{
    if (m_capacity == m_size) {
        size_t capacity = 0;
        if (m_capacity == 0) {
            capacity = 1;
        }
        else {
            capacity = 2 * m_capacity;
        }

        auto it = begin();
        auto end_it = end();

        T * tmp = m_data;
        m_data = new T[capacity];

        size_t i = 0;
        for (; it != pos; ++i, ++it) {
            m_data[i] = std::move(*it);
        }

        m_data[i] = std::move(value);

        Iterator ret(m_data + i);
        ++i;

        for (; it != end_it; ++i, ++it) {
            m_data[i] = std::move(*it);
        }

        delete[]tmp;

        m_capacity = capacity;
        ++m_size;
        return ret;
    }

    auto it = end();
    for (; it != pos; --it) {
        auto prev = it;
        --prev;

        *it = std::move(*prev);
    }

    *it = std::move(value);
    ++m_size;
    return it;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::erase(Vector<T>::Iterator pos)
{
    auto next = pos;
    ++next;

    auto ret = pos;

    for (; next != end(); ++pos, ++next) {
        *pos = std::move(*next);
    }

    --m_size;
    return ret;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::erase(Iterator first, Iterator last)
{
	auto ret_it = first;
	auto end_it = end();

	for (Iterator it = last; first != last; ++first) {
		if (it != end_it) {
			*first = std::move(*it);
			++it;
		}
		--m_size;
	}

	return ret_it;
}

template <typename T>
template <typename ... Args>
typename Vector<T>::Iterator Vector<T>::emplace(Vector<T>::ConstIterator pos, Args && ... args)
{
    if (m_capacity == m_size) {
        size_t capacity = 0;
        if (m_capacity == 0) {
            capacity = 1;
        } else {
            capacity = 2 * m_capacity;
        }

        auto it = begin();
        auto end_it = end();

        T * tmp = m_data;
        m_data = new T[capacity];

        size_t i = 0;
        for (; it != pos; ++i, ++it) {
            m_data[i] = std::move(*it);
        }

        m_data[i] = T(std::forward<Args>(args)...);

        Iterator ret(m_data + i);
        ++i;

        for (; it != end_it; ++i, ++it) {
            m_data[i] = std::move(*it);
        }

        delete[]tmp;

        m_capacity = capacity;
        ++m_size;
        return ret;
    }

    auto it = end();
    for (; it != pos; --it) {
        auto prev = it;
        --prev;

        *it = std::move(*prev);
    }

    *it = T(std::forward<Args>(args)...);
    ++m_size;
    return it;
}

template <typename T>
void Vector<T>::push_back(const T & value)
{
    if (m_capacity == m_size) {
        size_t capacity = 0;
        if (m_capacity == 0) {
            capacity = 1;
        } else {
            capacity = 2 * m_capacity;
        }

        reserve(capacity);
    }

	m_data[m_size] = value;

    ++m_size;
}

template <typename T>
void Vector<T>::push_back(T && value)
{
    if (m_capacity == m_size) {
        size_t capacity = 0;
        if (m_capacity == 0) {
            capacity = 1;
        } else {
            capacity = 2 * m_capacity;
        }

        reserve(capacity);
    }

	m_data[m_size] = std::move(value);
    ++m_size;
}

template <typename T>
template <typename ... Args>
void Vector<T>::emplace_back(Args && ... args)
{
    if (m_capacity == m_size) {
        size_t capacity = 0;
        if (m_capacity == 0) {
            capacity = 1;
        } else {
            capacity = 2 * m_capacity;
        }

        reserve(capacity);
    }

    new (m_data + m_size * sizeof(T)) T(std::forward<Args>(args)...);
    ++m_size;
}

template <typename T>
void Vector<T>::pop_back()
{
    --m_size;
}

template <typename T>
void Vector<T>::resize(size_t size)
{
    reserve(size);

    m_size = size;
}

}
