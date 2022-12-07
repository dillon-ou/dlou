#pragma once

#include <type_traits>
#include <tuple>
#include <utility>
#include <optional>
#include <array>
#include <algorithm>

#define dlou_make_sorted_functor(...) ( \
	[]() -> auto { \
		constexpr auto pred_order = init_sorted_functor(__VA_ARGS__); \
		return make_sorted_functor<pred_order.first>(pred_order.second); \
	}() \
)

namespace dlou {

/* ex:
 *	constexpr auto pred_order = init_sorted_functor(
 *		std::make_pair("b"sv, std::integral_constant<int, 0>{}),
 *		std::make_pair("a"sv, std::integral_constant<int, 1>{})
 *	);
 *
 *	auto map =  make_sorted_functor<pred_order.first>(pred_order.second);
 *
 *	constexpr auto map2 = dlou_make_sorted_functor(
 *		std::make_pair("b"sv, std::integral_constant<int, 0>{}),
 *		std::make_pair("a"sv, std::integral_constant<int, 1>{})
 *	);
 */

template<class Key, class Compare, class Tuple>
class sorted_functor
{
	static constexpr size_t max_size = std::tuple_size_v<Tuple>;
public:
	using key_type = Key;
	using key_compare = Compare;
	using functors = Tuple;

private:
	using key_order = std::array<key_type, max_size>;

	template<size_t Pos = 0>
	struct result {
		template<class... Args>
		constexpr auto operator ()(Args&&... args) const {
			using result_type = std::common_type_t<
				std::invoke_result_t<std::tuple_element_t<Pos, functors>, Args...>,
				std::invoke_result_t<result<Pos + 1>, Args...>>;
			return result_type{};
		}
	};

	template<>
	struct result<max_size - 1> {
		template<class... Args>
		constexpr std::invoke_result_t<std::tuple_element_t<max_size - 1, functors>, Args...> operator ()(Args&&... args) const {
			return {};
		}
	};

	template<class... Args>
	using result_type = std::invoke_result_t<result<>, Args&&...>;

	template<size_t Front, size_t Back>
	struct find_functor_impl {
		static constexpr size_t Center = (Front + Back + 1) / 2;
		using first_functor = find_functor_impl<Front, Center - 1>;
		using second_functor = find_functor_impl<Center, Back>;

		template<class Tuple, class... Args>
		constexpr auto operator ()(const key_order& set, Tuple&& x, const key_type& key, Args&&... args) const {
			if (key_compare{}(key, set[Center]))
				return first_functor{}(set, std::forward<Tuple>(x), key, std::forward<Args>(args)...);
			else
				return second_functor{}(set, std::forward<Tuple>(x), key, std::forward<Args>(args)...);
		}
	};

	template<size_t Pos>
	struct find_functor_impl<Pos, Pos> {
		template<class Tuple, class... Args>
		constexpr std::optional<result_type<Args...>> operator ()(const key_order& set, Tuple&& x, const key_type& key, Args&&... args) const {
			key_compare cmp;
			auto& cmp_key = set[Pos];
			if (cmp(key, cmp_key) || cmp(cmp_key, key))
				return {};
			return std::get<Pos>(std::forward<Tuple>(x))(std::forward<Args>(args)...);
		}
	};

	using find_functor = find_functor_impl<0, max_size - 1>;

public:
	constexpr sorted_functor(const sorted_functor&) = default;
	constexpr sorted_functor& operator =(const sorted_functor&) = default;

	template<class... Args>
	constexpr sorted_functor(const key_order& arr, Args&&... args)
		: keys_(arr)
		, pred_(std::forward<Args>(args)...) {
	}

	constexpr size_t size() const { return max_size; }

	template<class... Args>
	constexpr auto operator ()(const key_type& key, Args&&... args) {
		return forward_call(keys_, pred_, key, std::forward<Args>(args)...);
	}

