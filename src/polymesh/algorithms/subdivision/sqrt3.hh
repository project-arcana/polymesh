#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
/// Performs a uniform sqrt-3 subdivision step (topology only)
/// A provided function is called for each new vertex with handles (v_new, v0, v1, v2)
template <class VertexF>
void subdivide_sqrt3(Mesh& m, VertexF&& vf);

// ======== IMPLEMENTATION ========

template <class VertexF>
void subdivide_sqrt3(Mesh& m, VertexF&& vf)
{
    auto e_end = m.edges().end();

    for (auto f : m.faces())
    {
        auto h = f.any_halfedge();
        auto v0 = h.vertex_from();
        auto v1 = h.vertex_to();
        auto v2 = h.next().vertex_to();
        POLYMESH_ASSERT(h.next().next().vertex_to() == v0 && "must be triangle mesh");

        // split face
        auto v = m.faces().split(f);

        // call vf
        vf(v, v0, v1, v2);
    }

    // rotate old edges
    for (auto it = m.edges().begin(); it != e_end; ++it)
    {
        auto e = *it;

        if (e.is_boundary())
            continue;

        m.edges().rotate_next(e);
    }
}
}
