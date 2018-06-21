#pragma once

#include "EdgeHandle.hh"
#include "FaceHandle.hh"
#include "HalfEdgeHandle.hh"
#include "VertexHandle.hh"

namespace polymesh
{
struct Mesh
{
private:
    struct Face
    {
        HalfEdgeIndex half_edge; ///< One half-edge bounding this face
    };

    struct Vertex
    {
        HalfEdgeIndex outgoing_half_edge; ///< outgoing half-edge
    };
};
}
