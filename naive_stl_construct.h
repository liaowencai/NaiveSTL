#ifndef NAIVE_STL_CONSTRUCT_H
#define NAIVE_STL_CONSTRUCT_H

#include <new>
#include "naive_stl_iterator.h"
#include "naive_type_traits.h"

template <class T1, class T2>
inline void construct(T1* p, const T2& value)
{
	new(p) T1(value); // placement new; invoke T1::T1(value);
}

// function destroy

// (1)destroy: accept a pointer
template <class T>
inline void destroy(T* pointer)
{
	pointer->~T(); // invoke ~T() explicitly
}

// (2)destroy: accept two iterators
// get the value type and use __type_traits<> to take the best actions
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
	__destroy(first, last, value_type(first));
}

// __destroy: value type of the parameter has/not has trivial destructor
template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
	__destroy_aux(first, last, trivial_destructor());
}

// if value type of the parameter has non-trivial destructor
template <class ForwardIterator>
inline void
__destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
	for (; first < last; ++first)
		destroy(&*first);
}

// if value type of the parameter has trivial destructor, 
// which is by default and nonsense
template <class ForwardIterator>
inline void
__destroy_aux(ForwardIterator first, ForwardIterator last, __true_type)
{
}

// if the iterator is char* or wchar_t*, 
// the object is array of char or wchar_t
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}

#endif