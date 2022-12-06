#pragma once

#include <type_traits>
#include <tuple>
#include <utility>

namespace dlou {

template<class... Predicates>
DLOU_REQUIRES(sizeof...(Predicates) > 0)
class functor_array
{
	static constexpr size_t max_size = sizeof...(Predicates);
public:
	using functors = std::tuple<Predicates...>;

private:
	template<size_t Front, size_t Back>
	struct find_functor_impl {
		static constexpr size_t Center = (Front + Back + 1) / 2;
		using first_functor = find_functor_impl<Front, Center - 1>;
		using second_functor = find_functor_impl<Center, Back>;

		template<class Tuple, class... Args>
		constexpr auto operator ()(Tuple&& x, size_t idx, Args&&... args) const {
			if (idx < Center)
				return first_functor{}(std::forward<Tuple>(x), idx, std::forward<Args>(args)...);
			else
				return second_functor{}(std::forward<Tuple>(x), idx, std::forward<Args>(args)...);
		}
	};

	template<size_t Position>
	struct find_functor_impl<Position, Position> {
		template<class Tuple, class... Args>
		constexpr std::common_type_t<typename std::invoke_result<Predicates, Args&&...>::type...> operator ()(
			Tuple&& x,
			size_t idx,
			Args&&... args
			) const {
			 return std::get<Position>(std::forward<Tuple>(x))(std::forward<Args>(args)...);
		}
	};

	using find_functor = find_functor_impl<0, max_size - 1>;

public:
	constexpr functor_array() = default;
	constexpr functor_array(const functor_array&) = default;
	constexpr functor_array& operator =(const functor_array&) = default;

	template<class... Args>
	constexpr functor_array(Args&&... args) : pred_(std::forward<Args>(args)...) {}

	constexpr size_t size() const { return max_size; }

	template<class... Args>
	constexpr std::common_type_t<typename std::invoke_result<Predicates, Args...>::type...> operator ()(size_t n, Args&&... args)  {
#ifdef DLOU_CHECK_ARGS
		if (max_size <= n)
			return {};
#endif
		return forward_call(pred_, n, std::forward<Args>(args)...);
	}

	template<class... Args>
	constexpr std::common_type_t<typename std::invoke_result<Predicates, Args...>::type...> operator ()(size_t n, Args&&... args) const {
#ifdef DLOU_CHECK_ARGS
		if (max_size <= n)
			return {};
#endif
		return forward_call(pred_, n, std::forward<Args>(args)...);
	}

protected:
	template<class Tuple, class... Args>
	static constexpr auto forward_call(Tuple&& x, size_t n, Args&&... args) {
		return find_functor{}(std::forward<Tuple>(x), n, std::forward<Args>(args)...);
	}

private:
	functors pred_;
};

template<class... Predicates>
constexpr auto make_functor_array(Predicates&&... args)
{
	return functor_array<std::remove_reference_t<Predicates>...>(args...);
}

} // namespace dlou
