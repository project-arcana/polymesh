#pragma once

#include <memory>
#include <utility>

namespace polymesh
{
// small template metaprogramming
namespace tmp
{
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
    T operator()(T x) const
    {
        return x;
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

// std::add_lvalue_reference
// template <class T>

/// For C++11
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
} // namespace tmp
} // namespace polymesh
