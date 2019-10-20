#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh::detail
{
bool add_delaunay_triangulation_delabella(Mesh& m, float const* pos);
}
