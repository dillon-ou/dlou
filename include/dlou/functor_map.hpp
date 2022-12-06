#pragma once

#include "sorted_array.hpp"
#include "functor_array.hpp"

#include <type_traits>
#include <tuple>
#include <utility>
#include <optional>

namespace dlou {

template<class Key, class Compare, class... Predicates>
DLOU_REQUIRES(sizeof...(Predicates) > 0)
class functor_map
{
	static constexpr size_t max_size = sizeof...(Predicates);
	using map_type = sorted_array<max_size, Key, size_t, Compare>;
public:
	using key_type = Key;
	using key_compare = Compare;
	using functors = std::tuple<Predicates...>;

protected:
	static constexpr map_type make_map(const key_type(&arr)[max_size]) {
		std::pair<key_type, size_t> tmp[max_size];
		for (size_t i = 0; i < max_size; ++i)
			tmp[i] = { arr[i], i };
		return tmp;
	}

public:
	constexpr functor_map(const functor_map&) = default;
	constexpr functor_map& operator =(const functor_map&) = default;

	template<class... Args>
	constexpr functor_map(const key_type(&arr)[max_size], Args&&... args)
		: map_(make_map(arr))
		, arr_(std::forward<Args>(args)...) {
	}

	template<class... Args>
	constexpr auto operator ()(const key_type& key, Args&&... args) {
		using result_type = std::invoke_result_t<decltype(arr_), size_t, Args...>;
		if constexpr (std::is_same_v<void, result_type>) {
			auto pos = map_.find(key);
			if (map_.end() == pos)
				return false;
			arr_(pos->second, std::forward<Args>(args)...);
			return true;
		}
		else {
			std::optional<result_type> ret;
			auto pos = map_.find(key);
			if (map_.end() != pos)
				ret = std::move(arr_(pos->second, std::forward<Args>(args)...));
			return ret;
		}
	}

	template<class... Args>
	constexpr auto operator ()(const key_type& key, Args&&... args) const {
		using result_type = std::invoke_result_t<decltype(arr_), size_t, Args...>;
		if constexpr (std::is_same_v<void, result_type>) {
			auto pos = map_.find(key);
			if (map_.end() == pos)
				return false;
			arr_(pos->second, std::forward<Args>(args)...);
			return true;
		}
		else {
			std::optional<result_type> ret;
			auto pos = map_.find(key);
			if (map_.end() != pos)
				ret = std::move(arr_(pos->second, std::forward<Args>(args)...));
			return ret;
		}
	}

private:
	map_type map_;
	functor_array<Predicates...> arr_;
};

template<class Key, class Compare = std::less<Key>, class... Predicates>
constexpr auto make_functor_map(const std::pair<Key, Predicates>&... args)
{
	return functor_map<Key, Compare, Predicates...>({ std::get<0>(args)... }, std::get<1>(args)...);
}

} // namespace dlou
