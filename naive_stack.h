/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_STACK_H
#define NAIVE_STACK_H

template <class T, class Sequence = deque<T>>
class stack
{
	// 定义友元函数
	friend bool operator==(const stack&, const stack&);
	friend boll operator<(const stack&, const stack&);

public:
	typedef typename Sequence::value_type value_type;
	typedef typename Sequence::size_type size_type;
	typedef typename Sequence::reference reference;
	typedef typename Sequence::const_reference const_reference;

protected:
	Sequence c; // 底层容器

public:
	// 完全利用Sequence c的操作，完成stack的操作
	bool empty() { return c.empty(); }
	size_type size() const { return c.size(); }
	reference top() { return c.back(); }
	const_reference top() const { return c.back(); }
	void push(const value_type& x) { c.push_back(x); }
	void pop() { c.pop_back(); }
};

template <class T, class Sequence>
bool operator==(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
{
	return x.c == y.c;
}

template <class T, class Sequence>
bool operator<(const stack<T, Sequence>& x, const stack<T, Sequence>& y)
{
	return x.c < y.c;
}

#endif // !NAIVE+STACK_H

