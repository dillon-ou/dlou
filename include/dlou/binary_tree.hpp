#pragma once

#include "node.hpp"

namespace dlou {

template<class Key = void, class Balance = void>
class binary_tree
{
public:
	using node = node<3, Key, Balance>;

protected:
	static constexpr node*& _parent(const node* p) {
		return const_cast<node*>(p)->n[2];
	}

private:
	template<bool Reverse>
	class _traversal {
	public:
		static constexpr size_t _0 = false != Reverse;
		static constexpr size_t _1 = !_0;

		static node* leftmost(node* root) {
			node* pos = nullptr;
			node* nxt = root;
			while (nxt) {
				pos = nxt;
				nxt = pos->n[_0];
			}
			return pos;
		}

		struct pre_order {
			static node* first(node* root) {
				return root;
			}

			static node* next(node* pos) {
				node* nxt = pos->n[_0];
				if (nxt)
					return nxt;

				while (!(nxt = pos->n[_1])) {
					for (;;) {
						nxt = _parent(pos);
						if (!nxt)
							return nullptr;

						if (pos == nxt->n[_0]) {
							pos = nxt;
							break;
						}

						pos = nxt;
					}
				}
				return nxt;
			}
		}; // struct pre_order

		struct in_order {
			static node* first(node* root) {
				return leftmost(root);
			}

			static node* next(node* pos) {
				node* nxt = pos->n[_1];
				if (nxt)
					return leftmost(nxt);

				while (nxt = _parent(pos)) {
					if (pos == nxt->n[_0])
						return nxt;
					pos = nxt;
				}
				return nullptr;
			}
		}; // struct in_order

		struct post_order {
			static node* first(node* root) {
				node* pos = nullptr;
				node* nxt = root;
				while (nxt) {
					pos = nxt;
					nxt = pos->n[_0];
					if (!nxt)
						nxt = pos->n[_1];
				}
				return pos;
			}

			static node* next(node* pos) {
				node* nxt = _parent(pos);
				if (nxt && pos == nxt->n[_0]) {
					pos = nxt->n[_1];
					if (pos)
						return first(pos);
				}
				return nxt;
			}
		}; // struct post_order

		struct level_order {
			static constexpr ptrdiff_t offset = Reverse ? -1 : 1;

			struct _post_order {
				static node* deepest_first(node* root, size_t& depth) {
					node* pos = nullptr;
					node* nxt = root;
					for (;;) {
						pos = nxt;
						if (nxt = pos->n[pos->n[_0] ? _0 : _1])
							++depth;
						else
							return pos;
					}
				}

				static node* deepest_next_leaf(node* root, node* pos, size_t& depth) {
					while (root != pos) {
						node* nxt = _parent(pos);
						--depth;

						if (pos == nxt->n[_0])
							if (pos = nxt->n[_1])
								return deepest_first(pos, ++depth);
						
						pos = nxt;
					}
					return nullptr;
				}

				static node* first(node* root, size_t depth) {
					node* pos = nullptr;
					node* nxt = root;
					while (depth) {
						pos = nxt;
						if (nxt = pos->n[pos->n[_0] ? _0 : _1])
							--depth;
						else {
							for (;;) {
								if (root == pos)
									return nullptr;

								nxt = _parent(pos);
								++depth;

								if (pos == nxt->n[_0]) {
									if (pos = nxt->n[_1]) {
										--depth;
										nxt = pos;
										break;
									}
								}
								pos = nxt;
							}
						}
					}
					return nxt;
				}
			}; // struct _post_order

			static size_t level(node*& root) {
				if (!root)
					return 0;

				size_t depth = 1;
				node* left = _post_order::deepest_first(root, depth);
				size_t maxdep = depth;

				node* pos = left;
				while (pos = _post_order::deepest_next_leaf(root, pos, depth)) {
					if (maxdep < depth) {
						maxdep = depth;
						left = pos;
					}
				}

				root = left;
				return maxdep;
			}

			static node* next(node* pos) {
				size_t depth = 0;
				while (node* nxt = _parent(pos)) {
					++depth;

					if (pos == nxt->n[_0]) {
						if (pos = nxt->n[_1])
							if (pos = _post_order::first(pos, depth - 1))
								return pos;
					}
					pos = nxt;
				}

				if constexpr (offset < 0) {
					if (!depth)
						return nullptr;
				}
				
				return _post_order::first(pos, depth += offset);
			}
		}; // struct level_order
	};

public:
	struct pre_order {
		static node* first(node* root) { return _traversal<false>::pre_order::first(root); }
		static node* last(node* root) { return _traversal<true>::post_order::first(root); }
		static node* next(node* pos) { return _traversal<false>::pre_order::next(pos); }
		static node* prev(node* pos) { return _traversal<true>::post_order::next(pos); }
	};

