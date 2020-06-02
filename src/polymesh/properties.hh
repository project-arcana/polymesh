#pragma once

#include <polymesh/Mesh.hh>
#include <polymesh/fields.hh>

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
//
// ------------ Topological Properties ------------
//

/// returns true if the vertex lies at a boundary
bool is_boundary(vertex_handle v);
/// returns true if the vertex lies at a boundary
bool is_vertex_boundary(vertex_handle v);
/// returns true if the face lies at a boundary
bool is_boundary(face_handle f);
/// returns true if the face lies at a boundary
bool is_face_boundary(face_handle f);
/// returns true if the edge lies at a boundary
bool is_boundary(edge_handle e);
/// returns true if the edge lies at a boundary
bool is_edge_boundary(edge_handle e);
/// returns true if the half-edge lies at a boundary (NOTE: a half-edge is boundary if it has no face)
bool is_boundary(halfedge_handle h);
/// returns true if the half-edge lies at a boundary (NOTE: a half-edge is boundary if it has no face)
bool is_halfedge_boundary(halfedge_handle h);

/// returns true if the vertex has no neighbors
bool is_isolated(vertex_handle v);
/// returns true if the vertex has no neighbors
bool is_vertex_isolated(vertex_handle v);
/// returns true if the edge has no neighboring faces
bool is_isolated(edge_handle e);
/// returns true if the edge has no neighboring faces
bool is_edge_isolated(edge_handle e);

/// returns the vertex valence (number of adjacent vertices)
int valence(vertex_handle v);

/// returns true iff face is a triangle
bool is_triangle(face_handle f);
/// returns true iff face is a quad
bool is_quad(face_handle f);

/// returns the edge between two vertices
/// (returns invalid handle if edge not found)
/// (O(valence) running time)
edge_handle edge_between(vertex_handle v0, vertex_handle v1);

/// returns the halfedge between two vertices
/// (returns invalid handle if halfedge not found)
/// (O(valence) running time)
halfedge_handle halfedge_from_to(vertex_handle v_from, vertex_handle v_to);

/// returns true iff there is an edge between v0 and v1
bool are_adjacent(vertex_handle v0, vertex_handle v1);

/// returns true iff all faces are triangles
bool is_triangle_mesh(Mesh const& m);
/// returns true iff all faces are quads
bool is_quad_mesh(Mesh const& m);

/// returns the euler characteristic for this mesh
int euler_characteristic(Mesh const& m);

/// returns true iff the mesh is closed (has no boundary)
/// isolated vertices are ignored
bool is_closed_mesh(Mesh const& m);

/// returns true if m.halfedges().collapse(h) is possible topologically
/// NOTE: only works on triangles
bool can_collapse(halfedge_handle h);

/// returns true if m.edges().flip(e) would work
bool can_flip(edge_handle e);

/// returns true if m.edges().rotate_next(e) would work
bool can_rotate_next(edge_handle e);

/// returns true if m.edges().rotate_prev(e) would work
bool can_rotate_prev(edge_handle e);

/// returns true if m.halfedges().rotate_next(h) would work
bool can_rotate_next(halfedge_handle h);

/// returns true if m.halfedges().rotate_prev(h) would work
bool can_rotate_prev(halfedge_handle h);

/// returns true if m.edges().add_or_get(v_from, v_to) would work
bool can_add_or_get_edge(vertex_handle v_from, vertex_handle v_to);

/// returns true if m.edges().add_or_get(h_from, h_to) would work
bool can_add_or_get_edge(halfedge_handle h_from, halfedge_handle h_to);

//
// ------------ Geometrical Properties ------------
//

/// returns the area of the (flat) polygonal face
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar face_area(face_handle f, vertex_attribute<Pos3> const& position);

/// returns the center of gravity for a given (flat) polygonal face
template <class Pos3>
Pos3 face_centroid(face_handle f, vertex_attribute<Pos3> const& position);

/// returns the (CCW) oriented face normal (assumes planar polygon)
template <class Pos3>
typename field3<Pos3>::vec_t face_normal(face_handle f, vertex_attribute<Pos3> const& position);

