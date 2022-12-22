#pragma once

#include "node.hpp"

namespace dlou {
namespace double_ended {
	
class doubly_linked_list {
public:
	using node = node<2>;

protected:
	doubly_linked_list(const doubly_linked_list&) = default;
	doubly_linked_list& operator =(const doubly_linked_list& x) = default;

	doubly_linked_list& operator =(doubly_linked_list&& x) noexcept {
		if (x.empty())
			clear();
		else
			attach(x);
		return *this;
	}

	void clear() {
		end_ = { &end_, &end_ };
	}

	void replace(node* oldnode, node* newnode) {
		auto prev = _prev(oldnode);
		auto next = _next(oldnode);

		_prev(newnode) = prev;
		_next(newnode) = next;

		_next(prev) = newnode;
		_prev(next) = newnode;
	}

	void attach(doubly_linked_list& x) {
		replace(&x.end_, &end_);
		x.clear();
	}

	template<class Node>
	static auto& _next(Node* p) {
		return p->n[0];
	}

	template<class Node>
	static auto& _prev(Node* p) {
		return p->n[1];
	}

public:
	static node* next(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return nullptr;
#endif
		return _next(p);
	}

	static const node* next(const node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return nullptr;
#endif
		return _next(p);
	}

	static node* prev(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return nullptr;
#endif
		return _prev(p);
	}

	static const node* prev(const node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return nullptr;
#endif
		return _prev(p);
	}

public:
	doubly_linked_list()
		: end_({ &end_, &end_ }) {
	}

	doubly_linked_list(doubly_linked_list&& x) noexcept
		: end_({ &end_, &end_ }) {
		if (!x.empty())
			attach(x);
	}

	void swap(doubly_linked_list& x) {
		if (x.empty()) {
			if (!empty())
				x.attach(*this); // !empty() && x.empty()
		}
		else if (empty())
			attach(x); // empty() && !x.empty()
		else {
			// !empty() && !x.empty()
			auto tmp = x.end_;
			replace(&end_, &x.end_);
			replace(&tmp, &end_);
		}
	}

	bool empty() const {
		return &end_ == front();
	}

	node* front() const {
		return _next(&end_);
	}

	void push_front(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		insert(front(), p);
	}

	node* pop_front() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		auto ret = front();
		erase(ret);
		return ret;
	}

	node* back() const {
		return _prev(&end_);
	}

	void push_back(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		insert(&end_, p);
	}

	node* pop_back() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		auto ret = back();
		erase(ret);
		return ret;
	}
	
	const node* before_front() const {
		return &end_;
	}

	const node* after_back() const {
		return &end_;
	}

	void insert(const node* pos, node* newnode) {
		insert(pos, newnode, newnode);
	}

	void erase(node* pos) {
		erase(pos, pos);
	}

	// contains last
	void insert(const node* pos, node* first, node* last) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || !first || !last)
			return;
#endif
		auto prev = _prev(pos);
		auto next = const_cast<node*>(pos);

		_prev(first) = prev;
		_next(prev) = first;

		_next(last) = next;
		_prev(next) = last;
	}

	// contains last
	void erase(node* first, node* last) {
#ifdef DLOU_CHECK_ARGS
		if (!first || !last)
			return;
#endif
		auto prev = _prev(first);
		auto next = _next(last);

		_next(prev) = next;
		_prev(next) = prev;
	}

	void splice(const node* pos, doubly_linked_list& x) {
#ifdef DLOU_CHECK_ARGS
		if (x.empty())
			return;
#endif
		auto first = x.front();
		auto last = x.back();
		x.erase(first, last);
		insert(pos, first, last);
	}

protected:
	node end_;
};

} // namespace double_ended

using double_ended::doubly_linked_list;

} // namespace dlou
