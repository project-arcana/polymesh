#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
/// Given a flat polymesh with convex faces, naively triangulates all faces
void triangulate_naive(Mesh& m);
}
