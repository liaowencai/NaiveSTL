/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_STL_ALGO_H
#define NAIVE_STL_ALGO_H

#include "naive_stl_algobase.h"
#include "naive_stl_heap.h" // make_heap, push_heap, pop_heap, sort_heap

#include "naive_stl_iterator.h"

const int __stl_threshold = 16;
//----------------------------------------------------------------------------
// set��������㷨
// set��һ��sorted range������ģ�������һ���㷨��ǰ��

// set_union������S1/S2�Ĳ���
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	OutputIterator result)
{
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 < *first2)
		{
			*result = *first1;
			++first1;
		}
		else if (*first2 < *first1)
		{
			*result = *first2;
			++first2;
		}
		else // *first1 == *first2
		{
			*result = *first1;
			++first1;
			++first2;
		}
		++result;
	}
	// [first1, last1)��[first2, last2)������һ���ǿհ�����
	return copy(first2, last2, copy(first1, last1, result));
}

// set_intersection������S1/S2�Ľ���
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	OutputIterator result)
{
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 < *first2)
			++first1;
		else if (*first2 < *first1)
			++first2;
		else
		{
			*result = *first1;
			++first1;
			++first2;
			++result;
		}
		return result;
	}
}

// set_difference������S1/S2�ĲS1-S2
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	OutputIterator result)
{
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 < *last1)
		{
			*result = *first1;
			++first1;
			++result;
		}
		else if (*first2 < *first1)
			++first2;
		else
		{
			++first1;
			++first2;
		}
	}
	return copy(first1, last1, result);
}

// set_symmetric_difference��S1/S2�ĶԳƲ
// ������S1����������S2 + ������S2����������S1��ÿһ��Ԫ��
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(
	InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	OutputIterator result)
{
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 < *first2)
		{
			*result = *first1;
			++first1;
			++result;
		}
		else if (*first2 < *first1)
		{
			*result = *first2;
			++first2;
			++result;
		}
		else
		{
			++first1;
			++first2;
		}
	}
	result copy(first1, last1, copy(first2, last2, result));
}

// adjacent_find���ҳ���һ����������������Ԫ��
template <class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last)
{
	if (first == last)
		return last;

	ForwardIterator next = first;
	while (++next != last)
	{
		if (*first == *next)
			return first;
		first = next;
	}
	return last;
}
template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last,
	BianryPredicate binary_pred)
{
	if (first == last)
		return last;

	ForwardIterator next = first;
	while (++next != last)
	{
		if (binary_pred(*first, *next))
			return first;
	}
	return last;
}

// count��������value��ȵ�Ԫ�ظ���
template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T& value)
{
	typename iterator_traits<InputIterator>::difference_type n = 0;
	for (; first != last; ++first)
		if (*first == value)
			++n;
	return n;
}
template <class InputIterator, class Predicate>
typename iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred)
{
	typename iterator_traits<InputIterator>::difference_type n = 0;
	for (; first != last; ++first)
		if (pred(*first))
			++n;
	return n;
}

// find�����ص�һ��ָ�����value��Ԫ�صĵ�����
template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
	while (first != last && *first != value)
		++first;
	return first;
}
template <class InputIterator, class Predicate>
InputIterator find_if(InputIterator first, InputIterator last, Predicate pred)
{
	while (first != last && !pred(*first))
		++first;
	return first;
}

// find_end��������[first1, last1)�У���������[first2, last2)�����һ�γ��ֵ�
// ���򣬷��ص�����last1
template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1
find_end(ForwardIterator1 first1, ForwardIterator1 last1,
	ForwardIterator2 first2, ForwardIterator2 last2)
{
	typedef typename iterator_traits<ForwardIterator1>::iterator_category category1;
	typedef typename iterator_traits<ForwardIterator2>::iterator_categoty category2;

	return __find_end(first1, last1, first2, last2, category1(), category2());
}

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator1
__find_end(ForwardIterator1 first1, ForwardIterator2 last1,
	ForwardIterator2 first2, ForwardIterator last2,
	forward_iterator_tag, forward_iterator_tag)
{
	if (first2 == last2)
		return last1;
	else
	{
		ForwardIterator1 result = last1;
		while (1)
		{
			ForwardIterator1 new_result = search(first1, last1, first2, last2);
			if (new_result == last1) // û�ҵ�
				return result;
			else
			{
				result = new_result;
				first1 = new_result;
				++first1;
			}
		}
	}
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator1
__find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
	BidirectionalIterator2 first2, BidirectionalIterator2 last2,
	bidirectional_iterator_tag, bidirectional_iterator_tag)
{
	typedef reverse_iterator<BidirectionalIterator1> reviter1;
	typedef reverse_iterator<BidirectionalIterator1> reviter2;

	reviter1 rlast1(first1);
	reviter1 rlast2(first2);

	reviter1 rresult = search(reviter1(last1), rlast1, reviter2(last2), rlast2);
	if (rresult == rlast1)
		return last1;
	else
	{
		BidirectionalIterator1 result = rresult.base();
		advance(result, -distance(first2, last2));
		return result;
	}
}

