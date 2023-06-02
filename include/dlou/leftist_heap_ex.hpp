#pragma once

#include "node.hpp"

#include <type_traits>
#include <functional>
#include <utility>

namespace dlou {

template<class Key>
using leftist_heap_ex_node = node<3, Key, unsigned char>;

template<class NodeKey
	, class GetKey = node_key<leftist_heap_ex_node<NodeKey>>
	, class Compare = std::less<std::remove_cvref_t<std::invoke_result_t<GetKey, const leftist_heap_ex_node<NodeKey>&>>>>
class basic_leftist_heap_ex
	: private GetKey
	, private Compare
{
public:
	using node = leftist_heap_ex_node<NodeKey>;
	using key_type = std::remove_cvref_t<std::invoke_result_t<GetKey, const node&>>;
	using key_compare = Compare;
	using s_type = node_balance_t<node>;

protected:
	basic_leftist_heap_ex(const basic_leftist_heap_ex&) = default;
	basic_leftist_heap_ex& operator =(const basic_leftist_heap_ex&) = default;

	basic_leftist_heap_ex& operator =(basic_leftist_heap_ex&& x) {
		root_ = x.root_;
		x.root_ = nullptr;
		return *this;
	}

	void clear() {
		root_ = nullptr;
	}

	const key_type& key(const node& n) const {
		return GetKey::operator ()(n);
	}

	bool compare(const node& a, const node& b) const {
		return Compare::operator ()(key(a), key(b));
	}

public:
	basic_leftist_heap_ex()
		: root_(nullptr) {
	}

	basic_leftist_heap_ex(basic_leftist_heap_ex&& x)
		: root_(x.root_) {
		x.root_ = nullptr;
	}

	void swap(basic_leftist_heap_ex& x) {
		auto tmp = root_;
		root_ = x.root_;
		x.root_ = tmp;
	}

	bool empty() const {
		return !root_;
	}

	void merge(basic_leftist_heap_ex& x) {
		node* p = x.root_;
		if (!p)
			return;
		x.root_ = nullptr;
		root_ = root_ ? merge(root_, p) : p;
	}

	const node* peek() const {
		return root_;
	}

	void push(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		p->n[0] = nullptr;
		p->n[1] = nullptr;
		p->n[2] = nullptr;
		p->b = 1;
		root_ = root_ ? merge(root_, p) : p;
	}

	node* pop() {
		node* ret = root_;
#ifdef DLOU_CHECK_ARGS
		if (!ret)
			return nullptr;
#endif
		if (ret->n[1])
			root_ = merge(ret->n[0], ret->n[1]);
		else if(!(root_ = ret->n[0]))
			return ret;

		root_->n[2] = nullptr;

		return ret;
	}

	void erase(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		node* parent = p->n[2];
		node** branch = parent ? (parent->n + (p == parent->n[1])) : &root_;

		p = p->n[1] ? merge(p->n[0], p->n[1]) : p->n[0];
		*branch = p;
		if (p)
			p->n[2] = parent;

		while (parent) {
			s_type s1 = rank(parent->n[0]);
			s_type s2 = rank(parent->n[1]);
			if (s1 < s2) {
				p = parent->n[0];
				parent->n[0] = parent->n[1];
				parent->n[1] = p;
				++s1;
			}
			else
				s1 = s2 + 1;

			if (s1 == parent->b)
				return;

			parent->b = s1;
			parent = parent->n[2];
		}
	}

	//node* fault() const {
	//	if (!root_)
	//		return nullptr;
	//	return _fault(root_);
	//}

protected:
	static s_type rank(node* pos) {
		return pos ? pos->b : 0;
	}

	node* merge(node* p1, node* p2) {
		if (compare(*p1, *p2))
			std::swap(p1, p2);
		
		node* ret = p2;
		node* parent = ret;

		while (p2 = parent->n[1]) {
			if (compare(*p1, *p2))
				std::swap(p1, p2);

			p2->n[2] = parent;
			parent->n[1] = p2;
			parent = p2;
		}
		(parent->n[1] = p1)->n[2] = parent;

		for (;;) {
			s_type s1 = rank(parent->n[0]);
			s_type s2 = rank(parent->n[1]);
			if (s1 < s2) {
				p1 = parent->n[0];
				parent->n[0] = parent->n[1];
				parent->n[1] = p1;
				parent->b = s1 + 1;
			}
			else
				parent->b = s2 + 1;

			if (parent == ret)
				break;

			parent = parent->n[2];
		}

		return ret;
	}

	//node* _fault(node* pos) {
	//	if (!pos)
	//		return nullptr;
	//
	//	node* left = pos->n[0];
	//	node* right = pos->n[1];
	//	if (left) {
	//		if (pos != left->n[2])
	//			return left;
	//
	//		if (right) {
	//			if (pos != right->n[2])
	//				return right;
	//
	//			if (left->b < right->b)
	//				return pos;
	//			if (pos->b != right->b + 1)
	//				return pos;
	//			if (compare(*right, *pos))
	//				return pos;
	//		}
	//		if (compare(*left, *pos))
	//			return pos;
	//
	//		left = _fault(left);
	//		return left ? left : _fault(right);
	//	}
	//
	//	return right ? pos : nullptr;
	//}

protected:
	node* root_;
};

template<class Key, class Compare = std::less<Key>>
using leftist_heap_ex = basic_leftist_heap_ex<Key, node_key<leftist_heap_ex_node<Key>>, Compare>;

} // namespace dlou
