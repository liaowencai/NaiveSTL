/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_DEQUE_H
#define NAIVE_DEQUE_H

#include <cstddef>
#include "naive_stl_iterator.h"
#include "naive_stl_construct.h"
#include "naive_stl_alloc.h"
#include "naive_algorithm.h"

// ȫ�ֺ���__deque_buf_size()
// n != 0����ʾbuffer sizeʹ��Ĭ��ֵ������n
// n == 0����ʾbuffer sizeʹ��Ĭ��ֵ
inline size_t __deque_buf_size(size_t n, size_t sz)
{
	return n != 0 ? n : (sz < 512 ? static_cast<size_t>(512 / sz) : static_cast<size_t>(1));
}

template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator
{
	// δ�̳�std::iterator
	typedef __deque_iterator<T, T&, T*, BufSiz>				iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
	typedef random_access_iterator_tag						iterator_category;
	typedef T												value_type;
	typedef Ptr												pointer;
	typedef Ref												reference;
	typedef size_t											size_type;
	typedef ptrdiff_t										difference_type;
	typedef T**												map_pointer;

	typedef __deque_iterator self;

	__deque_iterator() {}
	__deque_iterator(T* x, map_pointer y) : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
	__deque_iterator(const iterator &x) : cur(x.cur), first(x.first), last(x.last), node(x.node) {}

	static size_t buffer_size()
	{
		return __deque_buf_size(BufSiz, sizeof(T));
	}

	// ����������������
	T *cur; // ��������currentԪ��
	T *frist; // ��������ͷ
	T *last; // ��������β�������ÿռ䣩
	map_pointer node; // ָ��map

	void set_node(map_pointer new_node)
	{
		node = new_node;
		first = *new_node;
		last = first + static_cast<difference_type>(buffer_size());
	}

	// ���������
	reference operator*() const { return *cur; }
	pointer operator->() const { return &(operator*()); } // ����
	difference_type operator-(const self& x) const
	{
		return
			static_cast<difference_type>(buffer_size()) * (node - x.node - 1)
			+ (cur - first) + (x.last - x.cur);
	}

	self& operator++()
	{
		++cur;
		if (cur == last)
		{
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		++*this;
		return tmp;
	}

	self& operator--()
	{
		if (cur == first)
		{
			set_node(node - 1);
			cur = last;
		}
		--cur;
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		--*this;
		reutrn tmp;
	}

	// ʵ�������ȡ������������ֱ����Ծn������
	self& operator+=(difference_type n)
	{
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < static_cast<difference_type>(buffer_size()))
			// Ŀ��λ����ͬһ����������
			cur += n;
		else
		{
			// ���λ�ò���ͬһ��������
			difference_type node_offset =
				offset > 0 ? offset / static_cast<difference_type>(buffer_size())
				: -static_cast<difference_type>((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			// �л�����ȷ��Ԫ��
			cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()));
		}
		return *this;
	}

	self operator+(difference_type n) const
	{
		self tmp = *this;
		return tmp += n;
	}

	self& operator-=(difference_type n) { return *this += -n; }
	self operator-(difference_type n) const
	{
		self tmp = *this;
		return tmp -= n;
	}

	// ʵ�������ȡ
	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}

	bool operator==(const self& x) const { return cur == x.cur; }
	bool operator!=(const self& x) cosnt { return !(*this == x); }
	bool operator<(const self& x) const
	{
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}
};

// ����deque��
template <class T, class Alloc = alloc, size_t BufSiz = 0>
class deque
{
public:
	typedef T									value_type;
	typedef value_type*							pointer;
	typedef value_type&							reference;
	typedef const value_type&					const_reference;
	typedef ptrdiff_t							difference_type;
	typedef	size_t								size_type;
	typedef __deque_iterator<T, T&, T*, BufSiz> iterator;

protected:
	// pointer of pointer of T
	typedef pointer* map_pointer;
	// �ռ���������ÿ������һ��Ԫ�ش�С
	typedef simple_alloc<value_type, Alloc> data_allocator;
	// �ռ���������ÿ������һ��ָ���С
	typedef simple_alloc<pointer, Alloc> map_allocator;

protected:
	iterator start; // ��һ���ڵ�
	iterator finish; // ���һ���ڵ�

