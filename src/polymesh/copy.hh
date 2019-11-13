#pragma once

#include "Mesh.hh"

#include <tuple>

namespace polymesh
{
/// copies a mesh and the given attributes (new versions are returned)
///
/// e.g.:
///   auto [m2, pos2] = copy(m, pos);
template <class... Attrs>
std::tuple<unique_ptr<Mesh>, Attrs...> copy(Mesh const& m, Attrs const&... attrs)
{
    auto m2 = m.copy();
    auto const& m2_ref = *m2;
    return std::make_tuple(std::move(m2), attrs.copy_to(m2_ref)...);
}
}
