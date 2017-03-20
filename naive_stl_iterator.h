/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_STL_ITERSTOR_H
#define NAIVE_STL_ITERSTOR_H

#include <cstddef>
#include <iostream>

// 5种迭代器
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// 自行开发的迭代器应当继承自本iterator类
template <class Category, class T, class Distance = ptrdiff_t,
          class Pointer = T*, class Reference = T&>
struct iterator 
{
    typedef Category  iterator_category;
    typedef T         value_type;
    typedef Distance  difference_type;
    typedef Pointer   pointer;
    typedef Reference reference;
};

// type-traits
template <class Iterator>
struct iterator_traits
{
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};

// for native-pointer
template <class T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef T*                         pointer;
    typedef T&                         reference;
};

// for pointer-to-const
template <class T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef ptrdiff_t                  difference_type;
    typedef const T*                   pointer;
    typedef const T&                   reference;
};

// function to determine Category of iterators
template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

// function to determine distance type of iterators
template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
    return static_cast<
        typename iterator_traits<Iterator>::difference_type*>(0);
}

// function to determine value type of iterators
template <class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
    return static_cast<
        typename iterator_traits<Iterator>::value_type*>(0);
}

// distance function
template <class InputIterator>
inline iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last,
           input_iterator_tag)
{
    iterator_traits<InputIterator>::difference_type n(0);

    while (first != last)
    {
        ++first;
        ++n;
    }

    return n;
}

template <class RandomAccessIterator>
inline iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last,
           random_access_iterator_tag)
{
    return last - first;
}

template <class InputIterator>
inline iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) 
{
    typedef typename
        iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

// advance function
template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n,
                      input_iterator_tag)
{
    while (n--)
        ++i;
}

template <class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator& i, Distance n,
                      bidirectional_iterator_tag) 
{
    if (n >= 0)
    {
        while (n--)
            ++i;
    } 
    else
    {
        whlie (n++)
            --i;
    }
}

template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n,
                      random_access_iterator_tag)
{
    i += n;
}

template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n)
{
    __advance(i, n, iterator_category(i));
}


//-----------------------------------------------------------------------
// iterator adapters
//-----------------------------------------------------------------------
template <class Container>
class back_insert_iterator
{
protected:
	Container* container;
public:
	typedef output_iterator_tag iterator_category;
	typedef void				value_type;
	typedef void				difference_type;
	typedef void				pointer;
	typedef void				reference;

	explicit back_insert_iterator(Container& x) : container(&x) {}
	back_insert_iterator<Container>&
		operator=(const typename Container::value_type& value)
	{	// 转而调用push_back
		container->push_back(value);
		return *this;
	}
	// 关闭下列三个功能
	back_insert_iterator<Container>& operator*() { return *this; }
	back_insert_iterator<Container>& operator++() { return *this; }
	back_insert_iterator<Container>& operator++(int) { return *this; }
};
template <class Container>
inline back_insert_iterator<Container> back_inserter(Container& x)
{
	return back_insert_iterator<Container>(x);
}

template <class Container>
class front_insert_iterator
{
protected:
	Container* container;
public:
	typedef output_iterator_tag iterator_category;
	typedef void				value_type;
	typedef void				difference_type;
	typedef void				pointer;
	typedef void				reference;

	explicit front_insert_iterator(Container& x) : container(&x) {}
	front_insert_iterator<Container>&
		operator=(const typename Container::value_type& value)
	{	// 转而调用push_front
		container->push_front(value);
		return *this;
	}
	// 关闭下列三个功能
	front_insert_iterator<Container>& operator*() { return *this; }
	front_insert_iterator<Container>& operator++() { return *this; }
	front_insert_iterator<Container>& operator++(int) { return *this; }
};
template <class Container>
inline front_insert_iterator<Container> front_inserter(Container& x)
{
	return front_insert_iterator<Container>(x);
}

template <class Container>
class insert_iterator
{
protected:
	Container* container;
	typename Container::iterator iter;
public:
	typedef output_iterator_tag iterator_category;
	typedef void				value_type;
	typedef void				difference_type;
	typedef void				pointer;
	typedef void				reference;

