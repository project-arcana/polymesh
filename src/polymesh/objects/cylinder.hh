#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh::objects
{
/// Adds a (tessellated) cylinder to the given mesh
/// qf is called with (v, x, y), with vertex handle v and (x,y) from 0..1 (row-by-row)
/// x is tesselated from 0..1
/// y is either 0 or 1
/// returns the one of the new vertices
/// NOTE: the result is NOT triangulated!
template <class CylinderF>
auto add_cylinder(Mesh& m, CylinderF&& qf, int segments, bool closed = true) -> decltype(qf(vertex_handle{}, float{}, float{}), vertex_handle{});

// ======== IMPLEMENTATION ========

template <class CylinderF>
auto add_cylinder(Mesh& m, CylinderF&& qf, int segments, bool closed) -> decltype(qf(vertex_handle{}, float{}, float{}), vertex_handle{})
{
    POLYMESH_ASSERT(segments > 2);

    std::vector<vertex_index> v_top(segments);
    std::vector<vertex_index> v_bot(segments);

    for (auto i = 0; i < segments; ++i)
    {
        {
            auto v = m.vertices().add();
            v_top[i] = v;
            qf(v, i / float(segments), 0.0f);
        }
        {
            auto v = m.vertices().add();
            v_bot[i] = v;
            qf(v, i / float(segments), 1.0f);
        }
    }

    for (auto i = 0; i < segments; ++i)
    {
        auto v00 = v_top[i].of(m);
        auto v10 = v_top[(i + 1) % segments].of(m);
        auto v01 = v_bot[i].of(m);
        auto v11 = v_bot[(i + 1) % segments].of(m);

        m.faces().add(v00, v01, v11, v10);
    }

    if (closed)
    {
        m.faces().add(v_top);

        reverse(begin(v_bot), end(v_bot));
        m.faces().add(v_bot);
    }

    return v_top[0].of(m);
}
}
