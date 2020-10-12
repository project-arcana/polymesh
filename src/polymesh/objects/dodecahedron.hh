#pragma once

#include <polymesh/Mesh.hh>
#include <polymesh/detail/math.hh>
#include <polymesh/detail/unique_array.hh>
#include <polymesh/fields.hh>

namespace polymesh::detail
{
unique_array<pos3f> add_unit_dodecahedron_impl(Mesh& m);
}

namespace polymesh::objects
{
/// Adds a dodecahedron to the given mesh.
/// Note: This will not triangulate the mesh!
/// sf is called with (v, x, y, z), with vertex handle v and coordinates (x,y,z) on the unit sphere
template <class SphereF>
vertex_handle add_dodecahedron(Mesh& m, SphereF&& sf);

/// same as add_dodecahedron but directly fills a position attribute
template <class Pos3>
vertex_handle add_dodecahedron(Mesh& m, vertex_attribute<Pos3>& pos);

// ======== IMPLEMENTATION ========

template <class SphereF>
vertex_handle add_dodecahedron(Mesh& m, SphereF&& sf)
{
    auto const last_vertex_count = m.all_vertices().size();
    auto const positions = detail::add_unit_dodecahedron_impl(m);
    for (auto i = 0; i < 20; ++i)
    {
        sf(vertex_index(last_vertex_count + i).of(m), positions[i].x, positions[i].y, positions[i].z);
    }
    return vertex_index(last_vertex_count).of(m);
}

template <class Pos3>
auto add_dodecahedron(Mesh& m, vertex_attribute<Pos3>& pos) -> vertex_handle
{
    return add_dodecahedron(m, [&](vertex_handle v, float x, float y, float z) { pos[v] = field3<Pos3>::make_pos(x, y, z); });
}
}
