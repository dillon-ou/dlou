#pragma once

#include <type_traits>
#include <iterator>


namespace dlou {

template<class Iterator
	, class Category = typename Iterator::iterator_category>
class basic_const_iterator
{
	template<class T, class U>
	friend T& iterator_cast(basic_const_iterator<T, U>&);

	Iterator iter_;
public:
	using iterator_type = Iterator;
	using iterator_category = typename iterator_type::iterator_category;
	using difference_type = typename iterator_type::difference_type;
	using value_type = const typename iterator_type::value_type;
	using pointer = value_type*;
	using reference = value_type&;

public:
	constexpr basic_const_iterator(const basic_const_iterator&) = default;
	constexpr basic_const_iterator& operator =(const basic_const_iterator&) = default;
	constexpr basic_const_iterator(basic_const_iterator&&) = default;
	constexpr basic_const_iterator& operator =(basic_const_iterator&&) = default;

	constexpr basic_const_iterator(const iterator_type& it) : iter_(it) {}

	constexpr basic_const_iterator& operator ++() { ++iter_; return *this; }
	constexpr basic_const_iterator operator ++(int) { auto ret = *this; ++iter_; return ret; }
};

template<class Iterator>
class basic_const_iterator<Iterator, std::input_iterator_tag>
{
	template<class T, class U>
	friend T& iterator_cast(basic_const_iterator<T, U>&);

	Iterator iter_;
public:
	using iterator_type = Iterator;
	using iterator_category = typename iterator_type::iterator_category;
	using difference_type = typename iterator_type::difference_type;
	using value_type = const typename iterator_type::value_type;
	using pointer = value_type*;
	using reference = value_type&;

public:
	constexpr basic_const_iterator(const basic_const_iterator&) = default;
	constexpr basic_const_iterator& operator =(const basic_const_iterator&) = default;
	constexpr basic_const_iterator(basic_const_iterator&&) = default;
	constexpr basic_const_iterator& operator =(basic_const_iterator&&) = default;

	constexpr basic_const_iterator(const iterator_type& it) : iter_(it) {}

	constexpr bool operator ==(const Iterator& x) const { return iter_ == x; }
	constexpr bool operator !=(const Iterator& x) const { return iter_ != x; }

	constexpr bool operator ==(const basic_const_iterator& x) const { return iter_ == x.iter_; }
	constexpr bool operator !=(const basic_const_iterator& x) const { return iter_ != x.iter_; }

	constexpr reference operator *() const { return *iter_; }
	constexpr pointer operator ->() const { return &*iter_; }

	constexpr basic_const_iterator& operator ++() { ++iter_; return *this; }
	constexpr basic_const_iterator operator ++(int) { auto ret = *this; ++iter_; return ret; }
};

template<class Iterator>
class basic_const_iterator<Iterator, std::forward_iterator_tag>
{
	template<class T, class U>
	friend T& iterator_cast(basic_const_iterator<T, U>&);

	Iterator iter_;
public:
	using iterator_type = Iterator;
	using iterator_category = typename iterator_type::iterator_category;
	using difference_type = typename iterator_type::difference_type;
	using value_type = const typename iterator_type::value_type;
	using pointer = value_type*;
	using reference = value_type&;

public:
	constexpr basic_const_iterator() = default;
	constexpr basic_const_iterator(const basic_const_iterator&) = default;
	constexpr basic_const_iterator& operator =(const basic_const_iterator&) = default;
	constexpr basic_const_iterator(basic_const_iterator&&) = default;
	constexpr basic_const_iterator& operator =(basic_const_iterator&&) = default;

	constexpr basic_const_iterator(const iterator_type & it) : iter_(it) {}

	constexpr bool operator ==(const Iterator & x) const { return iter_ == x; }
	constexpr bool operator !=(const Iterator & x) const { return iter_ != x; }

	constexpr bool operator ==(const basic_const_iterator & x) const { return iter_ == x.iter_; }
	constexpr bool operator !=(const basic_const_iterator & x) const { return iter_ != x.iter_; }

	constexpr reference operator *() const { return *iter_; }
	constexpr pointer operator ->() const { return &*iter_; }

	constexpr basic_const_iterator& operator ++() { ++iter_; return *this; }
	constexpr basic_const_iterator operator ++(int) { auto ret = *this; ++iter_; return ret; }
};

template<class Iterator>
class basic_const_iterator<Iterator, std::bidirectional_iterator_tag>
{
	template<class T, class U>
	friend T& iterator_cast(basic_const_iterator<T, U>&);

	Iterator iter_;
public:
	using iterator_type = Iterator;
	using iterator_category = typename iterator_type::iterator_category;
	using difference_type = typename iterator_type::difference_type;
	using value_type = const typename iterator_type::value_type;
	using pointer = value_type*;
	using reference = value_type&;

public:
	constexpr basic_const_iterator() = default;
	constexpr basic_const_iterator(const basic_const_iterator&) = default;
	constexpr basic_const_iterator& operator =(const basic_const_iterator&) = default;
	constexpr basic_const_iterator(basic_const_iterator&&) = default;
	constexpr basic_const_iterator& operator =(basic_const_iterator&&) = default;

