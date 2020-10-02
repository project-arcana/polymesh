#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh::objects
{
/// Adds a (tessellated) cone to the given mesh
/// qf is called with (v, x, y), with vertex handle v and (x,y) from 0..1 (row-by-row)
/// x is tesselated from 0..1 and called with y = 0 (disk)
/// apex is called with (0, 1)
/// returns the one of the new vertices
/// NOTE: the result is NOT triangulated!
template <class coneF>
auto add_cone(Mesh& m, coneF&& qf, int segments, bool closed = true) -> decltype(qf(vertex_handle{}, float{}, float{}), vertex_handle{});

// ======== IMPLEMENTATION ========

template <class coneF>
auto add_cone(Mesh& m, coneF&& qf, int segments, bool closed) -> decltype(qf(vertex_handle{}, float{}, float{}), vertex_handle{})
{
    POLYMESH_ASSERT(segments > 2);

    auto v_top = m.vertices().add();
    qf(v_top, 0.0f, 1.0f);

    std::vector<vertex_index> v_bot(segments);

    for (auto i = 0; i < segments; ++i)
    {
        auto v = m.vertices().add();
        v_bot[i] = v;
        qf(v, i / float(segments), 0.0f);
    }

    for (auto i = 0; i < segments; ++i)
    {
        auto v01 = v_bot[i].of(m);
        auto v11 = v_bot[(i + 1) % segments].of(m);

        m.faces().add(v_top, v01, v11);
    }

    if (closed)
    {
        reverse(begin(v_bot), end(v_bot));
        m.faces().add(v_bot);
    }

    return v_top;
}
}