// find_first_of��������[first1, last1)�У���������[first2, last2)��ĳһ��Ԫ�صĵ�һ�γ��ֵ�
// ���δ�����κ�Ԫ�أ����ص�����last1
template <class InputIterator, class ForwardIterator>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
	ForwardIterator first2, ForwardIterator last2)
{
	for (; first1 != last1; ++first1)
		for (ForwardIterator iter = first2; iter != last2; ++iter)
			if (*first1 == *iter)
				return first1;
	return last1;
}
template <class InputIterator, class ForwardIterator, class BinaryPredicate>
InputIterator find_first_of(InputIterator first1, InputIterator last1,
	ForwardIterator first2, ForwardIterator last2,
	BinaryPredicate comp)
{
	for (; first1 != last1; ++first1)
		for (ForwardIterator iter = first2; iter != last2; ++iter)
			if (comp(*first1, *iter))
				return first1;
	return last1;
}

// ����������fʵ����[first, last)�����ڵ�ÿһ��Ԫ������
// f�����Ըı�Ԫ�����ݣ���Ϊfirst��last����InputIterators
// ����֤���ܸ�ֵ��Ϊ
template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f)
{
	for (; first != last; ++first)
		f(*first);
	return f;
}

// generate
template <class ForwardIterator, class Generator>
void generate(ForwardIterator first, ForwardIterator last, Generator gen)
{
	for (; first != last; ++first)
		*first = gen();
}
template <class OutputIterator, class Size, class Generator>
OutputIterator generate_n(OutputIterator first, Size n, Generator gen)
{
	for (; n > 0; --n, ++first)
		*first = gen();
	return first;
}

// includes���Ƿ񺭸�
template <class InputIterator1, class InputIterator2>
bool includes(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2)
{
	while (first1 != last1 && first2 != last2)
		if (*first2 < *first1)
			return false;
		else if (*first1 < *first2)
			++first1;
		else
			++first1, ++first2;
	return first2 == last2;
}
template <class InputIterator1, class InputIterator2, class Compare>
bool includes(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	Compare comp)
{
	while (first1 != last1 && first2 != last2)
		if (comp(*first2, *first1))
			reutnr false;
		else if (comp(*first1, *first2))
			++first1;
		else
			++first1, ++first2;

	return first2 == last2;
}
// max_element
template <class ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last)
{
	if (first == last)
		return first;
	ForwardIterator result = first;
	while (++first != last)
		if (*result < *first)
			result = first;
	return result;
}
template <class ForwardIterator, class Compare>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare comp)
{
	if (first == last)
		return first;
	ForwardIterator result = first;
	while (++first != last)
		if (comp(*result, *first))
			result = first;
	return result;
}
// min-element
template <class ForwardIterator>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last)
{
	if (first == last)
		return first;
	ForwardIterator result = first;
	while (++first != last)
		if (*first < *result)
			result = first;
	return result;
}
template <class ForwardIterator, class Compare>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare comp)
{
	if (first == last)
		return first;
	ForwardIterator result = first;
	while (++first != last)
		if (comp(*first, *result))
			result = first;
	return result;
}
// merge��Ӧ������������
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator 
merge(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	OutputIterator result)
{
	while (first1 != last1 && first2 != last2)
	{
		if (*first2 < *first1)
		{
			*result = *first2;
			++first2;
		}
		else
		{
			*result = *first1;
			++first1;
		}		
		++result;
	}
	// һ����������һ��������һ��Ϊ��
	return copy(first2, last2, copy(first1, last1, result));
}
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	OutputIterator result, Compare comp)
{
	while (first1 != last1 && first2 != last2)
	{
		if (comp(*first2, *first1))
		{
			*result = *first2;
			++first2;
		}
		else
		{
			*result = *first1;
			++first1;
		}
		++result;
	}
	return copy(first2, last2, copy(first1, last1, result));
}