	map_pointer map; // map�ǿ������ռ䣬����ÿ��Ԫ�ض���ָ�룬ָ��һ���ڵ�
	size_type map_size; // map�ڵ�ָ������Ҳ����������Ŀ

protected:
	static size_type initial_map_size() { return 8; }

	pointer allocate_node()
	{
		return data_allocator::allocate(buffer_size());
	}

	void deallocate_node(pointer p)
	{
		data_allocator::deallocate(p, buffer_size());
	}

	// fill_initialize()������������ź�deque�Ľṹ
	// ����ʼ��Ԫ�صĳ�ֵ
	void fill_initialize(size_type n, const value_type& value)
	{
		create_map_and_nodes(n); // ��deque�Ľṹ�����������ź�
		map_pointer cur;

		// Ϊÿ���ڵ�Ļ������趨��ֵ
		for (cur = start.node; cur < finish.node; ++cur)
			uninitialized_fill(*cur, *cur + buffer_size(), value);
		// ���һ���ڵ���趨���в�ͬ
		// ��Ϊβ�˿����б��ÿռ䣬�������ֵ
		uninitialized_fill(finish.first, finish.cur, value);
	}

	// create_map_and_nodes������������ź�deque�Ľṹ
	void create_map_and_nodes(size_type num_elements)
	{
		// ��Ҫ�Ľڵ���=(Ԫ�ظ���/ÿ�������������ɵ�Ԫ�ظ���)+1
		// ����պ������������һ���ڵ�
		size_type num_nodes = num_elements / buffer_size() + 1;

		// һ��mapҪ����Ľڵ���map_size������8������ࡰ����ڵ���+2��
		// 2��Ϊ�ˣ�ǰ���Ԥ��һ��������ʱ����ʹ��
		map_size = max(initial_map_size(), num_nodes + 2);
		map = map_allocator::allocate(map_size);

		// ��nstart nfinishָ��map��ӵ�е�ȫ���ڵ������������
		// �����������룬��ʹͷβ���˵���������һ����ÿ���ڵ��Ӧһ��������
		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur;

		// Ϊmap��ÿ���ڵ����û����������л���������������deque�Ŀ��ÿռ�
		// ���һ����������������һЩ��ԣ
		for (cur = nstart; cur <= nfinish; ++cur)
			*cur = allocate_node();

		// Ϊdq�ڵ�����������start��end�趨��ȷ����
		start.set_node(nstart);
		finish.set_node(nfinish);

		start.cur = start.first;
		finish.cur = finish.first + num_elements % buffer_size();
	}

	void reserve_map_at_back(size_type nodes_to_add = 1)
	{
		if (nodes_to_add + 1 > map_size - (finish.node - map))
			// ���mapβ�˵Ľڵ㱸�ÿռ䲻��
			// ������������������ػ�һ��map
			reallocate_map(nodes_to_add, false);
	}
	void reserve_map_at_front(size_type nodes_to_add = 1)
	{
		if (nodes_to_add > start.node - map)
			reallocate_map(nodes_to_add, true);
	}

	void reallocate_map(size_type nodes_to_add, bool add_at_front)
	{
		size_type old_num_nodes = finish.node - start.node + 1;
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_nstart;
		if (map_size > 2 * new_num_nodes)
		{	// �������Ľڵ��ƫ������λ��
			new_nstart = map + (map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			if (new_nstart < start.node)
				copy(start.node, finish.node + 1, new_nstart);
			else
				copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
		}
		else
		{
			size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
			// ����һ��ռ䣬׼������mapʹ��
			map_pointer new_map = map_allocator::allocate(new_map_size);
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			// ��ԭmap���ݿ�������
			copy(start.node, finish.node + 1, new_nstart);
			// �ͷ�ԭmap
			map_allocator::deallocate(map, map_size);
			// �趨��map����ʼ��ַ�ʹ�С
			map = new_map;
			map_size = new_map_size;
		}

		// �����趨������start��finish
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}

	// ֻ�е�finish.cur == finish.last - 1ʱ�Żᱻ����
	// Ҳ����˵��ֻ�е����һ��������ֻʣһ������Ԫ�ؿռ�ʱ�Żᱻ����
	void push_back_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_back(); // ������ĳ��������������¸���һ��map
		*(finish.node + 1) = allocate_node(); // ����һ���µĻ�����

		try
		{
			construct(finish.cur, t_copy); // ��Ա��Ԫ������
			finish.set_node(finish.node + 1); // �ı�finish������ָ���½ڵ�
			finish.cur = finish.first; // ����finish�Ѿ��ı䣬���finish.cur��Ҫ����
		}
		catch (...)
		{
			deallocate_node(*(finish.node + 1));
			throw;
		}
	}