/// returns the area of a given triangle
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar triangle_area(face_handle f, vertex_attribute<Pos3> const& position);

/// returns the center of gravity for a given triangle
template <class Pos3>
Pos3 triangle_centroid(face_handle f, vertex_attribute<Pos3> const& position);

/// returns the (CCW) oriented face normal
template <class Pos3>
typename field3<Pos3>::vec_t triangle_normal(face_handle f, vertex_attribute<Pos3> const& position);

/// returns the (CCW) oriented face normal (not normalized, thus scaled by 2 * face_area)
template <class Pos3>
typename field3<Pos3>::vec_t triangle_normal_unorm(face_handle f, vertex_attribute<Pos3> const& position);

/// returns a barycentric interpolation of a triangular face
template <class Pos3>
Pos3 bary_interpolate(face_handle f, Pos3 bary, vertex_attribute<Pos3> const& position);

/// calculates the barycentric coordinates of a given point p within a face f
/// NOTE: asserts that f is triangular
/// NOTE: also works for other points in the same plane as f
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Pos3 barycoords_of(face_handle f, vertex_attribute<Pos3> const& positions, Pos3 p);

/// returns the length of an edge
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar edge_length(edge_handle e, vertex_attribute<Pos3> const& position);

/// returns the length of an edge
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar edge_length(halfedge_handle h, vertex_attribute<Pos3> const& position);

/// returns the (non-normalized) vector from -> to
template <class Pos3>
typename field3<Pos3>::vec_t edge_vector(halfedge_handle h, vertex_attribute<Pos3> const& position);

/// returns the (normalized) vector from -> to (0 if from == to)
template <class Pos3>
typename field3<Pos3>::vec_t edge_dir(halfedge_handle h, vertex_attribute<Pos3> const& position);

/// calculates the angle between this half-edge and the next one
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar angle_to_next(halfedge_handle h, vertex_attribute<Pos3> const& position);

/// calculates the angle between this half-edge and the previous one
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar angle_to_prev(halfedge_handle h, vertex_attribute<Pos3> const& position);

/// sum of face angles at this vertex
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar angle_sum(vertex_handle v, vertex_attribute<Pos3> const& position);

/// difference between 2pi and the angle sum (positive means less than 2pi)
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar angle_defect(vertex_handle v, vertex_attribute<Pos3> const& position);

/// computes the laplacian cotan weight for a single edge
/// NOTE: only works for triangles!
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
Scalar cotan_weight(edge_handle e, vertex_attribute<Pos3> const& position);

/// efficiently computes the voronoi areas of all vertices
/// assumes triangle meshes for now
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
vertex_attribute<Scalar> vertex_voronoi_areas(vertex_attribute<Pos3> const& position);

/// efficiently computes vertex normals by uniformly weighting face normals
/// assumes triangle meshes for now
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
vertex_attribute<typename field3<Pos3>::vec_t> vertex_normals_uniform(vertex_attribute<Pos3> const& position);

/// efficiently computes vertex normals by area weighting face normals
/// assumes triangle meshes for now
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
vertex_attribute<typename field3<Pos3>::vec_t> vertex_normals_by_area(vertex_attribute<Pos3> const& position);

/// efficiently computes face normal attribute
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
face_attribute<typename field3<Pos3>::vec_t> face_normals(vertex_attribute<Pos3> const& position);

/// efficiently computes face normal attribute (assuming triangles)
template <class Pos3>
face_attribute<typename field3<Pos3>::vec_t> triangle_normals(vertex_attribute<Pos3> const& position);

/// efficiently computes a face area attribute (assuming triangles)
template <class Pos3>
face_attribute<typename field3<Pos3>::scalar_t> triangle_areas(vertex_attribute<Pos3> const& position);

/// efficiently computes per-edge cotangent weights
/// NOTE: only works for triangle meshes!
template <class Pos3>
edge_attribute<typename field3<Pos3>::scalar_t> cotan_weights(vertex_attribute<Pos3> const& position);

/// creates a Pos3 halfedge attribute with barycentric coordinates per to-vertex (i.e. 100, 010, 001)
/// assumes triangle mesh
/// useful for creating renderable meshes with barycoords
template <class Pos3>
halfedge_attribute<Pos3> barycentric_coordinates(Mesh const& m);

