#pragma once

#include "../properties.hh"

namespace polymesh
{
template <class CollectionT, class FuncT>
template <class Func2T>
auto readonly_property<CollectionT, FuncT>::view(Func2T&& f) const -> readonly_property<readonly_property<CollectionT, FuncT>, Func2T>
{
    return {*this, f};
}
}
