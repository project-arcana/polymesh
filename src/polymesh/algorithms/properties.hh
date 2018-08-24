#pragma once

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
bool is_vertex_boundary(vertex_handle v);
/// returns true if the face lies at a boundary
bool is_boundary(face_handle f);
bool is_face_boundary(face_handle f);
/// returns true if the edge lies at a boundary
bool is_boundary(edge_handle e);
bool is_edge_boundary(edge_handle e);
/// returns true if the half-edge lies at a boundary (NOTE: a half-edge is boundary if it has no face)
bool is_boundary(halfedge_handle h);
bool is_halfedge_boundary(halfedge_handle h);

/// returns true if the vertex has no neighbors
bool is_isolated(vertex_handle v);
bool is_vertex_isolated(vertex_handle v);
/// returns true if the edge has no neighboring faces
bool is_isolated(edge_handle e);
bool is_edge_isolated(edge_handle e);

/// returns the vertex valence (number of adjacent vertices)
int valence(vertex_handle v);

/// returns true iff face is a triangle
bool is_triangle(face_handle f);
/// returns true iff face is a quad
bool is_quad(face_handle f);

/// returns true iff all faces are triangles
bool is_triangle_mesh(Mesh const& m);
/// returns true iff all faces are quads
bool is_quad_mesh(Mesh const& m);

/// returns the area of the (flat) polygonal face
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
Scalar face_area(face_handle f, vertex_attribute<Vec3> const& position);

/// returns the center of gravity for a given (flat) polygonal face
template <class Vec3>
Vec3 face_centroid(face_handle f, vertex_attribute<Vec3> const& position);

/// returns the (CCW) oriented face normal (assumes planar polygon)
template <class Vec3>
Vec3 face_normal(face_handle f, vertex_attribute<Vec3> const& position);

/// returns the area of a given triangle
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
Scalar triangle_area(face_handle f, vertex_attribute<Vec3> const& position);

/// returns the center of gravity for a given triangle
template <class Vec3>
Vec3 triangle_centroid(face_handle f, vertex_attribute<Vec3> const& position);

/// returns the (CCW) oriented face normal
template <class Vec3>
Vec3 triangle_normal(face_handle f, vertex_attribute<Vec3> const& position);

/// returns a barycentric interpolation of a triangular face
template <class Vec3>
Vec3 bary_interpolate(face_handle f, Vec3 bary, vertex_attribute<Vec3> const& position);

/// returns the length of an edge
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
Scalar edge_length(edge_handle e, vertex_attribute<Vec3> const& position);

/// returns the length of an edge
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
Scalar edge_length(halfedge_handle h, vertex_attribute<Vec3> const& position);

/// returns the (non-normalized) vector from -> to
template <class Vec3>
Vec3 edge_vector(halfedge_handle h, vertex_attribute<Vec3> const& position);

/// returns the (normalized) vector from -> to (0 if from == to)
template <class Vec3>
Vec3 edge_dir(halfedge_handle h, vertex_attribute<Vec3> const& position);

/// calculates the angle between this half-edge and the next one
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
Scalar angle_to_next(halfedge_handle h, vertex_attribute<Vec3> const& position);

/// calculates the angle between this half-edge and the previous one
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
Scalar angle_to_prev(halfedge_handle h, vertex_attribute<Vec3> const& position);

/// sum of face angles at this vertex
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
Scalar angle_sum(vertex_handle v, vertex_attribute<Vec3> const& position);

/// difference between 2pi and the angle sum (positive means less than 2pi)
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
Scalar angle_defect(vertex_handle v, vertex_attribute<Vec3> const& position);

/// efficiently computes the voronoi areas of all vertices
/// assumes triangle meshes for now
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
vertex_attribute<Scalar> vertex_voronoi_areas(Mesh const& m, vertex_attribute<Vec3> const& position);

/// ======== IMPLEMENTATION ========

inline bool is_boundary(vertex_handle v) { return v.is_boundary(); }
inline bool is_vertex_boundary(vertex_handle v) { return v.is_boundary(); }

inline bool is_boundary(face_handle f) { return f.is_boundary(); }
inline bool is_face_boundary(face_handle f) { return f.is_boundary(); }

inline bool is_boundary(edge_handle e) { return e.is_boundary(); }
inline bool is_edge_boundary(edge_handle e) { return e.is_boundary(); }

inline bool is_boundary(halfedge_handle h) { return h.is_boundary(); }
inline bool is_halfedge_boundary(halfedge_handle h) { return h.is_boundary(); }

inline bool is_isolated(vertex_handle v) { return v.is_isolated(); }
inline bool is_vertex_isolated(vertex_handle v) { return v.is_isolated(); }

inline bool is_isolated(edge_handle e) { return e.is_isolated(); }
inline bool is_edge_isolated(edge_handle e) { return e.is_isolated(); }

inline int valence(vertex_handle v) { return v.adjacent_vertices().size(); }

inline bool is_triangle(face_handle f) { return f.halfedges().size() == 3; }
inline bool is_quad(face_handle f) { return f.halfedges().size() == 5; }

