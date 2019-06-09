namespace naive {

template <typename T>
List<T>::ListItem::ListItem(ListItem * next, const T & data) :
    m_next(next),
    m_data(data)
{
}

template <typename T>
template <typename ... Args>
List<T>::ListItem::ListItem(ListItem * next, Args && ... args) :
    m_next(next),
    m_data(std::forward<Args>(args)...)
{
}

template <typename T>
List<T>::Iterator::Iterator() :
    m_item(nullptr)
{
}

template <typename T>
List<T>::Iterator::Iterator(const Iterator & it) :
    m_item(it.m_item),
	m_before_begin(it.m_before_begin)
{
}

template <typename T>
List<T>::Iterator::Iterator(List<T>::ListItem * item, bool before_begin) :
    m_item(item),
	m_before_begin(before_begin)
{
}

template <typename T>
typename List<T>::Iterator & List<T>::Iterator::operator=(typename const List<T>::Iterator & it)
{
    m_item = it.m_item;
    return *this;
}

template <typename T>
typename List<T>::Iterator & List<T>::Iterator::operator++()
{
    m_item = m_item->m_next;
    return *this;
}

template <typename T>
typename List<T>::Iterator List<T>::Iterator::operator++(int)
{
    Iterator it = *this;
    m_item = m_item->m_next;
    return it;
}

template <typename T>
List<T>::ConstIterator::ConstIterator() :
    m_item(nullptr)
{
}

template <typename T>
List<T>::ConstIterator::ConstIterator(const ConstIterator & it) :
    m_item(it.m_item),
	m_before_begin(it.m_before_begin)
{
}

template <typename T>
List<T>::ConstIterator::ConstIterator(const List<T>::ListItem * item, bool before_begin) :
    m_item(item),
	m_before_begin(before_begin)
{
}

template <typename T>
typename List<T>::ConstIterator & List<T>::ConstIterator::operator=(typename const List<T>::ConstIterator & it)
{
    m_item = it.m_item;
    return *this;
}

template <typename T>
typename List<T>::ConstIterator & List<T>::ConstIterator::operator++()
{
    m_item = m_item->m_next;
    return *this;
}

template <typename T>
typename List<T>::ConstIterator List<T>::ConstIterator::operator++(int)
{
    ConstIterator it = *this;
    m_item = m_item->m_next;
    return it;
}

template <typename T>
List<T>::List(const List & list)
{
    do_copy(list);
}

template <typename T>
List<T>::List(List<T> && list) :
    m_head(list.m_head),
    m_size(list.m_size)
{
    list.m_head = nullptr;
    list.m_size = 0;
}

template <typename T>
List<T>::~List()
{
    clear();
}

template <typename T>
List<T> & List<T>::operator=(const List<T> & list)
{
    clear();
    do_copy(list);
    return *this;
}

template <typename T>
List<T> & List<T>::operator=(List<T> && list)
{
    m_head = list.m_head;
    m_size = list.m_size;

    list.m_head = nullptr;
    list.m_size = 0;

    return *this;
}

template <typename T>
T & List<T>::front()
{
    return *begin();
}

template <typename T>
const T & List<T>::front() const
{
    return *cbegin();
}

template <typename T>
typename List<T>::Iterator List<T>::before_begin()
{
    return List<T>::Iterator(m_head, true);
}

template <typename T>
typename List<T>::ConstIterator List<T>::cbefore_begin() const
{
	return List<T>::ConstIterator(m_head, true);
}

template <typename T>
typename List<T>::Iterator List<T>::begin()
{
    return List<T>::Iterator(m_head);
}

template <typename T>
typename List<T>::ConstIterator List<T>::cbegin() const
{
	return List<T>::ConstIterator(m_head);
}

template <typename T>
typename List<T>::Iterator List<T>::end()
{
    return List<T>::Iterator();
}

template <typename T>
typename List<T>::ConstIterator List<T>::cend() const
{
    return List<T>::ConstIterator();
}

template<typename T>
bool List<T>::empty() const
{ return m_head == nullptr; }

template<typename T>
size_t List<T>::size() const
{ return m_size; }

template<typename T>
void List<T>::clear()
{
    while (m_head != nullptr) {
        List<T>::ListItem * next = m_head->m_next;
        delete m_head;
        m_head = next;
    }

    m_size = 0;
}

template <typename T>
typename List<T>::Iterator List<T>::insert_after(typename List<T>::Iterator it, const T & value)
{
	if (it.before_begin()) {
		push_front(value);
		return Iterator(m_head);
	}

    if (it == end()) {
        return end();
	}

    auto it_next = it;
    ++it_next;

    List<T>::ListItem * item = new List<T>::ListItem(it_next.m_item, value);
    (it.m_item)->m_next = item;

    ++m_size;
    return List<T>::Iterator(item);
}

template <typename T>
typename List<T>::Iterator List<T>::insert_after(typename List<T>::Iterator it, T && value)
{
	if (it.before_begin()) {
		push_front(std::move(value));
		return Iterator(m_head);
	}

    if (it == end()) {
        return end();
    }

    auto it_next = it;
    ++it_next;

    List<T>::ListItem * item = new List<T>::ListItem(it_next.m_item, std::move(value));
    (it.m_item)->m_next = item;

    ++m_size;
    return List<T>::Iterator(item);
}

template <typename T>
template <typename ... Args>
typename List<T>::Iterator List<T>::emplace_after(typename List<T>::Iterator it, Args && ... args)
{
	if (it.before_begin()) {
		emplace_front(std::forward<Args>(args)...);
		return Iterator(m_head);
	}

    if (it == end()) {
        return end();
    }

    auto it_next = it;
    ++it_next;

    List<T>::ListItem * item = new List<T>::ListItem(it_next.m_item, std::forward<Args>(args)...);
    (it.m_item)->m_next = item;

    ++m_size;
    return List<T>::Iterator(item);
}

template <typename T>
typename List<T>::Iterator List<T>::erase_after(typename List<T>::Iterator it)
{
    if (it == end() || (it.m_item)->m_next == nullptr) {
         return end();
    }

    List<T>::Iterator next_it = it;
    ++next_it;

    (it.m_item)->m_next = (next_it.m_item)->m_next;

    delete next_it.m_item;
    --m_size;

    return ++it;
}

template <typename T>
void List<T>::push_front(const T & value)
{
    m_head = new List<T>::ListItem(!empty() ? m_head : nullptr, value);
    ++m_size;
}

template <typename T>
void List<T>::push_front(T && value)
{
    m_head = new List<T>::ListItem(!empty() ? m_head : nullptr, std::move(value));
    ++m_size;
}

template <typename T>
template <typename ... Args>
void List<T>::emplace_front(Args && ... args)
{
    m_head = new List<T>::ListItem(!empty() ? m_head : nullptr, std::forward<Args>(args)...);
    ++m_size;
}

template <typename T>
void List<T>::pop_front()
{
    if (empty()) {
        return;
    }

    List<T>::ListItem * head = m_head;
    m_head = m_head->m_next;
    delete head;
}

template<typename T>
void List<T>::do_copy(const List & list)
{
    ListItem<T> * list_current = nullptr;
    if (list.m_head != nullptr) {
        m_head = new ListItem<T>(list.m_head->m_data);
        list_current = list.m_head->m_next;
    }

    ListItem<T> * current = m_head;
    while (list_current != nullptr) {
        current->m_next = new ListItem<T>(list_current->m_data);
        current = current->m_next;
        list_current = list_current->m_next;
    }

    m_size = list.m_size;
}

} /*namespace naive*/