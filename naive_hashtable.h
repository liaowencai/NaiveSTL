/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_HASHTABLE_H
#define NAIVE_HASHTABLE_H

#include <cstddef>
#include "naive_stl_alloc.h"
#include "naive_stl_construct.h"
#include "naive_stl_iterator.h"
#include "naive_vector.h"
//#include "naive_algorithm.h"
#include <algorithm> // to include alogrithm lower_bound

// defintion of hashtable class
template <class Value, class Key, class HashFcn,
	class ExtractKey, class EqualKey, class Alloc = alloc>
class hashtable;

template <class Value>
struct __hashtable_node
{
	__hashtable_node* next;
	Value val;
};

template <class Value, class Key, class HashFcn, 
	class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator
{
	typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
	typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
	typedef __hashtable_node<Value> node;

	typedef forward_iterator_tag iterator_category;
	typedef Value value_type;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;
	typedef Value& reference;
	typedef Value* pointer;

	node* cur;		  // ������Ŀǰ��ָ�Ľڵ�
	hashtable* ht;	  // ���ֶ����������ӹ�ϵ

	__hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
	__hashtable_iterator() {}
	reference operator*() const { return cur->val; }
	pointer operator->() const { return &(operator*()); }
	iterator& operator++()
	{
		const node* old = cur;
		cur = cur->next;  // ������ھ��������������һ��if����
		if (cur == nullptr)
		{
			size_type bucket = ht->bkt_num(old->val);
			while (cur == nullptr && ++bucket < ht->buckets.size())
				cur = ht->buckets[bucket];
		}
		return *this;
	}
	iterator operator++(int)
	{
		iterator tmp = *this;
		++*this;
		return tmp;
	}
	bool operator==(const iterator& it) const { return cur == it.cur; }
	bool operator!=(const iterator& it) const { return !(*this == it); }
};

static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
	53ul,         97ul,         193ul,       389ul,       769ul,
	1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
	49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
	1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
	50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
	1610612741ul, 3221225473ul, 4294967291ul
};

inline unsigned long __stl_next_prime(unsigned long n)
{
	const unsigned long* first = __stl_prime_list;
	const unsigned long* last = __stl_prime_list + __stl_num_primes;
	const unsigned long* pos = std::lower_bound(first, last, n);
	return pos == last ? *(last - 1) : *pos;
}


template <class Value, class Key, class HashFcn,
	class ExtractKey, class EqualKey, class Alloc>
class hashtable
{
public:
	typedef HashFcn hasher;
	typedef EqualKey key_equal;
	typedef Value value_type;
	typedef Key key_type;
	typedef size_t size_type;
	typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;

private:
	// ����3�߶���function objects
	hasher hash;
	key_equal equals;
	ExtractKey get_key;

	typedef __hashtable_node<Value> node;
	typedef simple_alloc<node, Alloc> node_allocator;

	vector<node*, Alloc> buckets; // buckets�ۺ���
	size_type num_elements;

	node* new_node(const value_type& obj)
	{
		node* n = node_allocator::allocate();
		n->next = nullptr;
		try
		{
			construct(&n->val, obj);
			return n;
		}
		catch (...)
		{
			node_allocator::deallocate(n);
			throw;
		}
	}

	void delete_node(node* n)
	{
		destroy(&n->val);
		node_allocator::deallocate(n);
	}

	void initialize_buckets(size_type n)
	{
		const size_type n_buckets = next_size(n);
		buckets.reserve(n_buckets);
		buckets.insert(buckets.end(), n_buckets, (node*)0);
		num_elements = 0;
	}

	void resize(size_type num_elements_hint)
	{
		const size_type old_n = buckets.size();
		if (num_elements_hint > old_n)
		{
			vector<node*, A> tmp(n, (node*)0);
			try
			{
				for (size_type bucket = 0; bucket < old_n; ++bucket)
				{
					node* first = buckets[bucket];
					while (first)
					{
						size_type new_bucket = bkt_num(first->val, n);
						buckets[bucket] = first->next;
						first->next = tmp[new_bucket];
						tmp[new_bucket] = first;
						first = buckets[bucket];
					}
				}
				buckets.swap(tmp);
			}
			catch(...)
			{
				throw;
			}
		}
	}

