#pragma once

#include <type_traits>
#include <utility>
#include <memory>

namespace dlou {

template<class T>
class alignas(T) manual_object
{
	manual_object& operator =(const manual_object&) = delete;
	manual_object& operator =(manual_object&&) = delete;
public:
	constexpr manual_object() = default;
	template<class... Args>
	constexpr manual_object(Args&&... args) { construct(std::forward<Args>(args)...); }

	template<class... Args>
	constexpr void construct(Args&&... args) { std::construct_at(&get(), std::forward<Args>(args)...); }
	constexpr void destroy() { std::destroy_at(&get()); }

	constexpr T& get() { return *reinterpret_cast<T*>(buf_); }
	constexpr const T& get() const { return const_cast<manual_object*>(this)->get(); }

	constexpr T& operator *() { return get(); }
	constexpr const T& operator *() const { return get(); }

	constexpr T* operator ->() { return &get(); }
	constexpr const T* operator ->() const { return &get(); }

	constexpr operator T& () { return get(); }
	constexpr operator const T& () const { return get(); }

	template<class... Args>
	constexpr typename std::invoke_result_t<T&, Args...> operator ()(Args&&... args) { return get()(std::forward<Args>(args)...); }

	template<class... Args>
	constexpr typename std::invoke_result_t<T&, Args...> operator ()(Args&&... args) const { return get()(std::forward<Args>(args)...); }

private:
	char buf_[sizeof(T)];
};

template<class T>
class defer_object
	: protected manual_object<T>
{
public:
	using manual_object<T>::construct;
	using manual_object<T>::get;
	using manual_object<T>::operator *;
	using manual_object<T>::operator ->;
	using manual_object<T>::operator T*;
	using manual_object<T>::operator const T*;
	using manual_object<T>::operator ();

	constexpr defer_object() = default;
	template<class... Args>
	constexpr defer_object(Args&&... args) : manual_object<T>(std::forward<Args>(args)...) {}
	constexpr ~defer_object() { this->destroy(); }
};

} // namespace dlou
