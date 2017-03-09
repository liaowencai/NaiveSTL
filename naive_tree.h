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
	} // while结束
	root->color = __rb_tree_black;
}

struct __rb_tree_node_base
{
	typedef __rb_tree_color_type color_type;
	typedef __rb_tree_node_base* base_ptr;

	color_type color; // 节点颜色
	base_ptr parent;  // 父节点
	base_ptr left;	  // 左节点
	base_ptr right;	  // 右节点

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

	Value value_field; // 节点值
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
			// 如果有右子节点
			node = node->right;
			while (node->left != nullptr)
				node = node->left;
		}
		else
		{
			// 没有右子节点
			base_ptr y = node->parent; // 找出父节点
			while (node == y->right)
			{
				node = y;			   // 如果现行节点本身是个右子节点
				y = y->parent;		   // 就一直上溯，直到“以为右子节点”为止
			}
			if (node->right != y)
				node = y;
		}
	}

	void decrement()
	{
		// node为header
		if (node->color == __rb_tree_red &&	 // 如果是红节点
			node->parent->parent == node) 	 // 且父节点的父节点等于自己
			node = node->right;
		else if (node->left != nullptr)		 // 如果有左子节点
		{									 // 
			base_ptr y = node->left;		 // 先向左走一步
			while (y->right != nullptr)		 // 然后向右走到底
				y = y->right;

			node = y;
		}
		else								 // 既不是根节点，又没左子节点
		{
			base_ptr y = node->parent;		 // 找出父节点
			while (node == y->left)			 // 当现行节点是左子节点
			{								 // 一直交替向上走，直到它不是左子节点
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

	link_type clone_node(link_type x) // 复制一个节点的值和颜色
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
	// 描述RB-tree的数据成员
	size_type node_count; // 节点数量
	link_type header;	  // header节点
	Compare key_compare;  // 一个function object，是键值大小的比较准则

	// 用于取得header的成员
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

	// 用于取得节点x的成员
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

	// 用于取得节点x的成员
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

	// 求取最大值和最小值
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
			header = get_node(); // 产生一个节点空间，令header指向它
			color(header) = __rb_tree_red; // 令header为红色，用来区分header和root

			root() = 0;
			// 令header的左右节点为自身
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
		link_type x = root(); // 从根节点开始
		bool comp = true;
		while (x != nullptr)
		{
			y = x;
			// v键值小于目前节点之键值？
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		} // 此时，y为插入点的父节点，必定为叶节点

		iterator j = iterator(y);  // 令迭代器j指向插入点之父节点y
		if (comp) // 如果离开while循环时comp为真，表示遇“大”，将插入于左侧
			if (j == begin()) // 如果插入点之父节点为最左节点
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
		link_type x = root(); // 从根节点开始
		while (x != nullptr)  // 向下寻找适当的插入点
		{
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		}
		// x为新值插入点，y为插入点之父节点，v为新值
		return __insert(x, y, v);
	}

	// 寻找rb树中是否有键值为k的节点
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
