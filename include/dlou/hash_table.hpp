#pragma once

#include "integer.hpp"

#include <cstddef>

#include <type_traits>
#include <functional>


namespace dlou {

template<size_t N, class Key, class Val = void, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, size_t Bucket = base2::ceil<size_t>(N * 2)>
DLOU_REQUIRES(N > 0 && Bucket > 0)
class chained_hash_table
{
	DLOU_REQUIRES_IN(N > 0 && Bucket > 0);

	template<size_t N, class, class, class, class, size_t Bucket>
	DLOU_REQUIRES(N > 0 && Bucket > 0)
	friend class chained_hash_table;

public:
	using key_type = Key;
	using mapped_type = typename std::conditional<std::is_same<void, Val>::value, Key, Val>::type;
	using hasher = Hash;
	using key_equal = Pred;
	using value_type = typename std::conditional<std::is_same<void, Val>::value, key_type, std::pair<key_type, mapped_type>>::type;
	using reference = const value_type&;
	using pointer = const value_type*;

private:
	using nodepos = uinteger_type<(base2::log_ceil(N + 1 /* invalid_pos */) + 7U) / 8U>;
	static constexpr nodepos invalid_pos = ~nodepos(0);

	typedef struct {
		nodepos next;
		value_type value;
	} node;

public:
	constexpr chained_hash_table(const chained_hash_table&) = default;
	constexpr chained_hash_table& operator =(const chained_hash_table&) = default;

	constexpr chained_hash_table(const value_type(&x)[N]) {
		clone(x);
	}

	constexpr chained_hash_table& operator =(const value_type(&x)[N]) {
		clone(x);
		return *this;
	}

	template<class OldHash, class OldPred, size_t OldSlots>
	constexpr chained_hash_table(const chained_hash_table<N, Key, Val, OldHash, OldPred, OldSlots>& x) {
		clone(x.data_);
	}

	template<class OldHash, class OldPred, size_t OldSlots>
	constexpr chained_hash_table& operator =(const chained_hash_table<N, Key, Val, OldHash, OldPred, OldSlots>& x) {
		clone(x.data_);
		return *this;
	}

	constexpr size_t size() const { return N; }

	constexpr const mapped_type& operator [](const key_type& k) const {
#ifdef DLOU_CHECK_ARGS
		return at(k);
#else
		return get_val(*find(k));
#endif
	}

	constexpr const mapped_type& at(const key_type& k, const mapped_type& invalid = mapped_type{}) const {
		auto ret = find(k);
		if (ret)
			return get_val(*ret);
		return invalid;
	}

	constexpr pointer find(const key_type& k) const {
		key_equal eq;

		size_t idx = hasher{}(k);
		auto pos = slot_[idx % Bucket];

		while (invalid_pos != pos) {
			const node& n = data_[pos];
			if (eq(get_key(n.value), k))
				return &n.value;
			pos = n.next;
		}
		return nullptr;
	}

private:
	static constexpr const key_type& get_key(const value_type& v) {
		if constexpr (std::is_same<void, Val>::value)
			return v;
		else
			return v.first;
	}
	static constexpr const mapped_type& get_val(const value_type& v) {
		if constexpr (std::is_same<void, Val>::value)
			return v;
		else
			return v.second;
	}
	static constexpr const value_type& get_pair(const value_type* p) {
		return *p;
	}
	static constexpr const value_type* get_pair(const node* p) {
		return p->value;
	}

