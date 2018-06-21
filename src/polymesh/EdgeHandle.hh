#pragma once

#include "EdgeIndex.hh"

namespace polymesh
{
struct Mesh;

struct EdgeHandle
{
    Mesh* const mesh;
    EdgeIndex const idx;

    EdgeHandle(Mesh* mesh, EdgeIndex idx) : mesh(mesh), idx(idx) {}
};
}
