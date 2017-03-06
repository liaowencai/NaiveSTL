/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_QUEUE_H
#define NAIVE_QUEUE_H

#include "naive_deque.h"

template <class T, class Sequence = deque<T>>
class queue
{
	// friend function
	friend bool operator==(const queue&x, const queue& y);
	friend bool operator<(const queue&x, const queue& y);

public:
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;
	typedef typename Sequence::const_reference const_reference;

protected:
	Sequence c; // µ×²ãÈÝÆ÷

public:
	bool empty() const { return c.empty(); }
	size_type size() const { return c.size(); }
	reference front() { return c.front(); }
	const_reference front() const { return c.front(); }
	reference back() { return c.back(); }
	const_reference back() const { return c.back(); }
	void push(const value_type& x) { c.push_back(x); }
	void pop() { c.pop_front(); }
};

template <class T, class Sequence>
bool operator==(const queue<T, Sequence>&x, const queue<T, Sequence>& y)
{
	return x.c == y.c;
}

template <class T, class Sequence>
bool operator<(const queue<T, Sequence>&x, const queue<T, Sequence>& y)
{
	return x.c < y.c;
}


#endif // !NAIVE_QUEUE_H