	template<class T>
	constexpr void clone(const T* p) {
		hasher hash;

		for (auto& v : slot_)
			v = invalid_pos;

		for (auto& n : data_) {
			n.value = get_pair(p++);

			size_t idx = hash(get_key(n.value));
			auto& pos = slot_[idx % Bucket];
			n.next = pos;
			pos = static_cast<nodepos>(&n - data_);
		}
	}

private:
	nodepos slot_[Bucket];
	node data_[N];
};

// Quadratic Probing
template<size_t N, class Key, class Val = void, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, size_t Slots = N * 2>
DLOU_REQUIRES(N > 0 && Slots >= N)
class hash_table
{
	DLOU_REQUIRES_IN(N > 0 && Slots >= N);

public:
	static const size_t slot_count = base2::ceil(Slots);
	using key_type = Key;
	using mapped_type = typename std::conditional<std::is_same<void, Val>::value, Key, Val>::type;
	using hasher = Hash;
	using key_equal = Pred;
	using value_type = typename std::conditional<std::is_same<void, Val>::value, key_type, std::pair<key_type, mapped_type>>::type;
	using reference = const value_type&;
	using pointer = const value_type*;

private:
	using nodepos = uinteger_type<(base2::log_ceil(N + 1 /* invalid_pos */) + 7U) / 8U>;
	static constexpr nodepos invalid_pos = ~nodepos(0);

public:
	constexpr hash_table(const hash_table&) = default;
	constexpr hash_table& operator =(const hash_table&) = default;

	constexpr hash_table(const value_type(&x)[N]) {
		clone(x);
	}

	constexpr hash_table& operator =(const value_type(&x)[N]) {
		clone(x);
		return *this;
	}

	template<class OldHash, class OldPred, size_t OldSlots>
	constexpr hash_table(const hash_table<N, Key, Val, OldHash, OldPred, OldSlots>& x) {
		clone(x.data());
	}

	template<class OldHash, class OldPred, size_t OldSlots>
	constexpr hash_table& operator =(const hash_table<N, Key, Val, OldHash, OldPred, OldSlots>& x) {
		clone(x.data());
		return *this;
	}

	constexpr size_t size() const { return N; }
	constexpr pointer data() const { return arr_; }
	constexpr pointer begin() const { return arr_; }
	constexpr pointer end() const { return arr_ + N; }

	constexpr const mapped_type& operator [](const key_type& k) const {
#ifdef DLOU_CHECK_ARGS
		return at(k);
#else
		return get_val(*find(k));
#endif
	}

	constexpr const mapped_type& at(const key_type& k, const mapped_type& invalid = mapped_type{}) const {
		auto ret = find(k);
		if (ret)
			return get_val(*ret);
		return invalid;
	}

	constexpr pointer find(const key_type& k) const {
		hasher hash;
		key_equal eq;

		auto idx = hash(k);
		for (size_t i = 1; i <= slot_count; ++i) {
			idx %= slot_count;
			auto pos = slot_[idx];
			if (invalid_pos == pos)
				break;
			auto ptr = arr_ + pos;
			if (eq(k, get_key(*ptr)))
				return ptr;
			idx += i;
		}
		return end();
	}

private:
	static constexpr const key_type& get_key(const value_type& v) {
		if constexpr (std::is_same<void, Val>::value)
			return v;
		else
			return v.first;
	}
	static constexpr const mapped_type& get_val(const value_type& v) {
		if constexpr (std::is_same<void, Val>::value)
			return v;
		else
			return v.second;
	}

	constexpr void clone(const value_type* p) {
		hasher hash;
		key_equal eq;

		for (auto& v : slot_)
			v = invalid_pos;

		for (auto& v : arr_) {
			v = *p++;

			auto idx = hash(get_key(v));
			for (size_t i = 1; i <= slot_count; ++i) {
				idx %= slot_count;
				auto& pos = slot_[idx];
				if (invalid_pos == pos) {
					pos = static_cast<nodepos>(&v - arr_);
					break;
				}
				idx += i;
			}
		}
	}

private:
	nodepos slot_[slot_count];
	value_type arr_[N];
};

// Double Hashing
template<size_t N, class Key, class Val = void, class Hash = std::hash<Key>, class Hash2 = std::hash<Key>, class Pred = std::equal_to<Key>, size_t Slots = N * 2>
DLOU_REQUIRES(N > 0 && Slots >= N)
class dhash_table
{
	DLOU_REQUIRES_IN(N > 0 && Slots >= N);

public:
	static const size_t slot_count = base2::ceil(Slots);
	using key_type = Key;
	using mapped_type = typename std::conditional<std::is_same<void, Val>::value, Key, Val>::type;
	using hasher = Hash;
	using hasher2 = Hash2;
	using key_equal = Pred;
	using value_type = typename std::conditional<std::is_same<void, Val>::value, key_type, std::pair<key_type, mapped_type>>::type;
	using reference = const value_type&;
	using pointer = const value_type*;

private:
	using nodepos = uinteger_type<(base2::log_ceil(N + 1 /* invalid_pos */) + 7U) / 8U>;
	static constexpr nodepos invalid_pos = ~nodepos(0);

public:
	constexpr dhash_table(const dhash_table&) = default;
	constexpr dhash_table& operator =(const dhash_table&) = default;

