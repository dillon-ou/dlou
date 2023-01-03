#pragma once

#include "macro.hpp"
#include "pair_int.hpp"

#include <cstdint>
#include <type_traits>
#include <limits>
#include <utility>
#include <array>

#ifdef __cpp_lib_int_pow2
#include <bit>
#endif

namespace dlou {

static constexpr uint8_t invalid_exp = -1;

namespace bit {

template<typename T>
DLOU_REQUIRES(std::is_integral<T>::value)
constexpr T reverse(T v)
{
	T ret = 0;
	for (uint8_t i = 0; i < 8 * sizeof(T); ++i) {
		ret <<= 1;
		ret |= v & T(1);
		v >>= 1;
	}

	return ret;
}

// asm : bsf
// gcc : __builtin_ctz 
// win : _BitScanForward(64)
// std : countr_zero
template<typename T>
DLOU_REQUIRES(std::is_integral<T>::value)
constexpr uint8_t bsf(T v)
{
	using uint = typename std::make_unsigned<T>::type;
	if (!v)
		return invalid_exp;
#ifdef __cpp_lib_int_pow2
	return (uint8_t)std::countr_zero(static_cast<uint>(v));
#else//__cpp_lib_int_pow2
	static constexpr uint8_t size = sizeof(T) * 8;
	static constexpr T hi_bit = uint(1) << (size - 1);
	uint8_t half = size / 2;
	uint8_t ret = 0;

	while (hi_bit != v) {
		if (T move = v << half) {
			ret += half;
			v = move;
		}
		half >>= 1;
	}

	return size - ret - 1;
#endif//__cpp_lib_int_pow2
}

// asm : bsr
// gcc : __builtin_clz 
// win : _BitScanReverse(64)
// std : bit_width - 1
template<typename T>
DLOU_REQUIRES(std::is_integral<T>::value)
constexpr uint8_t bsr(T v)
{
	using uint = typename std::make_unsigned<T>::type;
#ifdef __cpp_lib_int_pow2
	return (uint8_t)(std::bit_width(static_cast<uint>(v)) - 1);
#else//__cpp_lib_int_pow2
	if (!v)
		return invalid_exp;

	static constexpr uint8_t size = sizeof(T) * 8;
	static constexpr T lo_bit = uint(1);
	uint8_t half = size / 2;
	uint8_t ret = 0;

	while (lo_bit != v) {
		if (T move = v >> half) {
			ret += half;
			v = move;
		}
		half >>= 1;
	}

	return ret;
#endif//__cpp_lib_int_pow2
}

} // namespace bit

#ifndef DLOU_NO_ALIAS
using bit::bsf;
using bit::bsr;
#endif


namespace base2 {

// std : has_single_bit
template<typename T>
DLOU_REQUIRES(std::is_integral<T>::value)
constexpr bool ispow(T v)
{
	if constexpr (std::is_signed<T>::value)
		if (v < T(0))
			return false;
#ifdef __cpp_lib_int_pow2
	return std::has_single_bit(static_cast<typename std::make_unsigned<T>::type>(v));
#else//__cpp_lib_int_pow2
	return !(v == 0 || (v & (v - 1)));
#endif//__cpp_lib_int_pow2
}

template<typename T>
DLOU_REQUIRES(std::is_integral<T>::value)
constexpr uint8_t log(T v)
{
	using uint = typename std::make_unsigned<T>::type;
	if constexpr (std::is_signed<T>::value)
		if (v < T(0))
			return invalid_exp;
	return bit::bsr(v);
}

template<typename T>
constexpr uint8_t log_ceil(T v)
{
	return log(v) + !ispow(v);
}

// std : bit_ceil
template<typename T>
DLOU_REQUIRES(std::is_unsigned<T>::value)
constexpr T ceil(T v)
{
	using uint = typename std::decay<T>::type;
#ifdef __cpp_lib_int_pow2
	return std::bit_ceil(static_cast<uint>(v));
#else//__cpp_lib_int_pow2
	auto ret = static_cast<uint>(v);
	ret -= ispow2(ret);
	ret |= ret >> 1;
	ret |= ret >> 2;
	ret |= ret >> 4;
	if constexpr (sizeof(T) > 1)
		ret |= ret >> 8;
	if constexpr (sizeof(T) > 2)
		ret |= ret >> 16;
	if constexpr (sizeof(T) > 4)
		ret |= ret >> 32;
	return static_cast<T>(ret + 1);
#endif//__cpp_lib_int_pow2
}

// std : bit_floor
template<typename T>
DLOU_REQUIRES(std::is_unsigned<T>::value)
constexpr T floor(T v)
{
	using uint = typename std::decay<T>::type;
#ifdef __cpp_lib_int_pow2
	return std::bit_floor(static_cast<uint>(v));
#else//__cpp_lib_int_pow2
	auto ret = static_cast<uint>(v);
	ret |= ret >> 1;
	ret |= ret >> 2;
	ret |= ret >> 4;
	if constexpr (sizeof(T) > 1)
		ret |= ret >> 8;
	if constexpr (sizeof(T) > 2)
		ret |= ret >> 16;
	if constexpr (sizeof(T) > 4)
		ret |= ret >> 32;
	return static_cast<T>(ret & ~(ret >> 1));
#endif//__cpp_lib_int_pow2
}

} // namespace integer

DLOU_DEFINED_ALIAS_FUNCTION(ispow2_i, base2::ispow)
DLOU_DEFINED_ALIAS_FUNCTION(log2_i, base2::log)
DLOU_DEFINED_ALIAS_FUNCTION(log2_ceil_i, base2::log_ceil)
DLOU_DEFINED_ALIAS_FUNCTION(pow2_ceil_i, base2::ceil)
DLOU_DEFINED_ALIAS_FUNCTION(pow2_floor_i, base2::floor)


namespace zigzag {
	template<typename T>
	DLOU_REQUIRES(std::is_signed<T>::value)
	constexpr auto encode(T v)
	{
		return static_cast<typename std::make_unsigned<T>::type>(
			(v >> (sizeof(T) * 8 - 1)) ^ (v << 1)
			);
	}

