#pragma once

#include "macro.hpp"

#include <cstddef>

#include <type_traits>
#include <utility>
#include <functional>

namespace dlou {

template<size_t N, class Key, class Val = void, class Compare = std::less<Key>>
DLOU_REQUIRES(N > 0)
class sorted_array;

template<class = void, class Key, class Val, size_t N1, class Compare1, size_t N2, class Compare2>
constexpr auto merge(const sorted_array<N1, Key, Val, Compare1>&, const sorted_array<N2, Key, Val, Compare2>&);

template<class = void, class Key, class Val, size_t N1, class Compare1, size_t N2>
constexpr auto merge(const sorted_array<N1, Key, Val, Compare1>&, const std::pair<Key, Val>(&)[N2]);

template<class = void, class Key, size_t N1, class Compare1, size_t N2>
constexpr auto merge(const sorted_array<N1, Key, void, Compare1>&, const Key(&)[N2]);

template<size_t N, class Key, class Val, class Compare>
DLOU_REQUIRES(N > 0)
class sorted_array
{
	DLOU_REQUIRES_IN(N > 0);
	
	template<class, class Key, class Val, size_t N1, class Compare1, size_t N2, class Compare2>
	friend constexpr auto merge(const sorted_array<N1, Key, Val, Compare1>&, const sorted_array<N2, Key, Val, Compare2>&);

	template<class, class Key, class Val, size_t N1, class Compare1, size_t N2>
	friend constexpr auto merge(const sorted_array<N1, Key, Val, Compare1>&, const std::pair<Key, Val>(&)[N2]);

	template<class, class Key, size_t N1, class Compare1, size_t N2>
	friend constexpr auto merge(const sorted_array<N1, Key, void, Compare1>&, const Key(&)[N2]);

public:
	using key_type = Key;
	using mapped_type = typename std::conditional<std::is_same<void, Val>::value, Key, Val>::type;
	using key_compare = Compare;
	using value_type = typename std::conditional<std::is_same<void, Val>::value, key_type, std::pair<key_type, mapped_type>>::type;
	using reference = const value_type&;
	using pointer = const value_type*;

	struct value_compare {
		key_compare cmp;
		constexpr bool operator ()(reference a, reference b) const {
			return cmp(get_key(a), get_key(b));
		}
	};

public:
	constexpr sorted_array(const sorted_array&) = default;
	constexpr sorted_array& operator =(const sorted_array&) = default;

	constexpr sorted_array(const value_type(&x)[N]) {
		clone(x);
	}

	constexpr sorted_array& operator =(const value_type(&x)[N]) {
		clone(x);
		return *this;
	}

	template<class OldCompare>
	constexpr sorted_array(const sorted_array<N, Key, Val, OldCompare>& x) {
		clone(x.data());
	}

	template<class OldCompare>
	constexpr sorted_array& operator =(const sorted_array<N, Key, Val, OldCompare>& x) {
		clone(x.data());
		return *this;
	}

	constexpr size_t size() const { return N; }
	constexpr pointer data() const { return arr_; }
	constexpr pointer begin() const { return arr_; }
	constexpr pointer end() const { return arr_ + N; }

	constexpr const mapped_type& operator [](const key_type& k) const { return get_val(*find(k)); }

	constexpr const mapped_type& at(const key_type& k, const mapped_type& invalid = mapped_type{}) const {
		auto ret = lower_bound(k);
		if (Compare{}(k, get_key(*ret)))
			return invalid;
		return get_val(*ret);
	}

	constexpr pointer find(const key_type& k) const {
		auto ret = lower_bound(k);
		if (Compare{}(k, get_key(*ret)))
			return end();
		return ret;
	}

	constexpr pointer lower_bound(const key_type& k) const {
		Compare cmp;
		size_t l = 0;
		size_t r = N;
		size_t m;
		do {
			m = (l + r) / 2;
			if (cmp(get_key(arr_[m]), k))
				l = m + 1;
			else
				r = m;
		} while (l < r);
		return arr_ + l;
	}

