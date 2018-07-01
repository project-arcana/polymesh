#pragma once

#include <cstddef>
#include <vector>

#include "iterators.hh"

namespace polymesh
{

template<class this_t, class tag>
struct smart_range
{
    template<class AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;
    using handle = typename primitive<tag>::handle;

    /// returns the first element in this range
    /// returns invalid on empty ranges
    handle first() const;
    /// returns the last element in this range 
    /// returns invalid on empty ranges
    /// TODO: how to make this O(1)
    handle last() const;

    /// returns true if the range is non-empty
    bool any() const;
    /// returns true if any handle fulfils p(h)
    template<typename PredT>
    bool any(PredT&& p) const;
    /// returns true if any attribute is true for this range
    // bool any(attribute<bool> const& a) const;
    /// returns true if all handles fulfil p(h)
    template<typename PredT>
    bool all(PredT&& p) const;
    /// returns true if all attributes are true for this range
    // bool all(attribute<bool> const& a) const;

    /// returns the number of elements in this range
    /// NOTE: this is an O(n) operation, prefer size() if available
    /// TODO: maybe SFINAE to implement this via size() if available?
    int count() const;

    // TODO:
    // - average
    // - sum
    // - min
    // - max
    // - minmax (return struct {min, max})
    // - filter (or where?)
    // - map
    // - skip
    // - only_valid
    // - count
};

// ================= COLLECTION =================

template <class mesh_ptr, class tag, class iterator>
struct smart_collection : smart_range<smart_collection<mesh_ptr, tag, iterator>, tag>
{
    template <typename AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;

    /// Number of primitives, INCLUDING those marked for deletion
    /// O(1) computation
    int size() const;

    /// Ensures that a given number of primitives can be stored without reallocation
    void reserve(int capacity) const;

    /// Creates a new vertex attribute
    template <class PropT>
    attribute<PropT> make_attribute(PropT const& def_value = PropT());

    // Iteration:
    iterator begin() const;
    iterator end() const;

protected:
    /// Backreference to mesh
    mesh_ptr mesh;

    friend class Mesh;
};

/// Collection of all vertices of a mesh
/// Basically a smart std::vector
template <class iterator>
struct vertex_collection : smart_collection<Mesh*, vertex_tag, iterator>
{
    /// Adds a new vertex and returns its handle
    /// Does NOT invalidate any iterator!
    vertex_handle add() const;

    /// Removes a vertex (and all adjacent faces and edges)
    /// (marks them as removed, compactify mesh to actually remove them)
    void remove(vertex_handle v) const;
};

/// Collection of all faces of a mesh
/// Basically a smart std::vector
template <class iterator>
struct face_collection : smart_collection<Mesh*, face_tag, iterator>
{
    /// Adds a face consisting of N vertices
    /// The vertices must already be sorted in CCW order
    /// (note: trying to add already existing halfedges triggers assertions)
    template <size_t N>
    face_handle add(const vertex_handle (&v_handles)[N]) const;
    face_handle add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const;
    face_handle add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const;
    face_handle add(std::vector<vertex_handle> const& v_handles) const;
    face_handle add(vertex_handle const* v_handles, int vcnt) const;
    template <size_t N>
    face_handle add(const halfedge_handle (&half_loop)[N]) const;
    face_handle add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const;
    face_handle add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const;
    face_handle add(std::vector<halfedge_handle> const& half_loop) const;
    face_handle add(halfedge_handle const* half_loop, int vcnt) const;

    /// Removes a face (adjacent edges and vertices are NOT removed)
    /// (marks it as removed, compactify mesh to actually remove it)
    void remove(face_handle f) const;
};

/// Collection of all edges of a mesh
/// Basically a smart std::vector
template <class iterator>
struct edge_collection : smart_collection<Mesh*, edge_tag, iterator>
{
    /// Adds an edge between two existing, distinct vertices
    /// if edge already exists, returns it
    edge_handle add_or_get(vertex_handle v_from, vertex_handle v_to);

    // TODO: find

    /// Removes an edge (and both adjacent faces, vertices are NOT removed)
    /// (marks them as removed, compactify mesh to actually remove them)
    void remove(edge_handle e) const;
};

/// Collection of all half-edges of a mesh
/// Basically a smart std::vector
template <class iterator>
struct halfedge_collection : smart_collection<Mesh*, halfedge_tag, iterator>
{
    /// Adds an half-edge between two existing, distinct vertices
    /// if half-edge already exists, returns it
    /// (always adds opposite half-edge as well)
    halfedge_handle add_or_get(vertex_handle v_from, vertex_handle v_to);

    // TODO: find

