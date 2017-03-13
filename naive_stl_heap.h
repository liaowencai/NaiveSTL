/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_HEAP_H
#define NAIVE_HEAP_H

#include "naive_stl_iterator.h"

template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first,
	RandomAccessIterator last)
{
	// 每次函数被调用时，新元素应已置于底部容器的最尾端
	__push_heap_aux(first, last, distance_type(first));
}

template <class RandomAccessIterator, class Distance, class T>
inline void __push_heap_aux(RandomAccessIterator first,
	RandomAccessIterator last, Distance*, T*)
{
	// 新值必然置于底部
	// 容器的最尾端，holeIndex = last - first - 1
	__push_heap(first, Distance((last - first) - 1), Distance(0), 
		static_cast<T>(*(last - 1));
}

template <class RandomAccessIterator, class Distance, class T>
void __push_heap(RandomAccessIterator, Distance holeIndex,
	Distance topIndex, T value)
{
	Distance parent = (holeIndex - 1) / 2; // 找出父节点

	while (holeIndex > topIndex &&
		*(first + parent) < value)
	{
		// 当前尚未达到顶端，且父节点小于新值
		*(first + holeIndex) = *(first + parent); // 令洞值为父节点的值
		holeIndex = parent; // percolate up：上滤操作
		parent = (holeIndex - 1) / 2; // 更新parent
	}
	*(first + holeIndex) = value;
}

template <class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last)
{
	__pop_heap_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __pop_heap_aux(RandomAccessIterator first,
	RandomAccessIterator last, T*)
{
	__pop_heap(first, last - 1, last - 1, static_cast<T>(*(last - 1)),
		distance_type(first));
}

template <class RandomAccessIterator, class T, class Distance>
inline void __pop_heap(RandomAccessIterator first,
	RandomAccessIterator last,
	RandomAccessIterator result,
	T value, Distance*)
{
	*result = *first;
	__adjust_heap(first, Distance(0), Distance(last - first), value);
}

template <class RandomAccessIterator, class Distance, class T>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex,
	Distance len, T value)
{
	Distance topIndex = holeIndex;
	Distance secondChild = 2 * topIndex + 2;
	while (secondChild < len)
	{
		if (*(first + secondChild) < *(first + (secondChild - 1)))
			--secondChild;
		// percolate down
		*(first + holeIndex) = *(first + secondChild);
		holeIndex = secondChild;
		secondChild = 2 * secondChild + 1;
	}

	if (secondChild == len)
	{
		// 没有右子节点，只有左子节点
		*(first + holeIndex) = *(first + (secondChild - 1));
		holeIndex = secondChild - 1;
	}
	__push_heap(first, holeIndex, topIndex, value); // 我要你有何用？
}

template <class RandomAccessIterator>
void sort_heap(RandomAccessIterator first,
	RandomAccessIterator last)
{
	while (last - first > 1)
		pop_heap(first, last--);
}

// make_heap(first, last)将[first, last)排列成一个heap
template <class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first,
	RandomAccessIterator last)
{
	__make_heap(first, last, value_type(first), distance_type(first));
}

template <class RandomAccessIterator, class T, class Distance>
void __make_heap(RandomAccessIterator first,
	RandomAccessIterator last, T*, Distance*)
{
	if (last - first < 2) return;
	Distance len = last - first;
	Distance holeIndex = (len - 2) / 2;

	while (true)
	{
		__adjust_heap(first, parent, len, static_cast<T>(*(first + holeIndex)));
		if (holeIndex == 0) return;
		--holeIndex;
	}
}

#endif // !NAIVE_HEAP_H