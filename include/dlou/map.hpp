#pragma once

#include "macro.hpp"
#include "static.hpp"
#include "const_iterator.hpp"
#include "red_black_tree.hpp"

#include <cstddef>

#include <type_traits>
#include <iterator>
#ifdef __cpp_concepts
#include <concepts>
#endif

namespace dlou {

namespace _map {
	template<auto MemberObjPtr>
	using node_t = member_t<decltype(MemberObjPtr)>;

	template<auto MemberObjPtr>
	using key_t = node_key_t<node_t<MemberObjPtr>>;
	
#ifdef __cpp_concepts
	template<class Container>
	concept basic_map = requires (Container map, typename Container::node* node, typename Container::key_type key)
	{
		typename Container::node;
		typename Container::iterator;
		typename Container::reverse_iterator;
		std::default_initializable<Container>;
		std::move_constructible<Container>;
		{ map.swap(map) };
		{ map.empty() } -> std::same_as<bool>;
		{ map.front() } -> std::same_as<const typename Container::node*>;
		{ map.back() } -> std::same_as<const typename Container::node*>;
		{ map.insert(node) } -> std::same_as<typename Container::iterator>;
		{ map.erase(node) } -> std::same_as<typename Container::node*>;
		{ map.find(key) } -> std::same_as<typename Container::iterator>;
		{ map.lower_bound(key) } -> std::same_as<typename Container::iterator>;
		{ map.upper_bound(key) } -> std::same_as<typename Container::iterator>;
		{ map.begin() } -> std::same_as<typename Container::iterator>;
		{ map.end() } -> std::same_as<typename Container::iterator>;
		{ map.rbegin() } -> std::same_as<typename Container::reverse_iterator>;
		{ map.rend() } -> std::same_as<typename Container::reverse_iterator>;
	};
#endif//__cpp_concepts
} // namespace _map


template<
	auto MemberObjPtr,
	class Container = red_black_tree<_map::key_t<MemberObjPtr>>>
DLOU_REQUIRES(
	std::is_member_object_pointer_v<decltype(MemberObjPtr)>&& MemberObjPtr != nullptr &&
	_map::basic_map<Container> &&
	std::is_same_v<typename Container::node, _map::node_t<MemberObjPtr>>)
class map
{
public:
	using member_object_type = decltype(MemberObjPtr);
	using container_type = Container;
	using value_type = remove_member_t<member_object_type>;
	using key_type = typename container_type::key_type;
	using node = typename container_type::node;
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
	template<class Iterator>
	class basic_iterator
	{
	public:
		using iterator_type = Iterator;
		using iterator_category = typename iterator_type::iterator_category;
		using difference_type = typename iterator_type::difference_type;
		using value_type = typename map::value_type;
		using pointer = value_type*;
		using reference = value_type&;

	private:
		iterator_type iter_;

	public:
		constexpr basic_iterator() = default;
		constexpr basic_iterator(const basic_iterator&) = default;
		constexpr basic_iterator& operator =(const basic_iterator&) = default;
		constexpr basic_iterator(basic_iterator&&) = default;
		constexpr basic_iterator& operator =(basic_iterator&&) = default;

		constexpr basic_iterator(const iterator_type& it) : iter_(it) {}

		constexpr bool operator ==(const basic_iterator& x) const { return iter_ == x.iter_; }
		constexpr bool operator !=(const basic_iterator& x) const { return iter_ != x.iter_; }

		constexpr reference operator *() const { return *to_object(&*iter_); }
		constexpr pointer operator ->() const { return to_object(&*iter_); }

		constexpr basic_iterator& operator ++() { ++iter_; return *this; }
		constexpr basic_iterator operator ++(int) { auto ret = *this; ++iter_; return ret; }
		constexpr basic_iterator& operator --() { --iter_; return *this; }
		constexpr basic_iterator operator --(int) { auto ret = *this; --iter_; return ret; }
	};

	using iterator = basic_iterator<typename container_type::iterator>;
	using const_iterator = basic_const_iterator<iterator>;
	using reverse_iterator = basic_iterator<typename container_type::reverse_iterator>;
	using const_reverse_iterator = basic_const_iterator<reverse_iterator>;

public:
	map() = default;
	map(map&&) = default;

	container_type& base() { return map_; }
	const container_type& base() const { return map_; }

	void swap(map& x) { map_.swap(x.map_); }

	iterator insert(pointer p) { return map_.insert(to_node(p)); }
	pointer erase(const_pointer p) { return to_object(map_.erase(to_node(p))); }
	pointer erase(const_iterator it) {
		auto ret = const_cast<pointer>(&*it);
		map_.erase(to_node(ret));
		return ret;
	}
	pointer erase(const key_type& k) {
		auto it = map_.find(k);
		return (map_.end() != it) ? to_object(map_.erase(&*it)) : nullptr;
	}

	const_iterator find(const key_type& k) const { return map_.find(k); }
	const_iterator lower_bound(const key_type& k) const { return map_.lower_bound(k); }
	const_iterator upper_bound(const key_type& k) const { return map_.upper_bound(k); }

	iterator find(const key_type& k) { return map_.find(k); }
	iterator lower_bound(const key_type& k) { return map_.lower_bound(k); }
	iterator upper_bound(const key_type& k) { return map_.upper_bound(k); }

	const_pointer fornt() const { return to_object(map_.fornt()); }
	const_pointer back() const { return to_object(map_.back()); }

	pointer fornt() { return to_object(map_.fornt()); }
	pointer back() { return to_object(map_.back()); }

	iterator begin() { return map_.begin(); }
	iterator end() { return map_.end(); }
	reverse_iterator rbegin() { return map_.rbegin(); }
	reverse_iterator rend() { return map_.rend(); }

	const_iterator cbegin() const { return iterator(map_.begin()); }
	const_iterator cend() const { return iterator(map_.end()); }
	const_reverse_iterator crbegin() const { return reverse_iterator(map_.rbegin()); }
	const_reverse_iterator crend() const { return reverse_iterator(map_.rend()); }

	const_iterator begin() const { return cbegin(); }
	const_iterator end() const { return cend(); }
	const_reverse_iterator rbegin() const { return crbegin(); }
	const_reverse_iterator rend() const { return crend(); }

private:
	container_type map_;
};

} // namespace dlou
