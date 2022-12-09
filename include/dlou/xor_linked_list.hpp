#pragma once

#include "node.hpp"

#include <cstddef>

#include <iterator>


namespace dlou {
	
class xor_linked_list {
public:
	using node = node<1>;

	template<class Iterator>
	class basic_const_iterator : protected Iterator {
		friend class xor_linked_list;
	public:
		using typename Iterator::iterator_category;
		using typename Iterator::difference_type;
		using value_type = const typename Iterator::value_type;
		using pointer = value_type*;
		using reference = value_type&;

	public:
		basic_const_iterator() = default;
		basic_const_iterator(const basic_const_iterator&) = default;
		basic_const_iterator& operator =(const basic_const_iterator&) = default;

		basic_const_iterator(const Iterator& it) : Iterator(it) {}

		bool operator ==(const Iterator& x) const { return this->curr_ == x.curr_; }
		bool operator !=(const Iterator& x) const { return this->curr_ != x.curr_; }

		bool operator ==(const basic_const_iterator& x) const { return this->curr_ == x.curr_; }
		bool operator !=(const basic_const_iterator& x) const { return this->curr_ != x.curr_; }

		reference operator *() const { return *this->curr_; }
		pointer operator ->() const { return this->curr_; }

		basic_const_iterator& operator ++() { this->next(); return *this; }
		basic_const_iterator operator ++(int) { auto ret = *this; this->next(); return ret; }
		basic_const_iterator& operator --() { this->prev(); return *this; }
		basic_const_iterator operator --(int) { auto ret = *this; this->prev(); return ret; }
	};

	class iterator {
		friend class xor_linked_list;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = node;
		using pointer = node*;
		using reference = node&;

	protected:
		pointer prev_;
		pointer curr_;

		iterator(pointer prev, pointer curr)
			: prev_(prev), curr_(curr) {
		}
	public:
		iterator() : prev_(nullptr), curr_(nullptr) {}
		iterator(const iterator&) = default;
		iterator& operator =(const iterator&) = default;

		bool operator ==(const iterator& x) const { return curr_ == x.curr_; }
		bool operator !=(const iterator& x) const { return curr_ != x.curr_; }
		bool operator ==(const basic_const_iterator<iterator>& x) const { return x == *this; }
		bool operator !=(const basic_const_iterator<iterator>& x) const { return x != *this; }

		reference operator *() const { return *curr_; }
		pointer operator ->() const { return curr_; }

		iterator& operator ++() { next(); return *this; }
		iterator operator ++(int) { auto ret = *this; next(); return ret; }
		iterator& operator --() { prev(); return *this; }
		iterator operator --(int) { auto ret = *this; prev(); return ret; }

	protected:
		void next() { auto p = curr_; curr_ = _next(prev_, p); prev_ = p; }
		void prev() { auto p = prev_; prev_ = _next(curr_, p); curr_ = p; }
	};

	class reverse_iterator : public iterator {
		friend class xor_linked_list;
	public:
		using iterator_type = iterator;

	protected:
		reverse_iterator(pointer prev, pointer curr)
			: iterator(prev, curr) {
		}

	public:
		reverse_iterator() = default;
		reverse_iterator(const reverse_iterator&) = default;
		reverse_iterator& operator =(const reverse_iterator&) = default;

		explicit reverse_iterator(iterator_type it) : iterator(it.curr_, it.prev_) {}
		iterator_type base() const { return iterator(curr_, prev_); }

		bool operator ==(const reverse_iterator& x) const { return curr_ == x.curr_; }
		bool operator !=(const reverse_iterator& x) const { return curr_ != x.curr_; }
		bool operator ==(const basic_const_iterator<reverse_iterator>& x) const { return x == *this; }
		bool operator !=(const basic_const_iterator<reverse_iterator>& x) const { return x != *this; }

		reference operator *() const { return *curr_; }
		pointer operator ->() const { return curr_; }

		reverse_iterator& operator ++() { next(); return *this; }
		reverse_iterator operator ++(int) { auto ret = *this; next(); return ret; }
		reverse_iterator& operator --() { prev(); return *this; }
		reverse_iterator operator --(int) { auto ret = *this; prev(); return ret; }
	};

	using const_iterator = basic_const_iterator<iterator>;
	using const_reverse_iterator = basic_const_iterator<reverse_iterator>;

protected:
	xor_linked_list(const xor_linked_list&) = default;
	xor_linked_list& operator =(const xor_linked_list& x) = default;

