#pragma once

#include "../node.hpp"

#include <iterator>


namespace dlou {
namespace double_ended {
	
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

	node* _front() const {
		return _next(&end_);
	}

	node* _back() const {
		return _prev(&end_);
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

	void _insert(const node* pos, node* head, node* tail) {
		auto prev = _prev(pos);
		auto next = const_cast<node*>(pos);

		_prev(head) = prev;
		_next(tail) = next;

		_next(prev) = head;
		_prev(next) = tail;
	}

	void _erase(const node* head, const node* tail) {
		node* prev = _prev(head);
		node* next = _next(tail);
		_next(prev) = next;
		_prev(next) = prev;
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
		return &end_ == _front();
	}

	const node* front() const {
		return _front();
	}

	void push_front(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		_insert(_front(), p, p);
	}

	template<class InputIterator>
	void push_front(InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		last = rlink(first, last);
		_insert(_front(), &*last, &*first);
	}

	node* pop_front() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		auto ret = _front();
		_erase(ret, ret);
		return ret;
	}

	const node* back() const {
		return _back();
	}

	void push_back(node* p) {
#ifdef DLOU_CHECK_ARGS
		if (!p)
			return;
#endif
		_insert(after_back(), p, p);
	}

	template<class InputIterator>
	void push_back(InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif
		last = link(first, last);
		_insert(after_back(), &*first, &*last);
	}

	node* pop_back() {
#ifdef DLOU_CHECK_ARGS
		if (empty())
			return nullptr;
#endif
		auto ret = _back();
		_erase(ret, ret);
		return ret;
	}
	
	const node* before_front() const {
		return &end_;
	}

	const node* after_back() const {
		return &end_;
	}

	void insert(const node* pos, node* newnode) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || !newnode)
			return;
#endif
		_insert(pos, newnode, newnode);
	}

	template<class InputIterator>
	void insert(const node* pos, InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || first == last)
			return;
#endif
		last = link(first, last);
		_insert(pos, &*first, &*last);
	}

	node* erase(const node* pos) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || &end_ == pos)
			return nullptr;
#endif
		_erase(pos, pos);
		return const_cast<node*>(pos);
	}

	// Return doubly linked list from head to nullptr.
	//   ex: for (auto pos = ret; pos; pos = next(pos))
	node* erase(const node* first, const node* last) {
#ifdef DLOU_CHECK_ARGS
		if (!first || !last || first == last)
			return nullptr;
#endif
		node* head = const_cast<node*>(first);
		node* tail = _prev(last);

		_erase(head, tail);

		_next(tail) = nullptr;
		_prev(head) = nullptr;

		return head;
	}

	void splice(const node* pos, doubly_linked_list&& other) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || other.empty())
			return;
#endif
		node* head = other._front();
		node* tail = other._back();
		other._erase(head, tail);
		_insert(pos, head, tail);
	}

	void splice(const node* pos, doubly_linked_list&& other, const node* mov) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || other.empty() || !mov)
			return;
#endif

		node* nod = const_cast<node*>(mov);
		other._erase(nod, nod);
		_insert(pos, nod, nod);
	}

	void splice(const node* pos, doubly_linked_list&& other, const node* first, const node* last) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || first == last)
			return;
#endif

		node* tail = _prev(last);
		other._erase(first, tail);
		_insert(pos, const_cast<node*>(first), tail);
	}

	iterator begin() const { return{ _front() }; }
	iterator end() const { return{ after_back() }; }
	
	reverse_iterator rbegin() const { return{ _back() }; }
	reverse_iterator rend() const { return{ before_front() }; }

	void insert(iterator pos, node* newnode) {
#ifdef DLOU_CHECK_ARGS
		if (pos != end() || !newnode)
			return;
#endif
		_insert(&*pos, newnode, newnode);
	}

	template<class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last) {
#ifdef DLOU_CHECK_ARGS
		if (pos != end() || first == last)
			return;
#endif
		last = link(first, last);
		_insert(&*pos, &*first, &*last);
	}

	node* erase(iterator pos) {
#ifdef DLOU_CHECK_ARGS
		if (pos != end())
			return nullptr;
#endif

		node* nod = const_cast<node*>(&*pos);
		_erase(nod, nod);
		return nod;
	}

	// Return doubly linked list from head to nullptr.
	//   ex: for (auto pos = ret; pos; pos = next(pos))
	node* erase(iterator first, iterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first != last)
			return nullptr;
#endif
		node* head = const_cast<node*>(&*first);
		node* tail = _prev(&*last);

		_erase(head, tail);

		_next(tail) = nullptr;
		_prev(head) = nullptr;

		return head;
	}

	void splice(iterator pos, doubly_linked_list&& other) {
		splice(&*pos, (doubly_linked_list&&)other);
	}

	void splice(iterator pos, doubly_linked_list&& other, iterator mov) {
#ifdef DLOU_CHECK_ARGS
		if (other.end() == mov)
			return;
#endif

		auto&& nod = const_cast<node*>(&*mov);
		other._erase(nod, nod);
		_insert(&*pos, nod, nod);
	}

	void splice(iterator pos, doubly_linked_list&& other, iterator first, iterator last) {
#ifdef DLOU_CHECK_ARGS
		if (first == last)
			return;
#endif

		node* head = const_cast<node*>(&*last);
		node* tail = _prev(&*last);
		other._erase(head, tail);
		_insert(&*pos, head, tail);
	}

protected:
	node end_;
};

} // namespace double_ended

using double_ended::doubly_linked_list;

} // namespace dlou
