#pragma once

#include "node.hpp"
#include "singly_linked_list.hpp"

#include <cstddef>

#include <iterator>


namespace dlou {
	
class xor_linked_list {
public:
	using node = node<1>;

	class iterator {
		friend class xor_linked_list;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const node;
		using pointer = const node*;
		using reference = const node&;

	protected:
		node* prev_;
		node* curr_;

		iterator(pointer prev, pointer curr)
			: prev_(const_cast<node*>(prev)), curr_(const_cast<node*>(curr)) {
		}
	public:
		iterator() : prev_(nullptr), curr_(nullptr) {}
		iterator(const iterator&) = default;
		iterator& operator =(const iterator&) = default;

		bool operator ==(const iterator& x) const { return curr_ == x.curr_; }
		bool operator !=(const iterator& x) const { return curr_ != x.curr_; }

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

	class reverse_iterator {
		friend class xor_linked_list;
		iterator iter_;
	public:
		using iterator_type = iterator;
		using iterator_category = typename iterator_type::iterator_category;
		using difference_type = typename iterator_type::difference_type;
		using value_type = typename iterator_type::value_type;
		using pointer = typename iterator_type::pointer;
		using reference = typename iterator_type::reference;

	protected:
		reverse_iterator(pointer prev, pointer curr)
			: iter_(prev, curr) {
		}

	public:
		reverse_iterator() = default;
		reverse_iterator(const reverse_iterator&) = default;
		reverse_iterator& operator =(const reverse_iterator&) = default;

		explicit reverse_iterator(iterator_type it) : iter_(it.curr_, it.prev_) {}
		iterator_type base() const { return iter_; }

		bool operator ==(const reverse_iterator& x) const { return iter_ == x.iter_; }
		bool operator !=(const reverse_iterator& x) const { return iter_ != x.iter_; }

		reference operator *() const { return *iter_; }
		pointer operator ->() const { return &*iter_; }

		reverse_iterator& operator ++() { ++iter_; return *this; }
		reverse_iterator operator ++(int) { auto ret = *this; ++iter_; return ret; }
		reverse_iterator& operator --() { --iter_; return *this; }
		reverse_iterator operator --(int) { auto ret = *this; --iter_; return ret; }
	};

protected:
	xor_linked_list(const xor_linked_list&) = default;
	xor_linked_list& operator =(const xor_linked_list& x) = default;

	xor_linked_list& operator =(xor_linked_list&& x) noexcept {
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
		node* f = x._front();
		node* b = x._back();
		_xor_to(f->n[0], &x.head_, &head_);
		_xor_to(b->n[0], &x.tail_, &tail_);
		head_.n[0] = _xor(&tail_, f);
		tail_.n[0] = _xor(&head_, b);
		x.clear();
	}

	node* _front() const {
		return _next(&tail_, &head_);
	}

	node* _back() const {
		return _next(&head_, &tail_);
	}

	template<class InputIterator>
	InputIterator link(InputIterator first, InputIterator last) {
		auto iter = first;
		decltype(iter) prv = iter++;
		prv->n[0] = nullptr;
		while (iter != last) {
			iter->n[0] = &*prv;
			prv = iter++;
		}

		auto nxt = first;
		while (nxt != prv) {
			iter = nxt++;
			_xor_to(iter->n[0], &*nxt);
		}

		return prv;
	}

public:
	xor_linked_list()
		: head_({ 0 })
		, tail_({ 0 }) {
	}

	xor_linked_list(xor_linked_list&& x) noexcept
		: head_({ 0 })
		, tail_({ 0 }) {
		if (!x.empty())
			attach(x);
	}

	template<class InputIterator>
	xor_linked_list(InputIterator first, InputIterator last) {
		last = link(first, last);

		_xor_to(first->n[0], &head_);
		head_.n[0] = _xor(&tail_, &*first);

		_xor_to(last->n[0], &tail_);
		tail_.n[0] = _xor(&head_, &*last);
	}

	void swap(xor_linked_list& x) {
		xor_linked_list tmp = (xor_linked_list&&)*this;
		*this = (xor_linked_list&&)x;
		x = (xor_linked_list&&)tmp;
	}

