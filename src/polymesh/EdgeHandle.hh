#pragma once

#include "EdgeIndex.hh"

namespace polymesh
{
struct Mesh;

struct EdgeHandle
{
    Mesh const* mesh;
    EdgeIndex idx;

    EdgeHandle(Mesh const* mesh, EdgeIndex idx) : mesh(mesh), idx(idx) {}
};
}
