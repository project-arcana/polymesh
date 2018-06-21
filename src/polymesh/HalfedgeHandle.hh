#pragma once

#include "HalfedgeIndex.hh"

namespace polymesh
{
struct Mesh;

struct HalfedgeHandle
{
    Mesh const* mesh;
    HalfedgeIndex idx;

    HalfedgeHandle(Mesh const* mesh, HalfedgeIndex idx) : mesh(mesh), idx(idx) {}
};
}
