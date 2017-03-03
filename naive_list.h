#ifndef NAIVE_LIST_H
#define NAIVE_LIST_H

#include "naive_stl_iterator.h"

#include <cstddef>


// �ڵ���
template <class T>
struct __list_node
{
	typedef __list_node<T>* node_pointer;
	node_pointer prev;
	node_pointer next;
	T data;
};

// ������
template <class T, class Ref, class Ptr>
struct __list_iterator
{
	typedef __list_iterator<T, T&, T*>		iterator;
	typedef __list_iterator<T, Ref, Ptr>	self;
	
	typedef bidirectional_iterator_tag iterator_category;
	typedef T						   value_type;
	typedef Ptr						   pointer;
	typedef Ref						   reference;
	typedef __list_node<T>*			   link_type;
	typedef size_t					   size_type;
	typedef ptrdiff_t				   difference_type;

	// �������ڲ�ָ�룬ָ��list�Ľڵ�__list_node
	link_type node;

	// constructor
	__list_iterator(link_type x) : node(x) {}
	__list_iterator() {}
	__list_iterator(const iterator& x) : node(x.node) {}

	bool operator==(const self& x) const
	{
		return node == x.node;
	}
	bool operator!=(const self& x) const
	{
		return node != x.node;
	}
	// ȡ�ڵ������ֵ
	reference operator*() const
	{
		return (*node).data;
	}
	// ��������->������ı�׼����
	pointer operator->() const
	{
		return &(operator*());
	}
	// ǰ��һ���ڵ�
	self& operator++()
	{
		node = static_cast<link_type>((*node).next);
		return *this;
	}
	self& operator++(int)
	{
		self tmp = *this;
		++*this;
		return tmp;
	}

	// ����һ���ڵ�
	self& operator--()
	{
		node = static_cast<link_type>((*node).prev);
		return *this;
	}
	self& operator--(int)
	{
		self tmp = *this;
		--*this;
		return tmp;
	}
};

// list��
template <class T, class Alloc = alloc>
class list {
protected:
	typedef __list_node<T> list_node;
	typedef simple_alloc<list_node, Alloc> list_node_allocator;
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef list_node* link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef __list_iterator<T, T&, T*> iterator;

protected:
	link_type node;

	// ����һ���ڵ㲢����
	link_type get_node()
	{
		return list_node_allocator::alocate();
	}
	// �ͷ�һ���ڵ�
	void put_node(link_type p)
	{
		list_node_allocator::deallocate(p);
	}
	// ���������ò����죩һ���ڵ㣬����Ԫ��ֵ
	link_type create_node(const T& x)
	{
		link_type p = get_node();
		construct(&p->data, x);
		return p;
	}
	// ���٣��������ͷţ�һ���ڵ�
	void destroy_node(link_type p)
	{
		destroy(&p->data);
		put_node(p);
	}
	// ��ʼ��������
	void empty_initialize()
	{
		node = get_node();
		node->next = node;
		node->prev = node;
	}
	// Ǩ�Ʋ�������[first, last)�ڵ�����Ԫ���ƶ���position֮ǰ
	void transfer(iterator position, iterator first, iterator last)
	{
		if (position != last)
		{
			last.node->prev->next = position.node;
			first.node->prev->next = last.node;
			postion.node->prev->next = first.node;
			link_type tmp = position.node->prev;
			position.node->prev = last.node->prev;
			last.node->prev = first.node->prev;
			first.node->prev = tmp;
		}
	}
public:
	list() { empty_initialize(); }

