#pragma once

#include "node.hpp"

#include <type_traits>
#include <functional>

namespace dlou {

template<class Key>
using leftist_heap_node = node<2, Key, unsigned char>;

template<class NodeKey
	, class GetKey = node_key<leftist_heap_node<NodeKey>>
	, class Compare = std::less<std::remove_cvref_t<std::invoke_result_t<GetKey, const leftist_heap_node<NodeKey>&>>>>
class basic_leftist_heap
	: private GetKey
	, private Compare
{
public:
	using node = leftist_heap_node<NodeKey>;
	using key_type = std::remove_cvref_t<std::invoke_result_t<GetKey, const node&>>;
	using key_compare = Compare;
	using s_type = node_balance_t<node>;

protected:
	basic_leftist_heap(const basic_leftist_heap&) = default;
	basic_leftist_heap& operator =(const basic_leftist_heap&) = default;

	basic_leftist_heap& operator =(basic_leftist_heap&& x) {
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
	basic_leftist_heap()
		: root_(nullptr) {
	}

	basic_leftist_heap(basic_leftist_heap&& x)
		: root_(x.root_) {
		x.root_ = nullptr;
	}

	void swap(basic_leftist_heap& x) {
		auto tmp = root_;
		root_ = x.root_;
		x.root_ = tmp;
	}

	bool empty() const {
		return !root_;
	}

	void merge(basic_leftist_heap& x) {
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
		p->b = 1;
		root_ = root_ ? merge(root_, p) : p;
	}

	node* pop() {
		node* ret = root_;
#ifdef DLOU_CHECK_ARGS
		if (!ret)
			return nullptr;
#endif
		root_ = ret->n[0]
			? (ret->n[1] ? merge(ret->n[0], ret->n[1]) : ret->n[0])
			: ret->n[1];

		return ret;
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
		node* ret;
		if (compare(*p1, *p2)) {
			ret = p1;
			p1 = p2;
		}
		else {
			ret = p2;
		}
		p2 = ret->n[1];
		ret->n[1] = p2 ? merge(p1, p2) : p1;

		s_type s1 = rank(ret->n[0]);
		s_type s2 = rank(ret->n[1]);
		if (s1 < s2) {
			p1 = ret->n[0];
			ret->n[0] = ret->n[1];
			ret->n[1] = p1;
			ret->b = s1 + 1;
		}
		else
			ret->b = s2 + 1;

		return ret;
	}

	//node* _fault(node* pos) {
	//	if (!pos)
	//		return nullptr;
	//
	//	node* left = pos->n[0];
	//	node* right = pos->n[1];
	//	if (left) {
	//		if (right) {
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
using leftist_heap = basic_leftist_heap<Key, node_key<leftist_heap_node<Key>>, Compare>;

} // namespace dlou
