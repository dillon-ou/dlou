#pragma once

#include <type_traits>
#include <compare>
#include <utility>

#include <cstddef>

namespace dlou {

template<class T>
class pair_int
{
	using uint = std::make_unsigned_t<T>;
	using sint = std::make_signed_t<T>;
	using tint = std::conditional_t<std::is_signed_v<T>, uint, sint>;
	static constexpr size_t bits = sizeof(T) * 8;
	static constexpr size_t half = bits / 2;
	static constexpr T hi_mask = ~T(0) << half;
	static constexpr T lo_mask = ~hi_mask;
	static constexpr T negative = T(1) << (bits - 1);

public:
	using half_type = T;

	T lo;
	T hi;

protected:
	static constexpr T high_signed(const T& v) {
		if constexpr (std::is_unsigned_v<T>)
			return 0;
		else
			return v >> (bits - 1);
	}

public:
	static constexpr pair_int add(const T& a, const T& b) {
		pair_int r(high_signed(a) + high_signed(b), a + b);

		r.hi += T(uint((a & b) | ((a | b) & ~r.lo)) >> (bits - 1));
		return r;
	}

	static constexpr pair_int mul(const T& a, const T& b) {
		T a_hi = a >> half;
		T a_lo = a & lo_mask;
		T b_hi = b >> half;
		T b_lo = b & lo_mask;

		T lo = a_lo * b_lo;
		T mi1 = a_lo * b_hi;
		T mi2 = b_lo * a_hi;

		T mi = (uint(lo) >> half)
			+ (mi1 & lo_mask)
			+ (mi2 & lo_mask);

		return pair_int(
			(a_hi * b_hi) + (mi >> half) + (mi1 >> half) + (mi2 >> half),
			(lo & lo_mask) | (mi << half)
		);
	}

	static constexpr pair_int min() {
		if constexpr (std::is_unsigned_v<T>)
			return pair_int(T(0), T(0));
		else
			return pair_int(negative, T(0));
	}

	static constexpr pair_int max() {
		return ~min();
	}

public:
	constexpr pair_int() = default;
	constexpr pair_int(const pair_int&) = default;
	constexpr pair_int& operator =(const pair_int&) = default;

	constexpr pair_int(const T& h, const T& l)
		: lo(l)
		, hi(h) {
	}

	constexpr pair_int(const T& v)
		: lo(v)
		, hi(high_signed(v)) {
	}

	constexpr explicit operator T() const {
		return lo;
	}

	constexpr explicit operator pair_int<tint>() const {
		return { (tint)hi, (tint)lo };
	}

	constexpr explicit operator bool() const {
		return hi || lo;
	}

	constexpr bool operator ==(const pair_int& x) const = default;

	constexpr bool operator !=(const pair_int& x) const = default;

	constexpr bool operator >(const pair_int& x) const {
		if (hi > x.hi)
			return true;
		if (hi < x.hi)
			return false;
		if constexpr (std::is_unsigned_v<T>)
			return lo > x.lo;
		else
			return (uint)lo > (uint)x.lo;
	}

	constexpr bool operator <(const pair_int& x) const {
		return x > *this;
	}

	constexpr bool operator >=(const pair_int& x) const {
		return !(x > *this);
	}

	constexpr bool operator <=(const pair_int& x) const {
		return !(*this > x);
	}

	constexpr std::strong_ordering operator <=>(const pair_int& x) const {
		if (auto cmp = hi <=> x.hi;
			0 != cmp)
			return cmp;

		if constexpr (std::is_unsigned_v<T>)
			return lo <=> x.lo;
		else
			return (uint)lo <=> (uint)x.lo;
	}

	constexpr pair_int operator ~() const {
		return pair_int(~hi, ~lo);
	}

	constexpr pair_int operator &(const pair_int& x) const {
		return pair_int(hi & x.hi, lo & x.lo);
	}

	constexpr pair_int operator |(const pair_int& x) const {
		return pair_int(hi | x.hi, lo | x.lo);
	}

	constexpr pair_int operator ^(const pair_int& x) const {
		return pair_int(hi ^ x.hi, lo ^ x.lo);
	}

	constexpr pair_int operator <<(size_t n) const {
		if (bits < n) {
			return pair_int(lo << (n - bits), 0);
		}
		else {
			return pair_int((hi << n) | (uint(lo) >> (bits - n)), lo << n);
		}
	}

	constexpr pair_int operator >>(size_t n) const {
		if (bits < n)
			return pair_int(high_signed(hi), hi >> (n - bits));
		else
			return pair_int(hi >> n, (hi << (bits - n)) | (uint(lo) >> n));
	}

	constexpr pair_int& operator &=(const pair_int& x) { *this = *this & x; return *this; }
	constexpr pair_int& operator |=(const pair_int& x) { *this = *this | x; return *this; }
	constexpr pair_int& operator ^=(const pair_int& x) { *this = *this ^ x; return *this; }
	constexpr pair_int& operator <<=(size_t n) { *this = *this << n; return *this; }
	constexpr pair_int& operator >>=(size_t n) { *this = *this >> n; return *this; }

	constexpr pair_int operator -() const {
		return (~*this) + pair_int(T(0), T(1));
	}

	constexpr pair_int operator +(const pair_int& x) const {
		auto&& ret = pair_int<uint>::add(lo, x.lo);
		ret.hi += hi + x.hi;
		return (pair_int)ret;
	}

