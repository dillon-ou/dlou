#pragma once

#include "binary_search_tree.hpp"

#include <utility>
#include <random>

namespace dlou {

template<class Key, class Compare = std::less<Key>, class Random = std::random_device>
class treap
	: public basic_bst<Key, Compare, unsigned>
{
	using basic_type = basic_bst<Key, Compare, unsigned>;
public:
	using typename basic_type::key_type;
	using typename basic_type::node;

	using random_engine = Random;
	using priority_type = node_balance_t<node>;

protected:
	using basic_type::root_;
	using basic_type::_parent;
	using basic_type::_rotate;
	using basic_type::compare;

protected:
	priority_type make_priority() {
		std::uniform_int_distribution<priority_type> dist;
		return dist(rand_);
	}
	random_engine rand_;

	static priority_type& priority(const node* p) { return const_cast<priority_type&>(p->b); }

protected:
	bool _fault(const node* pos) const {
		const auto pri = priority(pos);

		if (auto child = pos->n[0]) {
			if (pos != _parent(child)
				|| compare(pos->k, child->k)
				|| pri >= priority(child))
				return true;
		}

		if (auto child = pos->n[1]) {
			if (pos != _parent(child)
				|| compare(child->k, pos->k)
				|| pri >= priority(child))
				return true;
		}

		return false;
	}

public:
	treap() = default;
	treap(treap&&) = default;

	template<class... Types>
	treap(Types&&... args)
		: rand_(std::forward<Types>(args)...) {
	}

	void swap(treap& x) {
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
		priority_type pri = priority(p) = make_priority();

		basic_type::insert(p);

		while (node* parent = _parent(p)) {
			if (pri >= priority(parent))
				break;

			basic_type::rotate(p != parent->n[1], parent);
		}
	}

	node* erase(const node* pos) {
		node* child;

		for (;;) {
			if (child = pos->n[0]) {
				if (pos->n[1]) {
					basic_type::rotate(priority(child) <= priority(pos->n[1]), pos);
					continue;
				}
			}
			else
				child = pos->n[1];
			break;
		}

		auto parent = _parent(pos);

		if (child)
			_parent(child) = parent;

		if (parent)
			parent->n[pos == parent->n[1]] = child;
		else
			root_ = child;

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
