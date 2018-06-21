#pragma once

#include "VertexIndex.hh"

namespace polymesh
{
struct Mesh;

struct VertexHandle
{
    Mesh* const mesh;
    VertexIndex const idx;

    VertexHandle(Mesh* mesh, VertexIndex idx) : mesh(mesh), idx(idx) {}
};
}
