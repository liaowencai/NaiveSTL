/*
* Copyright (c) 2017, https://liaowencai.github.com
*/

#ifndef NAIVE_TREE_H
#define NAIVE_TREE_H

#include <cstddef>
#include <utility> // for pair
#include "naive_stl_iterator.h"
#include "naive_stl_alloc.h"

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base;

inline void
__rb_tree_rotate_left(__rb_tree_node_base* x,
	__rb_tree_node_base*& root)
{
	__rb_tree_node_base* y = x->right;
	x->right = y->left;
	if (y->left != 0)
		y->left->parent = x;
	y->parent = x->parent;

	if (x == root)
		root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;
	y->left = x;
	x->parent = y;
}

inline void
__rb_tree_rotate_right(__rb_tree_node_base* x,
	__rb_tree_node_base*& root)
{
	__rb_tree_node_base* y = x->left;
	x->left = y->right;
	if (y->right != nullptr)
		y->right->parent = x;
	y->parent = x->parent;

	if (x == root)
		root = y;
	else if (x == x->parent->right)
		x->parent->right = y;
	else
		x->parent->left = y;
	y->right = x;
	x->parent = y;
}

inline void
__rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
	x->color = __rb_tree_red;
	while (x != root && x->parent->color == __rb_tree_red)
	{
		if (x->parent == x->parent->parent->left)
		{
			__rb_tree_node_base* y = x->parent->parent->right;
			if (y && y->color == __rb_tree_red)
			{
				x->parent->color = __rb_tree_black;
				y->color = __rb_tree_black;
				x->parent->parent->color = __rb_tree_red;
				x = x->parent->parent;
			}
			else
			{
				if (x == x->parent->right)
				{
					x = x->parent;
					__rb_tree_rotate_left(x, root)
				}
				x->parent->color = __rb_tree_black;
				x->parent->parent->color = __rb_tree_red;
				__rb_tree_rotate_right(x->parent->parent, root);
			}
		}
		else
		{
			__rb_tree_node_base* y = x->parent->parent->left;
			if (y && y->color == __rb_tree_red)
			{
				x->parent->color = __rb_tree_black;
				y->color = __rb_tree_black;
				x->parent->parent->color = __rb_tree_red;
				x = x->parent->parent;
			}
			else
			{
				if (x == x->parent->left)
				{
					x = x->parent;
					__rb_tree_rotate_right(x, root);
				}
				x->parent->color = __rb_tree_black;
				x->parent->parent->color = __rb_tree_red;
				__rb_tree_rotate_left(x->parent->parent, root);
			}
		}
	} // while����
	root->color = __rb_tree_black;
}

struct __rb_tree_node_base
{
	typedef __rb_tree_color_type color_type;
	typedef __rb_tree_node_base* base_ptr;

	color_type color; // �ڵ���ɫ
	base_ptr parent;  // ���ڵ�
	base_ptr left;	  // ��ڵ�
	base_ptr right;	  // �ҽڵ�

	static base_ptr minimum(base_ptr x)
	{
		while (x->left != nullptr) x = x->left;
		return x;
	}

	static base_ptr maximum(base_ptr x)
	{
		while (x->right != nullptr) x = x->right;
		return x;
	}
};

template <class Value>
struct __rb_tree_node : public __rb_tree_node_base
{
	typedef __rb_tree_node<Value>* link_type;

	Value value_field; // �ڵ�ֵ
};

struct __rb_tree_base_iterator
{
	typedef __rb_tree_node_base::base_ptr base_ptr;
	typedef bidirectional_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;

	base_ptr node; // make a reference to container

	void increment()
	{
		if (node->right != nullptr)
		{
			// ��������ӽڵ�
			node = node->right;
			while (node->left != nullptr)
				node = node->left;
		}
		else
		{
			// û�����ӽڵ�
			base_ptr y = node->parent; // �ҳ����ڵ�
			while (node == y->right)
			{
				node = y;			   // ������нڵ㱾���Ǹ����ӽڵ�
				y = y->parent;		   // ��һֱ���ݣ�ֱ������Ϊ���ӽڵ㡱Ϊֹ
			}
			if (node->right != y)
				node = y;
		}
	}

