#pragma once

/*
 * Naive single linked list implementation
 */

// TODO: tests

template <typename T>
class List
{
public:
	class ListItem
	{
	public:
		ListItem(const T & data, ListItem * next = nullptr);

	private:
		friend class List<T>;

	private:
		T m_data;
		ListItem * m_next;
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
	// Iterators
	ListIterator begin()
	{ return ListIterator(m_head); }

	ListIterator end()
	{ return ListIterator(); }

public:
	// Insert
	void push_front(T && value);
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