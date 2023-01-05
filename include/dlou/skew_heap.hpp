#pragma once

#include "node.hpp"

#include <type_traits>
#include <functional>

namespace dlou {

template<class Key>
using skew_heap_node = node<2, Key>;

template<class NodeKey
	, class GetKey = node_key<skew_heap_node<NodeKey>>
	, class Compare = std::less<std::remove_cvref_t<std::invoke_result_t<GetKey, const skew_heap_node<NodeKey>&>>>>
class basic_skew_heap
	: private GetKey
	, private Compare
{
public:
	using node = skew_heap_node<NodeKey>;
	using key_type = std::remove_cvref_t<std::invoke_result_t<GetKey, const node&>>;
	using key_compare = Compare;

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

	const key_type& key(const node& n) const {
		return GetKey::operator ()(n);
	}

	bool compare(const node& a, const node& b) const {
		return Compare::operator ()(key(a), key(b));
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
	//	return _fault(root_);
	//}

protected:
	node* merge(node* p1, node* p2) {
		node* ret;
		if (compare(*p1, *p2)) {
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

	//node* _fault(node* pos) {
	//	if (!pos)
	//		return nullptr;
	//
	//	node* child = pos->n[0];
	//	if (child && compare(*child, *pos))
	//		return pos;
	//	child = pos->n[1];
	//	if (child && compare(*child, *pos))
	//		return pos;
	//
	//	child = _fault(pos->n[0]);
	//	return child ? child : _fault(pos->n[1]);
	//}

protected:
	node* root_;
};

template<class Key, class Compare = std::less<Key>>
using skew_heap = basic_skew_heap<Key, node_key<skew_heap_node<Key>>, Compare>;

} // namespace dlou
