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

// 全局函数__deque_buf_size()
// n != 0，表示buffer size使用默认值，返回n
// n == 0，表示buffer size使用默认值
inline size_t __deque_buf_size(size_t n, size_t sz)
{
	return n != 0 ? n : (sz < 512 ? static_cast<size_t>(512 / sz) : static_cast<size_t>(1));
}

template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator
{
	// 未继承std::iterator
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

	// 保持与容器的联结
	T *cur; // 缓冲区的current元素
	T *frist; // 缓冲区的头
	T *last; // 缓冲区的尾（含备用空间）
	map_pointer node; // 指向map

	void set_node(map_pointer new_node)
	{
		node = new_node;
		first = *new_node;
		last = first + static_cast<difference_type>(buffer_size());
	}

	// 重载运算符
	reference operator*() const { return *cur; }
	pointer operator->() const { return &(operator*()); } // 标配
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

	// 实现随机存取，迭代器可以直接跳跃n个距离
	self& operator+=(difference_type n)
	{
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < static_cast<difference_type>(buffer_size()))
			// 目标位置在同一个缓存区内
			cur += n;
		else
		{
			// 标的位置不在同一缓存区内
			difference_type node_offset =
				offset > 0 ? offset / static_cast<difference_type>(buffer_size())
				: -static_cast<difference_type>((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			// 切换至正确的元素
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

	// 实现随机存取
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

// 定义deque类
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
	// 空间配置器，每次配置一个元素大小
	typedef simple_alloc<value_type, Alloc> data_allocator;
	// 空间配置器，每次配置一个指针大小
	typedef simple_alloc<pointer, Alloc> map_allocator;

protected:
	iterator start; // 第一个节点
	iterator finish; // 最后一个节点

	map_pointer map; // map是块连续空间，其中每个元素都是指针，指向一个节点
	size_type map_size; // map内的指针数，也即缓冲区数目

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

	// fill_initialize()负责产生并安排好deque的结构
	// 并初始化元素的初值
	void fill_initialize(size_type n, const value_type& value)
	{
		create_map_and_nodes(n); // 把deque的结构都产生并安排好
		map_pointer cur;

		// 为每个节点的缓冲区设定初值
		for (cur = start.node; cur < finish.node; ++cur)
			uninitialized_fill(*cur, *cur + buffer_size(), value);
		// 最后一个节点的设定稍有不同
		// 因为尾端可能有备用空间，不必设初值
		uninitialized_fill(finish.first, finish.cur, value);
	}

	// create_map_and_nodes负责产生并安排好deque的结构
	void create_map_and_nodes(size_type num_elements)
	{
		// 需要的节点数=(元素个数/每个缓冲区可容纳的元素个数)+1
		// 如果刚好整除，会多配一个节点
		size_type num_nodes = num_elements / buffer_size() + 1;

		// 一个map要管理的节点数map_size：最少8个，最多“所需节点数+2”
		// 2是为了：前后各预留一个，扩充时可以使用
		map_size = max(initial_map_size(), num_nodes + 2);
		map = map_allocator::allocate(map_size);

		// 令nstart nfinish指向map所拥有的全部节点的最中央区域
		// 保持在最中央，可使头尾两端的扩充能量一样大，每个节点对应一个缓存区
		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur;

		// 为map的每个节点配置缓冲区，所有缓冲区加起来就是deque的可用空间
		// 最后一个缓冲区可能留有一些余裕
		for (cur = nstart; cur <= nfinish; ++cur)
			*cur = allocate_node();

		// 为dq内的两个迭代器start和end设定正确内容
		start.set_node(nstart);
		finish.set_node(nfinish);

		start.cur = start.first;
		finish.cur = finish.first + num_elements % buffer_size();
	}

	void reserve_map_at_back(size_type nodes_to_add = 1)
	{
		if (nodes_to_add + 1 > map_size - (finish.node - map))
			// 如果map尾端的节点备用空间不足
			// 符合以上条件则必须重换一个map
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
		{	// 把启动的节点纠偏在中央位置
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
			// 配置一块空间，准备给新map使用
			map_pointer new_map = map_allocator::allocate(new_map_size);
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			// 将原map内容拷贝过来
			copy(start.node, finish.node + 1, new_nstart);
			// 释放原map
			map_allocator::deallocate(map, map_size);
			// 设定新map的起始地址和大小
			map = new_map;
			map_size = new_map_size;
		}

		// 重新设定迭代器start和finish
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}

	// 只有当finish.cur == finish.last - 1时才会被调用
	// 也就是说，只有当最后一个缓冲区只剩一个备用元素空间时才会被调用
	void push_back_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_back(); // 若符合某种条件则必须重新更换一个map
		*(finish.node + 1) = allocate_node(); // 配置一个新的缓冲区

		try
		{
			construct(finish.cur, t_copy); // 针对标的元素设置
			finish.set_node(finish.node + 1); // 改变finish，令其指向新节点
			finish.cur = finish.first; // 由于finish已经改变，因此finish.cur需要重设
		}
		catch (...)
		{
			deallocate_node(*(finish.node + 1));
			throw;
		}
	}

	// 只有当start.cur == start.first时才会被调用
	// 也就是说，只有当第一个缓冲区没有任何备用元素时才会被调用
	void push_front_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_front();  // 若符合某种条件则必须重换一个map
		*(start.node - 1) = allocate_node();

		try
		{
			start.set_node(start.node - 1); // 改变start，令其指向新节点
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

	// 只有当finish.cur == finish.first时才会被调用
	void pop_back_aux()
	{	
		destroy(finish.cur);
		deallocate_node(finish.first);
		finish.set_node(finish.node - 1);
		finish.cur = finish.last - 1;		
	}
	// 只有当start.cur == start.last - 1时才会被调用
	void pop_front_aux()
	{	// 将第一缓冲区唯一的元素析构
		destroy(start.cur);
		// 释放第一缓冲区
		deallocate_node(start.first);
		// 调整start的状态，使之指向下一个缓冲区的第一个元素
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
		return start[static_cast<difference_type>(n)]; // 调用__deque_iterator::operator[]
	}
	reference front() { return *start; } // 调用__deque_iterator::operator*
	reference back()
	{
		iterator tmp = finish;
		--tmp;	  // 调用__deque_iterator::operator--
		return *tmp;   // 调用__deque_iterator::operator*
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
			// 最后缓冲区尚有两个或以上的元素备用空间
			construct(finish.cur, t);
			++(finish.cur);
		}
		else // 最后缓冲区只有一个元素备用空间
			push_back_aux(t);
	}	
	void push_front(const value_type& t)
	{
		if (start.cur != start.first)
		{
			// 第一级缓冲区尚有备用空间
			construct(start.cur - 1, t); // 直接在备用空间上构造元素
			--start.cur;
		}
		else // 第一级缓冲区已无备用空间
			push_front_aux(t);
	}
	

	void pop_back()
	{
		if (finish.cur != finish.first)
		{
			// 最后缓冲区有一个或以上元素
			destroy(finish.cur);
			--finish.cur;			
		}
		else // 最后缓冲区没有任何元素
			pop_back_aux(); // 这里将进行缓冲区的释放工作
	}
	void pop_front()
	{
		if (start.cur != start.last - 1)
		{
			// 第一缓冲区有两个或以上元素
			destroy(start.cur);
			++start.cur;
		}
		else // 第一缓冲区仅有一个元素
			pop_front_aux(); // 这里将进行缓冲区的释放工作
	}

	// clear()：清除整个deque
	// 始终要保留一个缓冲区，也就是恢复到dq的初始状态
	void clear()
	{
		// 以下针对头尾以外的每一个缓冲区（它们一定是饱满的）
		for (map_pointer node = start.node + 1; node < finish.node; ++node)
		{
			// 将缓冲区内所有元素析构
			destroy(*node, *node + buffer_size());
			// 释放缓冲区内存
			data_allocator::deallocate(*node, buffer_size());
		}

		if (start.node != finish.node)
		{
			// 至少有头尾两个缓冲区

			// 将头尾缓冲区的目前所有元素析构
			destroy(start.cur, start.last);
			destroy(finish.first, finish.cur);

			// 释放尾缓冲区，保留头缓冲区
			data_allocator::deallocate(finish.first, buffer_size());
		}
		else // 只有一个缓冲区
			destroy(start.cur, finish.cur);

		finish = start; // 调整状态
	}

	// erase()：清除某个元素
	// pos为清除点
	iterator erase(iterator pos)
	{
		iterator next = pos;
		++next;
		// pos之前的元素个数
		difference_type index = pos - start;

		if (index < (size() >> 1))
		{
			// 如果元素比较少，就移动清除点之前的元素
			copy_backward(start, pos, next);
			// 移动完毕，最前一个元素冗余，去除之
			pop_front();
		}
		else
		{
			// 清除点之后的元素比较少，反其道而行之
			copy(next, finish, pos);
			pop_back();
		}
		return start + index;		
	}
	// erase()区间内的元素
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

			// 如果前方的元素比较少
			if (elems_before < (size() - n) / 2)
			{
				// 向后移动前方元素
				copy_backward(start, first, last);
				// 标记dq的新起点
				iterator new_start = start + n;
				destroy(start, new_start);

				// 释放冗余的缓冲区
				for (map_pointer cur = start.node; cur < new_start.node; ++cur)
					data_allocator::deallocate(*cur, buffer_size());

				start = new_start;
			}
			else
			{
				// 如果清除区间后方的元素比较少
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

	// insert(position, x)，在position之前插入元素x
	iterator insert(iterator position, const value_type& x)
	{
		if (position.cur == start.cur)
		{	// 插入点是最前端
			push_front(x);
			return start;
		}
		else if (position.cur == finsh.cur)
		{
			// 插入点是最后端
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

