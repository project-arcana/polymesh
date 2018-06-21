#pragma once

#include "FaceIndex.hh"

namespace polymesh
{
struct Mesh;

struct FaceHandle
{
    Mesh* const mesh;
    FaceIndex const idx;

    FaceHandle(Mesh* mesh, FaceIndex idx) : mesh(mesh), idx(idx) {}
};
}