	constexpr dhash_table(const value_type(&x)[N]) {
		clone(x);
	}

	constexpr dhash_table& operator =(const value_type(&x)[N]) {
		clone(x);
		return *this;
	}

	template<class OldHash, class OldHash2, class OldPred, size_t OldSlots>
	constexpr dhash_table(const dhash_table<N, Key, Val, OldHash, OldHash2, OldPred, OldSlots>& x) {
		clone(x.data());
	}

	template<class OldHash, class OldHash2, class OldPred, size_t OldSlots>
	constexpr dhash_table& operator =(const dhash_table<N, Key, Val, OldHash, OldHash2, OldPred, OldSlots>& x) {
		clone(x.data());
		return *this;
	}

	constexpr size_t size() const { return N; }
	constexpr pointer data() const { return arr_; }
	constexpr pointer begin() const { return arr_; }
	constexpr pointer end() const { return arr_ + N; }

	constexpr const mapped_type& operator [](const key_type& k) const {
#ifdef DLOU_CHECK_ARGS
		return at(k);
#else
		return get_val(*find(k));
#endif
	}

	constexpr const mapped_type& at(const key_type& k, const mapped_type& invalid = mapped_type{}) const {
		auto ret = find(k);
		if (ret)
			return get_val(*ret);
		return invalid;
	}

	constexpr pointer find(const key_type& k) const {
		hasher hash;
		hasher2 hash2;
		key_equal eq;

		size_t idx = hash(k);
		idx %= slot_count;
		auto pos = slot_[idx];
		if (invalid_pos == pos)
			return end();
		auto ptr = arr_ + pos;
		if (eq(k, get_key(*ptr)))
			return ptr;

		size_t offset = (hash2(k) << 1) | size_t(1);
		idx += offset;

		for (size_t i = 1; i < slot_count; ++i) {
			idx %= slot_count;
			auto pos = slot_[idx];
			if (invalid_pos == pos)
				break;
			auto ptr = arr_ + pos;
			if (eq(k, get_key(*ptr)))
				return ptr;
			idx += offset;
		}
		return end();
	}

private:
	static constexpr const key_type& get_key(const value_type& v) {
		if constexpr (std::is_same<void, Val>::value)
			return v;
		else
			return v.first;
	}
	static constexpr const mapped_type& get_val(const value_type& v) {
		if constexpr (std::is_same<void, Val>::value)
			return v;
		else
			return v.second;
	}

	constexpr void clone(const value_type* p) {
		hasher hash;
		hasher2 hash2;
		key_equal eq;

		for (auto& v : slot_)
			v = invalid_pos;

		for (auto& v : arr_) {
			v = *p++;

			auto& key = get_key(v);
			size_t idx = hash(key);
			idx %= slot_count;
			auto& pos = slot_[idx];
			if (invalid_pos == pos) {
				pos = static_cast<nodepos>(&v - arr_);
				break;
			}
			
			size_t offset = (hash2(key) << 1) | size_t(1);
			idx += offset;

			for (size_t i = 1; i < slot_count; ++i) {
				idx %= slot_count;
				auto& pos = slot_[idx];
				if (invalid_pos == pos) {
					pos = static_cast<nodepos>(&v - arr_);
					break;
				}
				idx += offset;
			}
		}
	}

private:
	nodepos slot_[slot_count];
	value_type arr_[N];
};


template<class Key, class Val, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, size_t N>
constexpr auto make_hash_map(const std::pair<Key, Val>(&arr)[N])
{
	return hash_table<N, Key, Val, Hash, Pred>(arr);
}

template<class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, size_t N>
constexpr auto make_hash_set(const Key(&arr)[N])
{
	return hash_table<N, Key, void, Hash, Pred>(arr);
}

} // namespace dlou
