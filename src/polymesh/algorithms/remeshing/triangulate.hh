#pragma once

#include <polymesh/Mesh.hh>
#include <polymesh/detail/delaunay.hh>
#include <unordered_set>

// Basic mesh operations, including:
// - elementary subdivision
// - intersections

namespace polymesh
{
/// Given a flat polymesh with convex faces, naively triangulates all faces
void triangulate_naive(Mesh& m);

/// Given a 2d mesh filled with vertices, creates a delauny triangulation
template <class Pos2>
bool add_delauny_triangulation(Mesh& m, vertex_attribute<Pos2> const& position);

/// ======== IMPLEMENTATION ========

inline void triangulate_naive(Mesh& m)
{
    std::vector<vertex_handle> vs;
    for (auto f : m.faces())
    {
        vs.clear();
        f.vertices().into_vector(vs);

        if (vs.size() <= 3)
            continue;

        // remove
        m.faces().remove(f);

        // triangulate
        for (auto i = 2u; i < vs.size(); ++i)
            m.faces().add(vs[0], vs[i - 1], vs[i]);
    }
}

template <class Pos2>
bool add_delauny_triangulation(Mesh& m, vertex_attribute<Pos2> const& pos)
{
    POLYMESH_ASSERT(m.vertices().size() >= 3 && "Mesh must have at least 3 vertices");

    auto p = std::vector<float>(pos.size() * 2);
    for (auto i = 0u; i < pos.size(); ++i)
    {
        p[i * 2 + 0] = float(pos[vertex_index(i)][0]);
        p[i * 2 + 1] = float(pos[vertex_index(i)][1]);
    }

    return detail::add_delauny_triangulation_delabella(m, p.data());
}


} // namespace polymesh
