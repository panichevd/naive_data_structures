#pragma once

/*
 * Naive single linked list implementation
 */

// TODO: resize, erase range

namespace naive {

template <typename T>
class List
{
public:
    class ListItem
    {
    public:
        ListItem(ListItem * next, const T & data);

        template <typename ... Args>
        ListItem(ListItem * next, Args && ... args);

    private:
        friend class List<T>;

    private:
        ListItem * m_next;
        T m_data;
    };

    class Iterator
    {
    public:
        // Constructor
        Iterator();
        Iterator(const Iterator & it);
        Iterator(List<T>::ListItem * item, bool before_begin = false);

    public:
        bool before_begin() const
        { return m_before_begin; }
    
    public:
        Iterator & operator=(const Iterator & it);

        bool operator==(const Iterator & it)
        { return m_item == it.m_item; }

        bool operator!=(const Iterator & it)
        { return m_item != it.m_item; }

        const T & operator*() const
        { return m_item->m_data; }
        
        T & operator*()
        { return m_item->m_data; }

        Iterator & operator++();
        Iterator operator++(int);

    private:
        friend class List<T>;

        List<T>::ListItem * m_item;
        bool m_before_begin = false;
    };

    class ConstIterator
    {
    public:
        // Constructor
        ConstIterator();
        ConstIterator(const ConstIterator & it);
        ConstIterator(const List<T>::ListItem * item, bool before_begin = false);

    public:
        // Operators
        ConstIterator & operator=(const ConstIterator & it);

        bool operator==(const ConstIterator & it)
        { return m_item == it.m_item; }

        bool operator!=(const ConstIterator & it)
        { return m_item != it.m_item; }

        const T & operator*() const
        { return m_item->m_data; }

        const T & operator*()
        { return m_item->m_data; }

        ConstIterator & operator++();
        ConstIterator operator++(int);

    private:
        friend class List<T>;

        const List<T>::ListItem * m_item;
        bool m_before_begin = false;
    };

public:
    List() = default;
    List(const List & list);
    List(List && list);

    ~List();

    List & operator=(const List<T> & list);
    List & operator=(List && list);	

public:
    T & front();
    const T & front() const;

public:
    Iterator before_begin();
    ConstIterator cbefore_begin() const;

    Iterator begin();
    ConstIterator cbegin() const;

    Iterator end();
    ConstIterator cend() const;

public:
    bool empty() const;
    size_t size() const;

public:
    void clear();

    Iterator insert_after(Iterator it, const T & value);
    Iterator insert_after(Iterator it, T && value);

    template <typename ... Args>
    Iterator emplace_after(Iterator it, Args && ... args);

    Iterator erase_after(Iterator it);

    void push_front(const T & value);
    void push_front(T && value);

    template <typename ... Args>
    void emplace_front(Args && ... args);

    void pop_front();

private:
    void do_copy(const List & list);

private:
    ListItem * m_head = nullptr;
    size_t m_size = 0;
};

} /*namespace naive*/

#include "SingleLinkedList.inc"