/// returns true if the edge satisfies the delaunay property
/// NOTE: only works on triangles
template <class Pos3>
bool is_delaunay(edge_handle e, vertex_attribute<Pos3> const& position);

/// returns true if m.halfedges().collapse(h) will not result in any flipped normals when h.vertex_to() is set to new_pos
/// NOTE: ALSO checks can_collapse(h)
/// NOTE: only works on triangles
template <class Pos3>
bool can_collapse_without_flips(halfedge_handle h, Pos3 new_pos, vertex_attribute<Pos3> const& position);

// ======== IMPLEMENTATION ========

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
inline bool is_quad(face_handle f) { return f.halfedges().size() == 4; }

inline edge_handle edge_between(vertex_handle v0, vertex_handle v1)
{
    for (auto h : v0.outgoing_halfedges())
        if (h.vertex_to() == v1)
            return h.edge();
    return {};
}

inline halfedge_handle halfedge_from_to(vertex_handle v_from, vertex_handle v_to)
{
    for (auto h : v_from.outgoing_halfedges())
        if (h.vertex_to() == v_to)
            return h;
    return {};
}

inline bool are_adjacent(vertex_handle v0, vertex_handle v1)
{
    for (auto h : v0.outgoing_halfedges())
        if (h.vertex_to() == v1)
            return true;
    return false;
}

inline bool is_triangle_mesh(Mesh const& m) { return m.faces().all(is_triangle); }
inline bool is_quad_mesh(Mesh const& m) { return m.faces().all(is_quad); }

inline int euler_characteristic(Mesh const& m) { return m.vertices().size() - m.edges().size() + m.faces().size(); }

inline bool is_closed_mesh(Mesh const& m) { return !m.halfedges().any(is_halfedge_boundary); }

template <class Pos3, class Scalar>
Scalar triangle_area(face_handle f, vertex_attribute<Pos3> const& position)
{
    auto h = f.any_halfedge();
    auto p0 = position[h.vertex_from()];
    auto p1 = position[h.vertex_to()];
    auto p2 = position[h.next().vertex_to()];

    return field3<Pos3>::length(field3<Pos3>::cross(p0 - p1, p0 - p2)) * field3<Pos3>::scalar(0.5f);
}

template <class Pos3>
Pos3 triangle_centroid(face_handle f, vertex_attribute<Pos3> const& position)
{
    auto h = f.any_halfedge();
    auto p0 = position[h.vertex_from()];
    auto p1 = position[h.vertex_to()];
    auto p2 = position[h.next().vertex_to()];

    return (p0 + p1 + p2) / field3<Pos3>::scalar(3);
}

template <class Pos3>
typename field3<Pos3>::vec_t face_normal(face_handle f, vertex_attribute<Pos3> const& position)
{
    auto c = face_centroid(f, position);
    auto e = f.any_halfedge();
    auto v0 = e.vertex_from()[position];
    auto v1 = e.vertex_to()[position];
    auto n = field3<Pos3>::cross(v0 - c, v1 - c);
    auto l = field3<Pos3>::length(n);
    return l == 0 ? field3<Pos3>::zero_vec() : n / l;
}

template <class Pos3>
typename field3<Pos3>::vec_t triangle_normal(face_handle f, vertex_attribute<Pos3> const& position)
{
    auto e = f.any_halfedge();
    auto v0 = e.vertex_from()[position];
    auto v1 = e.vertex_to()[position];
    auto v2 = e.next().vertex_to()[position];
    auto n = field3<Pos3>::cross(v1 - v0, v2 - v0);
    auto l = field3<Pos3>::length(n);
    return l == 0 ? field3<Pos3>::zero_vec() : n / l;
}

template <class Pos3>
typename field3<Pos3>::vec_t triangle_normal_unorm(face_handle f, vertex_attribute<Pos3> const& position)
{
    auto e = f.any_halfedge();
    auto v0 = e.vertex_from()[position];
    auto v1 = e.vertex_to()[position];
    auto v2 = e.next().vertex_to()[position];
    return field3<Pos3>::cross(v1 - v0, v2 - v0);
}

