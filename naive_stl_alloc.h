/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_STL_ALLOC_H
#define NAIVE_STL_ALLOC_H

#if 0
#	include <new>
#	define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#	include <iostream>
#	define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1);
#endif

// --------------------------------------------------------------------
// ��һ��������__malloc_alloc_template
// malloc-based allocator
template <int inst>
class __malloc_alloc_template
{
private:
	// oom: out of memory
	static void *oom_malloc(size_t);
	static void *oom_realloc(void *, size_t);
	static void(*__malloc_alloc_oom_handler)();

public:
	static void *allocate(size_t n)
	{
		void *result = malloc(n); // use malloc directly
		if (result == nullptr)
			result = oom_malloc(n);
		return result;
	}

	static void deallocate(void *p, size_t /* n */)
	{
		free(p); // use free directly
	}

	static void *reallocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void *result = realloc(p, new_sz);
		if (result == nullptr)
			result = oom_realloc(p, new_sz);
		return result;
	}

	// simulate set_new_handler
	static void(*set_malloc_handler(void(*f)()))()
	{
		void(*old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return (old);
	}
};

// malloc_alloc out-of-memory handling
template <int inst>
void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
	void(*my_alloc_handler)();
	void result;

	for ( ; ; )
	{
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (my_alloc_handler == nullptr)
			__THROW_BAD_ALLOC;
		(*my_alloc_handler)();
		result = malloc(n);
		if (result)
			return (result);
	}
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
	void(*my_alloc_handler)();
	void *result;

	for (;;)
	{
		my_alloc_handler = __malloc_alloc_oom_handler;
		if (my_alloc_handler == nullptr)
			__THROW_BAD_ALLOC;
		(*my_alloc_handler)();
		result = realloc(p, n);
		if (result)
			return (result);
	}
}

typedef __malloc_alloc_template<0> malloc_alloc;

// --------------------------------------------------------------------
// �ڶ���������__default_alloc_template
enum { __ALIGN = 8 };
enum { __MAX_BYTES = 128 };
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

template <bool threads, int inst>
class __default_alloc_template
{
private:
	// ROUND_UP��bytes�ϵ���8�ı���
	static size_t ROUND_UP(size_t bytes)
	{
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}

	union obj
	{
		union obj * free_list_link;
		char client_data[1];
	};

	// 16��free-list
	static obj * volatile free_list[__NFREELISTS];
	static size_t FREELIST_INDEX(size_t bytes)
	{
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}

	static void *refill(size_t n);
	static char *chunk_alloc(size_t size, int &nobjs);

	// chunk allocation state
	static char *start_free;
	static char *end_free;
	static size_t heap_size;

public:
	static void *allocate(size_t n)
	{
		obj * volatile *my_free_list;
		obj *result;

		// ����128�͵��õ�һ��������
		if (n > static_cast<size_t>(__MAX_BYTES))
		{
			return (malloc_alloc::allocate(n));
		}

		// Ѱ��16��free lists���ʵ���һ��
		my_free_list = free_list + FREELIST_INDEX(n);
		result = *my_free_list;
		// ���free list��û�п��������ˣ��͵���refill()��׼��Ϊfree list�������ռ�
		if (result == nullptr)
		{
			void *r = refill(ROUND_UP(n));
			return r;
		}

		// ����free list
		*my_free_list = result->free_list_link;
		reurn(result);
	}
	static void deallocate(void *p, size_t n)
	{
		obj *q = static_cast<obj *>(p);
		obj * volatiole *my_free_list;

		if (n > static_cast<size_t>(__MAX_BYTES))
		{
			malloc_alloc::deallocate(p, n);
			return;
		}

		// Ѱ�Ҷ�Ӧ��free list
		my_free_list = free_list + FREELIST_INDEX(n);
		// ����free list����������
		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}
	static void *reallocate(void *p, size_t old_sz, size_t new_sz);
};

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
char *__default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::obj * volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
	int nobjs = 20;
	// ����chunk_alloc()������ȡ��nobjs��������Ϊfree list���½ڵ�
	// ����nobjs��pass by reference
	char *chunk = chunk_alloc(n, nobjs);
	obj * volatile *my_free_list;
	obj *result;
	obj *current_obj, *next_obj;
	int i;

	// ���ֻ���һ�����飬�������ͷ�����������ã�free list���½ڵ�
	if (1 == nobjs)
		return (chunk);
	// ����׼������free list�������½ڵ�
	my_free_list = free_list + FREELIST_INDEX(n);

	// ������chunk�ռ��ڽ���free list
	result = static_cast<obj *>(chunk);
	// ���µ���free listָ�������õĿռ䣨ȡ���ڴ�أ�
	*my_free_list = next_obj = static_cast<obj *>(chunk + n);
	// ���½�free list�ĸ��ڵ㴮����
	for (i = 1; ; i++)
	{
		current_obj = next_obj;
		next_obj = static_cast<obj *>(static_cast<char *>(next_obj) + n);
		if (nobjs - 1 == i)
		{
			current_obj->free_list_link = 0;
			break;
		}
		else
		{
			current_obj->free_list_link = next_obj;
		}
	}
	return (result);
}

