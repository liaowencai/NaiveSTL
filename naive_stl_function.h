/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_STL_FUNCTION_H
#define NAIVE_STL_FUNCTION_H

template <class Arg, class Result>
struct unary_function
{
	typedef Arg argument_type;
	typedef Result result_type;
};

template <class Arg1, class Arg2, class Result>
struct binary_function
{
	typedef Arg1 first_argument_type;
	typedef Arg2 second_argument_type;
	typedef Result result_type;
};

// Arithmetic
template <class T>
struct plus : public binary_function<T, T, T>
{
	T operator()(const T& x, const T& y) const
	{
		return x + y;
	}
};
template <class T>
struct minus : public binary_function<T, T, T>
{
	T operator()(const T& x, const T& y) const
	{
		return x - y;
	}
};
template <class T>
struct multiplies : public binary_function<T, T, T>
{
	T operator()(const T& x, const T& y)
	{
		return x * y;
	}
};
template <class T>
struct divides : public binary_function<T, T, T>
{
	T operator()(const T& x, const T& y)
	{
		return x / y;
	}
};
template <class T>
struct modules : public binary_function<T, T, T>
{
	T operator()(const T& x, const T& y)
	{
		return x % y;
	}
};
template <class T>
struct negate : public unary_function<T, T>
{
	T operator()(const T& x) const
	{
		return -x;
	}
};

// Relational
template <class T>
struct equal_to : public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const
	{
		return x == y;
	}
};
template <class T>
struct not_equal_to : public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const
	{
		return x != y;
	}
};
template <class T>
struct greater : public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const
	{
		return x > y;
	}
};
template <class T>
struct less : public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const
	{
		return x < y;
	}
};
template <class T>
struct greater_equal : public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const
	{
		return x >= y;
	}
};
template <class T>
struct less_equal : public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y)	const
	{
		return x <= y;
	}
};

// Logical
template <class T>
struct logical_and : public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const
	{
		return x && y;
	}
};
template <class T>
struct logical_or : public binary_function<T, T, bool>
{
	bool operator()(const T& x, const T& y) const
	{
		return x || y;
	}
};
template <class T>
struct logical_not : public unary_function<T, bool>
{
	bool operator()(const T& x)	const
	{
		return !x;
	}
};

// identity, select, project
template <class T>
struct identity : public unary_function<T, T>
{
	const T& operator()(const T& x)
	{
		return x;
	}
};
template <class pair>
struct select1st : public unary_function<Pair, typename Pair::first_type>
{
	const typename Pair::first_type& operator()(const Pair& x) const
	{
		return x.first;
	}
};
template <class pair>
struct select2nd : public unary_function<Pair, typename Pair::second_type>
{
	const typename Pair::second_type& operator()(const Pair& x) const
	{
		return x.second;
	}
};

#endif // !NAIVE_STL_FUNCTION_H