	void decrement()
	{
		// nodeΪheader
		if (node->color == __rb_tree_red &&	 // ����Ǻ�ڵ�
			node->parent->parent == node) 	 // �Ҹ��ڵ�ĸ��ڵ�����Լ�
			node = node->right;
		else if (node->left != nullptr)		 // ��������ӽڵ�
		{									 // 
			base_ptr y = node->left;		 // ��������һ��
			while (y->right != nullptr)		 // Ȼ�������ߵ���
				y = y->right;

			node = y;
		}
		else								 // �Ȳ��Ǹ��ڵ㣬��û���ӽڵ�
		{
			base_ptr y = node->parent;		 // �ҳ����ڵ�
			while (node == y->left)			 // �����нڵ������ӽڵ�
			{								 // һֱ���������ߣ�ֱ�����������ӽڵ�
				node = y;
				y = y->parent;
			}
			node = y;
		}
	}
};

template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator
{
	typedef Value value_type;
	typedef Ref reference;
	typedef Ptr pointer;
	typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
	typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
	typedef __rb_tree_iterator<Value, Ref, Ptr> self;
	typedef __rb_tree_node<Value>* link_type;

	__rb_tree_iterator() {}
	__rb_tree_iterator(link_type x) { node = x; }
	__rb_tree_iterator(const iterator& it) { node == it.node; }

	reference operator*() const
	{
		return dynamic_cast<link_type>(node)->value_field;
	}
	pointer operator->() const { return &(operator*()); }

	self& operator++()
	{
		increment();
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		increment();
		return tmp;
	}

	self& operator--()
	{
		decrement();
		return *this;
	}
	self operator--(int)
	{
		self tmp = *this;
		decrement();
		return tmp;
	}
};

template <class Key, class Value, class KeyOfValue, class Compare,
	class Alloc = alloc>
class rb_tree
{
protected:
	typedef void* void_pointer;
	typedef __rb_tree_node_base* base_ptr;
	typedef __rb_tree_node<Value> rb_tree_node;
	typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
	typdef __rb_tree_color_type color_type;

public:
	typedef Key key_type;
	typedef Value value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef rb_tree_node* link_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef __rb_tree_iterator<value_type, reference, pointer> iterator;

protected:
	link_type get_node()
	{
		return rb_tree_node_allocator::allocate();
	}
	void put_node(link_type p)
	{
		rb_tree_node_allocator::deallocate(p);
	}

	link_type create_node(const value_type& x)
	{
		link_type tmp = get_node();
		try
		{
			construct(&tmp->value_field, x);
		}
		catch (...)
		{
			put_node(tmp);
			throw;
		}
		return tmp;
	}

	link_type clone_node(link_type x) // ����һ���ڵ��ֵ����ɫ
	{
		link_type tmp = create_node(x->value_field);
		tmp->color = x->color;
		tmp->left = 0;
		tmp->right = 0;
		return tmp;
	}

	void destroy_node(link_type p)
	{
		destroy(&p->value_field);
		put_node(p);
	}

protected:
	// ����RB-tree�����ݳ�Ա
	size_type node_count; // �ڵ�����
	link_type header;	  // header�ڵ�
	Compare key_compare;  // һ��function object���Ǽ�ֵ��С�ıȽ�׼��

	// ����ȡ��header�ĳ�Ա
	link_type& root() const
	{
		return dynamic_cast<link_type&>(header->parent);
	}
	link_type& leftmost() const
	{
		return dynamic_cast<link_type&>(header->left);
	}
	link_type& rightmost() const
	{
		return dynamic_cast<link_type&>(header->right);
	}

	// ����ȡ�ýڵ�x�ĳ�Ա
	static link_type& left(link_type x)
	{
		return dynamic_cast<link_type&>(x->left);
	}
	static link_type& right(link_type x)
	{
		return dynamic_cast<link_type&>(x->right);
	}
	static link_type& parent(link_type x)
	{
		return dynamic_cast<link_type&>(x->parent);
	}
	static reference value(link_type x)
	{
		return x->value_field;
	}
	static const Key& key(link_type x)
	{
		return static_cast<KeyOfValue()>(value(x));
	}
	static color_type& color(link_type x)
	{
		return static_cast<color_type&>(x->color);
	}