	// ֻ�е�start.cur == start.firstʱ�Żᱻ����
	// Ҳ����˵��ֻ�е���һ��������û���κα���Ԫ��ʱ�Żᱻ����
	void push_front_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_front();  // ������ĳ������������ػ�һ��map
		*(start.node - 1) = allocate_node();

		try
		{
			start.set_node(start.node - 1); // �ı�start������ָ���½ڵ�
			start.cur = start.last - 1;
			construct(start.cur, t_copy);
		}
		catch (...)
		{
			start.set_node(start.node + 1);
			start.cur = start.first;
			deallocate_node(*(start.node - 1));
			throw;
		}
	}

	// ֻ�е�finish.cur == finish.firstʱ�Żᱻ����
	void pop_back_aux()
	{	
		destroy(finish.cur);
		deallocate_node(finish.first);
		finish.set_node(finish.node - 1);
		finish.cur = finish.last - 1;		
	}
	// ֻ�е�start.cur == start.last - 1ʱ�Żᱻ����
	void pop_front_aux()
	{	// ����һ������Ψһ��Ԫ������
		destroy(start.cur);
		// �ͷŵ�һ������
		deallocate_node(start.first);
		// ����start��״̬��ʹָ֮����һ���������ĵ�һ��Ԫ��
		start.set_node(start.node + 1);
		start.cur = start.first;
	}

	iterator insert_aux(iterator pos, const value_type& x)
	{
		difference_type index = pos - start;
		value_type x_copy = x;
		if (index < size() / 2)
		{
			push_font(front());
			iterator front1 = start;
			++front1;
			iterator front2 = front1;
			++front2;
			pos = start + index;
			iterator pos1 = pos;
			++pos1;
			copy(front1, pos1, front1);
		}
		else
		{
			push_back(back());
			iterator back1 = finish;
			--back1;
			iterator back2 = back1;
			--back2;
			pos = start + index;
			copy_backward(pos, back2, back1);
		}
		*pos = x_copy;
		return pos;
	}

