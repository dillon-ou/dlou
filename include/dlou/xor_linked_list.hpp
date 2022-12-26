#pragma once

#include "node.hpp"
#include "glvalue.hpp"

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


	static void replace(node* cur, const node* a, const node* b) {
		_xor_to(cur->n[0], a, b);
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
		replace(f, &x.head_, &head_);
		replace(b, &x.tail_, &tail_);
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

	using gliterator = const glvalue<iterator>&;

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
		replace(f, p, &head_);
		replace(&head_, f, p);
	}

	template<class InputIterator>
	void push_front(InputIterator first, InputIterator last) {
		node* f = _front();

		last = link(first, last);
		replace(last, &head_);
		replace(&head_, f, &*last);

		replace(first, f);
		replace(f, &head_, &*first);
	}

	node* pop_front() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		node* f = _front();
		node* n = _next(&head_, f);
		replace(n, f, &head_);
		replace(&head_, f, n);
		return f;
	}

	const node* back() const {
		return _back();
	}

	void push_back(node* p) {
		node* b = _back();
		p->n[0] = _xor(&tail_, b);
		replace(b, p, &tail_);
		replace(&tail_, b, p);
	}

	template<class InputIterator>
	void push_back(InputIterator first, InputIterator last) {
		node* b = _back();

		last = link(first, last);
		replace(first, b);
		replace(b, &tail_, &*first);

		replace(last, &tail_);
		replace(&tail_, b, &*last);
	}

	node* pop_back() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		node* b = _back();
		node* n = _next(&tail_, b);
		replace(n, b, &tail_);
		replace(&tail_, b, n);
		return b;
	}

	iterator begin() const { return{ &head_, _front() }; }
	iterator end() const { return{ _back(), &tail_ }; }
	
	reverse_iterator rbegin() const { return{ &tail_, _back() }; }
	reverse_iterator rend() const { return{ _front(), &head_ }; }

	// Return inserted iterator
	// Update pos (after inserted)
	iterator insert(gliterator pos, node* newnode) {
		iterator ret = *pos;
		newnode->n[0] = _xor(ret.prev_, ret.curr_);
		replace(ret.curr_, ret.prev_, newnode);
		replace(ret.prev_, ret.curr_, newnode);
		pos->prev_ = newnode;
		ret.curr_ = newnode;
		return ret;
	}

	template<class InputIterator>
	iterator insert(gliterator pos, InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return *pos;
#endif
		iterator ret = *pos;

		last = link(first, last);
		replace(last, ret.curr_);
		replace(first, ret.prev_);

		ret.curr_ = &*first;
		pos->prev_ = &*last;

		replace(ret.prev_, pos->curr_, ret.curr_);
		replace(pos->curr_, ret.prev_, pos->prev_);

		return ret;
	}

	// Return removed node
	// Set pos to iterator following the last removed element
	node* erase(gliterator pos) {
		node* ret = pos->curr_;
		pos->curr_ = _next(pos->prev_, pos->curr_);
		replace(pos->curr_, ret, pos->prev_);
		replace(pos->prev_, ret, pos->curr_);
		return ret;
	}

	// Return singly linked list from head to nullptr.
	//   ex: for (auto pos = ret; pos; pos = pos->n[0])
	// Set first to iterator following the last removed element
	node* erase(gliterator first, iterator last) {
#ifdef DLOU_CHECK_ARGS
		if (*first == last)
			return {};
#endif
		auto head = first->curr_;

		replace(last.curr_, last.prev_, first->prev_);
		replace(first->prev_, head, last.curr_);
		first->curr_ = last.curr_;

		auto prv = first->prev_;
		auto pos = head;
		while (pos != last.prev_) {
			auto nxt = _next(prv, pos);
			prv = pos;
			pos = nxt;
			prv->n[0] = pos;
		}
		last.prev_->n[0] = nullptr;

		return head;
	}

	void splice(gliterator pos, xor_linked_list&& other) {
		return splice(pos, (xor_linked_list&&)other, other.begin(), other.end());
	}

	void splice(gliterator pos, xor_linked_list&& other, iterator it) {
		return splice(pos, (xor_linked_list&&)other, it, it);
	}

	void splice(gliterator pos, xor_linked_list&& other, iterator first, iterator last) {
		if (first == last)
			return;

		// sub
		replace(first.curr_, first.prev_, pos->prev_);
		replace(last.prev_, last.curr_, pos->curr_);

		// other
		replace(first.prev_, first.curr_, last.curr_);
		replace(last.curr_, last.prev_, first.prev_);

		// this
		replace(pos->prev_, pos->curr_, first.curr_);
		replace(pos->curr_, pos->prev_, last.prev_);

		pos->prev_ = last.prev_;
	}

protected:
	// back <-> tail_ <-> head_ <-> front <-> ... <-> back
	node head_, tail_;
};

} // namespace dlou
