#pragma once

#include "../node.hpp"

#include <iterator>


namespace dlou {
namespace double_ended {

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

		iterator& operator ++() { pos_ = next(pos_); return *this; }
		iterator operator ++(int) { auto ret = *this; pos_ = next(pos_); return ret; }
	};

protected:
	singly_linked_list(const singly_linked_list&) = default;
	singly_linked_list& operator =(const singly_linked_list&) = default;
	singly_linked_list& operator =(singly_linked_list&& x) = delete;

	void clear() {
		head_ = { &tail_ };
		tail_ = { &tail_ };
	}

	void reset() {
		_next(_back()) = &tail_;
	}

	void move(singly_linked_list& x) {
		*this = x;
		x.clear();
		reset();
	}

	template<class Node>
	static auto& _next(Node* p) {
		return p->n[0];
	}

	node* _before_front() const {
		return const_cast<node*>(&head_);
	}

	node* _front() const {
		return _next(&head_);
	}

	node* _back() const {
		return _next(&tail_);
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
		if (&tail_ == next)
			_next(&tail_) = tail;
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
		: head_({ &tail_ })
		, tail_({ &tail_ }) {
	}

	singly_linked_list(singly_linked_list&& x) noexcept {
		if (x.empty())
			clear();
		else
			move(x);
	}

	template<class InputIterator>
	singly_linked_list(InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		auto& back = _next(&tail_);
		back = &*link(first, last);
		_next(back) = &tail_;
		_next(_before_front()) = &*first;
	}

	void swap(singly_linked_list& x) {
		if (empty()) {
			if (x.empty())
				return;

			move(x);
		}
		else if (x.empty())
			x.move(*this);
		else {
			auto tmp = *this;
			*this = x;
			x = tmp;

			reset();
			x.reset();
		}
	}
	
	bool empty() const {
		return after_back() == _front();
	}

	const node* before_front() const {
		return _before_front();
	}

	const node* front() const {
		return _front();
	}
	
	void push_front(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		insert_after(before_front(), p);
	}

	// *InputIterator == node&
	template<class InputIterator>
	void push_front(InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		auto& nxt = _next(_before_front());
		node* tail = &*first;
		node* head = &*rlink(first, last);;
		_next(tail) = nxt;
		if (&tail_ == nxt)
			_next(&tail_) = tail;
		nxt = head;
	}
	
	node* pop_front() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		return erase_after(before_front());
	}

	const node* after_back() const {
		return &tail_;
	}

	const node* back() const {
		return _back();
	}

	void push_back(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		node* tail = _next(&tail_);
		_next((&tail_ == tail)  ? &head_ : tail) = p;

		_next(p) = &tail_;
		_next(&tail_) = p;
	}

	void insert_after(const node* pos, node* newnode) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || !newnode)
			return;
#endif
		auto& nxt = _next(const_cast<node*>(pos));
		_next(newnode) = nxt;
		if (&tail_ == nxt)
			_next(&tail_) = newnode;
		nxt = newnode;
	}

	// *InputIterator == node&
	template<class InputIterator>
	void insert_after(const node* pos, InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || first == last)
			return;
#endif
		node* head = &*first;
		node* tail = &*link(first, last);
		_insert_after(pos, head, tail);
	}
	
	node* erase_after(const node* pos) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || &tail_ == pos)
			return nullptr;
#endif
		node* prv = const_cast<node*>(pos);
		node* cur = _next(prv);
#ifdef DLOU_CHECK_ARGS
		if (&tail_ == cur)
			return nullptr;
#endif
		node* nxt = _next(cur);
		_next(prv) = nxt;
		if (&tail_ == nxt)
			_next(&tail_) = prv;
		return cur;
	}

	// Return singly linked list from head to nullptr.
	//   ex: for (auto pos = ret; pos; pos = next(pos))
	node* erase_after(const node* before_first, const node* before_last) {
#ifdef DLOU_CHECK_ARGS
		if (!before_first
			|| after_back() == before_last
			|| before_first == before_last)
			return nullptr;
#endif
		node* prev = const_cast<node*>(before_first);
		node* first = _next(prev);
		auto& last = _next(const_cast<node*>(before_last));
		if (&tail_ == last)
			_next(&tail_) = prev;
		_next(prev) = last;

		last = nullptr;
		return first;
	}

	void splice_after(const node* pos, singly_linked_list&& other) {
#ifdef DLOU_CHECK_ARGS
		if (!pos)
			return;
#endif
		if (other.empty())
			return;

		node* head = other._front();
		node* tail = other._back();
		other.clear();

		_insert_after(pos, head, tail);
	}

	void splice_after(const node* pos, singly_linked_list&& other, const node* prev) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || !prev || !next(prev))
			return;
#endif
		node* nod = other.erase_after(prev);
		_insert_after(pos, nod, nod);
	}

	void splice_after(const node* pos, singly_linked_list&& other, const node* before_first, const node* before_last) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || !before_first || before_first == before_last)
			return;
#endif
		_insert_after(pos
			, other.erase_after(before_first, before_last)
			, const_cast<node*>(before_last));
	}

	iterator before_begin() const { return{ before_front() }; }
	iterator begin() const { return{ _front() }; }
	iterator end() const { return{ after_back() }; }
	iterator find(const node* p) const { return{ p }; }

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

	// Return singly linked list from head to nullptr.
	//   ex: for (auto pos = ret; pos; pos = pos->n[0])
	node* erase_after(iterator before_first, iterator before_last) {
		return erase_after(&*before_first, &*before_last);
	}

	void splice_after(iterator pos, singly_linked_list&& other) {
		splice_after(&*pos, (singly_linked_list&&)other);
	}

	void splice_after(iterator pos, singly_linked_list&& other, iterator it) {
		splice_after(&*pos, (singly_linked_list&&)other, &*it);
	}

	void splice_after(iterator pos, singly_linked_list&& other, iterator before_first, iterator before_last) {
		splice_after(&*pos, (singly_linked_list&&)other, &*before_first, &*before_last);
	}

protected:
	node head_;
	node tail_;
};

} // namespace double_ended
} // namespace dlou
