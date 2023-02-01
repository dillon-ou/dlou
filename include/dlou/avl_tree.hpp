#pragma once

#include "binary_search_tree.hpp"

namespace dlou {

template<class Key, class Compare = std::less<Key>>
class avl_tree
	: public basic_bst<Key, Compare, unsigned char>
{
	using basic_type = basic_bst<Key, Compare, unsigned char>;
public:
	using typename basic_type::key_type;
	using typename basic_type::node;

	using height_type = node_balance_t<node>;
	using balance_type = std::make_signed_t<height_type>;

protected:
	using basic_type::root_;
	using basic_type::_parent;
	using basic_type::_rotate;
	using basic_type::compare;

protected:
	static height_type height(node* p) { return p ? p->b : 0; }
	static height_type max_hei(height_type a, height_type b) { return a > b ? a : b; }
	static void reset_hei(node* p) { p->b = 1 + max_hei(height(p->n[0]), height(p->n[1])); }

	node* rotate(bool right, node* pos) {
		const size_t _0 = false == right;
		const size_t _1 = !_0;
		
		node* parent = _parent(pos);
		node** branch = parent ? (pos == parent->n[0] ? parent->n : (parent->n + 1)) : &root_;
		node* child = pos->n[_0];

		if (height(child->n[_0]) < height(child->n[_1])) {
			auto top = _rotate(!right, child);
			pos->n[_0] = top;
			_parent(top) = pos;
			reset_hei(child);
		}

		child = pos;
		pos = _rotate(right, child);
		*branch = pos;
		_parent(pos) = parent;
		reset_hei(child);
		reset_hei(pos);

		return pos;
	}

protected:
	bool _fault(const node* pos) const {
		auto h0 = height(pos->n[0]);
		auto h1 = height(pos->n[1]);
		if (pos->b != 1 + max_hei(h0, h1))
			return true;

		balance_type b = h0 - h1;
		if (-1 > b || b > 1)
			return true;

		if (auto child = pos->n[0]) {
			if (pos != _parent(child) || compare(pos->k, child->k))
				return true;
		}

		if (auto child = pos->n[1]) {
			if (pos != _parent(child) || compare(child->k, pos->k))
				return true;
		}

		return false;
	}

public:
	avl_tree() = default;
	avl_tree(avl_tree&&) = default;

	void swap(avl_tree& x) {
		basic_type::swap(dynamic_cast<basic_type&>(x));
	}

	const node* fault() const {
		using order = typename basic_type::pre_order;

		node* pos = order::first(const_cast<node*>(root_));
		while (pos) {
			if (_fault(pos))
				return pos;

			pos = order::next(pos);
		}
		return nullptr;
	}

	void insert(node* p) {
		p->b = 1;

		basic_type::insert(p);
		node* pos = _parent(p);

		while (pos) {
			balance_type balance = height(pos->n[0]) - height(pos->n[1]);
			if (0 == balance)
				break;
			if (1 & ~balance) {
				pos = rotate(0 <= balance, pos);
				break;
			}

			++pos->b;
			pos = _parent(pos);
		}
	}

	node* erase(const node* pos) {
		node* parent = basic_type::_erase(const_cast<node*>(pos));

		while (parent) {
			height_type h0 = height(parent->n[0]);
			height_type h1 = height(parent->n[1]);
			balance_type balance = h0 - h1;
			if (1 & balance)
				break;
			if (balance) {
				h0 = parent->b;
				parent = rotate(0 <= balance, parent);
				if (h0 == parent->b)
					break;
			}
			else
				--parent->b;

			parent = _parent(parent);
		}

		return const_cast<node*>(pos);
	}
	
	node* erase(const key_type& key) {
		auto p = find(key);
		if (p)
			erase(p);
		return p;
	}
};


} // namespace dlou