	constexpr pair_int operator -(const pair_int& x) const {
		return *this + (-x);
	}

	constexpr pair_int operator *(const pair_int& x) const {
		pair_int low = mul(lo, x.lo);
		pair_int mid = pair_int(0, low.hi)
			+ pair_int(0, mul(lo, x.hi).lo)
			+ pair_int(0, mul(hi, x.lo).lo);

		return pair_int(mid.lo, low.lo);
	}

	struct div_t {
		pair_int quot;
		pair_int rem;
	};

	constexpr div_t div(const pair_int& x) const {
		if (!x)
			return { .quot = max(), .rem = *this };

		pair_int rem = *this;
		pair_int div = x;
		pair_int quo(0, 0);
		pair_int one(0, 1);
		size_t off = 1;

		if constexpr (std::is_unsigned_v<T>) {
			while (rem >= div && (negative & ~div.hi)) {
				div <<= 1;
				++off;
			}

			while (off--) {
				quo <<= 1;
				if (rem >= div) {
					rem -= div;
					quo |= one;
				}
				div >>= 1;
			}
		}
		else {
			if (negative & (rem.hi ^ div.hi)) {
				div = -div;
				one = -one;
			}

#if 1
			off = (negative & div.hi) ? div.countl_one() : div.countl_zero();
			div <<= off - 1;
#else
			for (;;) {
				pair_int&& tmp = div << 1;
				if (negative & (tmp.hi ^ div.hi))
					break;
				div = tmp;
				++off;
			}
#endif

			while (off--) {
				quo <<= 1;
				pair_int&& dif = rem - div;

				if (!dif) {
					rem = dif;
					quo += one;
					quo <<= off;
					break;
				}

				if (negative & (dif.hi ^ ~rem.hi)) {
					rem = dif;
					quo += one;
				}
				div >>= 1;
			}
		}

		return { .quot = quo, .rem = rem };
	}

	constexpr pair_int operator /(const pair_int& x) const {
		return div(x).quot;
	}

	constexpr pair_int operator %(const pair_int& x) const {
		return div(x).rem;
	}

	constexpr pair_int& operator +=(const pair_int& x) { *this = *this + x; return *this; }
	constexpr pair_int& operator -=(const pair_int& x) { *this = *this - x; return *this; }
	constexpr pair_int& operator *=(const pair_int& x) { *this = *this * x; return *this; }
	constexpr pair_int& operator /=(const pair_int& x) { *this = *this / x; return *this; }
	constexpr pair_int& operator %=(const pair_int& x) { *this = *this % x; return *this; }

	constexpr pair_int& operator ++() { return (*this) += pair_int(T(0), T(1)); }
	constexpr pair_int& operator --() { return (*this) -= pair_int(T(0), T(1)); }
	constexpr pair_int operator ++(int) { auto r = *this; ++*this; return r; }
	constexpr pair_int operator --(int) { auto r = *this; --*this; return r; }

protected:
	static constexpr size_t _bit_width(uint v) noexcept {
		constexpr uint one = uint(1);
		size_t pos = half;
		size_t ret = 1;

		if (!v)
			return 0;

		while (one != v) {
			if (uint&& move = v >> pos) {
				ret += pos;
				v = move;
			}
			pos >>= 1;
		}

		return ret;
	}

	static constexpr size_t _countr_zero(T v) noexcept {
		size_t pos = half;
		size_t ret = 1;

		if (!v)
			return bits;

		while (negative != v) {
			if (T&& move = v << pos) {
				ret += pos;
				v = move;
			}
			pos >>= 1;
		}

		return bits - ret;
	}

public:
	constexpr size_t bit_width() const noexcept {
		return hi ? _bit_width(hi) + bits : _bit_width(lo);
	}

	constexpr size_t countl_zero() const noexcept {
		return bits * 2 - bit_width();
	}

	constexpr size_t countl_one() const noexcept {
		return (~*this).countl_zero();
	}

	constexpr size_t countr_zero() const noexcept {
		return lo ? _countr_zero(lo) : _countr_zero(hi) + bits;
	}

	constexpr size_t countr_one() const noexcept {
		return (~*this).countr_zero();
	}
};

} // namespace dlou


namespace std {

template <class T>
struct is_signed<dlou::pair_int<T>> : is_signed<T> {};

template <class T>
struct is_unsigned<dlou::pair_int<T>> : is_unsigned<T> {};

template <class T>
struct make_signed<dlou::pair_int<T>> {
	using type = dlou::pair_int<typename make_signed<T>::type>;
};

template <class T>
struct make_unsigned<dlou::pair_int<T>> {
	using type = dlou::pair_int<typename make_unsigned<T>::type>;
};

#ifdef __cpp_lib_int_pow2
template <class T>
constexpr int bit_width(const dlou::pair_int<T>& x) noexcept
{
	return (int)x.bit_width();
}

template <class T>
constexpr int countl_zero(const dlou::pair_int<T>& x) noexcept
{
	return (int)x.countl_zero();
}

template <class T>
constexpr int countl_one(const dlou::pair_int<T>& x) noexcept
{
	return (int)x.countl_one();
}

template <class T>
constexpr int countr_zero(const dlou::pair_int<T>& x) noexcept
{
	return (int)x.countr_zero();
}

template <class T>
constexpr int countr_one(const dlou::pair_int<T>& x) noexcept
{
	return (int)x.countr_one();
}
#endif//__cpp_lib_int_pow2

} // namespace std