    /// Removes the edge and both half-edges belonging to it (and both adjacent faces, vertices are NOT removed)
    /// (marks them as removed, compactify mesh to actually remove them)
    void remove_edge(halfedge_handle h) const;
};

// vertices

struct all_vertex_collection : vertex_collection<primitive<vertex_tag>::all_iterator>
{
};

struct all_vertex_const_collection : smart_collection<Mesh const*, vertex_tag, primitive<vertex_tag>::all_iterator>
{
};

struct valid_vertex_collection : vertex_collection<primitive<vertex_tag>::valid_iterator>
{
};

struct valid_vertex_const_collection : smart_collection<Mesh const*, vertex_tag, primitive<vertex_tag>::valid_iterator>
{
};

// faces

struct all_face_collection : face_collection<primitive<face_tag>::all_iterator>
{
};

struct all_face_const_collection : smart_collection<Mesh const*, face_tag, primitive<face_tag>::all_iterator>
{
};

struct valid_face_collection : face_collection<primitive<face_tag>::valid_iterator>
{
};

struct valid_face_const_collection : smart_collection<Mesh const*, face_tag, primitive<face_tag>::valid_iterator>
{
};

// edges

struct all_edge_collection : edge_collection<primitive<edge_tag>::all_iterator>
{
};

struct all_edge_const_collection : smart_collection<Mesh const*, edge_tag, primitive<edge_tag>::all_iterator>
{
};

struct valid_edge_collection : edge_collection<primitive<edge_tag>::valid_iterator>
{
};

struct valid_edge_const_collection : smart_collection<Mesh const*, edge_tag, primitive<edge_tag>::valid_iterator>
{
};

// half-edges

struct all_halfedge_collection : halfedge_collection<primitive<halfedge_tag>::all_iterator>
{
};

struct all_halfedge_const_collection : smart_collection<Mesh const*, halfedge_tag, primitive<halfedge_tag>::all_iterator>
{
};

struct valid_halfedge_collection : halfedge_collection<primitive<halfedge_tag>::valid_iterator>
{
};

struct valid_halfedge_const_collection : smart_collection<Mesh const*, halfedge_tag, primitive<halfedge_tag>::valid_iterator>
{
};

// ================= RINGS =================

template <class this_t, class tag>
struct primitive_ring : smart_range<this_t, tag>
{
    using handle = typename primitive<tag>::handle;

    face_handle face;

    /// Number of elements
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(handle v) const;
};

template <class tag, class circulator>
struct face_primitive_ring : primitive_ring<face_primitive_ring<tag, circulator>, tag>
{
    face_handle face;
    face_primitive_ring(face_handle f) { face = f; }

    // Iteration:
    circulator begin() const { return {face.any_halfedge(), false}; }
    circulator end() const { return {face.any_halfedge(), true}; }
};

template <class tag, class circulator>
struct vertex_primitive_ring : primitive_ring<vertex_primitive_ring<tag, circulator>, tag>
{
    vertex_handle vertex;
    vertex_primitive_ring(vertex_handle v) { vertex = v; }

    // Iteration:
    circulator begin() const { return {vertex.any_outgoing_halfedge(), vertex.is_isolated()}; }
    circulator end() const { return {vertex.any_outgoing_halfedge(), true}; }
};

/// all vertices belonging to a face
struct face_vertex_ring : face_primitive_ring<vertex_tag, face_vertex_circulator>
{
    using face_primitive_ring<vertex_tag, face_vertex_circulator>::face_primitive_ring;
};

/// all halfedges belonging to a face
struct face_halfedge_ring : face_primitive_ring<halfedge_tag, face_halfedge_circulator>
{
    using face_primitive_ring<halfedge_tag, face_halfedge_circulator>::face_primitive_ring;
};

/// all edges belonging to a face
struct face_edge_ring : face_primitive_ring<edge_tag, face_edge_circulator>
{
    using face_primitive_ring<edge_tag, face_edge_circulator>::face_primitive_ring;
};

/// all adjacent faces belonging to a face
struct face_face_ring : face_primitive_ring<face_tag, face_face_circulator>
{
    using face_primitive_ring<face_tag, face_face_circulator>::face_primitive_ring;
};

/// all outgoing half-edges from a vertex
struct vertex_halfedge_out_ring : vertex_primitive_ring<halfedge_tag, vertex_halfedge_out_circulator>
{
    using vertex_primitive_ring<halfedge_tag, vertex_halfedge_out_circulator>::vertex_primitive_ring;
};

/// all incoming half-edges from a vertex
struct vertex_halfedge_in_ring : vertex_primitive_ring<halfedge_tag, vertex_halfedge_in_circulator>
{
    using vertex_primitive_ring<halfedge_tag, vertex_halfedge_in_circulator>::vertex_primitive_ring;
};

/// all adjacent vertices of a vertex
struct vertex_vertex_ring : vertex_primitive_ring<vertex_tag, vertex_vertex_circulator>
{
    using vertex_primitive_ring<vertex_tag, vertex_vertex_circulator>::vertex_primitive_ring;
};

/// all adjacent edges of a vertex
struct vertex_edge_ring : vertex_primitive_ring<edge_tag, vertex_edge_circulator>
{
    using vertex_primitive_ring<edge_tag, vertex_edge_circulator>::vertex_primitive_ring;
};

/// all adjacent faces of a vertex (INCLUDES invalid ones for boundaries)
struct vertex_face_ring : vertex_primitive_ring<face_tag, vertex_face_circulator>
{
    using vertex_primitive_ring<face_tag, vertex_face_circulator>::vertex_primitive_ring;
};


/// ======== IMPLEMENTATION ========

template <class this_t, class element_handle>
int primitive_ring<this_t, element_handle>::size() const
{
    auto cnt = 0;
    for (auto v : *static_cast<this_t const*>(this))
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

template <class this_t, class tag>
bool primitive_ring<this_t, tag>::contains(handle v) const
{
    for (auto v2 : *static_cast<this_t const*>(this))
        if (v == v2)
            return true;
    return false;
}
}