template <bool threads, int inst>
char*
__default_alloc_template<threads, inst>::
chunk_alloc(size_t size, int& nobjs)
// ע��nobjs��pass-by-reference
{
	char *result;
	size_t total_bytes = size * nobjs;
	size_t bytes_left = end_free - start_free; // �ڴ��ʣ��ռ�

	if (bytes_left >= total_bytes)
	{ // �ڴ��ʣ��ռ���ȫ����������
		result = start_free;
		start_free += total_bytes;
		return (result);
	}
	else if (bytes_left >= size)
	{ // �ڴ��ʣ��ռ䲻����ȫ���������������㹻��Ӧһ�����ϵ�����
		nobjs = bytes_left / size;
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return (result);
	}
	else
	{ // �ڴ��ʣ��ռ���һ������Ĵ�С���޷��ṩ
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		// �����������ڴ���еĲ�����ͷ�������ü�ֵ
		if (bytes_left > 0)
		{
			// �ڴ���л���һЩ��ͷ��������ʵ���free list
			// ����Ѱ���ʵ���free list
			obj * volatile *my_free_list =
				free_list + FREELIST_INDEX(bytes_left);
			// ����free list�����ڴ���еĲ���ռ����
			(static_cast<obj *>(start_free))->free_list_link = *my_free_list;
			*my_free_list = static_cast<obj *>(start_free);
		}

		// ����heap�ռ䣬���ڲ����ڴ��
		start_free = static_cast<char *>(malloc(bytes_to_get));
		if (start_free == nullptr)
		{
			// heap�ռ䲻�㣬malloc()ʧ��
			int i;
			obj * volatile *my_free_list, *p;
			// ������Ѱ�ʵ���free list��
			// ��������δʹ�����飬�������㹻�󡱵�free list
			for (i = size; i <= __MAX_BYTES; i += __ALIGN)
			{
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (nullptr == p)
				{
					*my_free_list = p->free_list_link;
					start_free = static_cast<char *>(p);
					end_free = start_free + i;
					// �ݹ�����Լ���Ϊ������nobjs
					return (chunk_alloc(size, nobjs));
				}
			}

			end_free = 0;
			start_free = static_cast<char *>(malloc_alloc::allocate(bytes_to_get));
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		// �ݹ�����Լ���Ϊ������nobjs
		return (chunk_alloc(size, nobjs));
	}
}

// ��allocΪ�ڶ���������
typedef __default_alloc_template<false, 0> alloc;

// ��װsimple_alloc
template <class T, class Alloc>
class simple_alloc
{
public:
	static T *allocate(size_t n)
	{
		return 0 == n ? 0 :
			static_cast<T*>(Alloc::allocate(n * sizeof(T)));
	}
	static T *allocate(void)
	{
		return static_cast<T*>(Alloc::allocate(sizeof(T)));
	}
	static void deallocate(T *p, size_t n)
	{
		if (0 != n)
			Alloc::deallocate(p, n * sizeof(T));
	}
	static void deallocate(T *p)
	{
		Alloc::deallocate(p, sizeof(T));
	}
};

#endif