	constexpr pointer upper_bound(const key_type& k) const {
		Compare cmp;
		size_t l = 0;
		size_t r = N;
		size_t m;
		do {
			m = (l + r) / 2;
			if (!cmp(k, get_key(arr_[m])))
				l = m + 1;
			else
				r = m;
		} while (l < r);
		return arr_ + l;
	}

	constexpr std::pair<pointer, pointer> equal_range(const key_type& k) const {
		return range(k, k);
	}

	constexpr std::pair<pointer, pointer> range(const key_type& min_key, const key_type& max_key) const {
		Compare cmp;
		pointer first;
		size_t l = 0;
		size_t r = N;
		size_t m;
		do {
			m = (l + r) / 2;
			if (cmp(get_key(arr_[m]), min_key))
				l = m + 1;
			else
				r = m;
		} while (l < r);
		first = arr_ + l;

		r = N;
		do {
			m = (l + r) / 2;
			if (!cmp(max_key, get_key(arr_[m])))
				l = m + 1;
			else
				r = m;
		} while (l < r);
		return { first, arr_ + l };
	}

private:
	sorted_array() = default;

	constexpr void clone(const value_type* p) {
		for (auto& v : arr_)
			v = *p++;
		sort();
	}

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

	constexpr void sort() {
		sort(arr_, N);
	}

