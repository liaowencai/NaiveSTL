#ifndef NAIVE_LIST_H
#define NAIVE_LIST_H

#include "naive_stl_iterator.h"

#include <cstddef>


// 节点类
template <class T>
struct __list_node
{
	typedef __list_node<T>* node_pointer;
	node_pointer prev;
	node_pointer next;
	T data;
};

// 迭代器
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

	// 迭代器内部指针，指向list的节点__list_node
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
	// 取节点的数据值
	reference operator*() const
	{
		return (*node).data;
	}
	// 迭代器的->运算符的标准做法
	pointer operator->() const
	{
		return &(operator*());
	}
	// 前进一个节点
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

	// 后退一个节点
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

// list类
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

	// 配置一个节点并传回
	link_type get_node()
	{
		return list_node_allocator::alocate();
	}
	// 释放一个节点
	void put_node(link_type p)
	{
		list_node_allocator::deallocate(p);
	}
	// 产生（配置并构造）一个节点，带有元素值
	link_type create_node(const T& x)
	{
		link_type p = get_node();
		construct(&p->data, x);
		return p;
	}
	// 销毁（析构并释放）一个节点
	void destroy_node(link_type p)
	{
		destroy(&p->data);
		put_node(p);
	}
	// 初始化空链表
	void empty_initialize()
	{
		node = get_node();
		node->next = node;
		node->prev = node;
	}
	// 迁移操作：将[first, last)内的所有元素移动到position之前
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
	// 取头节点的内容（元素值）
	reference front() { return *begin(); }
	// 取尾节点的内容（元素值）
	reference back() { return *(--end()); }
	// 插入一个节点，作为头节点
	void push_front(const T& x) { insert(begin(), x); }
	// 插入一个节点，作为尾节点
	void push_back(const T& x) { insert(end(), x); }
	// 在指定位置插入一个节点，内容为x
	iterator insert(iterator positon, const T& x)
	{
		link_type tmp = create_node(x);
		tmp->next = position.node;
		tmp->prev = position.node->prev;
		static_cast<link_type>(position.node->prev)->next = tmp;
		position.node->prev = tmp;
		return tmp;
	}
	// 移除迭代器postion所指节点
	iterator erase(iterator position)
	{
		link_type next_node = static_cast<link_type>(position.node->next);
		link_type prev_node = static_cast<link_type>(position.node->prev);
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(position.node);
		return static_cast<iterator>(next_node);
	}
	// 移除头节点
	void pop_front() { erase(begin()); }
	// 移除尾节点
	void pop_back()
	{
		iterator tmp = end();
		erase(--tmp);
	}
	// 清除所有节点
	void clear()
	{
		link_type cur = static_cast<link_type>(node->next);// begin();
		while (cur != node)
		{
			// 遍历每一个节点
			link_type tmp = cur;
			cur = static_cast<link_type>(cur->next);
			destroy_node(tmp);
		}
		// 恢复node原始状态
		node->next = node;
		node->prev = node;
	}
	// 将数值为value的所有元素移除
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
	// 移除数值连续而相同的元素，只留下一个
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
	// 将x接合于position所指位置之前，x必须不同于*this
	void splice(iterator position, list& x)
	{
		if (!x.empty())
			transfer(position, x.begin(), x.end());
	}
	// 将i所指元素接合于position所指位置之前，position和i可指向同一个list
	void splice(iterator position, list&, iterator i)
	{
		iterator j = i;
		++j;
		if (position == i || position == j)
			return;
		transfer(position, i, j);
	}
	// 将[first, last)内的所有元素接合于position所指位置之前
	// position和[frist, last)可指向同一个list
	// 但position不能位于[first, last)之内
	void splice(iterator position, list&, iterator first, iterator last)
	{
		if (first != last)
			transfer(position, first, last);
	}

	// merge（）将x合并到*this身上，两个lists的内容都必须先经过递增排序
	void merge(list &x)
	{
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = x.begin();
		iterator last2 = x.end();

		// 注意：前提是，两个lists都已经过递增排序
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

	// 将*this的内容逆向重置
	void reverse()
	{
		// 如果是一个空链表或者只有一个元素，不进行任何操作
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
	// swap函数
	void swap(list& x)
	{
		link_type tmp = x.node;
		x.node = this->node;
		this->node = tmp;
	}

	// sort()，采用quick sort
	void sort()
	{
		// size() == 0 || size() == 1，直接返回
		if (node->next == node || node->next->next == node)
			return;

		// 一些新的lists，作为中介数据存放区
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