public:
	// ctor
	deque(int n, const value_type& value)
		: start(), finish(), map(0), map_size(0)
	{
		fill_initialize(n, value);
	}

	iterator begin() { return start; }
	iterator end() { return finish; }

	reference operator[](size_type n)
	{
		return start[static_cast<difference_type>(n)]; // ����__deque_iterator::operator[]
	}
	reference front() { return *start; } // ����__deque_iterator::operator*
	reference back()
	{
		iterator tmp = finish;
		--tmp;	  // ����__deque_iterator::operator--
		return *tmp;   // ����__deque_iterator::operator*
	}

	size_type size() const
	{
		return finish - start;
	}
	size_type max_size() const
	{
		return static_cast<size_type>(-1); // what's on hell?
	}
	size_type buffer_size() const
	{
		return start.buffer_size();
	}
	bool empty() const { return finish == start; }
	
	void push_back(const value_type& t)
	{
		if (finish.cur != finish.last - 1)
		{
			// ��󻺳����������������ϵ�Ԫ�ر��ÿռ�
			construct(finish.cur, t);
			++(finish.cur);
		}
		else // ��󻺳���ֻ��һ��Ԫ�ر��ÿռ�
			push_back_aux(t);
	}	
	void push_front(const value_type& t)
	{
		if (start.cur != start.first)
		{
			// ��һ�����������б��ÿռ�
			construct(start.cur - 1, t); // ֱ���ڱ��ÿռ��Ϲ���Ԫ��
			--start.cur;
		}
		else // ��һ�����������ޱ��ÿռ�
			push_front_aux(t);
	}
	

	void pop_back()
	{
		if (finish.cur != finish.first)
		{
			// ��󻺳�����һ��������Ԫ��
			destroy(finish.cur);
			--finish.cur;			
		}
		else // ��󻺳���û���κ�Ԫ��
			pop_back_aux(); // ���ｫ���л��������ͷŹ���
	}
	void pop_front()
	{
		if (start.cur != start.last - 1)
		{
			// ��һ������������������Ԫ��
			destroy(start.cur);
			++start.cur;
		}
		else // ��һ����������һ��Ԫ��
			pop_front_aux(); // ���ｫ���л��������ͷŹ���
	}

	// clear()���������deque
	// ʼ��Ҫ����һ����������Ҳ���ǻָ���dq�ĳ�ʼ״̬
	void clear()
	{
		// �������ͷβ�����ÿһ��������������һ���Ǳ����ģ�
		for (map_pointer node = start.node + 1; node < finish.node; ++node)
		{
			// ��������������Ԫ������
			destroy(*node, *node + buffer_size());
			// �ͷŻ������ڴ�
			data_allocator::deallocate(*node, buffer_size());
		}

		if (start.node != finish.node)
		{
			// ������ͷβ����������

			// ��ͷβ��������Ŀǰ����Ԫ������
			destroy(start.cur, start.last);
			destroy(finish.first, finish.cur);

			// �ͷ�β������������ͷ������
			data_allocator::deallocate(finish.first, buffer_size());
		}
		else // ֻ��һ��������
			destroy(start.cur, finish.cur);

		finish = start; // ����״̬
	}

	// erase()�����ĳ��Ԫ��
	// posΪ�����
	iterator erase(iterator pos)
	{
		iterator next = pos;
		++next;
		// pos֮ǰ��Ԫ�ظ���
		difference_type index = pos - start;

		if (index < (size() >> 1))
		{
			// ���Ԫ�رȽ��٣����ƶ������֮ǰ��Ԫ��
			copy_backward(start, pos, next);
			// �ƶ���ϣ���ǰһ��Ԫ�����࣬ȥ��֮
			pop_front();
		}
		else
		{
			// �����֮���Ԫ�رȽ��٣����������֮
			copy(next, finish, pos);
			pop_back();
		}
		return start + index;		
	}
	// erase()�����ڵ�Ԫ��
	iterator erase(iterator first, iterator last)
	{
		if (first == start && last == finish)
		{
			clear();
			return finish;
		}
		else
		{
			difference_type n = last - first;
			difference_type elems_before = first - start;

			// ���ǰ����Ԫ�رȽ���
			if (elems_before < (size() - n) / 2)
			{
				// ����ƶ�ǰ��Ԫ��
				copy_backward(start, first, last);
				// ���dq�������
				iterator new_start = start + n;
				destroy(start, new_start);

				// �ͷ�����Ļ�����
				for (map_pointer cur = start.node; cur < new_start.node; ++cur)
					data_allocator::deallocate(*cur, buffer_size());

				start = new_start;
			}
			else
			{
				// ����������󷽵�Ԫ�رȽ���
				copy(last, finish, first);
				lterator new_finish = finish - n;
				destroy(new_finish, finish);

				for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
					data_allocator::deallocate(*cur, buffer_size());

				finish = new_finish;
			}
			return start + elems_before;
		}
	}

	// insert(position, x)����position֮ǰ����Ԫ��x
	iterator insert(iterator position, const value_type& x)
	{
		if (position.cur == start.cur)
		{	// ���������ǰ��
			push_front(x);
			return start;
		}
		else if (position.cur == finsh.cur)
		{
			// �����������
			push_back(x);
			iterator tmp = finsh;
			--tmp;
			return tmp;
		}
		else
		{
			return insert_aux(position, x);
		}
	}
	

};


#endif // !NAIVE_DEQUE_H

