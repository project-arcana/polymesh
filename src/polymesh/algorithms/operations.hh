#pragma once

#include "../Mesh.hh"

#include <glm/glm.hpp>

// Basic mesh operations, including:
// - elementary subdivision
// - intersections

namespace polymesh
{
/// Given a flat polymesh with convex faces, naively triangulates all faces
void triangulate_naive(Mesh& m);

/// ======== IMPLEMENTATION ========

inline void triangulate_naive(Mesh& m)
{
    std::vector<vertex_handle> vs;
    for (auto f : m.faces())
    {
        vs = f.vertices().to_vector();
        if (vs.size() <= 3)
            continue;

        // remove
        m.faces().remove(f);

        // triangulate
        for (auto i = 2u; i < vs.size(); ++i)
            m.faces().add(vs[0], vs[1], vs[i]);
    }
}
}
