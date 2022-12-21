#pragma once

#include "node.hpp"

#include <functional>

namespace dlou {

template<class Compare, class Key = void>
class basic_skew_heap
{
public:
	using node_compare = Compare;
	using key_type = Key;
	using node = node<2, key_type>;

protected:
	basic_skew_heap(const basic_skew_heap&) = default;
	basic_skew_heap& operator =(const basic_skew_heap&) = default;

	basic_skew_heap& operator =(basic_skew_heap && x) {
		root_ = x.root_;
		x.root_ = nullptr;
		return *this;
	}

	void clear() {
		root_ = nullptr;
	}

public:
	basic_skew_heap()
		: root_(nullptr) {
	}

	basic_skew_heap(basic_skew_heap&& x)
		: root_(x.root_) {
		x.root_ = nullptr;
	}

	void swap(basic_skew_heap& x) {
		auto tmp = root_;
		root_ = x.root_;
		x.root_ = tmp;
	}

	bool empty() const {
		return !root_;
	}

	void merge(basic_skew_heap& x) {
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
	//	return fault(root_);
	//}

protected:
	static node* merge(node* p1, node* p2) {
		node_compare cmp;
		node* ret;
		if (cmp(p1, p2)) {
			ret = p1;
			p1 = p2;
		}
		else
			ret = p2;

		p2 = ret->n[1];
		ret->n[1] = ret->n[0];
		ret->n[0] = p2 ? merge(p1, p2) : p1;
		return ret;
	}

	//static node* fault(node* pos) {
	//	if (!pos)
	//		return nullptr;
	//
	//	node_compare cmp;
	//	node* child = pos->n[0];
	//	if (child && cmp(child, pos))
	//		return pos;
	//	child = pos->n[1];
	//	if (child && cmp(child, pos))
	//		return pos;
	//
	//	child = fault(pos->n[0]);
	//	return child ? child : fault(pos->n[1]);
	//}

protected:
	node* root_;
};

template<class Key>
using skew_heap_node = node<2, Key>;

template<class Key, class Compare = std::less<Key>>
using skew_heap = basic_skew_heap<node_compare<skew_heap_node<Key>, Compare>, Key>;

} // namespace dlou