	struct in_order {
		static node* first(node* root) { return _traversal<false>::in_order::first(root); }
		static node* last(node* root) { return _traversal<true>::in_order::first(root); }
		static node* next(node* pos) { return _traversal<false>::in_order::next(pos); }
		static node* prev(node* pos) { return _traversal<true>::in_order::next(pos); }
	};

	struct post_order {
		static node* first(node* root) { return _traversal<false>::post_order::first(root); }
		static node* last(node* root) { return _traversal<true>::pre_order::first(root); }
		static node* next(node* pos) { return _traversal<false>::post_order::next(pos); }
		static node* prev(node* pos) { return _traversal<true>::pre_order::next(pos); }
	};

	struct level_order {
		static node* first(node* root) { return root; }
		static node* last(node* root) { _traversal<true>::level_order::level(root); return root; }
		static node* next(node* pos) { return _traversal<false>::level_order::next(pos); }
		static node* prev(node* pos) { return _traversal<true>::level_order::next(pos); }
	};
	
protected:
	static node* _leftmost(node* pos) { return _traversal<false>::leftmost(pos); }
	static node* _rightmost(node* pos) { return _traversal<true>::leftmost(pos); }
	
	static bool _fault(const node* pos) {
		if (auto sub = pos->n[0]) {
			if (pos != _parent(sub))
				return true;
		}
		if (auto sub = pos->n[1]) {
			if (pos != _parent(sub))
				return true;
		}
		return false;
	}

protected:
	binary_tree(node* p) : root_(p) {}
	binary_tree(binary_tree&) = default;
	binary_tree& operator =(binary_tree&) = default;

public:
	static constexpr node*& parent(node* p) {
		return _parent(p);
	}

	static constexpr const node* parent(const node* p) {
		return _parent(p);
	}

	static constexpr node*& child(node* p, bool right) {
		return p->n[false != right];
	}

	static constexpr const node* child(const node* p, bool right) {
		return p->n[false != right];
	}

public:
	binary_tree() : root_(nullptr) {}
	binary_tree(binary_tree&& x) : root_(x.root_) { x.root_ = nullptr; }
	//binary_tree& operator =(binary_tree&& x) { root_ = x.root_; x.root_ = nullptr; return *this; }

	const node* leftmost() const { return _leftmost(const_cast<node*>(root_)); }
	const node* rightmost() const { return _rightmost(const_cast<node*>(root_)); }
	const node* root() const { return root_; }
	bool empty() const { return !root_; }

	void swap(binary_tree& x) {
		auto tmp = x.root_;
		x.root_ = root_;
		root_ = tmp;
	}

	size_t level() const {
		node* tmp = const_cast<node*>(root_);
		return _traversal<true>::level_order::level(tmp);
	}

	const node* fault() const {
		node* pos = pre_order::first(const_cast<node*>(root_));
		while (pos) {
			if (_fault(pos))
				return pos;

			pos = pre_order::next(pos);
		}
		return nullptr;
	}

	node* replace(const node* pos, node* nod) {
		if constexpr (!std::is_same_v<void, Balance>)
			nod->b = pos->b;

		if (_parent(nod) = _parent(pos))
			_parent(nod)->n[pos == _parent(nod)->n[1]] = nod;
		else
			root_ = nod;

		if (nod->n[0] = pos->n[0])
			_parent(nod->n[0]) = nod;

		if (nod->n[1] = pos->n[1])
			_parent(nod->n[1]) = nod;

		return const_cast<node*>(pos);
	}

	void swap_node(const node* a, const node* b) {
		node* nod[] = {
			const_cast<node*>(a),
			const_cast<node*>(b)
		};
		node* tmp;

		tmp = _parent(nod[0]);
		_parent(nod[0]) = _parent(nod[1]);
		if (tmp)
			tmp->n[nod[0] == tmp->n[1]] = nod[1];
		else
			root_ = nod[1];

		_parent(nod[1]) = tmp;
		if (tmp = _parent(nod[0]))
			tmp->n[nod[1] == tmp->n[1]] = nod[0];
		else
			root_ = nod[0];

		tmp = nod[0]->n[0];
		nod[0]->n[0] = nod[1]->n[0];
		nod[1]->n[0] = tmp;

		tmp = nod[0]->n[1];
		nod[0]->n[1] = nod[1]->n[1];
		nod[1]->n[1] = tmp;

		if (tmp = nod[0]->n[0])
			_parent(tmp) = nod[0];
		if (tmp = nod[0]->n[1])
			_parent(tmp) = nod[0];

		if (tmp = nod[1]->n[0])
			_parent(tmp) = nod[1];
		if (tmp = nod[1]->n[1])
			_parent(tmp) = nod[1];

		if constexpr (!std::is_same_v<void, Balance>) {
			auto t = nod[0]->b;
			nod[0]->b = nod[1]->b;
			nod[1]->b = t;
		}
	}

