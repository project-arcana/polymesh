#pragma once

#include <polymesh/Mesh.hh>

#include <polymesh/properties.hh>

namespace polymesh
{
/// Compute per-vertex normals (stored on halfedges) by averaging per-face normals and respecting hard edges.
/// is_hard_edge is a function (edge_handle) -> bool
/// Normals are not smoothed over edges where is_hard_edge is true
/// NOTE:
///   - is_hard_edge is never called for boundaries
///   - the normal belongs to halfedge.vertex_to()
template <class Vec3, class IsHardEdgeF>
[[nodiscard]] halfedge_attribute<Vec3> normal_estimation(face_attribute<Vec3> const& face_normals, IsHardEdgeF&& is_hard_edge)
{
    Mesh const& m = face_normals.mesh();

    auto const hard_edges = m.edges().map([&](edge_handle e) { return e.is_boundary() ? true : is_hard_edge(e); });

    return m.halfedges().map([&](halfedge_handle h) {
        Vec3 n = face_normals[h.face()];

        auto h0 = h;
        auto h1 = h.next().opposite();

        // iterate over h0
        auto hh = h0;
        while (hh != h1 && !hard_edges[hh])
        {
            hh = hh.opposite().prev();
            n += face_normals[hh.face()];
        }

        // iterate over h1 if not reached around
        if (hh != h1)
        {
            hh = h1;
            while (hh != h0 && !hard_edges[hh])
            {
                n += face_normals[hh.face()];
                hh = hh.next().opposite();
            }
        }

        // normalize
        auto l = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        return n / (l + 1e-30f);
    });
}

/// same as normal_estimation(face_attribute<Vec3>, ...)
/// but computes face normals first via pm::face_normals
template <class Pos3, class IsHardEdgeF>
[[nodiscard]] halfedge_attribute<typename field3<Pos3>::vec_t> normal_estimation(vertex_attribute<Pos3> const& pos, IsHardEdgeF&& is_hard_edge)
{
    return normal_estimation(pm::face_normals(pos), std::forward<IsHardEdgeF>(is_hard_edge));
}
}
