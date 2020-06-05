#pragma once

#include <polymesh/Mesh.hh>
#include <polymesh/detail/math.hh>
#include <polymesh/detail/unique_array.hh>

namespace polymesh::detail
{
unique_array<pos3f> add_ico_sphere_impl(Mesh& m, int subdiv);
}

namespace polymesh::objects
{
/// Adds a (subdivided) ico_sphere to the given mesh
/// sf is called with (v, x, y, z), with vertex handle v and coordinates (x,y,z) on the unit sphere
template <class SphereF>
vertex_handle add_ico_sphere(Mesh& m, SphereF&& sf, int subdiv = 0)
{
    POLYMESH_ASSERT(subdiv >= 0);

    auto const last_vertex_count = m.all_vertices().size();
    auto const positions = detail::add_ico_sphere_impl(m, subdiv);
    auto const total_positions = 12 + 30 * subdiv + 20 * subdiv * (subdiv - 1) / 2;
    for (auto i = 0; i < total_positions; ++i)
    {
        sf(vertex_index(last_vertex_count + i).of(m), positions[i].x, positions[i].y, positions[i].z);
    }
    return vertex_index(last_vertex_count).of(m);
}
}
