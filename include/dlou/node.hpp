#pragma once

#include <cstddef>

namespace dlou {

template<size_t Way, class Key = void, class Balance = void>
struct node {
	struct node* n[Way];
	Key k;
	Balance b;
};

template<size_t Way, class Key>
struct node<Way, Key, void> {
	struct node* n[Way];
	Key k;
};

template<size_t Way, class Balance>
struct node<Way, void, Balance> {
	struct node* n[Way];
	Balance b;
};

template<size_t Way>
struct node<Way, void, void> {
	struct node* n[Way];
};

template<class Node>
struct node_key {
	using type = void;
};

template<size_t Way, class Balance>
struct node_key<node<Way, void, Balance>> {
	using type = void;
};

template<size_t Way, class Key, class Balance>
struct node_key<node<Way, Key, Balance>> {
	using type = Key;

	Key& operator ()(node<Way, Key, Balance>& x) const {
		return x.k;
	}

	const Key& operator ()(const node<Way, Key, Balance>& x) const {
		return x.k;
	}
};

template<class Node>
using node_key_t = typename node_key<Node>::type;

template<class Node>
struct node_balance {
	using type = void;
};

template<size_t Way, class Key>
struct node_balance<node<Way, Key, void>> {
	using type = void;
};

template<size_t Way, class Key, class Balance>
struct node_balance<node<Way, Key, Balance>> {
	using type = Balance;

	Balance& operator ()(node<Way, Key, Balance>& x) const {
		return x.b;
	}

	const Balance& operator ()(const node<Way, Key, Balance>& x) const {
		return x.b;
	}
};

template<class Node>
using node_balance_t = typename node_balance<Node>::type;

template<class Node, class Compare>
struct node_compare : private Compare {
	bool operator ()(const Node* a, const Node* b) const {
		return Compare::operator ()(a->k, b->k);
	}
};

} // namespace dlou