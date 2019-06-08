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
    m_item(it.m_item)
{
}

template <typename T>
List<T>::Iterator::Iterator(List<T>::ListItem * item) :
    m_item(item)
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
typename List<T>::Iterator List<T>::insert(typename List<T>::Iterator it, const T & value)
{
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
typename List<T>::Iterator List<T>::insert(typename List<T>::Iterator it, T && value)
{
    if (it == end()) {
        return end();
    }

    auto it_next = it;
    ++it_next;

    List<T>::ListItem * item = new List<T>::ListItem(it_next.m_item, std::move(value), );
    (it.m_item)->m_next = item;

    ++m_size;
    return List<T>::Iterator(item);
}

template <typename T>
template <typename ... Args>
void List<T>::emplace_front(Args && ... args)
{
    m_head = new List<T>::ListItem(!empty() ? m_head : nullptr, std::forward<Args>(args)...);
    ++m_size;
}

template <typename T>
template <typename ... Args>
typename List<T>::Iterator List<T>::emplace(typename List<T>::Iterator it, Args && ... args)
{
    if (it == end()) {
        return end();
    }

    auto it_next = it;
    ++it_next;

    List<T>::ListItem * item = new List<T>::ListItem(!empty() ? m_head : nullptr, std::forward<Args>(args)...);
    (it.m_item)->m_next = item;

    ++m_size;
    return List<T>::Iterator(item);
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

template <typename T>
typename List<T>::Iterator List<T>::remove(typename List<T>::Iterator it)
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