// partition��������[first, last)�е�Ԫ����������
// �ж�Ϊtrue��Ԫ�أ����������ǰ��
// �ж�Ϊfalse��Ԫ�أ���������ĺ��
// ����֤����ԭ�������λ��
template <class BidirectionalIterator, class Predicate>
BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last, Predicate pred)
{
	while (true)
	{
		while (true)
		{
			if (first == last)
				return first;
			else if (pred(*first))
				++first;
			else
				break;
		}
		--last;
		while (true)
		{
			if (first == last)
				return first;
			else if (!pred(*last))
				--last;
			else
				break;
		}

		iter_swap(first, last);
		++first;
	}
}

// remove���Ƴ�������ɾ����
template <class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value)
{
	first = find(first, last, value);
	ForwardIterator next = first;
	return first == last ? first : remove_copy(++next, last, first, value);
}
// remove_copy���Ƴ�[first, last)������������value��ȵ�Ԫ��
// ��������Ƶ�һ����result��ʾ��ʼλ�õ�������
// ����ֵָ�������Ƶ����Ԫ�ص���һλ��
template <class InputIterator, class OutputIterator, class T>
OutputIterator remove_copy(InputIterator first, InputIterator last,
	OutputIterator result, const T& value)
{
	for ( ; first != last; ++first)
		if (*first != value)
		{
			*result = *first;
			++result;
		}
	return result;
}
// remove_if��������remove��ͬ�����Ƴ����ж���׼��pred�˶�Ϊtrue��Ԫ��
template <class ForwardIterator, class Predicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate pred)
{
	first = find_if(first, last, pred);
	ForwardIterator next = first;
	return first == last ? first : remove_copy_if(++next, last, first, pred);
}
// remove_copy_if
template <class InputIterator, class OutputIterator, class Predicate>
OutputIterator remove_copy_if(InputIterator first, InputIterator last, OutputIterator result,
	Predicate pred)
{
	for (; first != last; ++first)
		if (!pred(*first))
		{
			*result = *first;
			++result;
		}
	return result;
}

// replace����[first, last)�����ڵ�����old_value����new_valueȡ��
template <class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last,
	const T& old_value, const T& new_value)
{
	for (; first != last; ++first)
		if (*first == old_value)
			*first = new_value;
}
// replace_copy
template <class InputIterator, class OutputIterator, class T>
OutputIterator replace_copy(InputIterator first, InputIterator last,
	OutputIterator result, const T& old_value, const T& new_value)
{
	for (; first != last; ++first)
		*result = (*first == old_value ? new_value : *first);
}
// replace_if
template <class ForwardIterator, class Predicate, class T>
void replace_if(ForwardIterator first, ForwardIterator last, Predicate pred, const T& new_value)
{
	for (; first != last; ++first)
		if (pred(*first))
			*first = new_value;
}
// replace_copy_if
template <class InputIterator, class OutputIterator, class Predicate, class T>
OutputIterator replace_copy_if(InputIterator first, InputIterator last,
	OutputIterator result, Predicate pred, const T& new_value)
{
	for (; first != last; ++first, ++result)
		*result = (pred(*first) ? new_value : *first);
}

// reverse��������[first, last)��Ԫ���������еߵ�����
template <class BidirectionalIterator>
inline void reverse(BidirectionalIterator first, BidirectionalIterator last)
{
	__reverse(first, last, iterator_category(first));
}
template <class BidirectionalIterator>
void __reverse(BidirectionalIterator first, BidirectionalIterator last, bidirectional_iterator_tag)
{
	while (true)
	{	// ��ϸ΢�����澫��
		if (first == last || first == --last)
			return;
		else
			iter_swap(first++, last);
	}
}
template <class RandomAccessIterator>
void __reverse(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{	// ֻ��random iterators������<����
	while (first < last)
		iter_swap(first++, --last);
}
template <class BidirectionalIterator, class OutputIterator>
OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result)
{
	while (first != last)
	{
		--last;
		*result = *last;
		++result;
	}
	return result;
}

