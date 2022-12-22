#pragma once

#include "../node.hpp"

namespace dlou {
namespace circular {

class singly_linked_list {
public:
	using node = node<1>;

protected:
	singly_linked_list(const singly_linked_list&) = default;
	singly_linked_list& operator =(const singly_linked_list&) = default;

	singly_linked_list& operator =(singly_linked_list && x) noexcept {
		tail_ = x.tail_;
		x.clear();
		return *this;
	}

	void clear() {
		tail_ = nullptr;
	}

	template<class Node>
	static auto& _next(Node* p) {
		return p->n[0];
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

public:
	singly_linked_list()
		: tail_(nullptr) {
	}

	singly_linked_list(singly_linked_list&& x) noexcept
		: tail_(x.tail_) {
		x.clear();
	}

	void swap(singly_linked_list& x) {
		node* tmp = tail_;
		tail_ = x.tail_;
		x.tail_ = tmp;
	}

	bool empty() const {
		return !tail_;
	}

	node* front() const {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		return _next(tail_);
	}

	void push_front(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		if (tail_)
			insert_after(tail_, p);
		else
			tail_ = _next(p) = p;
	}

	node* pop_front() {
		return erase_after(tail_);
	}

	node* back() const {
		return tail_;
	}

	void push_back(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		if (tail_) {
			insert_after(tail_, p);
			tail_ = p; // move_anchor
		}
		else
			tail_ = _next(p) = p;
	}

	void insert_after(node* pos, node* newnode) {
#ifdef DLOU_CHECK_ARGS
		if (!newnode)
			return;

		if (!pos) {
			if (!tail_) {
				tail_ = _next(newnode) = newnode;
				return;
			}
			pos = tail_;
		}
#endif
		node* nxt = _next(pos);
		_next(pos) = newnode;
		_next(newnode) = nxt;
	}

	node* erase_after(node* pos) {
#ifdef DLOU_CHECK_ARGS
		if (!pos) {
			if (empty())
				return nullptr;
			pos = tail_;
		}
#endif
		node* nxt = _next(pos);
		if (nxt == pos)
			tail_ = nullptr;
		else
			_next(pos) = _next(nxt);
		return nxt;
	}

	void move_anchor() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return;
#endif
		tail_ = _next(tail_);
	}

protected:
	node* tail_;
};

} // namespace circular
} // namespace dlou
