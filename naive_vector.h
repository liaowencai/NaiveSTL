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
	// ʹ��simple_alloc
	typedef simple_alloc<value_type, Alloc> data_allocator;
	iterator start;
	iterator finish; // ʹ�ÿռ��β
	iterator end_of_storage; // ���ÿռ��β

	void insert(iterator position, size_type n, const T& x);
	void insert_aux(iterator position, const T& x);
	void deallocate()
	{
		if (start != nullptr)
			data_allocator::deallocate(start, end_of_storage - start);
	}

	// ��䲢���Գ�ʼ��
	void fill_initialize(size_type n, const T& value)
	{
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

	// ���ÿռ䲢��������
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
		deallocate(); // vector��member function
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
		else // �Ѿ�û�б��ÿռ�
			insert_aux(end(), x); // vector��member function
	}

	void pop_back()
	{
		--finsh;
		destroy(finish);
	}

	iterator erase(iterator position) // ���ĳλ���ϵ�Ԫ��
	{
		if (position + 1 != end())
			uninitialized_copy(position + 1, finish, position); // ����Ԫ����ǰ�ƶ�
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
			erase(begin() + new_size, end()); // �ض�
		else
			insert(end(), new_size - size(), x); // ����
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
	{ // ���б��ÿռ�
		// �ڱ��ÿռ���ʼ������һ��Ԫ�أ�����vector���һ��Ԫ��ֵΪ���ֵ
		construct(finish, *(finish - 1));
		// ����ˮλ
		++finish;
		T x_copy = x;
		copy_backward(position, finish - 2, finish - 1);
		*position = x_copy;
	}
	else
	{ // ���ޱ��ÿռ�
		const size_type old_size = size();
		// ����ԭ��ԭ��СΪ0��������1����������Ϊԭ��С��2����ע���Ƶ����Ӷ�
		const size_type len = ((old_size != 0) ? 2 * old_size : 1);

		iterator new_start = data_allocator::allocate(len);
		iterator new_finish = new_start;

		try
		{
			// ��ԭvector�����ݿ�������vector
			new_finish = uninitialized_copy(start, position, new_start);
			// Ϊ��Ԫ���趨��ֵx
			construct(new_finish, x);
			//����ˮλ
			++new_finish;
			// ��������ԭ����Ҳ��������
			new_finish = uninitialized_copy(position, finish, new_finish);
		}
		catch (...)
		{ // "commit or rollback" semantics
			destroy(new_start, new_finish);
			data_allocator::deallocate(new_start, len);
			throw;
		}

		// �������ͷ�ԭvector
		destroy(begin(), end());
		deallocate();

		// ������������ָ����vector
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
			// ���ÿռ���ڵ��ڡ�����Ԫ�ظ�����
			T x_copy = x;
			// ��������֮�������Ԫ�ظ���
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n)
			{
				// �������֮�������Ԫ�ظ��������ڡ�����Ԫ�ظ�����
				uninitialized_copy(finish - n, finish, old_finish);
				finish += n; // ��vectorβ�˱�Ǻ���
				copy_backward(position, old_finish - n, old_finish);
				fill(position, position + n, x_copy);
			}
			else
			{
				// �������֮�������Ԫ�ظ�����<=������Ԫ�ظ�����
				uninitialized_fill_n(finish, n - elems_after, x_copy);
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				fill(position, old_finish, x_copy);
			}
		}
		else
		{
			// ���ÿռ� < ������Ԫ�ظ��������������ö�����ڴ棩
			// ���Ⱦ������ȣ��ɳ��ȵ�������ɳ���+����Ԫ�ظ���
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			// ���������µ�vector�ռ�
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

