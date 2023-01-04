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

template<auto MemberObjPtr>
class mem_obj
{
public:
	using member_object_pointer = decltype(MemberObjPtr);
	using value_type = remove_member_t<member_object_pointer>;
	using member_type = member_t<member_object_pointer>;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;

	member_type& operator ()(reference r) const {
		return r.*MemberObjPtr;
	}

	const member_type& operator ()(const_reference r) const {
		return r.*MemberObjPtr;
	}

	member_type* operator ()(pointer p) const {
		return &(p->*MemberObjPtr);
	}

	const member_type* operator ()(const_pointer p) const {
		return &(p->*MemberObjPtr);
	}

	ptrdiff_t offset_of() const {
		return (ptrdiff_t)(&(((pointer)0)->*MemberObjPtr));
	}

	reference operator [](member_type& m) const {
		return *(pointer)(((char*)&m) - offset_of());
	}

	const_reference operator [](const member_type& m) const {
		return *(pointer)(((char*)&m) - offset_of());
	}

	pointer operator [](member_type* p) const {
		return (pointer)(((char*)p) - offset_of());
	}

	const_pointer operator [](const member_type* p) const {
		return (pointer)(((char*)p) - offset_of());
	}
};

} // namespace dlou
