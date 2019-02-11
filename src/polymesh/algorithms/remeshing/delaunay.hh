#pragma once

#include <cassert>

#include <polymesh/Mesh.hh>
#include <polymesh/algorithms/properties.hh>

namespace polymesh
{
/// Given a triangular mesh, performs edge flips until all flippable edges are delaunay
/// (extrinsic delaunay triangulation)
/// returns the number of flips
template <class Vec3>
int make_delaunay(Mesh& m, vertex_attribute<Vec3> const& position);

/// ======== IMPLEMENTATION ========

template <class Vec3>
int make_delaunay(Mesh& m, vertex_attribute<Vec3> const& position)
{
    auto flips = 0;
    std::vector<edge_index> queue;

    for (auto e : m.edges())
        queue.push_back(e);

    while (!queue.empty())
    {
        auto e = queue.back().of(m);
        queue.pop_back();

        assert(e.is_valid());
        assert(!e.is_removed());
        assert(e.vertexA() != e.vertexB());

        if (e.is_boundary())
            continue;

        if (is_delaunay(e, position))
            continue;

        if (valence(e.vertexA()) <= 2 || valence(e.vertexB()) <= 2)
            continue;

        queue.push_back(e.halfedgeA().next().edge());
        queue.push_back(e.halfedgeA().prev().edge());
        queue.push_back(e.halfedgeB().next().edge());
        queue.push_back(e.halfedgeB().prev().edge());

        m.edges().rotate_next(e);
        ++flips;
    }

    return flips;
}
} // namespace polymesh
