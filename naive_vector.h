#ifndef NAIVE_VECTOR_H
#define NAIVE_VECTOR_H

#include "naive_stl_iterator.h"
#include "naive_stl_construct.h"
#include "naive_stl_alloc.h"
#include "naive_stl_uninitialized.h"
#include "naive_algorithm.h"

template <class T, class Alloc = alloc>
class vector
{
public:
	typedef T			value_type;
	typedef value_type* pointer;
	typedef value_type* iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;

protected:
	// 使用simple_alloc
	typedef simple_alloc<value_type, Alloc> data_allocator;
	iterator start;
	iterator finish; // 使用空间的尾
	iterator end_of_storage; // 可用空间的尾

	void insert(iterator position, size_type n, const T& x);
	void insert_aux(iterator position, const T& x);
	void deallocate()
	{
		if (start != nullptr)
			data_allocator::deallocate(start, end_of_storage - start);
	}

	// 填充并予以初始化
	void fill_initialize(size_type n, const T& value)
	{
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

	// 配置空间并填满内容
	iterator allocate_and_fill(size_type n, const T& x)
	{
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}

public:
	iterator begin() { return start; }
	iterator end() { return finish; }
	size_type size() const { return static_cast<size_type>(end() - begin()); }
	size_type capacity() const
	{
		return static_cast<size_type>(end_of_storage - begin());
	}
	bool empty() const { return begin() == end(); }
	reference operator[](size_type n) { return *(begin() + n); }

	vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
	vector(size_type n, const T& value) { fill_initialize(n, value); }
	vector(int n, const T& value) { fill_initialize(n, value); }
	vector(long n, const T& value) { fill_initialize(n, value); }
	explicit vector(size_type n) { fill_initialize(n, T()); }

	~vector()
	{
		destroy(start, finish);
		deallocate(); // vector的member function
	}
	reference front() { return *begin(); }
	reference back() { return *(end() - 1); }
	void push_back(const T& x)
	{
		if (finish != end_of_storage)
		{
			construct(finish, x);
			++finish;
		}
		else // 已经没有备用空间
			insert_aux(end(), x); // vector的member function
	}

	void pop_back()
	{
		--finsh;
		destroy(finish);
	}

	iterator erase(iterator position) // 清除某位置上的元素
	{
		if (position + 1 != end())
			uninitialized_copy(position + 1, finish, position); // 后续元素向前移动
		--finish;
		destroy(finish);
		return position;
	}
	iterator erase(iterator first, iterator last)
	{
		uinitialized_copy(last, finish, first)
		size_t n = last - first;
		destory(finish - n, finish);
		finish -= n;
		return first;
	}

	void resize(size_type new_size, const T& x)
	{
		if (new_size < size())
			erase(begin() + new_size, end()); // 截断
		else
			insert(end(), new_size - size(), x); // 补充
	}
	void resize(size_type new_size)
	{
		resize(new_size, T());
	}

	void clear() { erase(begin(), end()); }



};

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x)
{
	if (finish != end_of_storage)
	{ // 还有备用空间
		// 在备用空间起始处构造一个元素，并以vector最后一个元素值为其初值
		construct(finish, *(finish - 1));
		// 调整水位
		++finish;
		T x_copy = x;
		copy_backward(position, finish - 2, finish - 1);
		*position = x_copy;
	}
	else
	{ // 已无备用空间
		const size_type old_size = size();
		// 配置原则：原大小为0，则配置1，否则配置为原大小的2倍，注意推导复杂度
		const size_type len = ((old_size != 0) ? 2 * old_size : 1);

		iterator new_start = data_allocator::allocate(len);
		iterator new_finish = new_start;

		try
		{
			// 将原vector的内容拷贝到新vector
			new_finish = uninitialized_copy(start, position, new_start);
			// 为新元素设定初值x
			construct(new_finish, x);
			//调整水位
			++new_finish;
			// 将安插点的原内容也拷贝过来
			new_finish = uninitialized_copy(position, finish, new_finish);
		}
		catch (...)
		{ // "commit or rollback" semantics
			destroy(new_start, new_finish);
			data_allocator::deallocate(new_start, len);
			throw;
		}

		// 析构并释放原vector
		destroy(begin(), end());
		deallocate();

		// 调整迭代器，指向新vector
		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + len;
	}
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
	if (n != 0)
	{
		if (static_cast<size_type>(end_of_storage - finish) >= n)
		{
			// 备用空间大于等于“新增元素个数”
			T x_copy = x;
			// 计算插入点之后的现有元素个数
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n)
			{
				// “插入点之后的现有元素个数”大于“新增元素个数”
				uninitialized_copy(finish - n, finish, old_finish);
				finish += n; // 将vector尾端标记后移
				copy_backward(position, old_finish - n, old_finish);
				fill(position, position + n, x_copy);
			}
			else
			{
				// “插入点之后的现有元素个数”<=“新增元素个数”
				uninitialized_fill_n(finish, n - elems_after, x_copy);
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				fill(position, old_finish, x_copy);
			}
		}
		else
		{
			// 备用空间 < “新增元素个数”（必须配置额外的内存）
			// 首先决定长度：旧长度的两倍或旧长度+新增元素个数
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			// 以下配置新的vector空间
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;

			try
			{
				new_finish = uninitialized_copy(start, position, new_start);
				new_finish = uninitialized_copy(new_finish, n, x);
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...)
			{
				destory(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			destroy(start, finish);
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}

#endif // !NAIVE_VECTOR_H

