#pragma once

#include <polymesh/Mesh.hh>
#include <polymesh/detail/delaunay.hh>
#include <polymesh/properties.hh>

namespace polymesh
{
/// Given a triangular mesh, performs edge flips until all flippable edges are delaunay
/// (extrinsic delaunay triangulation)
/// returns the number of flips
template <class Vec3>
int make_delaunay(Mesh& m, vertex_attribute<Vec3> const& position);

/// Given a 2d mesh filled with vertices, creates a delaunay triangulation
/// NOTE:
///     requires at least 3 vertices
///     mesh must not have edges are faces
template <class Pos2>
bool create_delaunay_triangulation(Mesh& m, vertex_attribute<Pos2> const& position);


// ======================== IMPLEMENTATION ========================

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

        POLYMESH_ASSERT(e.is_valid());
        POLYMESH_ASSERT(!e.is_removed());
        POLYMESH_ASSERT(e.vertexA() != e.vertexB());

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

template <class Pos2>
bool create_delaunay_triangulation(Mesh& m, vertex_attribute<Pos2> const& pos)
{
    POLYMESH_ASSERT(m.vertices().size() >= 3 && "Mesh must have at least 3 vertices");
    POLYMESH_ASSERT(m.faces().empty() && m.edges().empty() && "Mesh must only consist of vertices so far");

    auto p = std::vector<float>(pos.size() * 2);
    for (auto i = 0u; i < pos.size(); ++i)
    {
        p[i * 2 + 0] = float(pos[vertex_index(i)][0]);
        p[i * 2 + 1] = float(pos[vertex_index(i)][1]);
    }

    return detail::add_delaunay_triangulation_delabella(m, p.data());
}
}