	// child(newnode, right_child) = pos
	void insert(const node* pos, node* newnode, bool right_child) {
		// newnode->n[_0] = pos
		const size_t _0 = false != right_child;
		const size_t _1 = !_0;
#ifdef DLOU_CHECK_ARGS
		if (!pos || !newnode)
			return;
#endif
		node* nxt = const_cast<node*>(pos);
		node* prv = _parent(nxt);

		_parent(newnode) = prv;
		newnode->n[_0] = nxt;
		newnode->n[_1] = nullptr;

		_parent(nxt) = newnode;
		if (prv)
			prv->n[pos == prv->n[1]] = newnode;
		else
			root_ = newnode;
	}

	// child(newnode, right_child) = old_root
	void insert_root(node* newnode, bool right_child) {
		// newnode->n[_0] = root_
		const size_t _0 = false != right_child;
		const size_t _1 = !_0;
#ifdef DLOU_CHECK_ARGS
		if (!newnode)
			return;
#endif
		_parent(newnode) = nullptr;
		newnode->n[_0] = root_;
		newnode->n[_1] = nullptr;

		if (root_)
			_parent(root_) = newnode;
		root_ = newnode;
	}

	// child(pos, right) = newnode
	void insert_after(const node* pos, node* newnode, bool right) {
		// pos->n[_0] = newnode
		const size_t _0 = false != right;
		const size_t _1 = !_0;
#ifdef DLOU_CHECK_ARGS
		if (!pos || !newnode)
			return;
#endif
		node* prv = const_cast<node*>(pos);
		node* nxt = prv->n[_0];

		_parent(newnode) = prv;
		newnode->n[_0] = nxt;
		newnode->n[_1] = nullptr;

		prv->n[_0] = newnode;
		if (nxt)
			_parent(nxt) = newnode;
	}

	// child(parent(pos), pos_branch) = child(pos, !and_right)
	node* erase(const node* pos, bool and_right) {
		// erase n[_0]
		const size_t _0 = false != and_right;
		const size_t _1 = !_0;
#ifdef DLOU_CHECK_ARGS
		if (!pos)
			return nullptr;
#endif
		node* ret = const_cast<node*>(pos);
		node* prv = _parent(ret);
		node* nxt = ret->n[_1];

		if (prv)
			prv->n[pos == prv->n[1]] = nxt;
		else
			root_ = nxt;

		if (nxt)
			_parent(nxt) = prv;

#ifdef DLOU_CHECK_ARGS
		_parent(ret) = nullptr;
		ret->n[_1] = nullptr;
#endif
		return ret;
	}

	// erase leaf
	node* erase(const node* pos) {
#ifdef DLOU_CHECK_ARGS
		if (!pos || pos->n[0] || pos->n[1])
			return nullptr;
#endif
		node* ret = const_cast<node*>(pos);
		node* prv = _parent(ret);

		if (prv)
			prv->n[pos == prv->n[1]] = nullptr;
		else
			root_ = nullptr;

		return ret;
	}

	// child(pos, right) = nullptr
	node* erase_subtree(const node* pos, bool right) {
		const size_t _1 = false != right;

		node* prv = const_cast<node*>(pos);
		node* ret = prv->n[_1];
		prv->n[_1] = nullptr;

#ifdef DLOU_CHECK_ARGS
		_parent(ret) = nullptr;
#endif
		return ret;
	}

public:
	template<class Order>
	class basic_iterator {
		friend class binary_tree;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const node;
		using pointer = const node*;
		using reference = const node&;

	protected:
		node* pos_;
		
		basic_iterator(pointer p)
			: pos_(const_cast<node*>(p)) {
		}
		
	public:
		basic_iterator() = default;
		basic_iterator(const basic_iterator&) = default;
		basic_iterator& operator =(const basic_iterator&) = default;

		template<class Other> basic_iterator(const basic_iterator<Other>& x) : basic_iterator(&*x) {}
		template<class Other> basic_iterator& operator =(const basic_iterator<Other>& x) : basic_iterator(&*x) {}
		
		bool operator ==(const basic_iterator& x) const { return pos_ == x.pos_; }
		bool operator !=(const basic_iterator& x) const { return pos_ != x.pos_; }

