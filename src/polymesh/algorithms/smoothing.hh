#pragma once

#include <polymesh/Mesh.hh>
#include <polymesh/fields.hh>

namespace polymesh
{
/// Performs a single step of a per-vertex smoothing
/// WeightF: (halfedge_handle) -> weight
/// FactorF: (vertex_handle) -> factor
template <class Pos3, class WeightF = tmp::constant_rational<scalar_of<Pos3>, 1, 1>, class FactorF = tmp::constant_rational<scalar_of<Pos3>, 1, 2>>
vertex_attribute<Pos3> smoothing_iteration(vertex_attribute<Pos3> const& pos, WeightF&& weightF = {}, FactorF&& factorF = {})
{
    auto const& m = pos.mesh();
    return m.vertices().map([&](vertex_handle v) {
        auto const f = factorF(v);
        auto const p = pos[v];

        if (f != decltype(f)(0))
        {
            auto const dir = v.outgoing_halfedges().weighted_avg([&](halfedge_handle h) { return pos[h.vertex_to()] - p; }, weightF);
            return p + dir * f;
        }
        else
            return p;
    });
}
}
