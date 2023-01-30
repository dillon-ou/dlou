#pragma once

#include "binary_search_tree.hpp"

namespace dlou {
	
template<class Key, class Compare = std::less<Key>>
class splay_tree
	: public binary_search_tree<Key, Compare, void>
{
	using basic_type = binary_search_tree<Key, Compare, void>;
public:
	using typename basic_type::key_type;
	using typename basic_type::key_compare;
	using typename basic_type::node;

protected:
	using basic_type::root_;
	using basic_type::_parent;
	using basic_type::_rotate;

	splay_tree(node* p) : basic_type(p) {}

	void _splay(node* pos) {
		node* grand;
		node* parent = _parent(pos);
		bool pleft;
		bool gleft;

		while (parent) {
			pleft = pos == parent->n[0];

			node* grand = _parent(parent);
			if (!grand) {
				_rotate(pleft, parent);
				break;
			}

			node* nxt = _parent(grand);
			gleft = parent == grand->n[0];

			if (gleft == pleft)
				_rotate(gleft, _rotate(gleft, grand));
			else {
				_rotate(pleft, parent);

				if (auto* sub = grand->n[pleft] = pos->n[gleft])
					_parent(sub) = grand;

				pos->n[gleft] = grand;
				_parent(grand) = pos;
			}

			if (_parent(pos) = nxt)
				nxt->n[grand == nxt->n[1]] = pos;

			parent = nxt;
		}
	}

public:
	splay_tree() = default;
	splay_tree(splay_tree&&) = default;

	void swap(splay_tree& x) {
		basic_type::swap(dynamic_cast<basic_type&>(x));
	}

	void splay(const node* pos) {
		_splay(const_cast<node*>(pos));

		root_ = const_cast<node*>(pos);
		_parent(pos) = nullptr;
	}

	void insert(node* p) {
		basic_type::insert(p);
		splay(p);
	}
	
	void erase(const node* pos) {
		auto curr = const_cast<node*>(pos);
		_splay(curr);

		if (root_ = curr->n[0]) {
			if (auto sub = curr->n[1]) {
				_parent(sub) = nullptr;
				sub = basic_type::_leftmost(sub);
				_splay(sub);

				_parent(root_) = sub;
				sub->n[0] = root_;

				root_ = sub;
			}

			_parent(root_) = nullptr;
		}
		else if (root_ = curr->n[1])
			_parent(root_) = nullptr;
		else
			root_ = nullptr;
	}
	
	node* erase(const key_type& key) {
		node* p = basic_type::find(key);
		if (p)
			erase(p);
		return p;
	}

	const node* find_splay(const key_type& key) {
		auto* p = basic_type::find(key);
		if (p)
			splay(p);
		return p;
	}

	void join_minimum(splay_tree&& x) {
		if (!root_) {
			root_ = x.root_;
			x.root_ = nullptr;
			return;
		}
		auto root = const_cast<node*>(basic_type::leftmost());
		auto pos = basic_type::upper_bound(root->k);
		auto sub = x.split(pos);
		x.swap(sub);
		if (!sub.empty()) {
			splay(root);
			root->n[0] = sub.root_;
			_parent(sub.root_) = root;
			sub.root_ = nullptr;
		}
	}

	void join_maximum(splay_tree&& x) {
		if (!root_) {
			root_ = x.root_;
			x.root_ = nullptr;
			return;
		}
		auto root = const_cast<node*>(basic_type::rightmost());
		auto sub = x.split(root->k);
		if (!sub.empty()) {
			splay(root);
			root->n[1] = sub.root_;
			_parent(sub.root_) = root;
			sub.root_ = nullptr;
		}
	}

	splay_tree split(const node* pos) {
		auto curr = const_cast<node*>(pos);
		_splay(curr);
		if (root_ = curr->n[0])
			_parent(root_) = nullptr;
		curr->n[0] = nullptr;

		return { curr };
	}

	// return lower_bound ~ end
	splay_tree split(const key_type& key) {
		auto pos = basic_type::lower_bound(key);
		if (pos)
			return split(pos);
		return {};
	}
};

} // namespace dlou