	constexpr basic_const_iterator(const iterator_type & it) : iter_(it) {}

	constexpr bool operator ==(const Iterator & x) const { return iter_ == x; }
	constexpr bool operator !=(const Iterator & x) const { return iter_ != x; }

	constexpr bool operator ==(const basic_const_iterator & x) const { return iter_ == x.iter_; }
	constexpr bool operator !=(const basic_const_iterator & x) const { return iter_ != x.iter_; }

	constexpr reference operator *() const { return *iter_; }
	constexpr pointer operator ->() const { return &*iter_; }

	constexpr basic_const_iterator& operator ++() { ++iter_; return *this; }
	constexpr basic_const_iterator operator ++(int) { auto ret = *this; ++iter_; return ret; }
	constexpr basic_const_iterator& operator --() { --iter_; return *this; }
	constexpr basic_const_iterator operator --(int) { auto ret = *this; --iter_; return ret; }
};

template<class Iterator>
class basic_const_iterator<Iterator, std::random_access_iterator_tag>
{
	template<class T, class U>
	friend T& iterator_cast(basic_const_iterator<T, U>&);

	Iterator iter_;
public:
	using iterator_type = Iterator;
	using iterator_category = typename iterator_type::iterator_category;
	using difference_type = typename iterator_type::difference_type;
	using value_type = const typename iterator_type::value_type;
	using pointer = value_type*;
	using reference = value_type&;

public:
	constexpr basic_const_iterator() = default;
	constexpr basic_const_iterator(const basic_const_iterator&) = default;
	constexpr basic_const_iterator& operator =(const basic_const_iterator&) = default;
	constexpr basic_const_iterator(basic_const_iterator&&) = default;
	constexpr basic_const_iterator& operator =(basic_const_iterator&&) = default;

	constexpr basic_const_iterator(const iterator_type & it) : iter_(it) {}

	constexpr bool operator ==(const Iterator& x) const { return iter_ == x; }
	constexpr bool operator !=(const Iterator & x) const { return iter_ != x; }

	constexpr bool operator ==(const basic_const_iterator& x) const { return iter_ == x.iter_; }
	constexpr bool operator !=(const basic_const_iterator & x) const { return iter_ != x.iter_; }

	constexpr reference operator *() const { return *iter_; }
	constexpr pointer operator ->() const { return &*iter_; }

	constexpr basic_const_iterator& operator ++() { ++iter_; return *this; }
	constexpr basic_const_iterator operator ++(int) { auto ret = *this; ++iter_; return ret; }
	constexpr basic_const_iterator& operator --() { --iter_; return *this; }
	constexpr basic_const_iterator operator --(int) { auto ret = *this; --iter_; return ret; }

	constexpr basic_const_iterator operator +(difference_type n) const { return iter_ + n; }
	constexpr basic_const_iterator operator -(difference_type n) const { return iter_ - n; }
	constexpr difference_type operator -(const basic_const_iterator& x) const { return iter_ - x.iter_; }

	constexpr bool operator <(const basic_const_iterator& x) const { return iter_ < x.iter_; }
	constexpr bool operator >(const basic_const_iterator& x) const { return iter_ > x.iter_; }
	constexpr bool operator <=(const basic_const_iterator& x) const { return iter_ <= x.iter_; }
	constexpr bool operator >=(const basic_const_iterator& x) const { return iter_ >= x.iter_; }

	constexpr basic_const_iterator& operator +=(difference_type n) { iter_ += n; return *this; }
	constexpr basic_const_iterator& operator -=(difference_type n) { iter_ -= n; return *this; }

	constexpr reference operator [](difference_type n) const { return iter_[n]; }
};

namespace _const_iterator {
	template<class Iterator>
	struct convert {
		using type = basic_const_iterator<Iterator>;
	};

	template<class Iterator>
	struct convert<basic_const_iterator<Iterator>> {
		using type = basic_const_iterator<Iterator>;
	};

	template<class Iterator>
	struct convert<Iterator*> {
		using type = const Iterator*;
	};
} // namespace _const_iterator

template<class Iterator>
using const_iterator = typename _const_iterator::convert<std::remove_cvref_t<Iterator>>::type;

template<class Iterator, class Category>
Iterator& iterator_cast(basic_const_iterator<Iterator, Category>& x)
{
	return x.iter_;
}

template<class Iterator, class Category>
const Iterator& iterator_cast(const basic_const_iterator<Iterator, Category>& x)
{
	return iterator_cast<Iterator, Category>(const_cast<basic_const_iterator<Iterator, Category>&>(x));
}

template<class Iterator>
Iterator* iterator_cast(const Iterator* p)
{
	return const_cast<Iterator*>(p);
}

} // namespace dlou

template<class Iterator, class Category>
constexpr inline bool operator ==(const Iterator& x, dlou::basic_const_iterator<Iterator, Category>& y)
{
	return y == x;
}

template<class Iterator, class Category>
constexpr inline bool operator !=(const Iterator& x, dlou::basic_const_iterator<Iterator, Category>& y)
{
	return y != x;
}