// rotate����[first, middle)�ڵ�Ԫ�غ�[middle, last)�ڵ�Ԫ�ػ���
// ��Ҫ�����߳������
template <class ForwardIterator>
inline void rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last)
{
	if (first == middle || middle == last)
		return;
	__rotate(first, middle, last, distance_type(first), iterator_category(first));
}
template <class ForwardIterator, class Distance>
void __rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last, Distance*, forward_iterator_tag)
{
	for (ForwardIterator i = middle; ;)
	{
		iter_swap(first, i);
		++first;
		if (first == middle)
		{
			if (i == last)
				return;
			middle = i;
		}
		else if (i == last)
			i = middle;
	}
}
template <class BidirectionalIterator, class Distance>
void __rotate(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, Distance*, bidirectional_iterator_tag)
{
	reverse(first, middle);
	reverse(middle, last);
	reverse(first, last);
}
template <class RandomAccessIterator, class Distance>
void __rotate(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Distance*, random_access_iterator_tag)
{
	Distance n = __gcd(last - first, middle - first);
	while (n--)
		__rotate_cycle(first, last, first + n, middle - first, value_type(first));
}
template <class EculideanRingElement>
EculideanRingElement __gcd(EculideanRingElement m, EculideanRingElement n)
{
	while (n != 0)
	{
		EculideanRingElement t = m % n;
		m = n;
		n = t;
	}
}
template <class RandomAccessIterator, class Distance, class T>
void __rotate_cycle(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator initial, Distance shift, T*)
{
	T value = *initial;
	RandomAccessIterator ptr1 = initial;
	RandomAccessIterator ptr2 = ptr1 + shift;
	while (ptr2 != initial)
	{
		*ptr1 = *ptr2;
		ptr1 = ptr2;
		if (last - ptr2 > shift)
			ptr2 += shift;
		else
			ptr2 = first + (shift - (last - ptr2));
	}
	*ptr1 = value;
}
template <class ForwardIterator, class OutputIterator>
OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last, OutputIterator result)
{
	return copy(first, middle, copy(middle, last, result));
}

// search����[first1, last1)�����ǵ������У�����[first2, last2)���״γ��ֵ�
// �����������ȫƥ��������У��򷵻ص�����last1
template <class ForwardIterator1, class ForwardIterator2>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
	ForwardIterator2 first2, ForwardIterator2 last2)
{
	return __search(first1, last1, first2, last2, distance_type(first1), distance_type(first2));
}
template <class ForwardIterator1, class ForwardIterator2, class Distance1, class Distance2>
ForwardIterator1 __search(ForwardIterator1 first1, ForwardIterator1 last1,
	ForwardIterator2 first2, ForwardIterator2 last2,
	Distance1*, Distance2*)
{
	Distance1 d1 = 0;
	d1 = distance(first1, last1);
	Distance2 d2 = 0;
	d2 = distance(first2, last2);

	if (d1 < d2)
		return last1;

	ForwardIterator1 current1 = first1;
	ForwardIterator2 current2 = first2;

	while (current2 != last2)
	{
		if (*current1 == *current2)
		{
			++current1;
			++current2;
		}
		else
		{
			if (d1 == d2)
				return last1;
			else
			{
				current1 = ++first1;
				current2 = first2;
				--d1;
			}
		}
	}
	return first1;
}

