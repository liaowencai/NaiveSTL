/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_STL_ALGOBASE_H
#define NAIVE_STL_ALGOBASE_H

#include <cstring>
#include <utility>
#include <cstddef>
#include "naive_stl_iterator.h"
#include "naive_type_traits.h"

// equal：判断两个序列在[first, last)区间内相等
template <class InputIterator1, class InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2)
{
	for (; first1 != last1; ++first1, ++first1)
		if (*first1 != *first2)
			return false;
	return true;
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
inline bool equal(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, BinaryPredicate binary_pred)
{
	for (; first1 != last1; ++first1, ++first2)
		if (!binary_pred(*first1, *first2))
			return false;
	return true;
}

// fill：将[first, last)内的所有元素该填新值
template <class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value)
{
	for (; first != last; ++first)
		*first = value;
}

// fill_n：将[first, last)内的前n个元素该填新值，
// 返回的迭代器指向被填入的最后一个元素的下一个位置
template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value)
{
	for (; n > 0; --n, ++first)
		*first = value;
	return first;
}

// iter_swap：将两个ForwardIterators所指的对象对调
template <class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
{
	__iter_swap(a, b, value_type(a));
}
template <class ForwardIterator1, class ForwardIterator2, class T>
inline void __iter_swap(ForwardIterator1 a, ForwardIterator2 b, T*)
{
	T tmp = *a;
	*a = *b;
	*b = tmp;
}

// lexicographical_compare：比较字典序大小
template <class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (*first1 < *first2)
			return true;
		if (*first2 < *first1)
			return false;
	}
	return first1 == last1 && first2 != last2;
}
template <class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2, Compare comp)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (comp(*first1, *first2))
			return true;
		if (comp(*first2, *first1))
			return false;
	}
	return first1 == last1 && first2 != last2;
}
inline bool lexicographical_compare(const unsigned char* first1,
	const unsigned char* last1,
	const unsigned char* first2,
	const unsigned char* last2)
{
	const size_t len1 = last1 - first1;
	const size_t len2 = last2 - first2;
	const int result = memcmp(first1, first2, min(len1, len2));

	// 如果不相上下，则长度较长者被视为比较大
	return (result != 0) ? (result < 0) : (len1 < len2);
}
inline bool lexicographical_compare(const char* first1,
	const char* last1,
	const char* first2,
	const char* last2)
{
	const size_t len1 = last1 - first1;
	const size_t len2 = last2 - first2;
	const int result = memcmp(first1, first2, min(len1, len2));

	// 如果不相上下，则长度较长者被视为比较大
	return (result != 0) ? (result < 0) : (len1 < len2);
}

// max
template <class T>
inline const T& max(const T& a, const T& b)
{
	return (a < b) ? b : a;
}
template <class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp)
{
	return comp(a, b) ? b : a;
}

// min
template <class T>
inline const T& min(const T& a, const T& b)
{
	return (b < a) ? b : a;
}
template <class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp)
{
	return comp(b, a) ? b : a;
}

// mismatch：比较两个序列，返回一对迭代器，指出两者之间的第一个不匹配点
template <class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2> mismatch(
	InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2)
{
	// 显然，序列一的元素个数不能少于序列二的元素个数
	while (first1 != last1 && *first1 == *first2)
	{
		++first1;
		++first2;
	}
	return pair<InputIterator1, InputIterator2>(first1, first2);
}
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
pair<InputIterator1, InputIterator2> mismatch(
	InputIterator1 first1, InputIterator1 last1,
	InputIterator2 firts2,
	BinaryPredicate binary_pred)
{
	while (first1 != last1 && binary_pred(*first1, *first2))
	{
		++first1;
		++first2;
	}
	return pair<InputIterator1, InputIterator2>(first1, first2);
}

// swap：对调两个对象的内容
template <class T>
inline void swap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

// copy：将输入区间[first, last)内的元素复制到输出区间[result, result+(last-first))内
// 返回迭代器result+(last-first)
// 特别注意，如果输入区间与输出区间重叠，可能会导致错误结果，需要特别小心
template <class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
	// __copy_dispatch是一个类
	return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}

inline char* copy(const char* first, const char* last, char* result)
{
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}

template <class InputIterator, class OutputIterator>
struct __copy_dispatch
{
	OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy(first, last, result, iterator_category(first));
	}
};

template <class T>
struct __copy_dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};

template <class T>
struct __copy_dispatch<const T*, T*>
{
	T* operator()(const T* first, const T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};

// __copy的InputIterator版本
template <class InputIterator, class OutputIterator>
inline OutputIterator __copy(InputIterator first, InputIterator last,
	OutputIterator result, input_iterator_tag)
{
	for (; first != last; ++result, ++first)
		*result = *first;
	return result;
}

// __copy的RandomAccessIterator版本
template <class InputIterator, class OutputIterator>
inline OutputIterator __copy(InputIterator first, InputIterator last,
	OutputIterator result, random_access_iterator_tag)
{
	return __copy_d(first, last, result, distance_type(first));
}

template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator
__copy_d(RandomAccessIterator first, RandomAccessIterator last,
	OutputIterator result, Distance*)
{
	for (Distance n = last - first; n > 0; --n, ++result, ++first)
		*result = *first;
	return result;
}

// __copy_t：指针所指对象是否具备trivial assignment operator
template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, __true_type)
{
	memmove(result, first, sizeof(T) * (last - first));
	return result + (last - first);
}

template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, __false_type)
{
	return __copy_d(frist, last, result, (ptrdiff_t*)0);
}
#endif // !NAIVE_STL_ALGOBASE_H