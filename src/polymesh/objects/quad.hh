#pragma once

#include "../Mesh.hh"

namespace polymesh
{
namespace objects
{
/// Adds a (tessellated) quad to the given mesh
/// qf is called with (v, x, y), with vertex handle v and (x,y) from 0..1 (row-by-row)
/// (w, h) is the number of sub-quads in each dimension
/// returns the one of the new vertices (usually the first)
/// NOTE: the result is NOT triangulated!
template <class QuadF>
vertex_handle add_quad(Mesh& m, QuadF&& qf, int w = 1, int h = 1);

/// ======== IMPLEMENTATION ========

template <class QuadF>
vertex_handle add_quad(Mesh& m, QuadF&& qf, int w, int h)
{
    assert(w > 0 && h > 0);
    std::vector<vertex_index> verts((w + 1) * (h + 1));

    auto i = 0;
    for (auto y = 0; y <= h; ++y)
        for (auto x = 0; x <= w; ++x)
        {
            auto pu = x / float(w);
            auto pv = y / float(h);

            auto v = m.vertices().add();
            verts[i] = v;

            qf(v, pu, pv);
            ++i;
        }

    for (auto y = 0; y < h; ++y)
        for (auto x = 0; x < w; ++x)
        {
            auto v00 = verts[(y + 0) * (w + 1) + (x + 0)].of(m);
            auto v10 = verts[(y + 0) * (w + 1) + (x + 1)].of(m);
            auto v01 = verts[(y + 1) * (w + 1) + (x + 0)].of(m);
            auto v11 = verts[(y + 1) * (w + 1) + (x + 1)].of(m);

            m.faces().add(v00, v01, v11, v10);
        }

    return verts.front().of(m);
}
}
}