inline bool is_triangle_mesh(Mesh const& m) { return m.faces().all(is_triangle); }
inline bool is_quad_mesh(Mesh const& m) { return m.faces().all(is_quad); }

template <class Vec3, class Scalar>
Scalar triangle_area(face_handle f, vertex_attribute<Vec3> const& position)
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
Vec3 face_normal(face_handle f, vertex_attribute<Vec3> const& position)
{
    auto c = face_centroid(f, position);
    auto e = f.any_halfedge();
    auto v0 = e.vertex_from()[position];
    auto v1 = e.vertex_to()[position];
    auto n = field_3d<Vec3>::cross(v0 - c, v1 - c);
    auto l = field_3d<Vec3>::length(n);
    return l == 0 ? field_3d<Vec3>::zero() : n / l;
}

template <class Vec3>
Vec3 triangle_normal(face_handle f, vertex_attribute<Vec3> const& position)
{
    auto e = f.any_halfedge();
    auto v0 = e.vertex_from()[position];
    auto v1 = e.vertex_to()[position];
    auto v2 = e.next().vertex_to()[position];
    auto n = field_3d<Vec3>::cross(v1 - v0, v2 - v0);
    auto l = field_3d<Vec3>::length(n);
    return l == 0 ? field_3d<Vec3>::zero() : n / l;
}

template <class Vec3, class Scalar>
Scalar face_area(face_handle f, vertex_attribute<Vec3> const& position)
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

template <class Vec3, class Scalar>
Scalar edge_length(edge_handle e, vertex_attribute<Vec3> const& position)
{
    return field_3d<Vec3>::length(position[e.vertexA()] - position[e.vertexB()]);
}

template <class Vec3, class Scalar>
Scalar edge_length(halfedge_handle h, vertex_attribute<Vec3> const& position)
{
    return field_3d<Vec3>::length(position[h.vertex_from()] - position[h.vertex_to()]);
}

template <class Vec3>
Vec3 edge_vector(halfedge_handle h, vertex_attribute<Vec3> const& position)
{
    return position[h.vertex_to()] - position[h.vertex_from()];
}

template <class Vec3>
Vec3 edge_dir(halfedge_handle h, vertex_attribute<Vec3> const& position)
{
    auto d = position[h.vertex_to()] - position[h.vertex_from()];
    auto l = field_3d<Vec3>::length(d);
    if (l == 0)
        return field_3d<Vec3>::zero();
    return d / l;
}

template <class Vec3, class Scalar>
Scalar angle_to_next(halfedge_handle h, vertex_attribute<Vec3> const& position)
{
    auto v0 = h.vertex_from()[position];
    auto v1 = h.vertex_to()[position];
    auto v2 = h.next().vertex_to()[position];

    auto v01 = v0 - v1;
    auto v21 = v2 - v1;

    auto l01 = field_3d<Vec3>::length(v01);
    auto l21 = field_3d<Vec3>::length(v21);

    if (l01 == 0 || l21 == 0)
        return 0;

    auto ca = field_3d<Vec3>::dot(v01, v21) / (l01 * l21);
    return std::acos(ca);
}

template <class Vec3, class Scalar>
Scalar angle_to_prev(halfedge_handle h, vertex_attribute<Vec3> const& position)
{
    auto v0 = h.vertex_to()[position];
    auto v1 = h.vertex_from()[position];
    auto v2 = h.prev().vertex_from()[position];

    auto v01 = v0 - v1;
    auto v21 = v2 - v1;

    auto l01 = field_3d<Vec3>::length(v01);
    auto l21 = field_3d<Vec3>::length(v21);

    if (l01 == 0 || l21 == 0)
        return 0;

    auto ca = field_3d<Vec3>::dot(v01, v21) / (l01 * l21);
    return std::acos(ca);
}

template <class Vec3, class Scalar>
Scalar angle_sum(vertex_handle v, vertex_attribute<Vec3> const& position)
{
    Scalar sum = 0;
    for (auto h : v.outgoing_halfedges())
        if (!h.is_boundary())
            sum += angle_to_prev(h, position);
    return sum;
}

template <class Vec3, class Scalar>
Scalar angle_defect(vertex_handle v, vertex_attribute<Vec3> const& position)
{
    return 2 * M_PI - angle_sum(v, position);
}

template <class Vec3>
Vec3 bary_interpolate(face_handle f, Vec3 bary, vertex_attribute<Vec3> const& position)
{
    auto h = f.any_halfedge();
    auto v0 = h.vertex_to()[position];
    auto v1 = h.next().vertex_to()[position];
    auto v2 = h.next().next().vertex_to()[position];
    return v0 * bary[0] + v1 * bary[1] + v2 * bary[2];
}

template <class Vec3, class Scalar>
vertex_attribute<Scalar> vertex_voronoi_areas(Mesh const& m, vertex_attribute<Vec3> const& position)
{
    vertex_attribute<Scalar> areas = m.vertices().make_attribute_with_default(Scalar(0));

    for (auto f : m.faces())
    {
        Scalar a = face_area(f, position);
        int v_cnt = f.vertices().size();
        for (auto v : f.vertices())
            areas[v] += a / v_cnt;
    }

    return areas;
}
}
