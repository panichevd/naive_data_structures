#pragma once

/*
 * Naive single linked list implementation
 */

// TODO: const iterators 

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
        Iterator(List<T>::ListItem * item);

	public:
		// Operators
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
	};

public:
	// Constructors/Destructor
	List() = default;
	List(const List & list);
	List(List && list);

	~List();

public:
	// Assignment
	List<T> & operator=(const List<T> & list);
	List & operator=(List && list);	

public:
	// Capacity
	bool empty() const
	{ return m_head == nullptr; }

	size_t size() const
	{ return m_size; }

public:
	// Access
	Iterator begin()
	{ return Iterator(m_head); }

	Iterator end()
	{ return Iterator(); }

	T & front()
	{ return *begin(); }

	const T & front() const
	{ return *begin(); }

public:
	// Cannot use perfect forwarding as T is already deduced without references

	// Insert
	void push_front(const T & value);
	void push_front(T && value);
	Iterator insert(Iterator it, const T & value);
	Iterator insert(Iterator it, T && value);

	template <typename ... Args>
	void emplace_front(Args && ... args);
	template <typename ... Args>
	Iterator emplace(Iterator it, Args && ... args);

public:
	// Remove
	void pop_front();
	Iterator remove(Iterator it);

	void clear();

private:
	void do_copy(const List & list);

private:
	ListItem * m_head = nullptr;
	size_t m_size = 0;
};

} /*namespace naive*/

#include "SingleLinkedList.inc"