template <class Pos3, class Scalar>
Scalar face_area(face_handle f, vertex_attribute<Pos3> const& position)
{
    auto varea = field3<Pos3>::zero_vec();

    auto h = f.any_halfedge();

    auto v0 = h.vertex_from();
    auto p0 = v0[position];

    auto p_prev = h.vertex_to()[position];
    h = h.next();

    do
    {
        auto p_curr = h.vertex_to()[position];

        varea += field3<Pos3>::cross(p_prev - p0, p_curr - p0);

        // circulate
        h = h.next();
        p_prev = p_curr;
    } while (h.vertex_to() != v0);

    return field3<Pos3>::length(varea) * 0.5f;
}

template <class Pos3>
Pos3 face_centroid(face_handle f, vertex_attribute<Pos3> const& position)
{
    // TODO: make correct for non-convex polygons!

    auto area = field3<Pos3>::scalar(0);
    decltype(Pos3{} + Pos3{}) centroid = field3<Pos3>::zero_pos();

    auto h = f.any_halfedge();

    auto v0 = h.vertex_from();
    auto p0 = v0[position];

    auto p_prev = h.vertex_to()[position];
    h = h.next();

    do
    {
        auto p_curr = h.vertex_to()[position];

        auto a = field3<Pos3>::length(field3<Pos3>::cross(p_prev - p0, p_curr - p0));
        area += a;
        centroid += (p_prev + p_curr + p0) * a;

        // circulate
        h = h.next();
        p_prev = p_curr;
    } while (h.vertex_to() != v0);

    return centroid / (3.0f * area);
}

template <class Pos3, class Scalar>
Scalar edge_length(edge_handle e, vertex_attribute<Pos3> const& position)
{
    return field3<Pos3>::length(position[e.vertexA()] - position[e.vertexB()]);
}

template <class Pos3, class Scalar>
Scalar edge_length(halfedge_handle h, vertex_attribute<Pos3> const& position)
{
    return field3<Pos3>::length(position[h.vertex_from()] - position[h.vertex_to()]);
}

template <class Pos3>
typename field3<Pos3>::vec_t edge_vector(halfedge_handle h, vertex_attribute<Pos3> const& position)
{
    return position[h.vertex_to()] - position[h.vertex_from()];
}

template <class Pos3>
typename field3<Pos3>::vec_t edge_dir(halfedge_handle h, vertex_attribute<Pos3> const& position)
{
    auto d = position[h.vertex_to()] - position[h.vertex_from()];
    auto l = field3<Pos3>::length(d);
    if (l == 0)
        return field3<Pos3>::zero_vec();
    return d / l;
}

template <class Pos3, class Scalar>
Scalar angle_to_next(halfedge_handle h, vertex_attribute<Pos3> const& position)
{
    auto v0 = h.vertex_from()[position];
    auto v1 = h.vertex_to()[position];
    auto v2 = h.next().vertex_to()[position];

    auto v01 = v0 - v1;
    auto v21 = v2 - v1;

    auto l01 = field3<Pos3>::length(v01);
    auto l21 = field3<Pos3>::length(v21);

    if (l01 == 0 || l21 == 0)
        return 0;

    auto ca = field3<Pos3>::dot(v01, v21) / (l01 * l21);
    return std::acos(ca);
}

template <class Pos3, class Scalar>
Scalar angle_to_prev(halfedge_handle h, vertex_attribute<Pos3> const& position)
{
    auto v0 = h.vertex_to()[position];
    auto v1 = h.vertex_from()[position];
    auto v2 = h.prev().vertex_from()[position];

    auto v01 = v0 - v1;
    auto v21 = v2 - v1;

    auto l01 = field3<Pos3>::length(v01);
    auto l21 = field3<Pos3>::length(v21);

    if (l01 == 0 || l21 == 0)
        return 0;

    auto ca = field3<Pos3>::dot(v01, v21) / (l01 * l21);
    return std::acos(ca);
}

template <class Pos3, class Scalar>
Scalar angle_sum(vertex_handle v, vertex_attribute<Pos3> const& position)
{
    Scalar sum = 0;
    for (auto h : v.outgoing_halfedges())
        if (!h.is_boundary())
            sum += angle_to_prev(h, position);
    return sum;
}

