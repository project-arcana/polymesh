#pragma once

#include "../Mesh.hh"

namespace polymesh
{
namespace objects
{
/// Adds a cube to the given mesh
/// cf is called with (v, x, y, z), with vertex handle v and (x,y,z) from 0..1 (int literals)
/// returns the one of the new vertices (usually the first)
/// NOTE: the result is NOT triangulated!
template <class CubeF>
vertex_handle add_cube(Mesh& m, CubeF&& cf);

/// ======== IMPLEMENTATION ========

template <class CubeF>
vertex_handle add_cube(Mesh& m, CubeF&& cf)
{
    auto v000 = m.vertices().add();
    auto v001 = m.vertices().add();
    auto v010 = m.vertices().add();
    auto v011 = m.vertices().add();
    auto v100 = m.vertices().add();
    auto v101 = m.vertices().add();
    auto v110 = m.vertices().add();
    auto v111 = m.vertices().add();

    m.faces().add(v000, v010, v110, v100);
    m.faces().add(v000, v100, v101, v001);
    m.faces().add(v000, v001, v011, v010);

    m.faces().add(v001, v101, v111, v011);
    m.faces().add(v010, v011, v111, v110);
    m.faces().add(v100, v110, v111, v101);

    cf(v000, 0, 0, 0);
    cf(v001, 0, 0, 1);
    cf(v010, 0, 1, 0);
    cf(v011, 0, 1, 1);
    cf(v100, 1, 0, 0);
    cf(v101, 1, 0, 1);
    cf(v110, 1, 1, 0);
    cf(v111, 1, 1, 1);

    return v000;
}
}
}
