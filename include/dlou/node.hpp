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

template<size_t Way>
struct node<Way, void, void> {
	struct node* n[Way];
};

template<class Node>
struct node_key {
	using type = void;
};

template<size_t Way, class Key, class Balance>
struct node_key<node<Way, Key, Balance>> {
	using type = Key;
};

template<class Node>
using node_key_t = typename node_key<Node>::type;

template<class Node>
struct node_balance {
	using type = void;
};

template<size_t Way, class Key, class Balance>
struct node_balance<node<Way, Key, Balance>> {
	using type = Balance;
};

template<class Node>
using node_balance_t = typename node_balance<Node>::type;

} // namespace dlou