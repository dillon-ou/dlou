#pragma once

#include "pair_int.hpp"

#include <type_traits>
#include <compare>


namespace dlou {

struct base_construct_t { explicit base_construct_t() = default; };
inline constexpr base_construct_t base_construct{};

template<class Base, size_t Scale>
class decimal
{
public:
	using value_type = Base;

protected:
	value_type value_;

	static constexpr unsigned long long pow10[] = {
		1ULL,
		10ULL,
		100ULL,
		1000ULL,
		10000ULL,
		100000ULL,
		1000000ULL,
		10000000ULL,
		100000000ULL,
		1000000000ULL,
		10000000000ULL,
		100000000000ULL,
		1000000000000ULL,
		10000000000000ULL,
		100000000000000ULL,
		1000000000000000ULL,
		10000000000000000ULL,
		100000000000000000ULL,
		1000000000000000000ULL,
		10000000000000000000ULL
	};

public:
	constexpr decimal() = default;
	constexpr decimal(const decimal&) = default;
	constexpr decimal& operator =(const decimal&) = default;

	constexpr decimal(base_construct_t, const value_type & v)
		: value_(v) {
	}

	constexpr decimal(const value_type& v)
		: value_(v * pow10[Scale]) {
	}

	constexpr decimal(double v)
		: value_(v* pow10[Scale]) {
	}

	constexpr explicit operator bool() const {
		return value_type(0) != value_;
	}

	constexpr explicit operator value_type() const {
		return value_ / pow10[Scale];
	}

	constexpr explicit operator double() const {
		return double(value_) / double(pow10[Scale]);
	}

	template<class T, size_t N>
	constexpr explicit operator decimal<T, N>() const {
		using alg_t = std::common_type_t<T, value_type>;
		if constexpr (N == Scale)
			return { base_construct, T(value_) };
		else if constexpr (N < Scale)
			return { base_construct, T(alg_t(value_) / alg_t(pow10[Scale - N])) };
		else
			return { base_construct, T(alg_t(value_) * alg_t(pow10[N - Scale])) };
	}

	constexpr value_type base() const {
		return value_;
	}

	constexpr size_t places() const {
		return Scale;
	}

	constexpr bool operator ==(const decimal&) const = default;
	constexpr bool operator !=(const decimal&) const = default;
	constexpr bool operator >(const decimal&) const = default;
	constexpr bool operator <(const decimal&) const = default;
	constexpr bool operator >=(const decimal&) const = default;
	constexpr bool operator <=(const decimal&) const = default;
	constexpr std::strong_ordering operator <=>(const decimal&) const = default;

	constexpr decimal operator ~() const { return { base_construct, ~value_ }; }
	constexpr decimal operator &(const decimal& x) const { return decimal(*this) &= x; }
	constexpr decimal operator |(const decimal& x) const { return decimal(*this) |= x; }
	constexpr decimal operator ^(const decimal& x) const { return decimal(*this) ^= x; }
	constexpr decimal operator <<(size_t n) const { return decimal(*this) <<= n; }
	constexpr decimal operator >>(size_t n) const { return decimal(*this) >>= n; }

	constexpr decimal& operator &=(const decimal& x) { value_ &= x.value_; return *this; }
	constexpr decimal& operator |=(const decimal& x) { value_ |= x.value_; return *this; }
	constexpr decimal& operator ^=(const decimal& x) { value_ ^= x.value_; return *this; }

	constexpr decimal& operator <<=(size_t n) {
#ifdef DLOU_CHECK_ARGS
		if (n >= std::extent_v<decltype(pow10)>)
			value_ = 0;
		else
#endif
			value_ *= pow10[n];
		return *this;
	}

	constexpr decimal& operator >>=(size_t n) {
#ifdef DLOU_CHECK_ARGS
		if (n >= std::extent_v<decltype(pow10)>)
			value_ = 0;
		else
#endif
			value_ /= pow10[n];
		return *this;
	}

	constexpr decimal operator -() const { return { base_construct, -value_ }; }
	constexpr decimal operator +(const decimal& x) const { return decimal(*this) += x; }
	constexpr decimal operator -(const decimal& x) const { return decimal(*this) -= x; }
	constexpr decimal operator *(const decimal& x) const { return decimal(*this) *= x; }
	constexpr decimal operator /(const decimal& x) const { return decimal(*this) /= x; }

	constexpr decimal& operator +=(const decimal& x) { value_ += x.value_; return *this; }
	constexpr decimal& operator -=(const decimal& x) { value_ -= x.value_; return *this; }

	template<class T, size_t N>
	constexpr decimal& operator *=(const decimal<T, N>& x) {
		auto r = math::mul(base(), x.base());
		if constexpr (std::is_integral_v<decltype(r)>) {
			value_ = (value_type&&)(r /= pow10[N]);
		}
		else {
			value_ = (r == r.lo)
				? (r.lo / (value_type)pow10[N])
				: (r /= (value_type)pow10[N]).lo;
		}
		return *this;
	}

	template<class T, size_t N>
	constexpr decimal& operator /=(const decimal<T, N>& x) {
		auto r = math::mul(base(), (T)pow10[N]);
		if constexpr (std::is_integral_v<decltype(r)>) {
			value_ = (value_type&&)(r / x.base());
		}
		else {
			value_ = (r == r.lo)
				? (r.lo / x.base())
				: (r /= x.base()).lo;
		}
		return *this;
	}

	//constexpr decimal operator %(const decimal& x) const { return decimal(*this) %= x; }
	//constexpr decimal& operator %=(const decimal& x) { }

	//constexpr decimal& operator ++() {}
	//constexpr decimal& operator --() {}
	//constexpr decimal operator ++(int) {}
	//constexpr decimal operator --(int) {}

};

} // namespace dlou
