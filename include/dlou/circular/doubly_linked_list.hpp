#pragma once

#include "../node.hpp"

#include <iterator>


namespace dlou {
namespace circular {

class doubly_linked_list
{
public:
	using node = node<2>;

	class iterator {
		friend class doubly_linked_list;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
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
		iterator& operator --() { pos_ = _prev(pos_); return *this; }
		iterator operator --(int) { auto ret = *this; pos_ = _prev(pos_); return ret; }
	};

	class reverse_iterator {
		friend class doubly_linked_list;
	public:
		using iterator_type = iterator;
		using iterator_category = typename iterator_type::iterator_category;
		using difference_type = typename iterator_type::difference_type;
		using value_type = typename iterator_type::value_type;
		using pointer = typename iterator_type::pointer;
		using reference = typename iterator_type::reference;

	protected:
		iterator iter_;

		reverse_iterator(pointer pos)
			: iter_(pos) {
		}

	public:
		reverse_iterator() = default;
		reverse_iterator(const reverse_iterator&) = default;
		reverse_iterator& operator =(const reverse_iterator&) = default;

		explicit reverse_iterator(iterator_type it) : iter_(it) {}
		iterator_type base() const { return iter_; }

		bool operator ==(const reverse_iterator& x) const { return iter_ == x.iter_; }
		bool operator !=(const reverse_iterator& x) const { return iter_ != x.iter_; }

		reference operator *() const { return *iter_; }
		pointer operator ->() const { return &*iter_; }

		reverse_iterator& operator ++() { --iter_; return *this; }
		reverse_iterator operator ++(int) { auto ret = *this; --iter_; return ret; }
		reverse_iterator& operator --() { ++iter_; return *this; }
		reverse_iterator operator --(int) { auto ret = *this; ++iter_; return ret; }
	};

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

	template<class InputIterator>
	InputIterator link(InputIterator first, InputIterator last) {
		InputIterator prv = first++;
		while (first != last) {
			_next(&*prv) = &*first;
			_prev(&*first) = &*prv;
			prv = first++;
		}

		return prv;
	}

	template<class InputIterator>
	InputIterator rlink(InputIterator first, InputIterator last) {
		InputIterator prv = first++;
		while (first != last) {
			_prev(&*prv) = &*first;
			_next(&*first) = &*prv;
			prv = first++;
		}

		return prv;
	}

	enum class is_front {
		unknow,
		yes,
		no
	};

	void _insert(const node* pos, const node* head, const node* tail) {
		auto prev = _prev(pos);
		auto next = const_cast<node*>(pos);

		_prev(head) = prev;
		_next(tail) = next;

		_next(prev) = const_cast<node*>(head);
		_prev(next) = const_cast<node*>(tail);
	}

	template<is_front IsFront>
	void _erase(const node* pos) {
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

	static node*& _next(const node* p) {
		return const_cast<node*>(p)->n[0];
	}

	static node*& _prev(const node* p) {
		return const_cast<node*>(p)->n[1];
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

	static node* prev(node* p) {
#ifdef DLOU_CHECK_ARGS
		return p ? _prev(p) : p;
#else
		return _prev(p);
#endif
	}

	static const node* prev(const node* p) {
#ifdef DLOU_CHECK_ARGS
		return p ? _prev(p) : p;
#else
		return _prev(p);
#endif
	}

public:
	doubly_linked_list()
		: head_(nullptr) {
	}

	doubly_linked_list(doubly_linked_list&& x) noexcept
		: head_(x.head_) {
		x.clear();
	}

	template<class InputIterator>
	doubly_linked_list(InputIterator first, InputIterator last) {
		node* head = &*first;
		node* tail = &*link(first, last);
		_next(tail) = head;
		_prev(head) = tail;
		head_ = head;
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

	// *InputIterator == node&
	template<class InputIterator>
	void push_front(InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		node* tail = &*first;
		node* head = &*rlink(first, last);

		if (head_)
			_insert(head_, head, tail);
		else {
			_next(tail) = head;
			_prev(head) = tail;
		}
		head_ = head;
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

	// *InputIterator == node&
	template<class InputIterator>
	void push_back(InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		node* head = &*first;
		node* tail = &*link(first, last);

		if (head_)
			_insert(head_, head, tail);
		else {
			_next(tail) = head;
			_prev(head) = tail;
			head_ = head;
		}
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
		if (!pos || !newnode)
			return;
#endif
		_insert(pos, newnode, newnode);
		if (pos == head_)
			head_ = newnode;
	}

	node* erase(const node* pos) {
#ifdef DLOU_CHECK_ARGS
		if (!pos)
			return nullptr;
#endif
		_erase<is_front::unknow>(pos);
		return const_cast<node*>(pos);
	}

	void splice(const node* pos, doubly_linked_list&& other) {
#ifdef DLOU_CHECK_ARGS
		if (!pos)
			return;
#endif
		if (other.empty())
			return;

		node* head = other.head_;
		other.clear();
		_insert(pos, head, _prev(head));
		if (pos == head_)
			head_ = head;
	}

	void splice(const node* pos, doubly_linked_list&& other, const node* mov) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || !mov)
			return;
#endif
		other._erase<is_front::unknow>(mov);
		_insert(pos, mov, mov);
		if (pos == head_)
			head_ = const_cast<node*>(mov);
	}

	void rotate() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return;
#endif
		head_ = _next(head_);
	}

	void counter_rotate() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return;
#endif
		head_ = _prev(head_);
	}

	/*
	* if (!empty()) {
	*     iterator iter = last = begin();
	*     do {
	*         *iter;
	*     } while (++iter != last);
	* }
	*/
	iterator begin() const { return{ front() }; }
	reverse_iterator rbegin() const { return{ back() }; }

	void insert(iterator pos, node* newnode) {
#ifdef DLOU_CHECK_ARGS
		if (!newnode)
			return;
#endif
		_insert(&*pos, newnode, newnode);
		if (pos == head_)
			head_ = newnode;
	}

	template<class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		node* head = &*first;
		node* tail = &*link(first, last);
		_insert(&*pos, head, tail);
		if (pos == head_)
			head_ = head;
	}

	node* erase(iterator pos) {
		node* nod = const_cast<node*>(&*pos);
		_erase<is_front::unknow>(nod);
		return nod;
	}

	void splice(iterator pos, doubly_linked_list&& other) {
		splice(&*pos, (doubly_linked_list&&)other);
	}

	void splice(iterator pos, doubly_linked_list&& other, iterator mov) {
		splice(&*pos, (doubly_linked_list&&)other, &*mov);
	}


protected:
	node* head_;
};

} // namespace circular
} // namespace dlou
