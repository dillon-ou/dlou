#pragma once

#include "../node.hpp"

#include <iterator>


namespace dlou {
namespace circular {

// back(anchor) -> front -> ... -> back
class singly_linked_list
{
public:
	using node = node<1>;

	class iterator {
		friend class singly_linked_list;
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const node;
		using pointer = const node*;
		using reference = const node&;

	protected:
		pointer pos_;

		iterator(pointer pos)
			: pos_(pos) {
		}
	public:
		iterator() : pos_(nullptr) {}
		iterator(const iterator&) = default;
		iterator& operator =(const iterator&) = default;

		bool operator ==(const iterator& x) const { return pos_ == x.pos_; }
		bool operator !=(const iterator& x) const { return pos_ != x.pos_; }

		reference operator *() const { return *pos_; }
		pointer operator ->() const { return pos_; }

		iterator& operator ++() { pos_ = _next(pos_); return *this; }
		iterator operator ++(int) { auto ret = *this; pos_ = _next(pos_); return ret; }
	};

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

	// first -> return
	template<class InputIterator>
	InputIterator link(InputIterator first, InputIterator last) {
		InputIterator prv = first++;
		while (first != last) {
			_next(&*prv) = &*first;
			prv = first++;
		}
		//_next(&*prv) = nullptr;

		return prv;
	}

	// first <- return
	template<class InputIterator>
	InputIterator rlink(InputIterator first, InputIterator last) {
		InputIterator prv = first++;
		//_next(&*prv) = nullptr;
		while (first != last) {
			_next(&*first) = &*prv;
			prv = first++;
		}

		return prv;
	}

	void _insert_after(const node* pos, node* head, node* tail) {
		auto& next = _next(const_cast<node*>(pos));
		_next(tail) = next;
		next = head;
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
		: tail_(nullptr) {
	}

	singly_linked_list(singly_linked_list&& x) noexcept
		: tail_(x.tail_) {
		x.clear();
	}

	template<class InputIterator>
	singly_linked_list(InputIterator first, InputIterator last) {
		node* head = &*first;
		node* tail = &*link(first, last);
		_next(tail) = head;
		tail_ = tail;
	}

	void swap(singly_linked_list& x) {
		node* tmp = tail_;
		tail_ = x.tail_;
		x.tail_ = tmp;
	}

	bool empty() const {
		return !tail_;
	}

	const node* before_front() const {
		return back();
	}

	const node* front() const {
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
			_insert_after(before_front(), p, p);
		else
			tail_ = _next(p) = p;
	}

	// *InputIterator == node&
	template<class InputIterator>
	void push_front(InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		node* tail = &*first;
		node* head = &*rlink(first, last);

		if (tail_)
			_insert_after(before_front(), head, tail);
		else {
			_next(tail) = head;
			tail_ = tail;
		}
	}

	node* pop_front() {
		return erase_after(tail_);
	}

	const node* back() const {
		return tail_;
	}

	void push_back(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		if (tail_) {
			_insert_after(tail_, p, p);
			tail_ = p; // move_anchor
		}
		else
			tail_ = _next(p) = p;
	}

	// *InputIterator == node&
	template<class InputIterator>
	void push_back(InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		node* head = &*first;
		node* tail = &*link(first, last);

		if (tail_)
			_insert_after(before_front(), head, tail);
		else
			_next(tail) = head;

		tail_ = tail;
	}

	void insert_after(const node* pos, node* newnode) {
#ifdef DLOU_CHECK_ARGS
		if (!newnode)
			return;
#endif

		if (pos)
			_insert_after(pos, newnode, newnode);
		else
			tail_ = _next(newnode) = newnode;
	}

	// *InputIterator == node&
	template<class InputIterator>
	void insert_after(const node* pos, InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		node* head = &*first;
		node* tail = &*link(first, last);

		if (pos)
			_insert_after(pos, head, tail);
		else {
			_next(tail) = head;
			tail_ = tail;
		}
	}

	node* erase_after(const node* pos) {
#ifdef DLOU_CHECK_ARGS
		if (!pos)
			return nullptr;
#endif
		node* nxt = _next(pos);
		if (nxt == pos)
			tail_ = nullptr;
		else {
			_next(const_cast<node*>(pos)) = _next(nxt);
			if (nxt == tail_)
				tail_ = const_cast<node*>(pos);
		}
		return nxt;
	}

	void splice_after(const node* pos, singly_linked_list&& other) {
		if (other.empty())
			return;

		node* tail = other.tail_;
		other.clear();

		if (pos)
			_insert_after(pos, _next(tail), tail);
		else
			tail_ = tail;
	}

	void splice_after(const node* pos, singly_linked_list&& other, const node* prev) {
#ifdef DLOU_CHECK_ARGS
		if (!prev)
			return;
#endif
		node* nod = other.erase_after(prev);
		if (pos)
			_insert_after(pos, nod, nod);
		else {
			_next(nod) = nod;
			tail_ = nod;
		}
	}

	void rotate() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return;
#endif
		tail_ = _next(tail_);
	}

	iterator before_begin() const { return{ before_front() }; }

	/*
	* if (!empty()) {
	*     iterator iter = last = begin();
	*     do {
	*         *iter;
	*     } while (++iter != last);
	* }
	*/
	iterator begin() const { return{ front() }; }

	// Return inserted iterator
	iterator insert_after(iterator pos, node* newnode) {
		insert_after(&*pos, newnode);
		return { newnode };
	}

	// Return iterator to last element inserted, or pos if first==last
	template<class InputIterator>
	iterator insert_after(iterator pos, InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return pos;
#endif

		insert_after(&*pos, first, last);
		return { &*last };
	}

	// Return removed node
	node* erase_after(iterator pos) {
		return erase_after(&*pos);
	}

	void splice_after(iterator pos, singly_linked_list&& other) {
		splice_after(&*pos, (singly_linked_list&&)other);
	}

	void splice_after(iterator pos, singly_linked_list&& other, iterator it) {
		splice_after(&*pos, (singly_linked_list&&)other, &*it);
	}

protected:
	node* tail_;
};

} // namespace circular
} // namespace dlou
