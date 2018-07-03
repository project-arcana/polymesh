#pragma once

#include <cstddef>
#include <map>
#include <set>
#include <vector>

#include "iterators.hh"

namespace polymesh
{
template <class T>
struct aabb
{
    T min;
    T max;
};

template <class V, class W>
struct weighted_sample
{
    V value;
    W weight;
};

template <class this_t, class ElementT>
struct smart_range
{
    /// returns the first element in this range
    /// returns invalid on empty ranges (or default ctor'd one)
    ElementT first() const;
    /// returns the last element in this range
    /// returns invalid on empty ranges (or default ctor'd one)
    /// TODO: how to make this O(1)
    ElementT last() const;

    /// returns true if the range is non-empty
    bool any() const;
    /// returns true if any value fulfils p(v)
    /// also works for boolean attributes
    template <class PredT>
    bool any(PredT&& p) const;
    /// returns true if all values fulfil p(v)
    /// also works for boolean attributes
    template <class PredT>
    bool all(PredT&& p) const;

    /// returns the number of elements in this range
    /// NOTE: this is an O(n) operation, prefer size() if available
    /// TODO: maybe SFINAE to implement this via size() if available?
    int count() const;

    /// calculates min(f(e)) over all elements
    /// undefined behavior if range is empty
    /// works for std::min and everything reachable by ADL (calls min(_, _))
    template <class FuncT>
    auto min(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>;
    /// calculates max(f(e)) over all elements
    /// undefined behavior if range is empty
    /// works for std::max and everything reachable by ADL (calls max(_, _))
    template <class FuncT>
    auto max(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>;
    /// calculates the sum of f(e) over all elements
    /// undefined behavior if range is empty
    /// requires operator+ for the elements
    template <class FuncT>
    auto sum(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>;
    /// calculates the avg of f(e) over all elements
    /// undefined behavior if range is empty
    /// requires operator+ for the elements as well as operator/(ElementT, int)
    template <class FuncT>
    auto avg(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>;

    /// calculates the weighted avg of f(e) with weight w(e) over all elements
    /// undefined behavior if range is empty
    /// requires operator+ for the elements and weights as well as operator/(ElementT, WeightT)
    template <class FuncT, class WeightT>
    auto weighted_avg(FuncT&& f, WeightT&& w) const -> tmp::decayed_result_type_of<FuncT, ElementT>;

    /// calculates the aabb (min and max) of f(e) over all elements
    /// undefined behavior if range is empty
    /// works for std::min/max and everything reachable by ADL (calls min/max(_, _))
    template <class FuncT>
    auto aabb(FuncT&& f) const -> polymesh::aabb<tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// same as aabb(...)
    template <class FuncT>
    auto minmax(FuncT&& f) const -> polymesh::aabb<tmp::decayed_result_type_of<FuncT, ElementT>>;

    /// converts this range to a vector
    std::vector<ElementT> to_vector() const;
    /// converts this range to a set
    std::set<ElementT> to_set() const;
    /// converts this range to a vector containing f(v) entries
    template <class FuncT>
    auto to_vector(FuncT&& f) const -> std::vector<tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// converts this range to a set containing f(v) entries
    template <class FuncT>
    auto to_set(FuncT&& f) const -> std::set<tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// converts this range to a map containing {v, f(v)} entries
    template <class FuncT>
    auto to_map(FuncT&& f) const -> std::map<ElementT, tmp::decayed_result_type_of<FuncT, ElementT>>;

    // TODO: (requires new ranges)
    // - filter (or where?)
    // - map
    // - skip
    // - only_valid
    // - conversions from vector/set/map
};

// ================= COLLECTION =================

template <class mesh_ptr, class tag, class iterator>
struct smart_collection : smart_range<smart_collection<mesh_ptr, tag, iterator>, typename primitive<tag>::handle>
{
    template <class AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;
    using handle = typename primitive<tag>::handle;

    /// Number of primitives, INCLUDING those marked for deletion
    /// O(1) computation
    int size() const;

    /// Ensures that a given number of primitives can be stored without reallocation
    void reserve(int capacity) const;

    /// Creates a new primitive attribute
    template <class PropT>
    attribute<PropT> make_attribute() const;
    /// Creates a new primitive attribute with a given default value
    template <class PropT>
    attribute<PropT> make_attribute_default(PropT const& def_value) const;
    /// Creates a new primitive attribute and initializes it with f(h) for each handle h
    template <class FuncT, class PropT = tmp::decayed_result_type_of<FuncT, handle>>
    attribute<PropT> make_attribute(FuncT&& f, PropT const& def_value = PropT()) const;

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

    /// Collapsed the given vertex by removing it and merging the adjacent faces
    /// Preserves half-edge properties but not face ones
    void collapse(vertex_handle v) const;

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

    /// Splits a face by inserting a vertex (which is returned) and creating triangles towards it
    /// Preserves half-edge attributes
    /// The face itself is deleted and multiple new ones are created
    vertex_handle split(face_handle f) const;

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
    edge_handle add_or_get(vertex_handle v_from, vertex_handle v_to) const;

    /// Returns the edge handle between two vertices (invalid if not found)
    /// O(valence) computation
    edge_handle find(vertex_handle v_from, vertex_handle v_to) const;

    /// Splits this edge in half by inserting a vertex (which is returned)
    /// Preserves face attributes
    /// The edge itself is deleted and two new ones are created
    vertex_handle split(edge_handle e) const;

    /// Moves both half-edges vertices to their next half-edge vertex
    /// Equivalent to an edge flip if both faces are triangular
    /// Preserves all attributes
    /// NOTE: does not work on boundaries!
    /// TODO: image
    void rotate_next(edge_handle e) const;
    /// Same as rotate_next but with the previous half-edge
    void rotate_prev(edge_handle e) const;

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
    halfedge_handle add_or_get(vertex_handle v_from, vertex_handle v_to) const;

    /// Returns the half-edge handle between two vertices (invalid if not found)
    /// O(valence) computation
    halfedge_handle find(vertex_handle v_from, vertex_handle v_to) const;

    /// Collapsed the given half-edge by removing it, keeping the to_vertex, and creating new triangles
    /// Preserves half-edge properties but not face or vertex ones
    /// Similar to a vertex collapse of the `from` vertex with triangulation towards `to`
    void collapse(halfedge_handle h) const;

    /// Splits this half-edge in half by inserting a vertex (which is returned)
    /// Preserves face attributes
    /// Contrary to edges().split, the edge is preserved and a single new one is inserted AFTER h
    /// (thus h->next() is the newly inserted edge and h->vertex_to() is the returned vertex)
    vertex_handle split(halfedge_handle h) const;

    /// Moves the to-vertex of this half-edge to the same as the next half-edge
    /// Preserves all attributes
    /// NOTE: does not work on boundaries!
    /// NOTE: does not work on triangles!
    /// TODO: image
    void rotate_next(halfedge_handle h) const;
    /// Same as rotate_next but with the previous half-edge
    void rotate_prev(halfedge_handle h) const;

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
struct primitive_ring : smart_range<this_t, typename primitive<tag>::handle>
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

template <class tag, class circulator>
struct halfedge_primitive_ring : primitive_ring<halfedge_primitive_ring<tag, circulator>, tag>
{
    halfedge_handle halfedge;
    halfedge_primitive_ring(halfedge_handle h) { halfedge = h; }

    // Iteration:
    circulator begin() const { return {halfedge, false}; }
    circulator end() const { return {halfedge, true}; }
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

/// all half-edges along a ring (next -> next -> ...)
struct halfedge_ring : halfedge_primitive_ring<halfedge_tag, halfedge_ring_circulator>
{
    using halfedge_primitive_ring<halfedge_tag, halfedge_ring_circulator>::halfedge_primitive_ring;
};

}
