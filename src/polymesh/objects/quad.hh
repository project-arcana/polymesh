#pragma once

#include <polymesh/Mesh.hh>

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
auto add_quad(Mesh& m, QuadF&& qf, int w = 1, int h = 1) -> decltype(qf(vertex_handle{}, float{}, float{}), vertex_handle{});

// ======== IMPLEMENTATION ========

template <class QuadF>
auto add_quad(Mesh& m, QuadF&& qf, int w, int h) -> decltype(qf(vertex_handle{}, float{}, float{}), vertex_handle{})
{
    POLYMESH_ASSERT(w > 0 && h > 0);

    // fast path
    if (w == 1 && h == 1)
    {
        auto v00 = m.vertices().add();
        auto v10 = m.vertices().add();
        auto v01 = m.vertices().add();
        auto v11 = m.vertices().add();

        qf(v00, 0.0f, 0.0f);
        qf(v10, 1.0f, 0.0f);
        qf(v01, 0.0f, 1.0f);
        qf(v11, 1.0f, 1.0f);

        m.faces().add(v00, v01, v11, v10);

        return v00;
    }
    else
    {
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
} // namespace objects
} // namespace polymesh