	bool empty() const {
		return nullptr == head_.n[0];
	}

	const node* front() const {
		return _front();
	}

	void push_front(node* p) {
		node* f = _front();
		p->n[0] = _xor(&head_, f);
		_xor_to(f->n[0], p, &head_);
		_xor_to(head_.n[0], f, p);
	}

	template<class InputIterator>
	void push_front(InputIterator first, InputIterator last) {
		node* f = _front();

		last = link(first, last);
		_xor_to(last->n[0], &head_);
		_xor_to(head_.n[0], f, &*last);

		_xor_to(first->n[0], f);
		_xor_to(f->n[0], &head_, &*first);
	}

	node* pop_front() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		node* f = _front();
		node* n = _next(&head_, f);
		_xor_to(n->n[0], f, &head_);
		_xor_to(head_.n[0], f, n);
		return f;
	}

	const node* back() const {
		return _back();
	}

	void push_back(node* p) {
		node* b = _back();
		p->n[0] = _xor(&tail_, b);
		_xor_to(b->n[0], p, &tail_);
		_xor_to(tail_.n[0], b, p);
	}

	template<class InputIterator>
	void push_back(InputIterator first, InputIterator last) {
		node* b = _back();

		last = link(first, last);
		_xor_to(first->n[0], b);
		_xor_to(b->n[0], &tail_, &*first);

		_xor_to(last->n[0], &tail_);
		_xor_to(tail_.n[0], b, &*last);
	}

	node* pop_back() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		node* b = _back();
		node* n = _next(&tail_, b);
		_xor_to(n->n[0], b, &tail_);
		_xor_to(tail_.n[0], b, n);
		return b;
	}

	iterator begin() const { return{ &head_, _front() }; }
	iterator end() const { return{ _back(), &tail_ }; }
	
	reverse_iterator rbegin() const { return{ &tail_, _back() }; }
	reverse_iterator rend() const { return{ _front(), &head_ }; }

	// Return inserted iterator
	// Update pos (after inserted)
	iterator insert(iterator& pos, node* newnode) {
		iterator ret = pos;
		newnode->n[0] = _xor(ret.prev_, ret.curr_);
		_xor_to(ret.curr_->n[0], ret.prev_, newnode);
		_xor_to(ret.prev_->n[0], ret.curr_, newnode);
		pos.prev_ = newnode;
		ret.curr_ = newnode;
		return ret;
	}

	template<class InputIterator>
	iterator insert(iterator& pos, InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return pos;
#endif
		iterator ret = pos;

		last = link(first, last);
		_xor_to(last->n[0], ret.curr_);
		_xor_to(first->n[0], ret.prev_);

		ret.curr_ = &*first;
		pos.prev_ = &*last;

		_xor_to(ret.prev_->n[0], pos.curr_, ret.curr_);
		_xor_to(pos.curr_->n[0], ret.prev_, pos.prev_);

		return ret;
	}

	// Return pos node
	// Set pos to next
	node* erase(iterator& pos) {
		node* ret = pos.curr_;
		pos.curr_ = _next(pos.prev_, pos.curr_);
		_xor_to(const_cast<node*>(pos.curr_)->n[0], ret, pos.prev_);
		_xor_to(const_cast<node*>(pos.prev_)->n[0], ret, pos.curr_);
		return ret;
	}

	// Set first to last position.
	singly_linked_list erase(iterator& first, iterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return {};
#endif

		iterator beg = first;
		iterator end = last;

		_xor_to(end.curr_->n[0], end.prev_, beg.prev_);
		_xor_to(beg.prev_->n[0], beg.curr_, end.curr_);
		first.curr_ = end.curr_;

		auto prv = beg.prev_;
		auto pos = beg.curr_;
		while (pos != end.prev_) {
			auto nxt = _next(prv, pos);
			prv = pos;
			pos = nxt;
			prv->n[0] = pos;
		}
		end.prev_->n[0] = nullptr;

		singly_linked_list ret;
		ret.push_front(beg.curr_, end.prev_);
		return (singly_linked_list&&)ret;
	}

protected:
	// back <-> tail_ <-> head_ <-> front <-> ... <-> back
	node head_, tail_;
};

} // namespace dlou