		reference operator *() const { return *pos_; }
		pointer operator ->() const { return pos_; }

		basic_iterator& operator ++() { pos_ = Order::next(pos_); return *this; }
		basic_iterator operator ++(int) { auto tmp = *this; ++*this; return tmp; }
		basic_iterator& operator --() { pos_ = Order::prev(pos_); return *this; }
		basic_iterator operator --(int) { auto tmp = *this; --*this; return tmp; }
	};
	
	template<class Order>
	class basic_reverse_iterator {
		friend class binary_tree;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const node;
		using pointer = const node*;
		using reference = const node&;

	protected:
		node* pos_;
		
		basic_reverse_iterator(pointer p)
			: pos_(const_cast<node*>(p)) {
		}
		
	public:
		basic_reverse_iterator() = default;
		basic_reverse_iterator(const basic_reverse_iterator&) = default;
		basic_reverse_iterator& operator =(const basic_reverse_iterator&) = default;

		template<class Other> basic_reverse_iterator(const basic_reverse_iterator<Other>&x) : basic_reverse_iterator(&*x) {}
		template<class Other> basic_reverse_iterator& operator =(const basic_reverse_iterator<Other>&x) : basic_reverse_iterator(&*x) {}

		template<class Other> explicit operator basic_iterator<Other>() const { return make_iterator<Other>(pos_); }
		
		bool operator ==(const basic_reverse_iterator& x) const { return pos_ == x.pos_; }
		bool operator !=(const basic_reverse_iterator& x) const { return pos_ != x.pos_; }

		reference operator *() const { return *pos_; }
		pointer operator ->() const { return pos_; }

		basic_reverse_iterator& operator ++() { pos_ = Order::prev(pos_); return *this; }
		basic_reverse_iterator operator ++(int) { auto tmp = *this; ++*this; return tmp; }
		basic_reverse_iterator& operator --() { pos_ = Order::next(pos_); return *this; }
		basic_reverse_iterator operator --(int) { auto tmp = *this; --*this; return tmp; }
	};
	
	using default_traversal = in_order;
	using iterator = basic_iterator<default_traversal>;
	using reverse_iterator = basic_reverse_iterator<default_traversal>;
	
	// in_order
	auto begin() const { return tbegin<default_traversal>(); }
	auto end() const { return tend<default_traversal>(); }
	auto rbegin() const { return trbegin<default_traversal>(); }
	auto rend() const { return trend<default_traversal>(); }
	
	template<class Order>
	basic_iterator<Order> tbegin() const { return basic_iterator<Order>(Order::first(root_)); }
	
	template<class Order>
	basic_iterator<Order> tend() const { return basic_iterator<Order>(nullptr); }
	
	template<class Order>
	basic_reverse_iterator<Order> trbegin() const { return basic_reverse_iterator<Order>(Order::last(root_)); }
	
	template<class Order>
	basic_reverse_iterator<Order> trend() const { return basic_reverse_iterator<Order>(nullptr); }

	template<class Order>
	class traversal {
		friend class binary_tree;
		traversal(const binary_tree* p) : btree_(p) {}
	public:
		traversal(const traversal&) = default;
		traversal& operator =(const traversal&) = default;
		bool operator ==(const traversal&) const = default;
		bool operator !=(const traversal&) const = default;

		auto begin() const { return btree_->tbegin<Order>(); }
		auto end() const { return btree_->tend<Order>(); }
	private:
		const binary_tree* btree_;
	};

	template<class Order>
	class reverse_traversal {
		friend class binary_tree;
		reverse_traversal(const binary_tree* p) : btree_(p) {}
	public:
		reverse_traversal(const reverse_traversal&) = default;
		reverse_traversal& operator =(const reverse_traversal&) = default;
		bool operator ==(const reverse_traversal&) const = default;
		bool operator !=(const reverse_traversal&) const = default;

		auto begin() const { return btree_->trbegin<Order>(); }
		auto end() const { return btree_->trend<Order>(); }
	private:
		const binary_tree* btree_;
	};
	
	template<class Order = default_traversal>
	auto search() const { return traversal<Order>(this); }

	template<class Order = default_traversal>
	auto rsearch() const { return reverse_traversal<Order>(this); }

protected:
	template<class Order = default_traversal>
	static basic_iterator<Order> make_iterator(const node* p) { return basic_iterator<Order>(p); }

	template<class Order = default_traversal>
	static basic_iterator<Order> make_riterator(const node* p) { return basic_reverse_iterator<Order>(p); }

protected:
	node* root_;
}; // class binary_tree

} // namespace dlou