	template<typename T>
	DLOU_REQUIRES(std::is_unsigned<T>::value)
	constexpr auto decode(T v)
	{
		return static_cast<typename std::make_signed<T>::type>(
			(v >> 1) ^ -(v & T(1))
			);
	}
}

DLOU_DEFINED_ALIAS_FUNCTION(zigzag_encode, zigzag::encode)
DLOU_DEFINED_ALIAS_FUNCTION(zigzag_decode, zigzag::decode)

namespace _ {
	template<size_t Bytes>
	struct integer {};

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

	template<size_t Bytes, class = void>
	struct uinteger {};

	template<size_t Bytes>
	struct uinteger<Bytes, std::void_t<typename integer<Bytes>::type>> {
		using type = std::make_unsigned_t<typename integer<Bytes>::type>;
	};

} // namespace _

template<size_t Bytes, bool Signed = true>
using integer = std::conditional_t<Signed, _::integer<base2::ceil(Bytes)>, _::uinteger<base2::ceil(Bytes)>>;

template<size_t Bytes>
using sint = integer<base2::ceil(Bytes), true>;

template<size_t Bytes>
using uint = integer<base2::ceil(Bytes), false>;

template<size_t Bytes, bool Signed = true>
using integer_t = typename integer<Bytes, Signed>::type;

template<size_t Bytes>
using sint_t = typename sint<Bytes>::type;

template<size_t Bytes>
using uint_t = typename uint<Bytes>::type;

namespace math {

namespace _ {
	template<class T, class = void>
	struct add {
		using result_type = pair_int<T>;

		result_type operator ()(const T& a, const T& b) const {
			return pair_int<T>::add(a, b);
		}
	};

	template<class T>
	struct add<T, std::void_t<typename integer<sizeof(T) * 2, std::is_signed_v<T>>::type>> {
		using result_type = integer_t<sizeof(T) * 2, std::is_signed_v<T>>;

		result_type operator ()(const T& a, const T& b) const {
			return result_type(a) + result_type(b);
		}
	};

	template<class T, class = void>
	struct mul {
		using result_type = pair_int<T>;

		result_type operator ()(const T& a, const T& b) const {
			return pair_int<T>::mul(a, b);
		}
	};

	template<class T>
	struct mul<T, std::void_t<typename integer<sizeof(T) * 2, std::is_signed_v<T>>::type>> {
		using result_type = integer_t<sizeof(T) * 2, std::is_signed_v<T>>;

		result_type operator ()(const T& a, const T& b) const {
			return result_type(a) * result_type(b);
		}
	};
} // namespace _

template<class T, class U>
DLOU_REQUIRES(std::is_integral<T>::value && std::is_integral<U>::value)
inline constexpr auto add(const T& a, const U& b) {
	return _::add<std::common_type_t<T, U>>{}(a, b);
}

template<class T, class U>
DLOU_REQUIRES(std::is_integral<T>::value)
inline constexpr auto mul(const T& a, const U& b) {
	return _::mul<std::common_type_t<T, U>>{}(a, b);
}

} // namespace math

} // namespace dlou