	xor_linked_list& operator =(xor_linked_list&& x) {
		if (x.empty())
			clear();
		else
			attach(x);

		return *this;
	}

	template<typename... Args>
	static node* _xor(Args&&... args) {
		return (node*)(ptrdiff_t(args) ^ ...);
	}

	template<typename... Args>
	static void _xor_to(node*& dst, Args&&... args) {
		dst = (node*)(ptrdiff_t(dst) ^ ... ^ ptrdiff_t(args));
	}

	static node* _next(const node* prev, const node* curr) {
		return _xor(curr->n[0], prev);
	}

	void clear() {
		head_.n[0] = nullptr;
		tail_.n[0] = nullptr;
	}

	void attach(xor_linked_list& x) {
		node* f = x.front();
		node* b = x.back();
		_xor_to(f->n[0], &x.head_, &head_);
		_xor_to(b->n[0], &x.tail_, &tail_);
		head_.n[0] = _xor(&tail_, f);
		tail_.n[0] = _xor(&head_, b);
		x.clear();
	}

public:
	xor_linked_list()
		: head_({ 0 })
		, tail_({ 0 }) {
	}

	xor_linked_list(xor_linked_list&& x)
		: head_({ 0 })
		, tail_({ 0 }) {
		if (!x.empty())
			attach(x);
	}

	void swap(xor_linked_list& x) {
		xor_linked_list tmp = (xor_linked_list&&)*this;
		*this = (xor_linked_list&&)x;
		x = (xor_linked_list&&)tmp;
	}

	bool empty() const {
		return nullptr == head_.n[0];
	}

	node* front() const {
		return _next(&tail_, &head_);
	}

	void push_front(node* p) {
		node* f = front();
		p->n[0] = _xor(&head_, f);
		_xor_to(f->n[0], p, &head_);
		_xor_to(head_.n[0], f, p);
	}

	node* pop_front() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		node* f = front();
		node* n = _next(&head_, f);
		_xor_to(n->n[0], f, &head_);
		_xor_to(head_.n[0], f, n);
		return f;
	}

	node* back() const {
		return _next(&head_, &tail_);
	}

	void push_back(node* p) {
		node* b = back();
		p->n[0] = _xor(&tail_, b);
		_xor_to(b->n[0], p, &tail_);
		_xor_to(tail_.n[0], b, p);
	}

	node* pop_back() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		node* b = back();
		node* n = _next(&tail_, b);
		_xor_to(n->n[0], b, &tail_);
		_xor_to(tail_.n[0], b, n);
		return b;
	}

	iterator begin() { return{ &head_, front() }; }
	iterator end() { return{ back(), &tail_ }; }
	reverse_iterator rbegin() { return{ &tail_, back() }; }
	reverse_iterator rend() { return{ front(), &head_ }; }

	const_iterator cbegin() const { return const_cast<xor_linked_list*>(this)->begin(); }
	const_iterator cend() const { return const_cast<xor_linked_list*>(this)->end(); }
	const_reverse_iterator crbegin() const { return const_cast<xor_linked_list*>(this)->rbegin(); }
	const_reverse_iterator crend() const { return const_cast<xor_linked_list*>(this)->rend(); }

	const_iterator begin() const { return cbegin(); }
	const_iterator end() const { return cend(); }
	const_reverse_iterator rbegin() const { return crbegin(); }
	const_reverse_iterator rend() const { return crend(); }

	void insert(iterator& pos, node* newnode) {
		newnode->n[0] = _xor(pos.prev_, pos.curr_);
		_xor_to(const_cast<node*>(pos.curr_)->n[0], newnode, pos.prev_);
		_xor_to(const_cast<node*>(pos.prev_)->n[0], newnode, pos.curr_);
		pos.curr_ = newnode;
	}

	node* erase(iterator& pos) {
		auto p = const_cast<node*>(pos.curr_);
		pos.curr_ = _next(pos.prev_, pos.curr_);
		_xor_to(const_cast<node*>(pos.curr_)->n[0], p, pos.prev_);
		_xor_to(const_cast<node*>(pos.prev_)->n[0], p, pos.curr_);
		return p;
	}

protected:
	// back <-> tail_ <-> head_ <-> front <-> ... <-> back
	node head_, tail_;
};

} // namespace dlou