template <class Pos3, class Scalar>
Scalar angle_defect(vertex_handle v, vertex_attribute<Pos3> const& position)
{
    return Scalar(2 * 3.14159265358979323846) - angle_sum(v, position);
}

template <class Pos3, class Scalar>
Scalar cotan_weight(edge_handle e, vertex_attribute<Pos3> const& position)
{
    auto h0 = e.halfedgeA();
    auto h1 = e.halfedgeB();

    auto cot_a = Scalar(0);
    auto cot_b = Scalar(0);

    auto pi = position[h0.vertex_to()];
    auto pj = position[h1.vertex_to()];

    if (!h0.is_boundary())
    {
        POLYMESH_ASSERT(h0.next().next().vertex_to() == h0.vertex_from() && "only works on triangles");
        auto pa = position[h0.next().vertex_to()];

        auto e_ia = pi - pa;
        auto e_ja = pj - pa;

        cot_a = field3<Pos3>::dot(e_ia, e_ja) / field3<Pos3>::length(field3<Pos3>::cross(e_ia, e_ja));
    }

    if (!h1.is_boundary())
    {
        POLYMESH_ASSERT(h1.next().next().vertex_to() == h1.vertex_from() && "only works on triangles");
        auto pb = position[h1.next().vertex_to()];

        auto e_ib = pi - pb;
        auto e_jb = pj - pb;

        cot_b = field3<Pos3>::dot(e_ib, e_jb) / field3<Pos3>::length(field3<Pos3>::cross(e_ib, e_jb));
    }

    auto w = cot_a + cot_b;

    if (std::isnan(w))
        return Scalar(0);

    return w;
}

template <class Pos3>
Pos3 bary_interpolate(face_handle f, Pos3 bary, vertex_attribute<Pos3> const& position)
{
    auto h = f.any_halfedge();
    auto v0 = h.vertex_to()[position];
    auto v1 = h.next().vertex_to()[position];
    auto v2 = h.next().next().vertex_to()[position];
    return (v0 * bary[0] + v1 * bary[1] + v2 * bary[2]) / field3<Pos3>::scalar(1);
}

template <class Pos3, class Scalar>
Pos3 barycoords_of(face_handle f, vertex_attribute<Pos3> const& positions, Pos3 p)
{
    POLYMESH_ASSERT(f.vertices().size() == 3 && "only supports triangles");

    auto ps = f.vertices().to_array<3>(positions);

    auto e10 = ps[1] - ps[0];
    auto e21 = ps[2] - ps[1];

    auto n = field3<Pos3>::cross(e10, e21);

    auto signed_area = [&](Pos3 const& v0, Pos3 const& v1, Pos3 const& v2) {
        auto d1 = v1 - v0;
        auto d2 = v2 - v0;

        auto a = field3<Pos3>::cross(d1, d2);

        return field3<Pos3>::dot(a, n);
    };

    auto a = signed_area(ps[0], ps[1], ps[2]);
    auto a0 = signed_area(p, ps[1], ps[2]);
    auto a1 = signed_area(p, ps[2], ps[0]);
    auto a2 = signed_area(p, ps[0], ps[1]);

    auto inv_a = Scalar(1) / a;
    return field3<Pos3>::make_pos(a0 * inv_a, a1 * inv_a, a2 * inv_a);
}

template <class Pos3, class Scalar>
vertex_attribute<Scalar> vertex_voronoi_areas(vertex_attribute<Pos3> const& position)
{
    auto const& m = position.mesh();
    vertex_attribute<Scalar> areas = m.vertices().make_attribute(Scalar(0));

    for (auto f : m.faces())
    {
        Scalar a = face_area(f, position);
        int v_cnt = f.vertices().size();
        for (auto v : f.vertices())
            areas[v] += a / v_cnt;
    }

    return areas;
}

