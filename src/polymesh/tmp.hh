#pragma once

#include <cstddef>
#include <utility>

namespace polymesh
{
// small template metaprogramming
namespace tmp
{
template <class M, class T>
struct member_fun
{
    M T::*fun;
};

template <class FuncT>
struct decayed_result_of
{
    template <class ArgT>
    using type = typename std::decay<decltype(std::declval<FuncT>()(std::declval<ArgT>()))>::type;
};

template <class FuncT, class ArgT>
using decayed_result_type_of = typename decayed_result_of<FuncT>::template type<ArgT>;

template <class FuncT>
struct result_of
{
    template <class ArgT>
    using type = decltype(std::declval<FuncT>()(std::declval<ArgT>()));
};

template <class T>
struct dont_deduce_t
{
    using type = T;
};
template <class T>
using dont_deduce = typename dont_deduce_t<T>::type;

template <class FuncT, class ArgT>
using result_type_of = typename result_of<FuncT>::template type<ArgT>;

template <class T>
using enable_if_const_t = typename std::enable_if<std::is_const<T>::value, std::nullptr_t>::type;
template <class T>
using enable_if_mutable_t = typename std::enable_if<!std::is_const<T>::value, std::nullptr_t>::type;

template <bool Condition, class TrueType, class FalseType>
struct if_then_else;

template <class TrueType, class FalseType>
struct if_then_else<true, TrueType, FalseType>
{
    using result = TrueType;
};
template <class TrueType, class FalseType>
struct if_then_else<false, TrueType, FalseType>
{
    using result = FalseType;
};

template <class TargetT, class TestT>
using ref_if_mut = typename if_then_else<std::is_const<TestT>::value, TargetT, typename std::add_lvalue_reference<TargetT>::type>::result;
template <class TargetT, class TestT>
using cond_const_ref =
    typename if_then_else<std::is_const<TestT>::value, typename std::add_lvalue_reference<TargetT const>::type, typename std::add_lvalue_reference<TargetT>::type>::result;

struct identity
{
    template <typename T>
    decltype(auto) operator()(T&& x) const
    {
        return std::forward<T>(x);
    }
};

template <class T, class DivisorT>
struct can_divide_by
{
    template <class C>
    static bool test(decltype(std::declval<C>() / std::declval<DivisorT>())*);
    template <class C>
    static int test(...);

    enum
    {
        value = sizeof(test<T>(0)) == sizeof(bool)
    };
};

template <class ScalarT, int Nom, int Denom>
struct constant_rational
{
    template <class... Args>
    constexpr ScalarT operator()(Args&&...) const
    {
        return ScalarT(Nom) / ScalarT(Denom);
    }
};

namespace detail
{
template <class Container, class ElementT>
auto contiguous_range_test(int) -> decltype(static_cast<ElementT*>(std::declval<Container>().data()), //
                                            static_cast<size_t>(std::declval<Container>().size()),
                                            std::true_type{});
template <class Container, class ElementT>
std::false_type contiguous_range_test(char);

template <class Container, class ElementT, class = void>
struct is_range_t : std::false_type
{
};
template <class ElementT, size_t N>
struct is_range_t<ElementT[N], ElementT> : std::true_type
{
};
template <class ElementT, size_t N>
struct is_range_t<ElementT[N], ElementT const> : std::true_type
{
};
template <class ElementT, size_t N>
struct is_range_t<ElementT (&)[N], ElementT> : std::true_type
{
};
template <class ElementT, size_t N>
struct is_range_t<ElementT (&)[N], ElementT const> : std::true_type
{
};
template <class Container, class ElementT>
struct is_range_t<Container,
                  ElementT,
                  std::void_t<                                                              //
                      decltype(static_cast<ElementT&>(*std::declval<Container>().begin())), //
                      decltype(std::declval<Container>().end())                             //
                      >> : std::true_type
{
};
}

template <class Container, class ElementT>
static constexpr bool is_contiguous_range = decltype(detail::contiguous_range_test<Container, ElementT>(0))::value;

template <class Container, class ElementT>
static constexpr bool is_range = detail::is_range_t<Container, ElementT>::value;

} // namespace tmp
} // namespace polymesh
