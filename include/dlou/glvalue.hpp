#pragma once

namespace dlou {

template<class T>
class glvalue
{
	T& ref_;
public:
	using value_type = T;
	using reference = T&;
	using pointer = T*;

	glvalue() = delete;

	glvalue(T& x) : ref_(x) {}
	glvalue(T&& x) : ref_(x) {}

	glvalue(const glvalue&) = default;
	glvalue& operator =(const glvalue&) = default;

	reference operator *() const { return ref_; }
	pointer operator ->() const { return &ref_; }
};

} // namespace dlou
