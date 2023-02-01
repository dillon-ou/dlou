#pragma once

#include "binary_search_tree.hpp"

#include <utility>

namespace dlou {

template<class Key, class Compare = std::less<Key>>
class red_black_tree
	: public basic_bst<Key, Compare, bool>
{
	using basic_type = basic_bst<Key, Compare, bool>;
public:
	using typename basic_type::key_type;
	using typename basic_type::node;

	using color_type = node_balance_t<node>;
	static constexpr color_type black = 0;
	static constexpr color_type red = 1;

protected:
	using basic_type::root_;
	using basic_type::_parent;
	using basic_type::_rotate;
	using basic_type::compare;

protected:
	static color_type color(const node* p) { return p->b; }
	static color_type& color(node* p) { return p->b; }

	static bool is_red(const node* p) {
		static_assert(red == true, "Check color value");
		return p ? color(p) : black;
	}

	size_t _fault(node* pos) const {
		if (!pos)
			return 0;

		if (red == color(pos) && (is_red(pos->n[0]) | is_red(pos->n[1])))
			throw(pos);

		node* child;
		child = pos->n[0];
		if (child && (pos != _parent(child) || compare(pos->k, child->k)))
			throw(pos);

		child = pos->n[1];
		if (child && (pos != _parent(child) || compare(child->k, pos->k)))
			throw(pos);

		size_t n = _fault(pos->n[0]);
		if (n != _fault(pos->n[1]))
			throw(pos);

		return size_t(black == color(pos)) + n;
	}

	void _erase(node* pos) {
		node* curr;
		node* parent = basic_type::_erase(const_cast<node*>(pos), std::move(curr));

		if (red == color(pos))
			return;

		while (parent) {
			if (is_red(curr)) {
				color(curr) = black;
				return;
			}

			bool right = curr == parent->n[1];
			node* sibling = parent->n[!right];

			if (black != color(sibling)) {
				color(parent) = red;
				color(sibling) = black;
				basic_type::rotate(right, parent);
				sibling = parent->n[!right];
			}

			node* child = sibling->n[!right];
			if (is_red(child)) {
				color(sibling) = color(parent);
				color(parent) = black;
				color(child) = black;
				basic_type::rotate(right, parent);
				return;
			}

			child = sibling->n[right];
			if (is_red(child)) {
				//color(sibling) = red;
				color(child) = black;
				child = _rotate(!right, sibling);
				parent->n[!right] = child;
				_parent(child) = parent;

				color(child) = color(parent);
				color(parent) = black;
				color(sibling) = black;
				basic_type::rotate(right, parent);
				return;
			}

			color(sibling) = red;
			curr = parent;
			parent = _parent(curr);
		}

		if (root_)
			color(root_) = black;
	}

public:
	red_black_tree() = default;
	red_black_tree(red_black_tree&& x) = default;

	void swap(red_black_tree& x) {
		basic_type::swap(dynamic_cast<basic_type&>(x));
	}

	const node* fault() const {
		if (!root_)
			return nullptr;
		if (_parent(root_) || red == color(root_))
			return root_;
		try {
			_fault(root_);
		}
		catch (node* p) {
			return p;
		}
		return nullptr;
	}

	void insert(node* p) {
		node* pos;
		node* parent = nullptr;
		node** branch = &root_;
		bool i, j;
		while (pos = *branch) {
			i = false != compare(pos->k, p->k);
			branch = pos->n + i;
			parent = pos;
		}

		_parent(p) = parent;
		p->n[0] = p->n[1] = nullptr;
		color(p) = red;
		*branch = p;

		node* grand;
		node* uncle;

		pos = p;
		while (parent) {
			if (black == color(parent))
				break;

			grand = _parent(parent);
			j = parent == grand->n[1];
			uncle = grand->n[!j];

			if (!is_red(uncle)) {
				i = pos == parent->n[1];
				if (j != i) {
					parent = _rotate(!i, parent);
					_parent(parent) = grand;
					grand->n[j] = parent;
				}

				color(parent) = black;
				color(grand) = red;

				parent = _parent(grand);
				branch = parent
					? parent->n + (grand == parent->n[1])
					: &root_;
				pos = _rotate(!j, grand);
				_parent(pos) = parent;
				*branch = pos;
				return;
			}

			color(uncle) = black;
			color(parent) = black;
			color(grand) = red;

			pos = grand;
			parent = _parent(grand);
		}

		color(root_) = black;
	}
	
	node* erase(const node* pos) {
		_erase(const_cast<node*>(pos));
		return const_cast<node*>(pos);
	}
	
	node* erase(const key_type& key) {
		auto p = basic_type::find(key);
		if (p)
			erase(p);
		return p;
	}
};


} // namespace dlou
