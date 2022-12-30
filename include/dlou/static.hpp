#pragma once

#include "macro.hpp"
#include "integer.hpp"

#include <type_traits>


namespace dlou {

template<class T>
DLOU_REQUIRES(std::is_member_object_pointer_v<T>)
struct member {
};

template<class T, class M>
struct member<M T::*> {
	using type = M;
};

template<class T>
using member_t = typename member<T>::type;

template<class T>
DLOU_REQUIRES(std::is_member_object_pointer_v<T>)
struct remove_member {
};

template<class T, class M>
struct remove_member<M T::*> {
	using type = T;
};

template<class T>
using remove_member_t = typename remove_member<T>::type;

namespace _ {

	template<size_t Bytes>
	struct integer {
		using type = void;
	};

	template<>
	struct integer<1> {
		using type = signed char;
		static_assert(1 == sizeof(type), "Check sizeof(integer)");
	};

	template<>
	struct integer<2> {
		using type = short;
		static_assert(2 == sizeof(type), "Check sizeof(integer)");
	};

	template<>
	struct integer<4> {
		using type = long;
		static_assert(4 == sizeof(type), "Check sizeof(integer)");
	};

	template<>
	struct integer<8> {
		using type = long long;
		static_assert(8 == sizeof(type), "Check sizeof(integer)");
	};

	template<size_t Bytes>
	struct uinteger {
		using type = std::make_unsigned_t<typename integer<Bytes>::type>;
	};

} // namespace _

template<size_t Bytes>
using integer = _::integer<base2::ceil(Bytes)>;

template<size_t Bytes>
using uinteger = _::uinteger<base2::ceil(Bytes)>;

template<size_t Bytes>
using integer_t = typename integer<Bytes>::type;

template<size_t Bytes>
using uinteger_t = typename uinteger<Bytes>::type;

} // namespace dlou