// search_n����[first, last)�У����ҡ�����count������������Ԫ�ء����γɵ�������
// ����ָ����������ʼ���ĵ�����
// �Ҳ����ͷ���last
template <class ForwardIterator, class Integer, class T>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
	Integer count,
	const T& value)
{
	if (count <= 0)
		return first;
	else
	{
		first = find(first, last, value);
		while (first != last)
		{
			Integer n = count - 1;
			ForwardIterator i = first;
			++i;
			while (i != last && n != 0 && *i == value)
			{
				++i;
				--n;
			}
			if (n == 0)
				return first;
			else
				first = find(i, last, value);
		}
		return last;
	}
}
template <class ForwardIterator, class Integer, class T, class BinaryPredicate>
ForwardIterator search_n(ForwardIterator first, ForwardIterator last,
	Integer count,
	const T& value,
	BinaryPredicate binary_pred)
{
	if (count <= 0)
		return first;
	else
	{
		while (first != last)
		{
			if (binary_pred(*first, value))
				break;
			++first;
		}
		while (first != last)
		{
			Integer n = count - 1;
			ForwardIterator i = first;
			++i;

			while (i != last && n != 0 && binary_pred(*i, value))
			{
				++i;
				--n;
			}
			if (n == 0)
				return first;
			else
			{
				while (i != last)
				{
					 if (binary_pred(*i, value))
						break;
					 ++i;
				}
				first = i;
			}			
		}
		return last;
	}
}

// swap_ranges����[first1, last1)�ڵ�Ԫ�����first2��ʼ��������ͬ��Ԫ���໥����
template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2)
{
	for (; first1 != last1; ++first1, ++first2)
		iter_swap(first1, first2);
	return first2;
}

template <class InputIterator, class OutputIterator, class UnaryOperation>
OutputIterator transform(InputIterator first, InputIterator last,
	OutputIterator result, UnaryOperation op)
{
	for (; first != last; ++first, ++result)
		*result = op(*first);
	return result;
}
template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
OutputIterator transform(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, OutputIterator result,
	BinaryOperation binary_op)
{
	for (; first1 != last1; ++first1, ++first2, ++result)
		*result = binary_op(*first1, *first2);
	return result;
}

template <class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last)
{
	first = adjacent_find(first, last);
	return unique_copy(first, last, first);
}
template <class InputIterator, class OutputIterator>
inline OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result)
{
	if (first == last)
		return result;
	return __unique_copy(first, last, result, iterator_category(result));
}
template <class InputIterator, class ForwardIterator>
ForwardIterator __unique_copy(InputIterator first, InputIterator last,
	ForwardIterator result, forward_iterator_tag)
{
	*result = *first;
	while (++first != last)
		if (*result != *first)
			*++result = *first;
	return ++result;
}
template <class InputIterator, class OutputIterator>
inline OutputIterator __unique_copy(InputIterator first, InputIterator last, OutputIterator result,
	output_iterator_tag)
{
	return __unique_copy(first, last, result, value_type(first));
}
template <class InputIterator, class OutputIterator, class T>
OutputIterator __unique_copy(InputIterator first, InputIterator last,
	OutputIterator result, T*)
{
	T value = *first;
	*result = value;
	while (++first != last)
		if (value != *first)
		{
			value = *first;
			*++result = value;
		}
	return ++result;
}

