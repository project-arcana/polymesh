#pragma once

#include <utility>

namespace polymesh
{
// small template metaprogramming
namespace tmp
{
template <class FuncT, class ArgT>
struct result_of
{
    using type = typename std::decay<decltype(std::declval<FuncT>()(std::declval<ArgT>()))>::type;
};

template <class FuncT, class ArgT>
using result_type_of = typename result_of<FuncT, ArgT>::type;
}
}