	// ����ȡ�ýڵ�x�ĳ�Ա
	static link_type& left(base_ptr x)
	{
		return dynamic_cast<link_type&>(x->left);
	}
	static link_type& right(base_ptr x)
	{
		return dynamic_cast<link_type&>(x->right);
	}
	static link_type& parent(base_ptr x)
	{
		return dynamic_cast<link_type&>(x->parent);
	}
	static reference value(base_ptr x)
	{
		return (dynamic_cast<link_type>(x))->value_field;
	}
	static const Key& key(base_ptr x)
	{
		return static_cast<KeyOfValue()>(value(dynaic_cast<link_type>(x)));
	}
	static color_type& color(base_ptr x)
	{
		return static_cast<color_type&>((dynaic_cast<link_type>(x))->color);
	}

	// ��ȡ���ֵ����Сֵ
	static link_type minimum(link_type x)
	{
		return dynamic_cast<link_type>(__rb_tree_node_base::minimum(x));
	}
	static link_type maximum(link_type x)
	{
		return dynamic_cast<link_type>(__rb_tree_node_base::maximum(x));
	}

private:
		iterator __insert(base_ptr x, bast_ptr y, const value_type& v);
		link_type __copy(link_type x, link_type p);
		void __erase(link_type x);
		void init()
		{
			header = get_node(); // ����һ���ڵ�ռ䣬��headerָ����
			color(header) = __rb_tree_red; // ��headerΪ��ɫ����������header��root

			root() = 0;
			// ��header�����ҽڵ�Ϊ����
			leftmost() = header;
			rightmost() = header;
		}

		iterator __insert(base_ptr x_, base_ptr y_, const Value& v)
		{
			link_type x = dynamic_cast<link_type>(x_);
			link_type y = dynamic_cast<link_type>(y_);

			link_type z;

			if (y == header || x != nullptr ||
				key_compare(KeyOfValue()(v), key(y)))
			{
				z = create_node(v);
				left(y) = z;
				if (y == header)
				{
					root() = z;
					rightmost = z;
				}
				else if (y == leftmost())
					leftmost() = z;
			}
			else
			{
				z = create_node(v);
				right(y) = z;
				if (y == rightmost())
					rightmost() = z;
			}
			parent(z) = y;
			left(z) = nullptr;
			right(z) = nullptr;

			__rb_tree_rebalance(z, header->parent);
			++node_count;
			return iterator(z);
		}

public:
	rb_tree(const Compare& comp = Compare())
		: node_count(0), key_compare(comp)
	{
		init();
	}
	~rb_tree()
	{
		clear();
		put_node(header);
	}
	rb_tree& operator=(const rb_tree& x);

	// accessors
	Compare key_comp() const
	{
		return key_compare;
	}
	iterator begin() { return leftmost(); }
	iterator end() { return header; }
	bool empty() const { return node_count == 0; }
	size_type size() const { return node_count; }
	size_type max_size() const { return static_cast<size_type>(-1); }

	pair<iterator, bool> insert_unique(const value_type& x)
	{
		link_type y = header;
		link_type x = root(); // �Ӹ��ڵ㿪ʼ
		bool comp = true;
		while (x != nullptr)
		{
			y = x;
			// v��ֵС��Ŀǰ�ڵ�֮��ֵ��
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		} // ��ʱ��yΪ�����ĸ��ڵ㣬�ض�ΪҶ�ڵ�

		iterator j = iterator(y);  // �������jָ������֮���ڵ�y
		if (comp) // ����뿪whileѭ��ʱcompΪ�棬��ʾ�����󡱣������������
			if (j == begin()) // ��������֮���ڵ�Ϊ����ڵ�
				return pair<iterator, bool>(__insert(x, y, v), true);
			else
				--j;

		if (key_compare(key(j.node), KeyOfValue()(v)))
			return pair<iterator, bool>(__insert(x, y, v), true);

		return pair<iterator, bool>(j, false);
	}
	iterator insert_equal(const value_type& x)
	{
		link_type y = header;
		link_type x = root(); // �Ӹ��ڵ㿪ʼ
		while (x != nullptr)  // ����Ѱ���ʵ��Ĳ����
		{
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		}
		// xΪ��ֵ����㣬yΪ�����֮���ڵ㣬vΪ��ֵ
		return __insert(x, y, v);
	}

	// Ѱ��rb�����Ƿ��м�ֵΪk�Ľڵ�
	iterator find(const key& k)
	{
		link_type y = header;
		link_type x = root();

		while (x != nullptr)
			if (!key_compare(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);

		iterator j = iterator(y);
		return
			(j == end() || key_compare(k, key(j.node) ? end() : j);
	}
};

#endif // !NAIVE_TREE_H