	iterator begin() { return static_cast<link_type>((*node).next); }
	iterator end() { return node; }
	bool empty() const { return node->next == node; }
	size_type size() const
	{
		size_type result = 0;
		result = distance(begin(), end());
		return result;
	}
	// ȡͷ�ڵ�����ݣ�Ԫ��ֵ��
	reference front() { return *begin(); }
	// ȡβ�ڵ�����ݣ�Ԫ��ֵ��
	reference back() { return *(--end()); }
	// ����һ���ڵ㣬��Ϊͷ�ڵ�
	void push_front(const T& x) { insert(begin(), x); }
	// ����һ���ڵ㣬��Ϊβ�ڵ�
	void push_back(const T& x) { insert(end(), x); }
	// ��ָ��λ�ò���һ���ڵ㣬����Ϊx
	iterator insert(iterator positon, const T& x)
	{
		link_type tmp = create_node(x);
		tmp->next = position.node;
		tmp->prev = position.node->prev;
		static_cast<link_type>(position.node->prev)->next = tmp;
		position.node->prev = tmp;
		return tmp;
	}
	// �Ƴ�������postion��ָ�ڵ�
	iterator erase(iterator position)
	{
		link_type next_node = static_cast<link_type>(position.node->next);
		link_type prev_node = static_cast<link_type>(position.node->prev);
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(position.node);
		return static_cast<iterator>(next_node);
	}
	// �Ƴ�ͷ�ڵ�
	void pop_front() { erase(begin()); }
	// �Ƴ�β�ڵ�
	void pop_back()
	{
		iterator tmp = end();
		erase(--tmp);
	}
	// ������нڵ�
	void clear()
	{
		link_type cur = static_cast<link_type>(node->next);// begin();
		while (cur != node)
		{
			// ����ÿһ���ڵ�
			link_type tmp = cur;
			cur = static_cast<link_type>(cur->next);
			destroy_node(tmp);
		}
		// �ָ�nodeԭʼ״̬
		node->next = node;
		node->prev = node;
	}
	// ����ֵΪvalue������Ԫ���Ƴ�
	void remove(const T& value)
	{
		iterator first = begin();
		iterator last = end();
		while (first != last)
		{
			iterator next = first;
			++next;
			if (*first == value)
				erase(first);
			first = next;
		}
	}
	// �Ƴ���ֵ��������ͬ��Ԫ�أ�ֻ����һ��
	void unique()
	{
		iterator first = begin();
		iterator last = end();
		if (empty())
			return;
		iterator next = first;
		while (++next != last)
		{
			if (*first == *next)
				erase(next);
			else
				first = next;

			next = first;
		}
	}
	// ��x�Ӻ���position��ָλ��֮ǰ��x���벻ͬ��*this
	void splice(iterator position, list& x)
	{
		if (!x.empty())
			transfer(position, x.begin(), x.end());
	}
	// ��i��ָԪ�ؽӺ���position��ָλ��֮ǰ��position��i��ָ��ͬһ��list
	void splice(iterator position, list&, iterator i)
	{
		iterator j = i;
		++j;
		if (position == i || position == j)
			return;
		transfer(position, i, j);
	}
	// ��[first, last)�ڵ�����Ԫ�ؽӺ���position��ָλ��֮ǰ
	// position��[frist, last)��ָ��ͬһ��list
	// ��position����λ��[first, last)֮��
	void splice(iterator position, list&, iterator first, iterator last)
	{
		if (first != last)
			transfer(position, first, last);
	}

	// merge������x�ϲ���*this���ϣ�����lists�����ݶ������Ⱦ�����������
	void merge(list &x)
	{
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = x.begin();
		iterator last2 = x.end();

		// ע�⣺ǰ���ǣ�����lists���Ѿ�����������
		while (first1 != last1 && first2 != last2)
		{
			if (*first2 < *first1)
			{
				iterator next = first2;
				++next;
				transfer(first1, first2, next);
				first2 = next;
			}
			else
				++first1;
			// list1 <= list2
			if (first2 != last2)
				transfer(last1, first2, last2);
		}
	}

	// ��*this��������������
	void reverse()
	{
		// �����һ�����������ֻ��һ��Ԫ�أ��������κβ���
		if (node->next == node || node->next->next == node)
			return;

		iterator first = begin();
		++first;
		while (first != end())
		{
			itertor old = first;
			++first;
			transfer(begin(), old, first);
		}
	}
	// swap����
	void swap(list& x)
	{
		link_type tmp = x.node;
		x.node = this->node;
		this->node = tmp;
	}

	// sort()������quick sort
	void sort()
	{
		// size() == 0 || size() == 1��ֱ�ӷ���
		if (node->next == node || node->next->next == node)
			return;

		// һЩ�µ�lists����Ϊ�н����ݴ����
		list<T, Alloc> carry;
		list<T, Alloc> counter[64];
		int fill(0);

		while (!empty())
		{
			carry.splice(carry.begin(), *this, begin());
			int i(0);
			while (i < fill && !counter[i].empty())
			{
				counter[i].merge(carry);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill)
				++fill;
		}

		for (int i(1); i < fill; ++i)
			counter[i].merge(counter[i - 1]);

		swap(counter[fill - 1]);
	}


};

#endif // !NAIVE_LIST_H

