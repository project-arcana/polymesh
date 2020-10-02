#pragma once

#include <polymesh/Mesh.hh>
#include "../fields.hh"

namespace polymesh
{
/**
 * Traces a ray inside the triangle stopping at the next edge
 */

/// Traces from halfedge h into its triangle
/// Start point is given by interpolating h.from and h.to given x
/// Direction is given by virtual source with SQUARED distance d1_sqr from h.from and d2_sqr from h.to
/// Returns intersecting half-edge and new x parameter
template <class Scalar = float, class EdgeLengthF>
std::pair<halfedge_handle, float> trace_step(halfedge_handle h, EdgeLengthF&& edge_length, Scalar x, Scalar d1_sqr, Scalar d2_sqr);

// ======== IMPLEMENTATION ========

template <class Scalar, class EdgeLengthF>
std::pair<halfedge_handle, float> trace_step(halfedge_handle h, EdgeLengthF&& edge_length, Scalar x, Scalar d1_sqr, Scalar d2_sqr)
{
    POLYMESH_ASSERT(!h.is_boundary() && "cannot trace into boundary");
    auto f = h.face();
    POLYMESH_ASSERT(f.vertices().size() == 3 && "only supports triangles");

    // TODO
}
}
