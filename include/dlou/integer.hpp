#pragma once

#include "macro.hpp"

#include <cstdint>
#include <type_traits>
#include <limits>
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

} // namespace dlou
