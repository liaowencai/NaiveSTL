/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_SET_H
#define NAIVE_SET_H

#include <utility>
#include "stl_tree.h"
#include "naive_stl_alloc.h"
#include "naive_algorithm.h"

template <class Key, class Compare = less<Key>, class Alloc = alloc>
class set
{
public:
	// typedefs
	typedef Key key_type;
	typedef Key value_type;
	typedef Compare key_compare;
	typedef Compare value_compare;

private:
	template <class T>
	struct identity : public unary_function<T, T>
	{
		const T& operator()(const T& x) const
		{
			return x;
		}
	};

	typedef rb_tree<key_type, value_type,
		identity<value_type>, key_compare, Alloc> rep_type;
	rep_type t; // 采用red-black tree来表现set

public:
	typedef typename rep_type::const_pointer pointer;
	typedef typename rep_type::const_pointer const_pointer;
	typedef typename rep_type::const_reference reference;
	typedef typename rep_type::const_reference const_reference;
	// iterator是const iterator，因此不允许用户在任意处进行写入操作
	// 这是为了保证set内元素的有序性
	typedef typename rep_type::const_iterator iterator;
	typedef typename rep_type::const_iterator const_iterator;
	typedef typename rep_type::const_reverse_iterator reverse_iterator;
	typedef typename rep_type::const_reverse_iterator const_reverse_iterator;
	typedef typename rep_type::size_type size_type;
	typedef typename rep_type::difference_type difference_type;

	// set使用insert_unique()
	// multiset使用insert_equal()
	set() : t(Compare()) {}
	explicit set(const Compare& comp) : t(comp) {}

	template <class InputIterator>
	set(InputIterator first, InputIterator last)
		: t(Compare()) { 
		t.insert_unique(first, last); 
	}

	template <class InputIterator>
	set(InputIterator first, InputIterator last, const Compare& comp)
		: t(comp) {
		t.insert_unique(first, last);
	}
	set(const set& x) : t(x.t) {};

	set& operator=(const set& x)
	{
		t = x.t;
		return *this;
	}

	// accessors
	key_compare key_comp() const { return t.key_comp(); }
	value_compare value_comp() const { return t.key_comp(); }
	iterator begin() const { return t.begin(); }
	iterator end() const { return t.end(); }
	reverse_iterator rbegin() const { return t.rbegin(); }
	reverse_iterator rend() const { return t.rend(); }
	bool empty() const { return t.empty(); }
	size_type size() const { return t.size(); }
	size_type max_size() const { return t.max_size(); }
	void swap(set& x) { t.swap(x.t) };

	// insert/erase
	typedef pair<iterator, bool> pair_iterator_bool;
	pair<iterator, bool> insert(const value_type& x)
	{
		pair<typename rep_type::interator, bool> p = t.insert_unique(x);
		return pair<iterator, bool>(p.first, p.second);
	}
	iterator insert(iterator position, const value_type& x)
	{
		typedef typename rep_type::iterator rep_iterator;
		return t.insert_unique((rep_iteraotr&)position, x);
	}

	template <class InputIterator>
	void insert(InputIterator first, InputIterator last)
	{
		t.insert_unique(first, last);
	}

	void erase(iterator position)
	{
		typedef typename rep_type::iterator rep_iterator;
		t.erase((rep_iterator&)position);
	}
	size_type erase(const key_type& x)
	{
		return t.erase(x);
	}
	void erase(iterator first, iterator last)
	{
		typedef typename rep_type::iterator rep_iterator;
		t.erase((rep_iterator&)first, (rep_iterator&)last);
	}
	void clear() { t.clear(); }

	// set operations
	iterator find(const key_type& x) const { return t.find(x); }
	size_type count(const key_type& x) const { return t.count(x); }
	iterator lower_bound(const key_type& x) const
	{
		return t.lower_bound(x);
	}
	iterator upper_bound(const key_type& x) const
	{
		return t.upper_bound(x);
	}
	pair<iterator, iterator> equal_range(const key_type& x) const
	{
		return t.equql_range(x);
	}

	friend bool operator==(const set&, const set&);
	friend bool operator<(const set&, const set&);
};

template <class Key, class Compare, class Alloc>
inline bool operator==(const set<Key, Compare, Alloc>& x,
	const set<Key, Compare, Alloc>& y)
{
	return x.t == y.t;
}

template <class Key, class Compare, class Alloc>
inline bool operator<(const set<Key, Compare, Alloc>& x,
	const set<Key, Compare, Alloc>& y)
{
	return x.t < y.t;
}

#endif // !NAIVE_SET_H
