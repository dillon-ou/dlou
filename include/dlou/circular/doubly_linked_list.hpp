#pragma once

#include "../node.hpp"

namespace dlou {
namespace circular {

class doubly_linked_list {
public:
	using node = node<2>;

protected:
	doubly_linked_list(const doubly_linked_list&) = default;
	doubly_linked_list& operator =(const doubly_linked_list&) = default;

	doubly_linked_list& operator =(doubly_linked_list && x) noexcept {
		head_ = x.head_;
		x.clear();
		return *this;
	}

	void clear() {
		head_ = nullptr;
	}

	enum class is_front {
		unknow,
		yes,
		no
	};

	void _insert(const node* pos, node* first, node* last) {
		auto nxt = const_cast<node*>(pos);
		node* prv = _prev(nxt);

		_prev(first) = prv;
		_next(prv) = first;

		_next(last) = nxt;
		_prev(nxt) = last;
	}

	template<is_front IsFront>
	void _erase(node* pos) {
		node* nxt = _next(pos);
		if (nxt == pos)
			head_ = nullptr;
		else {
			node* prv = _prev(pos);
			_prev(nxt) = prv;
			_next(prv) = nxt;

			if constexpr (is_front::unknow == IsFront) {
				if (pos == head_)
					head_ = nxt;
			}
			else if constexpr (is_front::yes == IsFront) {
				head_ = nxt;
			}
		}
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
		: head_(nullptr) {
	}

	doubly_linked_list(doubly_linked_list&& x) noexcept
		: head_(x.head_) {
		x.clear();
	}

	void swap(doubly_linked_list& x) {
		node* tmp = head_;
		head_ = x.head_;
		x.head_ = tmp;
	}

	bool empty() const {
		return !head_;
	}

	const node* front() const {
		return head_;
	}

	void push_front(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		if (head_) {
			_insert(head_, p, p);
			head_ = p;
		}
		else
			head_ = _next(p) = _prev(p) = p;
	}

	node* pop_front() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		auto ret = head_;
		_erase<is_front::yes>(ret);
		return ret;
	}

	const node* back() const {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		return _prev(head_);
	}

	void push_back(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		if (head_)
			_insert(head_, p, p);
		else
			head_ = _next(p) = _prev(p) = p;
	}

	node* pop_back() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		node* ret = _prev(head_);
		_erase<is_front::no>(ret);
		return ret;
	}

	void insert(const node* pos, node* newnode) {
#ifdef DLOU_CHECK_ARGS
		if (!newnode)
			return;

		if (!pos) {
			if (!head_) {
				head_ = _next(newnode) = _prev(newnode) = newnode;
				return;
			}
			pos = head_;
		}
#endif
		_insert(pos, newnode, newnode);
		if (pos == head_)
			head_ = newnode;
	}

	void erase(node* pos) {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return;
#endif
		_erase<is_front::unknow>(pos);
	}

	void insert(const node* pos, node* first, node* last) {
#ifdef DLOU_CHECK_ARGS
		if (!first || !last)
			return;

		if (!pos) {
			if (!head_) {
				head_ = _next(last) = first;
				_prev(first) = last;
				return;
			}
			pos = head_;
		}
#endif
		_insert(pos, first, last);
		if (pos == head_)
			head_ = first;
	}

	void splice(const node* pos, doubly_linked_list& x) {
		auto first = x.head_;
#ifdef DLOU_CHECK_ARGS
		if (!first)
			return;
#endif
		x.head_ = nullptr;
		insert(pos, first, _prev(first));
	}

	void move_anchor() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return;
#endif
		head_ = _next(head_);
	}

	// reverse move anchor
	void rmove_anchor() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return;
#endif
		head_ = _prev(head_);
	}

protected:
	node* head_;
};

} // namespace circular
} // namespace dlou
