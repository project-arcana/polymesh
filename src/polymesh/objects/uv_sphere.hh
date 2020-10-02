#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh::objects
{
/// Adds a (tessellated) uv_sphere to the given mesh
/// Top and bottom vertex is shared
/// qf is called with (v, x, y), with vertex handle v and (x,y) from 0..1 (row-by-row)
/// top and bottom are generated with (0,0) and (0,1) respectively
/// returns the one of the new vertices
/// NOTE: the result is NOT triangulated!
template <class SphereF>
auto add_uv_sphere(Mesh& m, SphereF&& qf, int cnt_longitude, int cnt_latitude) -> decltype(qf(vertex_handle{}, float{}, float{}), vertex_handle{});

// ======== IMPLEMENTATION ========

template <class SphereF>
auto add_uv_sphere(Mesh& m, SphereF&& qf, int cnt_longitude, int cnt_latitude) -> decltype(qf(vertex_handle{}, float{}, float{}), vertex_handle{})
{
    // TODO: this is topologically not exactly correct I think

    POLYMESH_ASSERT(cnt_latitude > 1 && cnt_longitude > 2);

    std::vector<vertex_index> verts((cnt_latitude + 1) * (cnt_longitude + 1));

    auto v_top = m.vertices().add();
    auto v_bot = m.vertices().add();

    qf(v_top, 0.0f, 0.0f);
    qf(v_bot, 0.0f, 1.0f);

    auto i = 0;
    for (auto y = 0; y <= cnt_latitude; ++y)
        for (auto x = 0; x <= cnt_longitude; ++x)
        {
            if (y == 0)
            {
                verts[i] = v_top;
            }
            else if (y == cnt_latitude)
            {
                verts[i] = v_bot;
            }
            else
            {
                auto pu = x / float(cnt_longitude);
                auto pv = y / float(cnt_latitude);

                auto v = m.vertices().add();
                verts[i] = v;

                qf(v, pu, pv);
            }
            ++i;
        }

    for (auto y = 0; y < cnt_latitude; ++y)
        for (auto x = 0; x <= cnt_longitude; ++x)
        {
            auto v00 = verts[(y + 0) * (cnt_longitude + 1) + (x + 0) % (cnt_longitude + 1)].of(m);
            auto v10 = verts[(y + 0) * (cnt_longitude + 1) + (x + 1) % (cnt_longitude + 1)].of(m);
            auto v01 = verts[(y + 1) * (cnt_longitude + 1) + (x + 0) % (cnt_longitude + 1)].of(m);
            auto v11 = verts[(y + 1) * (cnt_longitude + 1) + (x + 1) % (cnt_longitude + 1)].of(m);

            if (v00 == v10)
                m.faces().add(v00, v01, v11);
            else if (v01 == v11)
                m.faces().add(v00, v11, v10);
            else
                m.faces().add(v00, v01, v11, v10);
        }

    return v_top;
}
}