template <class Pos3, class Scalar>
vertex_attribute<typename field3<Pos3>::vec_t> vertex_normals_uniform(vertex_attribute<Pos3> const& position)
{
    auto const& m = position.mesh();
    auto fnormals = m.faces().map([&](face_handle f) { return triangle_normal(f, position); });
    auto normals = m.vertices().make_attribute(field3<Pos3>::make_vec(0, 0, 0));

    for (auto f : m.faces())
        for (auto v : f.vertices())
            normals[v] += fnormals[f];

    for (auto& n : normals)
    {
        auto l = field3<Pos3>::length(n);
        if (l > 0)
            n /= l;
    }

    return normals;
}

template <class Pos3, class Scalar>
vertex_attribute<typename field3<Pos3>::vec_t> vertex_normals_by_area(vertex_attribute<Pos3> const& position)
{
    auto const& m = position.mesh();
    auto fnormals = m.faces().map([&](face_handle f) { return triangle_normal_unorm(f, position); });
    auto normals = m.vertices().make_attribute(field3<Pos3>::make_vec(0, 0, 0));

    for (auto f : m.faces())
        for (auto v : f.vertices())
            normals[v] += fnormals[f];

    for (auto& n : normals)
    {
        auto l = field3<Pos3>::length(n);
        if (l > 0)
            n /= l;
    }

    return normals;
}

template <class Pos3, class Scalar>
face_attribute<typename field3<Pos3>::vec_t> face_normals(vertex_attribute<Pos3> const& position)
{
    auto const& m = position.mesh();
    return m.faces().map([&](face_handle f) { return face_normal(f, position); });
}

template <class Pos3>
face_attribute<typename field3<Pos3>::vec_t> triangle_normals(vertex_attribute<Pos3> const& position)
{
    auto const& m = position.mesh();
    return m.faces().map([&](face_handle f) { return triangle_normal(f, position); });
}

template <class Pos3>
face_attribute<typename field3<Pos3>::scalar_t> triangle_areas(vertex_attribute<Pos3> const& position)
{
    auto const& m = position.mesh();
    return m.faces().map([&](face_handle f) { return triangle_area(f, position); });
}

template <class Pos3>
edge_attribute<typename field3<Pos3>::scalar_t> cotan_weights(vertex_attribute<Pos3> const& position)
{
    auto const& m = position.mesh();
    return m.edges().map([&](edge_handle e) { return cotan_weight(e, position); });
}

template <class Pos3>
halfedge_attribute<Pos3> barycentric_coordinates(Mesh const& m)
{
    halfedge_attribute<Pos3> coords(m);
    for (auto f : m.faces())
    {
        auto idx = 0;
        for (auto h : f.halfedges())
        {
            coords[h] = field3<Pos3>::make(idx == 0, idx == 1, idx == 2);
            ++idx;
        }
    }
    return coords;
}

template <class Pos3>
bool is_delaunay(edge_handle e, vertex_attribute<Pos3> const& position)
{
    return e.is_boundary() || cotan_weight(e, position) >= 0;
}

inline bool can_collapse(halfedge_handle h)
{
    auto v_from = h.vertex_from();

    auto ignore_v0 = h.is_boundary() ? vertex_handle::invalid : h.next().vertex_to();
    auto ignore_v1 = h.opposite().is_boundary() ? vertex_handle::invalid : h.opposite().next().vertex_to();

    for (auto v : h.vertex_to().adjacent_vertices())
    {
        if (v == ignore_v0 || v == ignore_v1 || v == v_from)
            continue;

        // TODO: can be made faster by buffering these vertices
        if (v_from.adjacent_vertices().contains(v))
            return false;
    }

    return true;
}

