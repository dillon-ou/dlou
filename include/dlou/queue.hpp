#pragma once

#include "macro.hpp"
#include "static.hpp"
#include "circular/singly_linked_list.hpp"

#include <cstddef>

#include <type_traits>
#ifdef __cpp_concepts
#include <concepts>
#endif

namespace dlou {

namespace _queue {
	template<auto MemberObjPtr>
	using node_t = member_t<decltype(MemberObjPtr)>;

#ifdef __cpp_concepts
	template<class Container>
	concept basic_queue = requires (Container list, typename Container::node * node)
	{
		typename Container::node;
		std::default_initializable<Container>;
		std::move_constructible<Container>;
		{ list.swap(list) };
		{ list.empty() } -> std::same_as<bool>;
		{ const_cast<const Container&>(list).front() } -> std::same_as<typename Container::node*>;
		{ const_cast<const Container&>(list).back() } -> std::same_as<typename Container::node*>;
		{ list.push_back(node) };
		{ list.pop_front() } -> std::same_as<typename Container::node*>;
	};
#endif//__cpp_concepts
} // namespace _queue


template<
	auto MemberObjPtr,
	class Container = circular::singly_linked_list>
DLOU_REQUIRES(
	std::is_member_object_pointer_v<decltype(MemberObjPtr)>&& MemberObjPtr != nullptr &&
	_queue::basic_queue<Container> &&
	std::is_same_v<typename Container::node, _queue::node_t<MemberObjPtr>>)
class queue
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
	queue() = default;
	queue(queue&&) = default;

	container_type& base() { return list_; }
	const container_type& base() const { return list_; }

	void swap(queue& x) { list_.swap(x.list_); }

	pointer front() const { return to_object(list_.front()); }
	pointer back() const { return to_object(list_.back()); }

	// push_back
	void push(pointer p) { list_.push_back(to_node(p)); }

	// pop_front
	pointer pop() { return to_object(list_.pop_front()); }

private:
	container_type list_;
};

} // namespace dlou