	template<class... Args>
	constexpr auto operator ()(const key_type& key, Args&&... args) const {
		return forward_call(keys_, pred_, key, std::forward<Args>(args)...);
	}


protected:
	template<class Tuple, class... Args>
	static constexpr auto forward_call(const key_order& set, Tuple&& x, const key_type& key, Args&&... args) {
		return find_functor{}(set, std::forward<Tuple>(x), key, std::forward<Args>(args)...);
	}

private:
	key_order keys_;
	functors pred_;
};

namespace _sorted_functor {
	template<class Tuple, size_t... Positions>
	DLOU_REQUIRES(std::tuple_size_v<Tuple> == sizeof...(Positions))
	struct sort
	{
		static constexpr size_t count = std::tuple_size_v<Tuple>;
		using functors = Tuple;

		static constexpr auto map = []() -> auto {
			using value_type = std::tuple<size_t, size_t>;
			std::array<value_type, count> pos = { value_type(Positions, 0)... };
			for (size_t i = 0; i < count; ++i)
				std::get<1>(pos[i]) = i;

			std::sort(pos.begin(), pos.end(), [](const value_type& x, const value_type& y) -> bool {
				return std::get<0>(x) < std::get<0>(y);
				});

			std::array<size_t, count> ret;
			for (size_t i = 0; i < count; ++i)
				ret[i] = std::get<1>(pos[i]);

			return ret;
		}();

		static consteval auto at(size_t n) {
			return map[map[n]];
		}

		using type = std::tuple<typename std::tuple_element<at(Positions), functors>::type...>;

		template<class T>
		static constexpr auto tie(T&& x) {
			return std::tie(std::get<at(Positions)>(std::forward<T>(x))...);
		}
	};

	template<size_t N, class Tuple, std::array<size_t, N> Arr, size_t Pos = 0, size_t... Elements>
	struct from_array : from_array<N, Tuple, Arr, Pos + 1, Elements..., Arr[Pos]> {
	};

	template<size_t N, class Tuple, std::array<size_t, N> Arr, size_t... Elements>
	struct from_array<N, Tuple, Arr, N, Elements...> : sort<Tuple, Elements...> {
	};

	template<class Key, class Compare, class... Predicates>
	struct info {
		std::array<Key, sizeof...(Predicates)> key;
		std::tuple<Predicates...> value;
	};
}

// before make_sorted_functor
template<class Key, class Compare = std::less<Key>, class... Predicates>
consteval auto init_sorted_functor(const std::pair<Key, Predicates>&... args)
{
	constexpr size_t max_size = sizeof...(Predicates);
	std::array<size_t*, max_size> map;
	std::pair<std::array<size_t, max_size>, _sorted_functor::info<Key, Compare, Predicates...>> ret;

	ret.second.key = std::to_array({ std::get<0>(args)... });
	ret.second.value = std::tie(std::get<1>(args)...);

	for (size_t i = 0; i < max_size; ++i)
		map[i] = &ret.first[i];

	Compare cmp;
	for (size_t i = max_size - 1; i > 0; --i) {
		for (size_t j = 0; j < i; ++j) {
			auto& k1 = ret.second.key[j];
			auto& k2 = ret.second.key[j + 1];
			if (cmp(k2, k1)) {
				std::swap(k1, k2);
				std::swap(map[j], map[j + 1]);
			}
		}
	}

	for (size_t i = 0; i < max_size; ++i)
		*map[i] = i;

	return ret;
}

// constexpr auto res = init_sorted_functor(...);
// constexpr auto map = make_sorted_functor<res.first>(res.second);
template<auto First, class Key, class Compare, class... Predicates>
consteval auto make_sorted_functor(const _sorted_functor::info<Key, Compare, Predicates...>& second)
{
	using namespace _sorted_functor;
	using sorted_tuple = from_array<sizeof...(Predicates), std::tuple<Predicates...>, First>;
	return sorted_functor<Key, Compare, typename sorted_tuple::type>(second.key, sorted_tuple::tie(second.value));
}

} // namespace dlou
