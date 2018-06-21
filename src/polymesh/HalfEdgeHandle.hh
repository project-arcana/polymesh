#pragma once

#include "HalfEdgeIndex.hh"

namespace polymesh
{
struct Mesh;

struct HalfEdgeHandle
{
    Mesh* const mesh;
    HalfEdgeIndex const idx;

    HalfEdgeHandle(Mesh* mesh, HalfEdgeIndex idx) : mesh(mesh), idx(idx) {}
};
}