	insert_iterator(Container& x, typename Container::iterator i) : container(&x), iter(i) {}
	insert_iterator<Container>&
		operator=(const typename Container::value_type& value)
	{	
		iter = container->insert(iter, value);
		++iter;		// insert iterator永远随着目标移动
		return *this;
	}
	// 关闭下列三个功能
	insert_iterator<Container>& operator*() { return *this; }
	insert_iterator<Container>& operator++() { return *this; }
	insert_iterator<Container>& operator++(int) { return *this; }
};
template <class Container， class Iterator>
inline insert_iterator<Container> inserter(Container& x, Iterator i)
{
	typedef typename Container::iterator iter;
	return insert_iterator<Container>(x, iter(i));
}

//---------------------------------------------------------------------------------------
//
template <class Iterator>
class reverse_iterator
{
protected:
	Iterator current;
public:
	typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
	typedef typename iterator_traits<Iterator>::value_type		  value_type;
	typedef typename iterator_traits<Iterator>::difference_type	  difference_type;
	typedef typename iterator_traits<Iterator>::pointer			  pointer;
	typedef typename iterator_traits<Iterator>::reference		  reference;

	typedef Iterator iterator_type;
	typedef reverse_iterator<Iterator> self;
public:
	reverse_iterator() {}
	explicit reverse_iterator(iterator_type x) : current(x) {}
	reverse_iterator(const self& x) : current(x.current) {}

	iterator_type base() const { return current; }
	reference operator*() const
	{
		Iterator tmp = current;
		// 对逆向迭代器取值，就是将对应的正向迭代器后退一格而后取值
		return *--tmp;
	}
	pointer operator->() const { return &(operator*()); }
	// 前进++变成后退--
	self& operator++()
	{
		--current;
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		--current;
		return tmp;
	}
	self& operator--()
	{
		++current;
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		++current;
		return tmp;
	}
	self operator+(difference_type n) const
	{
		return self(current - n);
	}
	self& operator+=(difference_type n)
	{
		current -= n;
		return *this;
	}
	self operator-(difference_type n) const
	{
		return self(current + n);
	}
	self& operator-=(difference_type n)
	{
		current += n;
		return *this;
	}

	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}
};

template <class T, class Distance = ptrdiff_t>
class istream_iterator
{
	friend bool operator==(const istream_iterator<T, Distance>& x,
		const istream_iterator<T, Distance>& y);
protected:
	istream* stream;
	T value;
	bool end_marker;
	void read()
	{
		end_marker = (*stream) ? true : false;
		if (end_marker)
			*stream >> value;
		end_marker = (*stream) ? true : false;
	}
public:
	typedef input_iterator_tag	iterator_category;
	typedef T					value_type;
	typedef Distance			distance_type;
	typedef const T*			pointer;
	typedef const T&			reference;

	istream_iterator() : stream(&cin), end_marker(false) {}
	istream_iterator(istream& s) : stream(&s) { read(); }

	reference operator*() const { return value; }
	pointer operator->() const { return &(operator*()); }

	istream_iterator<T, Distance>& operator++()
	{
		read();
		return *this;
	}
	istream_iterator<T, Distance> operator++(int)
	{
		istream_iterator<T, Distance> tmp = *this;
		read();
		return tmp;
	}
};

template <class T>
class ostream_iterator
{
protected:
	ostream* stream;
	const char* string;

public:
	typedef output_iterator_tag iterator_category;
	typedef void				value_type;
	typedef void				difference_type;
	typedef void				pointer;
	typedef void				reference;

	ostream_iterator(ostream& s) : stream(&s), string(nullptr) {}
	ostream_iterator(ostream& s, const char* c) : stream(&s), string(c) {}

	ostream_iterator<T>& operator=(const T& value)
	{
		*stream << value;
		if (string)
			*stream << string;
		return *this;
	}
	ostream_iterator<T>& operator*() { return *this; }
	ostream_iterator<T>& operator++() { return *this; }
	ostream_iterator<T>& operator++(int) { return *this; }
};


#endif // NAIVE_STL_ITERATOR_H