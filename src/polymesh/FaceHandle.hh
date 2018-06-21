#pragma once

#include "FaceIndex.hh"

namespace polymesh
{
struct Mesh;

struct FaceHandle
{
    Mesh const* mesh;
    FaceIndex idx;

    FaceHandle(Mesh const* mesh, FaceIndex idx) : mesh(mesh), idx(idx) {}
};
}