	static constexpr void sort(value_type* beg, size_t siz) {
		Compare cmp;
		for (size_t i = siz - 1; i > 0; --i) {
			for (size_t j = 0; j < i; ++j) {
				auto* p1 = beg + j;
				auto* p2 = p1 + 1;
				if (cmp(get_key(*p2), get_key(*p1)))
					std::swap(*p1, *p2);
			}
		}
	}

private:
	value_type arr_[N];
};


template<class NewCompare, class Key, class Val, size_t N1, class Compare1, size_t N2, class Compare2>
constexpr auto merge(const sorted_array<N1, Key, Val, Compare1>& x, const sorted_array<N2, Key, Val, Compare2>& y)
{
	using predicate = typename std::conditional<std::is_same<void, NewCompare>::value, Compare1, NewCompare>::type;
	using result_type = sorted_array<N1 + N2, Key, Val, predicate>;
	using value_type = typename result_type::value_type;
	result_type ret;
	value_type* dst = ret.arr_;

	if constexpr (std::is_same<predicate, Compare1>::value || std::is_same<predicate, Compare2>::value) {
		typename result_type::value_compare cmp;
		const value_type* pos1;
		const value_type* end1;
		const value_type* pos2;
		const value_type* end2;

		if constexpr (std::is_same<predicate, Compare1>::value && std::is_same<predicate, Compare2>::value) {
			pos1 = x.begin();
			end1 = x.end();
			pos2 = y.begin();
			end2 = y.end();
		}
		else if constexpr (std::is_same<predicate, Compare1>::value) {
			pos1 = x.begin();
			end1 = x.end();
			end2 = ret.end();

			auto tmp = dst + N1;
			pos2 = tmp;

			for (auto& v : y)
				*tmp++ = v;
			ret.sort(dst + N1, N2);
		}
		else {
			pos1 = y.begin();
			end1 = y.end();
			end2 = ret.end();

			auto tmp = dst + N2;
			pos2 = tmp;

			for (auto& v : x)
				*tmp++ = v;
			ret.sort(dst + N2, N1);
		}

		for (;;) {
			if (cmp(*pos1, *pos2)) {
				*dst++ = *pos1++;

				if (end1 == pos1) {
					do {
						*dst++ = *pos2++;
					} while (pos2 < end2);

					break;
				}
			}
			else {
				*dst++ = *pos2++;

				if (end2 == pos2) {
					do {
						*dst++ = *pos1++;
					} while (pos1 < end1);

					break;
				}
			}
		}
	}
	else {
		for (auto& v : x)
			*dst++ = v;
		for (auto& v : y)
			*dst++ = v;
		ret.sort();
	}

	return ret;
}

template<size_t N, class Key, class Val, class Compare = std::less<Key>>
DLOU_REQUIRES(!std::is_same<void, Val>::value)
using sorted_map = sorted_array<N, Key, Val>;

template<size_t N, class Key, class Compare = std::less<Key>>
using sorted_set = sorted_array<N, Key, void>;


template<class T, class Compare = std::less<T>, size_t N>
constexpr auto make_sorted_set(const T(&arr)[N])
{
	return sorted_array<N, typename std::remove_reference<T>::type, void, Compare>{arr};
}

template<class Key, class Val, class Compare = std::less<Key>, size_t N>
DLOU_REQUIRES(!std::is_same<void, Val>::value)
constexpr auto make_sorted_map(const std::pair<Key, Val>(&arr)[N])
{
	DLOU_REQUIRES_IN(!std::is_same<void, Val>::value);
	return sorted_array<N, typename std::remove_reference<Key>::type, typename std::remove_reference<Val>::type, Compare>{arr};
}


template<class NewCompare, class Key, class Val, size_t N1, class Compare1, size_t N2>
constexpr auto merge(const sorted_array<N1, Key, Val, Compare1>& x, const std::pair<Key, Val>(&y)[N2])
{
	using predicate = typename std::conditional<std::is_same<void, NewCompare>::value, Compare1, NewCompare>::type;
	using result_type = sorted_array<N1 + N2, Key, Val, predicate>;
	using value_type = typename result_type::value_type;
	result_type ret;
	value_type* dst = ret.arr_;

	if constexpr (std::is_same<predicate, Compare1>::value) {
		typename result_type::value_compare cmp;
		auto pos1 = x.begin();
		auto end1 = x.end();
		auto end2 = ret.end();

		auto tmp = dst + N1;
		decltype(end2) pos2 = tmp;

		for (auto& v : y)
			*tmp++ = v;
		ret.sort(dst + N1, N2);

		for (;;) {
			if (cmp(*pos1, *pos2)) {
				*dst++ = *pos1++;

				if (end1 == pos1) {
					do {
						*dst++ = *pos2++;
					} while (pos2 < end2);

					break;
				}
			}
			else {
				*dst++ = *pos2++;

				if (end2 == pos2) {
					do {
						*dst++ = *pos1++;
					} while (pos1 < end1);

					break;
				}
			}
		}
	}
	else {
		for (auto& v : x)
			*dst++ = v;
		for (auto& v : y)
			*dst++ = v;
		ret.sort();
	}

	return ret;
}

template<class NewCompare, class Key, class Val, size_t N1, size_t N2, class Compare2>
constexpr auto merge(const std::pair<Key, Val>(&x)[N1], const sorted_array<N2, Key, Val, Compare2>& y)
{
	return merge<NewCompare>(y, x);
}

template<class NewCompare, class Key, size_t N1, class Compare1, size_t N2>
constexpr auto merge(const sorted_array<N1, Key, void, Compare1>& x, const Key(&y)[N2])
{
	using predicate = typename std::conditional<std::is_same<void, NewCompare>::value, Compare1, NewCompare>::type;
	using result_type = sorted_array<N1 + N2, Key, void, predicate>;
	using value_type = typename result_type::value_type;
	result_type ret;
	value_type* dst = ret.arr_;

	if constexpr (std::is_same<predicate, Compare1>::value) {
		typename result_type::value_compare cmp;
		auto pos1 = x.begin();
		auto end1 = x.end();
		auto end2 = ret.end();

		auto tmp = dst + N1;
		decltype(end2) pos2 = tmp;

		for (auto& v : y)
			*tmp++ = v;
		ret.sort(dst + N1, N2);

		for (;;) {
			if (cmp(*pos1, *pos2)) {
				*dst++ = *pos1++;

				if (end1 == pos1) {
					do {
						*dst++ = *pos2++;
					} while (pos2 < end2);

					break;
				}
			}
			else {
				*dst++ = *pos2++;

				if (end2 == pos2) {
					do {
						*dst++ = *pos1++;
					} while (pos1 < end1);

					break;
				}
			}
		}
	}
	else {
		for (auto& v : x)
			*dst++ = v;
		for (auto& v : y)
			*dst++ = v;
		ret.sort();
	}

	return ret;
}

template<class NewCompare = void, class Key, size_t N1, size_t N2, class Compare2>
constexpr auto merge(const Key(&x)[N1], const sorted_array<N2, Key, void, Compare2>& y)
{
	return merge<NewCompare>(y, x);
}


} // namespace dlou
