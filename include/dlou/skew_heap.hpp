#pragma once

#include "node.hpp"

#include <functional>

namespace dlou {

template<class Key>
using skew_heap_node = node<2, Key>;

template<class Key, class Compare = std::less<Key>>
class skew_heap
{
public:
	using key_type = Key;
	using key_compare = Compare;
	using node = skew_heap_node<key_type>;

protected:
	skew_heap(const skew_heap&) = default;
	skew_heap& operator =(const skew_heap&) = default;

	skew_heap& operator =(skew_heap && x) {
		root_ = x.root_;
		x.root_ = nullptr;
		return *this;
	}

	void clear() {
		root_ = nullptr;
	}

public:
	skew_heap()
		: root_(nullptr) {
	}

	skew_heap(skew_heap&& x)
		: root_(x.root_) {
		x.root_ = nullptr;
	}

	void swap(skew_heap& x) {
		auto tmp = root_;
		root_ = x.root_;
		x.root_ = tmp;
	}

	bool empty() const {
		return !root_;
	}

	void merge(skew_heap& x) {
		node* p = x.root_;
		if (!p)
			return;
		x.root_ = nullptr;
		root_ = root_ ? merge(root_, p) : p;
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
		key_compare cmp;
		node* ret;
		if (cmp(p1->k, p2->k)) {
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
	//	key_compare cmp;
	//	node* child = pos->n[0];
	//	if (child && cmp(child->k, pos->k))
	//		return pos;
	//	child = pos->n[1];
	//	if (child && cmp(child->k, pos->k))
	//		return pos;
	//
	//	child = fault(pos->n[0]);
	//	return child ? child : fault(pos->n[1]);
	//}

protected:
	node* root_;
};

} // namespace dlou