	pair<iterator, bool> insert_unique_noresize(const value_type& obj)
	{
		const size_type n = bkt_num(obj); // ����objӦλ��#n bucket
		node* first = buckets[n];

		// ���buckets[n]�ѱ�ռ�ã���ʱfirst != 0�����ǽ���һ��ѭ����
		// �߹�bucket����Ӧ����������
		for (node* cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj)))
				return pair<iterator, bool>(iterator(cur, this), false);

		node* tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return pair<iterator, bool>(iterator(tmp, this), true);
	}

	iterator insert_equal_noresize(const value_type& obj)
	{
		const size_type n = bkt_num(obj);
		node* first = buckets[n];

		for (node* cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj)))
			{
				node* tmp = new_node(obj);
				tmp->next = cur->next;
				cur->next = tmp;
				++num_elements;
				return iterator(tmp, this);
			}

		node* tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return iterator(tmp, this);
	}

	size_type bkt_num(const value_type& obj, size_type n) const
	{
		return bkt_num_key(get_key(obj), n);
	}
	size_type bkt_num(const value_type& obj) const
	{
		return bkt_num_key(get_key(obj));
	}
	size_type bkt_num_key(const key_type& key) const
	{
		return bkt_num_key(key, buckets.size());
	}
	size_type bkt_num_key(const key_type& key, size_type n) const
	{
		return hash(key) % n;
	}

public:
	// bucket������buckets vector�Ĵ�С
	size_type bucket_count() const { return buckets.size(); }
	size_type max_bucket_count() const 
	{
		return __stl_prime_list[__stl_num_primes - 1];
	}

	hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
		: hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
	{
		initialize_buckets(n);
	}

	pair<iterator, bool> insert_unique(const value_type& obj)
	{
		resize(num_elements + 1);
		return insert_unique_noresize(obj);
	}

	iterator insert_equal(const value_type& obj)
	{
		resize(num_elements + 1); // �ж��Ƿ���Ҫ�ؽ��������Ҫ������
		return insert_equal_noresize(obj);
	}

	// hashtable��vector��linked-list��϶���
	// ���ƺ�����ɾ������Ҫ�ر�ע���ڴ���ͷ�����
	void clear()
	{
		// ���ÿһ��bucket
		for (size_type i = 0; i < buckets.size(); ++i)
		{
			node* cur = buckets[i];
			while (cur != nullptr)
			{
				node* next = cur->next;
				delete_node(cur);
				cur = next;
			}
			buckets[i] = nullptr; // ��buckets����Ϊnullptrָ��
		}
		num_elements = 0; // ���ܽڵ����Ϊ0
		// buckets vector��δ�ͷŵ��ռ䣬�Ա���ԭ����С
	}

	void copy_from(const hashtable& ht)
	{
		// �������������buckets vector
		buckets.clear();
		// Ϊ������buckets vector�����ռ䣬ʹ֮��Է���ͬ
		buckets.reserve(ht.buckets.size());
		// ��buckets vectorβ�˿�ʼ������n��nullptr
		buckets.insert(buckets.end(), ht.buckets.size(), nullptr);

		try
		{
			for (size_type i = 0; i < ht.buckets.size(); ++i)
			{
				if (const node* cur = ht.buckets[i])
				{
					node* copy = new_node(cur->val);
					buckets[i] = copy;

					for (node* next = cur->next; next; cur = next, next = cur->next)
					{
						copy->next = new_node(next->val);
						copy = copy->next;
					}
				}
			}
			num_elements = ht.num_elements;
		}
		catch (...)
		{
			clear();
			throw;
		}
	}
	
};

#endif // !NAIVE_HASHTABLE_H