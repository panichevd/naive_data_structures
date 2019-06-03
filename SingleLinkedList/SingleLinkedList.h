#pragma once

/*
 * Naive single linked list implementation
 */

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

	class ListIterator
	{
	public:
		// Constructor
		ListIterator();
		ListIterator(const ListIterator & it);
		ListIterator(List<T>::ListItem * item);

	public:
		// Operators
		ListIterator & operator=(const ListIterator & it);

		bool operator==(const ListIterator & it)
		{ return m_item == it.m_item; }

		bool operator!=(const ListIterator & it)
		{ return m_item != it.m_item; }

		const T & operator*() const
		{ return m_item->m_data; }

		T & operator*()
		{ return m_item->m_data; }

		ListIterator & operator++();
		ListIterator operator++(int);

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
	ListIterator begin()
	{ return ListIterator(m_head); }

	ListIterator end()
	{ return ListIterator(); }

	T & front()
	{ return *begin(); }

	const T & front() const
	{ return *begin(); }

public:
	// Cannot use perfect forwarding as T is already deduced without references

	// Insert
	void push_front(const T & value);
	void push_front(T && value);
	ListIterator insert(ListIterator it, const T & value);
	ListIterator insert(ListIterator it, T && value);

	template <typename ... Args>
	void emplace_front(Args && ... args);
	template <typename ... Args>
	ListIterator emplace(ListIterator it, Args && ... args);

public:
	// Remove
	void pop_front();
	ListIterator remove(ListIterator it);

	void clear();

private:
	void do_copy(const List & list);

private:
	ListItem * m_head = nullptr;
	size_t m_size = 0;
};

#include "SingleLinkedList.incl"