template <class Pos3>
bool can_collapse_without_flips(halfedge_handle h, Pos3 new_pos, vertex_attribute<Pos3> const& position)
{
    auto const v_to = h.vertex_to();
    auto const v_from = h.vertex_from();

    if (!can_collapse(h))
        return false;

    // check to-1-ring
    {
        auto const p_to = position[v_to];
        auto const ignore_h0 = h;
        auto const ignore_h1 = h.opposite().prev();

        for (auto hh : v_to.incoming_halfedges())
        {
            if (hh == ignore_h0 || hh == ignore_h1 || hh.is_boundary())
                continue;

            auto p0 = position[hh.vertex_from()];
            auto p1 = position[hh.next().vertex_to()];
            auto const n_before = field3<Pos3>::cross(p0 - p_to, p1 - p_to);
            auto const n_after = field3<Pos3>::cross(p0 - new_pos, p1 - new_pos);

            if (field3<Pos3>::dot(n_before, n_after) < 0)
                return false;
        }
    }

    // check from-1-ring
    {
        auto const p_from = position[v_from];
        auto const ignore_h0 = h;
        auto const ignore_h1 = h.opposite().next();

        for (auto hh : v_from.outgoing_halfedges())
        {
            if (hh == ignore_h0 || hh == ignore_h1 || hh.is_boundary())
                continue;

            auto p0 = position[hh.vertex_to()];
            auto p1 = position[hh.next().vertex_to()];
            auto const n_before = field3<Pos3>::cross(p0 - p_from, p1 - p_from);
            auto const n_after = field3<Pos3>::cross(p0 - new_pos, p1 - new_pos);

            if (field3<Pos3>::dot(n_before, n_after) < 0)
                return false;
        }
    }

    return true;
}

inline bool can_flip(edge_handle e)
{
    if (e.is_boundary())
        return false;

    if (e.halfedgeA().next().next().next() != e.halfedgeA())
        return false;

    if (e.halfedgeB().next().next().next() != e.halfedgeB())
        return false;

    return true;
}

inline bool can_rotate_next(edge_handle e)
{
    if (e.is_boundary())
        return false;

    if (e.vertexA().adjacent_vertices().size() <= 2)
        return false;

    if (e.vertexB().adjacent_vertices().size() <= 2)
        return false;

    auto va_next = e.halfedgeA().next().vertex_to();
    auto vb_next = e.halfedgeB().next().vertex_to();
    if (va_next.adjacent_vertices().contains(vb_next))
        return false;

    return true;
}

inline bool can_rotate_prev(edge_handle e)
{
    if (e.is_boundary())
        return false;

    if (e.vertexA().adjacent_vertices().size() <= 2)
        return false;

    if (e.vertexB().adjacent_vertices().size() <= 2)
        return false;

    auto va_prev = e.halfedgeA().prev().vertex_from();
    auto vb_prev = e.halfedgeB().prev().vertex_from();
    if (va_prev.adjacent_vertices().contains(vb_prev))
        return false;

    return true;
}

inline bool can_rotate_next(halfedge_handle h)
{
    if (h.edge().is_boundary())
        return false;

    if (h.vertex_to().adjacent_vertices().size() <= 2)
        return false;

    if (h.next().next().next() == h)
        return false;

    return true;
}

inline bool can_rotate_prev(halfedge_handle h)
{
    if (h.edge().is_boundary())
        return false;

    if (h.vertex_to().adjacent_vertices().size() <= 2)
        return false;

    if (h.prev().prev().prev() == h)
        return false;

    return true;
}

inline bool can_add_or_get_edge(vertex_handle v_from, vertex_handle v_to)
{
    POLYMESH_ASSERT(v_from.mesh == v_to.mesh);
    auto ll = low_level_api(v_from.mesh);

    if (v_from == v_to)
        return false; // no self-loops

    if (ll.find_halfedge(v_from, v_to).is_valid())
        return true; // existing

    if (!v_from.is_isolated() && ll.find_free_incident(v_from).is_invalid())
        return false; // from already full

    if (!v_to.is_isolated() && ll.find_free_incident(v_to).is_invalid())
        return false; // to already full

    return true;
}

inline bool can_add_or_get_edge(halfedge_handle h_from, halfedge_handle h_to)
{
    POLYMESH_ASSERT(h_from.mesh == h_to.mesh);
    auto ll = low_level_api(h_from.mesh);

    auto v_from = h_from.vertex_to();
    auto v_to = h_to.vertex_to();

    if (v_from == v_to)
        return false; // no self-loops

    auto ex_he = ll.find_halfedge(v_from, v_to);
    if (ex_he.is_valid())
        return true; // existing

    if (!ll.is_free(h_from))
        return false; // cannot insert into face

    if (!ll.is_free(h_to))
        return false; // cannot insert into face

    return true;
}

}
