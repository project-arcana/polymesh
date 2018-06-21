#pragma once

#include "VertexIndex.hh"

namespace polymesh
{
struct Mesh;

struct VertexHandle
{
    Mesh const* mesh;
    VertexIndex idx;

    VertexHandle(Mesh const* mesh, VertexIndex idx) : mesh(mesh), idx(idx) {}
};
}
