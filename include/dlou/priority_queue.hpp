#pragma once

#include "macro.hpp"
#include "static.hpp"
#include "leftist_heap.hpp"

#include <cstddef>

#include <type_traits>
#include <functional>
#ifdef __cpp_concepts
#include <concepts>
#endif

namespace dlou {

namespace _priority_queue {
	template<auto MemberObjPtr>
	using node_t = member_t<decltype(MemberObjPtr)>;

	template<auto MemberObjPtr>
	using key_t = node_key_t<node_t<MemberObjPtr>>;

#ifdef __cpp_concepts
	template<class Compare, auto MemberObjPtr>
	concept predicate = std::predicate<Compare, node_key_t<member_t<decltype(MemberObjPtr)>>, node_key_t<member_t<decltype(MemberObjPtr)>>>;

	template<class Container>
	concept basic_heap = requires (Container heap)
	{
		typename Container::node;
		std::default_initializable<Container>;
		std::move_constructible<Container>;
		{ heap.swap(heap) };
		{ heap.empty() } -> std::same_as<bool>;
		{ heap.merge(heap) };
		{ heap.peek() } -> std::same_as<const typename Container::node*>;
		{ heap.push((typename Container::node*)nullptr) };
		{ heap.pop() } -> std::same_as<typename Container::node*>;
	};
#endif//__cpp_concepts
} // namespace _priority_queue


template<
	auto MemberObjPtr,
	class Compare = std::less<_priority_queue::key_t<MemberObjPtr>>,
	class Container = leftist_heap<_priority_queue::key_t<MemberObjPtr>, Compare>>
DLOU_REQUIRES(
	std::is_member_object_pointer_v<decltype(MemberObjPtr)> && MemberObjPtr != nullptr &&
	_priority_queue::predicate<Compare, MemberObjPtr> &&
	_priority_queue::basic_heap<Container> &&
	std::is_same_v<typename Container::node, _priority_queue::node_t<MemberObjPtr>>)
class priority_queue
{
public:
	using member_object_type = decltype(MemberObjPtr);
	using key_compare = Compare;
	using container_type = Container;
	using value_type = remove_member_t<member_object_type>;
	using node = member_t<member_object_type>;
	using key_type = node_key_t<node>;

protected:
	static node* to_node(const value_type* p) {
		return &(const_cast<value_type*>(p)->*MemberObjPtr);
	}

	static value_type* to_object(const node* p) {
		return (value_type*)((char*)p - (ptrdiff_t)(&(((value_type*)0)->*MemberObjPtr)));
	}

	priority_queue(const priority_queue&) = delete;
	priority_queue& operator =(const priority_queue&) = delete;
	priority_queue& operator =(priority_queue&&) = delete;

public:
	priority_queue() = default;
	priority_queue(priority_queue&&) = default;

	container_type& base() { return heap_; }
	const container_type& base() const { return heap_; }

	void swap(priority_queue& x) { heap_.swap(x.heap_); }
	bool empty() const { return heap_.empty(); }
	void merge(priority_queue& x) { heap_.merge(x.heap_); }

	value_type* peek() const { return to_object(heap_.peek()); }
	void push(value_type* p) { heap_.push(to_node(p)); }
	value_type* pop() { return to_object(heap_.pop()); }

private:
	container_type heap_;
};

} // namespace dlou
