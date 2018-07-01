#pragma once

#include <glm/glm.hpp>

#include "../Mesh.hh"
#include "../fields.hh"

// Derived mesh properties, including:
// - valences
// - edge angles
// - face angles
// - face centroids
// - face area
// - mesh volume
// - face normal
// - vertex normal
// - curvature
// - topological properties
//
// Note: unary properties should be usable as free functions OR as index into handles
// e.g. valence(v) is the same as v[valence]
namespace polymesh
{
/// returns true if the vertex lies at a boundary
bool is_boundary(vertex_handle v);
/// returns true if the face lies at a boundary
bool is_boundary(face_handle v);
/// returns true if the edge lies at a boundary
bool is_boundary(edge_handle v);
/// returns true if the half-edge lies at a boundary (NOTE: a half-edge is boundary if it has no face)
bool is_boundary(halfedge_handle v);

/// returns true if the vertex has no neighbors
bool is_isolated(vertex_handle v);
/// returns true if the edge has no neighboring faces
bool is_isolated(edge_handle v);

/// returns the vertex valence (number of adjacent vertices)
int valence(vertex_handle v);

/// returns the area of the (flat) polygonal face
template <class Vec3>
typename field_3d<Vec3>::Scalar face_area(face_handle f, vertex_attribute<Vec3> const& position);

/// returns the center of gravity for a given (flat) polygonal face
template <class Vec3>
Vec3 face_centroid(face_handle f, vertex_attribute<Vec3> const& position);

/// returns the area of a given triangle
template <class Vec3>
typename field_3d<Vec3>::Scalar triangle_area(face_handle f, vertex_attribute<Vec3> const& position);

/// returns the center of gravity for a given triangle
template <class Vec3>
Vec3 triangle_centroid(face_handle f, vertex_attribute<Vec3> const& position);

/// ======== IMPLEMENTATION ========

inline bool is_boundary(vertex_handle v) { return v.is_boundary(); }

inline bool is_boundary(face_handle v) { return v.is_boundary(); }

inline bool is_boundary(edge_handle v) { return v.is_boundary(); }

inline bool is_boundary(halfedge_handle v) { return v.is_boundary(); }

inline bool is_isolated(vertex_handle v) { return v.is_isolated(); }

inline bool is_isolated(edge_handle v) { return v.is_isolated(); }

inline int valence(vertex_handle v) { return v.adjacent_vertices().size(); }

template <class Vec3>
typename field_3d<Vec3>::Scalar triangle_area(face_handle f, vertex_attribute<Vec3> const& position)
{
    auto h = f.any_halfedge();
    auto p0 = position[h.vertex_from()];
    auto p1 = position[h.vertex_to()];
    auto p2 = position[h.next().vertex_to()];

    return field_3d<Vec3>::length(field_3d<Vec3>::cross(p0 - p1, p0 - p2)) * field_3d<Vec3>::scalar(0.5f);
}

template <class Vec3>
Vec3 triangle_centroid(face_handle f, vertex_attribute<Vec3> const& position)
{
    auto h = f.any_halfedge();
    auto p0 = position[h.vertex_from()];
    auto p1 = position[h.vertex_to()];
    auto p2 = position[h.next().vertex_to()];

    return (p0 + p1 + p2) / field_3d<Vec3>::scalar(3);
}

template <class Vec3>
typename field_3d<Vec3>::Scalar face_area(face_handle f, vertex_attribute<Vec3> const& position)
{
    auto varea = field_3d<Vec3>::zero();

    auto h = f.any_halfedge();

    auto v0 = h.vertex_from();
    auto p0 = v0[position];

    auto p_prev = h.vertex_to()[position];
    h = h.next();

    do
    {
        auto p_curr = h.vertex_to()[position];

        varea += field_3d<Vec3>::cross(p_prev - p0, p_curr - p0);

        // circulate
        h = h.next();
        p_prev = p_curr;
    } while (h.vertex_to() != v0);

    return field_3d<Vec3>::length(varea) * 0.5f;
}

template <class Vec3>
Vec3 face_centroid(face_handle f, vertex_attribute<Vec3> const& position)
{
    // TODO: make correct for non-convex polygons!

    auto area = field_3d<Vec3>::scalar(0);
    auto centroid = field_3d<Vec3>::zero();

    auto h = f.any_halfedge();

    auto v0 = h.vertex_from();
    auto p0 = v0[position];

    auto p_prev = h.vertex_to()[position];
    h = h.next();

    do
    {
        auto p_curr = h.vertex_to()[position];

        auto a = field_3d<Vec3>::length(field_3d<Vec3>::cross(p_prev - p0, p_curr - p0));
        area += a;
        centroid += (p_prev + p_curr + p0) * a;

        // circulate
        h = h.next();
        p_prev = p_curr;
    } while (h.vertex_to() != v0);

    return centroid / (3.0f * area);
}
}
