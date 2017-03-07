/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_SLIST_H
#define NAIVE_SLIST_H

#include <cstddef>
#include "naive_stl_iterator.h"
#include "naive_stl_alloc.h"
#include "naive_stl_construct.h"

// single linked list�Ľڵ�����ṹ
struct __slist_node_base
{
	__slist_node_base *next;
};

// single linked list�Ľڵ�ṹ__slist_node�̳���__slist_node_base
template <class T>
struct __slist_node : public __slist_node_base
{
	T data;
};

// single linked list�����������ṹ
struct __slist_iterator_base
{
	typedef size_t size_type;
	typedef ptrdiff_t defference_type;
	typedef forward_iterator_tag iterator_category; // ���������

	__slist_node_base* node; // point to __slist_node_base

	__slist_iterator_base(__slist_node_base* x) : node(x) {};
	 
	// ǰ��һ���ڵ�
	void incr() { node = node->next; }

	bool operator==(const __slist_iterator_base& x) const
	{
		return node == x.node;
	}

	bool operator!=(const __slist_iterator_base& x) const
	{
		return !(*this == x);
	}
};

// single linked list�������ṹ
template <class T, class Ref, class Ptr>
struct __slist_iterator : public __slist_iterator_base
{
	typedef __slist_iterator<T, T&, T*>				iterator;
	typedef __slist_iterator<T, const T&, const T*> const_iterator;
	typedef __slist_iterator<T, Ref, Ptr>			self;

	typedef T				value_type;
	typedef Ptr				pointer;
	typedef Ref				reference;
	typedef __slist_node<T> list_node;

	__slist_iterator(list_node *x) : __slist_iterator_base(x) {}
	__slist_iterator() : __slist_iterator_base(0) {}
	__slist_iterator(const iterator& x) : __slist_iterator_base(x.node) {}

	reference operator*() const
	{	// �˴��ж�̬����ת��
		return dynamic_cast<list_node*>(node)->data;
	}
	pointer operator->() const
	{
		reutrn &(*operator*()) :
	}

	self& operator++()
	{
		incr();
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		incr();
		return tmp;
	}
};

// single linked list class slist
template <class T, class Alloc = alloc>
class slist
{
public:
	typedef T				  value_type;
	typedef value_type*		  pointer;
	typedef const value_type* const_pointer;
	typedef value_type&		  reference;
	typedef const value_type& const_reference;
	typedef size_t			  size_type;
	typedef ptrdiff_t		  difference_type;

	typedef __slist_iterator<T, T&, T*> iterator;
	typedef __slist_iterator<T, const T&, const T*> const_iterator;

private:
	typedef __slist_node<T>					list_node;
	typedef __slist_node_base				list_node_base;
	typedef __slist_iterator_base			iterator_base;
	typedef simple_alloc<list_node, Alloc>  list_node_allocator;

	static list_node* create_node(const value_type& x)
	{
		list_node* node = list_node_allocator::allocate();

		try
		{
			construct(&node->data, x);
		}
		catch (...)
		{
			list_node_allocator::deallocate(node);
			throw;
		}
	}

	static void destroy_node(list_node* node)
	{
		destroy(&node->data);
		list_node_allocator::deallocate(node);
	}

private:
	list_node_base head; // ͷ����ע����ʵ�����ָ��

public:
	slist() { head.next == nullptr; }
	~slist() { clear(); }

	iterator begin() { return iterator(dynamic_cast<list_node*>(head.next)); }
	iterator end() { return iterator(0); }
	size_type size() const
	{
		return __slist_size(head.next);
	}
	bool empty() const { return head.next == nullptr; }

	// ����slist������ֻҪ��headָ��������໥��������
	void swap(slist& L)
	{
		list_node_base* tmp = head.next;
		head.next = L.head.next;
		L.head.next = tmp;
	}

public:
	// ȡͷ��Ԫ��
	reference front()
	{
		return (dynamic_cast<list_node*>(head.next))->data;
	}
	// ��ͷ������Ԫ��
	void push_front(const value_type& x)
	{
		__slist_make_link(&head, create_node(x));
	}
	// ��ͷ��ȡ��Ԫ��
	void pop_front()
	{
		list_node* node = dynamic_cast<list_node*> (head.next);
		head.next = node->next;
		destroy_node(node);
	}

};


// global function����֪ĳһ�ڵ㣬�����½ڵ������
inline __slist_node_base* __slist_make_link(
	__slist_node_base* prev_node,
	__slist_node_base* new_node)
{
	new_node->next = prev_node->next;
	prev_node->next = new_node;
	return new_node;
}

// global function����������Ĵ�С
inline size_t __slist_size(__slist_node_base* node)
{
	size_t result = 0;
	for (; node != nullptr; node = node->next)
		++result;
	return result;
}

#endif // !NAIVE_SLIST_H
