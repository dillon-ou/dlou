#pragma once

/* Preprocessor Configure

#define DLOU_NO_ALIAS
#define DLOU_CHECK_ARGS

*/

#ifndef __cpp_constexpr
#error since C++11
#endif

#ifndef __cpp_if_constexpr
#error since C++17
#endif

#ifdef __cpp_if_consteval
#define DLOU_IS_CONSTEVAL consteval
#else
#define DLOU_IS_CONSTEVAL (std::is_constant_evaluated())
#endif

#ifdef __cpp_concepts
#define DLOU_REQUIRES(...) requires (##__VA_ARGS__)
#else
#define DLOU_REQUIRES(...)
#endif

#ifdef DLOU_NO_ALIAS

#define DLOU_DEFINED_ALIAS_FUNCTION(...)
#define DLOU_DEFINED_ALIAS_TEMPLATE_FUNCTION(...)

#else//DLOU_NO_ALIAS

#include <utility>
// reference
// std::forward

/*
ex:
 namespace foo {
  [conste(xpr|val)] auto bar(...);
 OR:
  template<class... Types>
  [conste(xpr|val)] auto bar(Types...) {}
 } // namespace foo
 DLOU_DEFINED_ALIAS_FUNCTION(baz, foo::bar)
*/
#define DLOU_DEFINED_ALIAS_FUNCTION(name, target) \
template<typename... Types> \
constexpr auto name(Types&&... args) -> decltype(target(std::forward<Types>(args)...)) { \
	return target(std::forward<Types>(args)...); \
}

/*
ex:
 namespace foo {
  template<class T>
  [conste(xpr|val)] T bar(...) {}
 } // namespace foo
 DLOU_DEFINED_ALIAS_TEMPLATE_FUNCTION(baz, foo::bar)
*/
#define DLOU_DEFINED_ALIAS_TEMPLATE_FUNCTION(name, target) \
namespace alias { \
template<typename... Types> \
struct name { \
	template<typename... Args> \
	constexpr auto operator()(Args&&... args) const { \
		return target<Types...>(std::forward<Args>(args)...); \
	} \
}; \
} \
template<typename... Types> \
static constexpr auto name = alias::name<Types...>{};

#endif//DLOU_NO_ALIAS
