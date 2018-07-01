#pragma once

#include <utility>

namespace polymesh
{
// small template metaprogramming
namespace tmp
{
template <class FuncT, class ArgT>
struct decayed_result_of
{
    using type = typename std::decay<decltype(std::declval<FuncT>()(std::declval<ArgT>()))>::type;
};

template <class FuncT, class ArgT>
using decayed_result_type_of = typename decayed_result_of<FuncT, ArgT>::type;

template <class FuncT, class ArgT>
struct result_of
{
    using type = decltype(std::declval<FuncT>()(std::declval<ArgT>()));
};

template <class FuncT, class ArgT>
using result_type_of = typename result_of<FuncT, ArgT>::type;
}
}
