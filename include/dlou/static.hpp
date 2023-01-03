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

} // namespace dlou