// lower_bound��Ӧ������������, ���ҿɲ���value�ĵ�һ������λ��
// ����[first, last)����Զ�ĵ�����i��ʹ��[first, i)�е�ÿ��������j
// ������*j < value
template <class ForwardIterator, class T>
inline ForwardIterator lower_bound(
	ForwardIterator first, ForwardIterator last,
	const T& value)
{
	return __lower_bound(first, last, value, distance_type(first), iterator_category(first));
}
template <class ForwardIterator, class T, class Distance>
ForwardIterator __lower_bound(
	ForwardIterator first, ForwardIterator last,
	const T& value, Distance*, forward_iterator_tag)
{
	Distance len = 0;
	len = distance(first, last);
	Distance half;
	ForwardIterator middle;

	while (len > 0)
	{
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (*middle < value)
		{
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}
template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __lower_bound(
	RandomAccessIterator first, RandomAccessIterator last,
	const T& vlaue, Distance*, random_access_iterator_tag)
{
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;

	while (len > 0)
	{
		half = len >> 1;
		middle = first + half;
		if (*middle < value)
		{
			first = middle + 1;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}

// upper_bound��Ӧ������������, ���ҿɲ���value�����һ������λ��
template <class ForwardIterator, class T>
inline ForwardIterator upper_bound(
	ForwardIterator first, ForwardIterator last,
	const T& value)
{
	return __upper_bound(first, last, value, distance_type(first), iterator_category(first));
}
template <class ForwardIterator, class T, class Distance>
ForwardIterator __upper_bound(
	ForwardIterator first, ForwardIterator last,
	const T& value, Distance*, forward_iterator_tag)
{
	while (len > 0)
	{
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (*middle <= value)
		{
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}
template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __upper_bound(
	RandomAccessIterator first, RandomAccessIterator last,
	const T& value, Distance*, random_access_iterator_tag)
{
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;

	while (len > 0)
	{
		half = len >> 1;
		middle = first + half;
		if (*middle <= value)
		{
			first = middle + 1;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}

// equal_range���ۺ�lower_bound��upper_bound
template <class ForwardIterator, class T>
inline pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& value)
{
	return __euqal_range(first, last, value, distance_type(first), iterator_category(first));
}
template <class RandomAccessIterator, class T, class Distance>
pair<RandomAccessIterator, RandomAccessIterator>
__equal_range(RandomAccessIterator first, RandomAccessIterator last, const T& value,
	Distance*, random_access_iterator_tag)
{
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle, left, right;

	while (len > 0)
	{
		half = len >> 1;
		middle = first + half;
		if (*middle < value)
		{
			first = middle + 1;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else
		{
			left = lower_bound(first, middle, value);
			right = upper_bound(++middle, first + len, value);
			return pair<RandomAccessIterator, RandomAccessIterator>(left, right);
		}
	}
	return pair<RandomAccessIterator, RandomAccessIterator>(first, first);
}
template <class ForwardIterator, class T, class Distance>
pair<ForwardIterator, ForwardIterator>
__equal_range(ForwardIterator first, ForwardIterator last,
	const T& value, Distance*, forward_iterator_tag)
{
	Distance len = distance(first, last);
	Distance half;
	ForwardIterator middle, left, right;

	while (len > 0)
	{
		half = len >> 1;
		middle = first;
		advance(middle, half);

		if (*middle < value)
		{
			first = middle;
			++first;
			len = len - half - 1;
		}
		else if (value < *middle)
			len = half;
		else
		{
			left = lower_bound(first, middle, value);
			advance(first, len);
			right = upper_bound(++middle, first, value);
			return pair<ForwardIterator, ForwardIterator>(left, right);
		}
	}
	return pair<ForwardIterator, ForwardIterator>(first, first);
}

// binary_search�����ֲ��ҷ�����value������Ϊtrue������Ϊfalse
template <class ForwardIterator, class T>
bool binary_search(
	ForwardIterator first, ForwardIterator last,
	const T& value)
{
	ForwardIterator i = lower_bound(first, last, value);
	return i != last && !(value < *i);
}

// next_permutation����һ�����У�ʮ����Ҫ�ı�����
template <class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first, BidirectioanlIterator last)
{
	// corner case
	if (first == last)
		return false;

	BidirectionalIterator i = first;
	++i;
	if (i == last)
		return false;
	
	// normal case
	i = last;
	--i;
	BidirectionalIterator ii = i;
	--i;

	if (*i < *ii)
	{
		BidirectionalIterator j = last;
		while (!(*i < *--j));
		iter_swap(i, j);
		reverse(ii, last);
		return true;
	}
	if (i == first)
	{
		reserve(first, last);
		return false;
	}
	
}

// prev_permutation����һ������
template <class BidirectionalIterator>
bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last)
{	// corner case
	if (first == last)
		return false;
	BidirectionalIterator i = first;
	++i;
	if (i == last)
		return false;
	
	i = last;
	--i;
	BidirectionalIterator ii = i;
	--i;
	if (*ii < *i)
	{
		BidirectionalIterator j = last;
		while (!(*--j < *i));
		iter_swap(i, j);
		reverse(ii, last);
		return true;
	}
	if (i == first)
	{
		reserve(first, last);
		return false;
	}
}

// partial_sort
template <class RandomAccessIterator>
inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last)
{
	__partial_sort(first, middle, last, value_type(first));
}
template <class RandomAccessIterator, class T>
void __partial_sort(RandomAccessIterator first, andomAccessIterator middle, RandomAccessIterator last,
	T*)
{
	make_heap(first, middle);
	for (RandomAccessIterator i = middle; i < last; ++i)
		if (*i < *first)
			__pop_heap(first, middle, i, T(*i), distance_type(first));
	sort_heap(first, middle);
}

// sort�����㷨
// ��������ʱʹ��quick sort��
// ��������ʱʹ��insertion sort,
// ����ݹ��ι���������heap sort
template <class RandomAccessIterator>
void __insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
	if (first == last)
		return;
	for (RandomAccessIterator i = first + 1; i != last; ++i)
		__linear_insert(first, i, value_type(first));
}
template <class RandomAccessIterator, class T>
inline void __linear_insert(RandomAccessIterator first, RandomAccessIterator last, T*)
{
	T value = *last;
	if (value < *first)
	{
		__copy_backward(first, last, last + 1);
		*first = value;
	}
	else
		__unguarded_linear_insert(last, value);
}
template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator
__copy_backward(RandomAccessIterator first, RandomAccessIterator last,
	OutputIterator result, Distance*)
{
	Distance n = last - first;
	for (RandomAccessIterator i = last - 1; n > 0; --n, --result, --i)
		*result = *i;
	return result;
}
template <class RandomAccessIterator, class T>
void __unguarded_linear_insert(RandomAccessIterator last, T value)
{
	RandomAccessIterator next = last;
	--next;
	while (value < *next)
	{
		*last = *next;
		last = next;
		--next;
	}
	*last = value;
}
// ����a, b, c֮��ֵ
template <class T>
inline const T& __median(const T& a, const T& b, const T& c)
{
	if (a < b)
		if (b < c)
			return b;
		else if (a < c)
			return c;
		else
			return a;
	else if (a < c)
		return a;
	else if (b < c)
		return c;
	else
		return b;
}
// partitioning�ָ��㷨
template <class RandomAccessIterator, class T>
RandomAccessIterator __unguarded_partition(RandomAccessIterator first, RandomAccessIterator last,
	T pivot)
{
	while (true)
	{
		while (*first < pivot)
			++first;
		while (pivot < *last)
			--last;
		if (!(first < last))
			return first;
		iter_swap(first, last);
		++first;
		--last;
	}
}

template <class Size>
inline Size __lg(Size n)
{
	Size k;
	for (k = 0; n > 1; n >>= 1)
		++k;
	return k;
}
template <class RandomAccessIterator, class T, class Size>
void __introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T*, Size depth_limit)
{
	while (last - first > __stl_threshold)
	{
		if (depth_limit == 0)
		{
		   partial_sort(first, last, last);
		   return;
		}
		--depth_limit;
		RandomAccessIterator cut = __unguarded_partition(first, last, T(__median(
			*first, *(first + (last - first) / 2), *(last - 1))));
		__introsort_loop(cut, last, value_type(first), depth_limit);
		last = cut;
	}
}
template <class RandomAccessIterator>
void __final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
	if (last - first > __stl_threshold)
	{
		__insertion_sort(first, first + __stl_threshold);
		__unguarded_insertion_sort(first + __stl_threshold, last);
	}
	else
		__insertion_sort(first, last);
}
template <class RandomAccessIterator>
inline void __unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last)
{
	__unguarded_insertion_sort_aux(first, last, value_type(first));
}
template <class RandomAccessIterator, class T>
void __unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last, T*)
{
	for (RandomAccessIterator i = first; i != last; ++i)
		__unguarded_linear_insert(i, T(*i));
}
// ǧ����ʼ�������̱����ð�����
template <class RandomAccessIterator>
inline void sort(RandomAccessIterator first, RandomAccessIterator last)
{
	if (first != last)
	{
		__introsort_loop(first, last, value_type(first), __lg(last - first) * 2);
		__final_insertion_sort(first, last);
	}
}

// nth_element�����ص�������ֵ�ǵ�nth���Ԫ��
template <class RandomAccessIterator>
inline void nth_element(RandomAccessIterator first,
	RandomAccessIterator nth, RandomAccessIterator last)
{
	__nth_element(first, nth, last, value_type(first));
}
template <class RandomAccessIterator, class T>
void __nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last,
	T*)
{
	while (last - first > 3)
	{
		RandomAccessIterator cut = __unguarded_partition(first, last,
			T(__median(*first,
				*(first + (last - first) / 2),
				*(last - 1))));
		if (cut <= nth)
			first = cut;
		else
			last = cut;
	}
	__insertion_sort(first, last);
}


#endif // !NAIVE_STL_ALGO_H