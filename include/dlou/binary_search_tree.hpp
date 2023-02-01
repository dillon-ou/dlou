#pragma once

#include "binary_tree.hpp"

#include <type_traits>
#include <functional>

namespace dlou {

template<class Key, class Compare = std::less<Key>, class Balance = void>
class basic_bst
	: protected binary_tree<Key, Balance>
	, private Compare
{
	using basic_type = binary_tree<Key, Balance>;
public:
	using key_type = Key;
	using key_compare = Compare;

	using typename basic_type::node;
	using typename basic_type::iterator;
	using typename basic_type::reverse_iterator;
	using typename basic_type::basic_iterator;
	using typename basic_type::basic_reverse_iterator;
	using typename basic_type::pre_order;
	using typename basic_type::in_order;
	using typename basic_type::post_order;
	using typename basic_type::level_order;
	using typename basic_type::traversal;
	using typename basic_type::reverse_traversal;

	using basic_type::parent;
	using basic_type::child;
	using basic_type::empty;
	using basic_type::level;
	using basic_type::begin;
	using basic_type::end;
	using basic_type::rbegin;
	using basic_type::rend;
	using basic_type::tbegin;
	using basic_type::tend;
	using basic_type::trbegin;
	using basic_type::trend;
	using basic_type::search;
	using basic_type::rsearch;

protected:
	using basic_type::root_;
	using basic_type::replace;
	using basic_type::swap_node;
	using basic_type::_parent;

	basic_bst(node* p) : basic_type(p) {}

	bool _fault(const node* pos) const {
		if (auto child = pos->n[0])
			if (pos != _parent(child)
				|| compare(pos->k, child->k))
				return true;

		if (auto child = pos->n[1])
			if (pos != _parent(child)
				|| compare(child->k, pos->k))
				return true;
		
		return false;
	}

	static node* _rotate(bool right, node* pos) {
		const size_t _0 = !right;
		const size_t _1 = !_0;

		node* par = pos->n[_0];
		node* sub = par->n[_1];

		par->n[_1] = pos;
		_parent(pos) = par;

		pos->n[_0] = sub;
		if (sub)
			_parent(sub) = pos;

		return par;
	}

	static node* _rebuild(node* root) {
		size_t count = 0;
		node* prv = nullptr;
		node* pos = root;
		if (pos) {
			++count;

			if (pos->n[0]) {
				do {
					pos = _rotate(true, pos);
				} while (pos->n[0]);

				_parent(pos) = prv;
				root = pos;
			}

			while (pos = (prv = pos)->n[1]) {
				++count;

				if (pos->n[0]) {
					do {
						pos = _rotate(true, pos);
					} while (pos->n[0]);

					_parent(pos) = prv;
					prv->n[1] = pos;
				}
			}
		}

		size_t width = 1;
		while (width <= count) {
			count -= width;
			width <<= 1;
		}
		--width;

		if (!count)
			count = (width >>= 1);

		while (count) {
			prv = nullptr;
			pos = root;

			pos = _rotate(false, pos);

			_parent(pos) = prv;
			root = pos;

			while (--count) {
				pos = _rotate(false, (prv = pos)->n[1]);

				_parent(pos) = prv;
				prv->n[1] = pos;
			}

			count = (width >>= 1);
		}

		return root;
	}

	bool compare(const key_type& a, const key_type& b) const {
		return key_compare::operator ()(a, b);
	}

	node* _erase(node* pos, node*&& subtree = nullptr) {
		node* parent = _parent(pos);
		node* child;
		node** branch = &root_;
		if (parent)
			branch = parent->n + (pos == parent->n[1]);

	remove:
		if (child = pos->n[0]) {
			if (pos->n[1]) {
				swap_node(pos, basic_type::_rightmost(child));
				parent = _parent(pos);
				branch = parent->n + (pos == parent->n[1]);
				goto remove;
			}

			_parent(child) = parent;
		}
		else if (child = pos->n[1])
			_parent(child) = parent;

		*branch = child;

		subtree = child;
		return parent;
	}

	void _erase_r(node* pos) {
		bool r = false;

		while (pos->n[0] && pos->n[1])
			rotate(r = !r, pos);

		auto parent = _parent(pos);

		auto child = pos->n[0];
		if (!child)
			child = pos->n[1];

		if (child)
			_parent(child) = parent;

		if (parent)
			parent->n[pos == parent->n[1]] = child;
		else
			root_ = child;
	}

public:
	basic_bst() = default;
	basic_bst(basic_bst&&) = default;

	void swap(basic_bst& x) {
		basic_type::swap(dynamic_cast<basic_type&>(x));
	}

	const node* find(const key_type& key) const {
		auto* ret = lower_bound(key);
		if (ret && compare(key, ret->k))
			ret = nullptr;
		return ret;
	}
	
	const node* lower_bound(const key_type& key) const {
		node* pos = root_;
		node* ret = nullptr;
		while (pos) {
			if (compare(pos->k, key))
				pos = pos->n[1];
			else
				pos = (ret = pos)->n[0];
		}
		return ret;
	}
	
	const node* upper_bound(const key_type& key) const {
		node* pos = root_;
		node* ret = nullptr;
		while (pos) {
			if (!compare(key, pos->k))
				pos = pos->n[1];
			else
				pos = (ret = pos)->n[0];
		}
		return ret;
	}

	const node* front() const {
		return in_order::first(const_cast<node*>(root_));
	}

	const node* back() const {
		return in_order::last(const_cast<node*>(root_));
	}

	const node* next(const node* p) const {
		return in_order::next(const_cast<node*>(p));
	}

	const node* prev(const node* p) const {
		return in_order::prev(const_cast<node*>(p));
	}

protected:
	const node* fault() const {
		node* pos = pre_order::first(const_cast<node*>(root_));
		while (pos) {
			if (_fault(pos))
				return pos;

			pos = pre_order::next(pos);
		}
		return nullptr;
	}

	// parent(pos) = child(pos, !right)
	const node* rotate(bool right, const node* pos)
	{
#ifdef DLOU_CHECK_ARGS
		if (!pos->n[!right])
			return nullptr;
#endif
		node* prv = _parent(pos);
		node* ret = _rotate(right, const_cast<node*>(pos));

		_parent(ret) = prv;
		if (prv)
			prv->n[pos == prv->n[1]] = ret;
		else
			root_ = ret;

		return ret;
	}

	void rebuild() {
		root_ = _rebuild(root_);
	}

	void insert(node* p) {
		node* pos;
		node* parent = nullptr;
		node** branch = &root_;
		bool idx;
		while (pos = *branch) {
			idx = compare(pos->k, p->k) ? 1 : 0;
			branch = pos->n + idx;
			parent = pos;
		}

		_parent(p) = parent;
		p->n[0] = p->n[1] = nullptr;
		*branch = p;
	}

	node* erase(const node* pos) {
		_erase(const_cast<node*>(pos));
		return const_cast<node*>(pos);
	}

	node* erase(const key_type& key) {
		auto p = const_cast<node*>(find(key));
		if (p)
			_erase(p);
		return p;
	}

	node* erase_rotate(const node* pos) {
		_erase_r(const_cast<node*>(pos));
		return const_cast<node*>(pos);
	}

	node* erase_rotate(const key_type& key) {
		auto p = const_cast<node*>(find(key));
		if (p)
			_erase_r(p);
		return p;
	}
}; // class basic_bst

template<class Key, class Compare = std::less<Key>>
class binary_search_tree
	: public basic_bst<Key, Compare, void>
{
	using basic_type = basic_bst<Key, Compare, void>;
public:
	using basic_type::fault;
	using basic_type::rotate;
	using basic_type::rebuild;
	using basic_type::insert;
	using basic_type::erase;
	using basic_type::erase_rotate;

public:
	binary_search_tree() = default;
	binary_search_tree(binary_search_tree&&) = default;
	void swap(binary_search_tree& x) { basic_type::swap(dynamic_cast<basic_type&>(x)); }

}; // binary_search_tree

} // namespace dlou
