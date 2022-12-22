#pragma once

#include "node.hpp"

namespace dlou {

class singly_linked_list {
public:
	using node = node<1>;

protected:
	singly_linked_list(const singly_linked_list&) = default;
	singly_linked_list& operator =(const singly_linked_list&) = default;

	singly_linked_list& operator =(singly_linked_list&& x) noexcept {
		head_ = x.head_;
		x.clear();
		return *this;
	}
	
	void clear() {
		head_ = { nullptr };
	}

	template<class Node>
	static auto& _next(Node* p) {
		return p->n[0];
	}

	node* _front() const {
		return _next(&head_);
	}

public:
	static node* next(node* p) {
#ifdef DLOU_CHECK_ARGS
		return p ? _next(p) : p;
#else
		return _next(p);
#endif
	}

	static const node* next(const node* p) {
#ifdef DLOU_CHECK_ARGS
		return p ? _next(p) : p;
#else
		return _next(p);
#endif
	}

public:
	singly_linked_list()
		: head_({ nullptr }) {
	}

	singly_linked_list(singly_linked_list&& x) noexcept
		: head_(x.head_) {
		x.clear();
	}

	void swap(singly_linked_list& x) {
		auto tmp = head_;
		head_ = x.head_;
		x.head_ = tmp;
	}
	
	bool empty() const {
		return !_front();
	}

	const node* before_front() const {
		return &head_;
	}

	const node* front() const {
		return _front();
	}
	
	void push_front(node* p) {
		insert_after(before_front(), p);
	}
	
	node* pop_front() {
		return erase_after(before_front());
	}

	void insert_after(const node* pos, node* newnode) {
		insert_after(pos, newnode, newnode);
	}

	// contains last
	void push_front(node* first, node* last) {
		insert_after(before_front(), first, last);
	}

	void insert_after(const node* pos, node* first, node* last) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || !first || !last)
			return;
#endif
		_next(last) = _next(pos);
		_next(const_cast<node*>(pos)) = first;
	}
	
	node* erase_after(const node* pos) {
		return erase_after(pos, const_cast<node*>(_next(pos)));
	}

	// contains last
	node* erase_after(const node* before_first, node* last) {
#ifdef DLOU_CHECK_ARGS
		if (!before_first || !last)
			return nullptr;
#endif
		node* pos = const_cast<node*>(before_first);
		node* ret = _next(pos);
		_next(pos) = _next(last);
		return ret;
	}

	void splice_after(const node* pos, singly_linked_list& x) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || x.empty())
			return;
#endif
		auto first = x._front();
		x.clear();
		auto nxt = first;
		decltype(nxt) last;
		do {
			last = nxt;
			nxt = _next(last);
		} while (nxt);
		insert_after(pos, first, last);
	}

protected:
	node head_;
};

} // namespace dlou
