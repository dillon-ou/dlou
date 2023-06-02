#pragma once

#include "macro.hpp"
#include "static.hpp"
#include "xor_linked_list.hpp"

#include <cstddef>

#include <type_traits>
#ifdef __cpp_concepts
#include <concepts>
#endif

namespace dlou {

namespace _deque {
	template<auto MemberObjPtr>
	using node_t = member_t<decltype(MemberObjPtr)>;

#ifdef __cpp_concepts
	template<class Container>
	concept basic_deque = requires (Container list, const Container clist, typename Container::node * node)
	{
		typename Container::node;
		std::default_initializable<Container>;
		std::move_constructible<Container>;
		{ list.swap(list) };
		{ list.empty() } -> std::same_as<bool>;
		{ clist.front() } -> std::same_as<const typename Container::node*>;
		{ clist.back() } -> std::same_as<const typename Container::node*>;
		{ list.push_front(node) };
		{ list.pop_front() } -> std::same_as<typename Container::node*>;
		{ list.push_back(node) };
		{ list.pop_back() } -> std::same_as<typename Container::node*>;
	};
#endif//__cpp_concepts
} // namespace _deque


template<
	auto MemberObjPtr,
	class Container = xor_linked_list>
DLOU_REQUIRES(
	std::is_member_object_pointer_v<decltype(MemberObjPtr)>&& MemberObjPtr != nullptr &&
	_deque::basic_deque<Container> &&
	std::is_same_v<typename Container::node, _deque::node_t<MemberObjPtr>>)
class deque
{
public:
	using member_object_type = decltype(MemberObjPtr);
	using container_type = Container;
	using value_type = remove_member_t<member_object_type>;
	using node = member_t<member_object_type>;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;

protected:
	static node* to_node(const_pointer p) {
		return &(const_cast<pointer>(p)->*MemberObjPtr);
	}

	static pointer to_object(const node* p) {
		return (pointer)((char*)p - (ptrdiff_t)(&(((pointer)0)->*MemberObjPtr)));
	}

public:
	deque() = default;
	deque(deque&&) = default;

	container_type& base() { return list_; }
	const container_type& base() const { return list_; }

	void swap(deque& x) { list_.swap(x.list_); }
	bool empty() const { return list_.empty(); }

	const_pointer front() const { return to_object(list_.front()); }
	// push
	void push_front(pointer p) { list_.push_front(to_node(p)); }
	// pop
	pointer pop_front() { return to_object(list_.pop_front()); }

	const_pointer back() const { return to_object(list_.back()); }
	// inject
	void push_back(pointer p) { list_.push_back(to_node(p)); }
	// eject
	pointer pop_back() { return to_object(list_.pop_back()); }

private:
	container_type list_;
};

} // namespace dlou
