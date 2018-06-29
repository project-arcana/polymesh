#pragma once

#include <cstddef>
#include <vector>

#include "iterators.hh"

namespace polymesh
{
template<class mesh_ptr, class tag, class iterator>
struct smart_collection
{
    template<typename AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;

    /// Number of primitives, INCLUDING those marked for deletion
    /// O(1) computation
    int size() const { return iterator::primitive_size(*mesh); }

    /// Ensures that a given number of primitives can be stored without reallocation
    void reserve(int capacity) const { return primitive<tag>::reserve(*mesh, capacity); }

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
template<class iterator>
struct vertex_collection : smart_collection<Mesh *, vertex_tag, iterator>
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
template<class iterator>
struct face_collection : smart_collection<Mesh *, face_tag, iterator>
{
    /// Adds a face consisting of N vertices
    /// The vertices must already be sorted in CCW order
    /// (note: trying to add already existing halfedges triggers assertions)
    template <size_t N>
    face_handle add(const vertex_handle(&v_handles)[N]) const;
    face_handle add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const;
    face_handle add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const;
    face_handle add(std::vector<vertex_handle> const& v_handles) const;
    face_handle add(vertex_handle const* v_handles, int vcnt) const;
    template <size_t N>
    face_handle add(const halfedge_handle(&half_loop)[N]) const;
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
template<class iterator>
struct edge_collection : smart_collection<Mesh *, edge_tag, iterator>
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
template<class iterator>
struct halfedge_collection : smart_collection<Mesh *, halfedge_tag, iterator>
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

// rings

/// all vertices belonging to a face
struct face_vertex_ring
{
    face_handle face;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(vertex_handle v) const;

    // Iteration:
    face_vertex_circulator begin() const { return {face, false}; }
    face_vertex_circulator end() const { return {face, true}; }
};

/// all halfedges belonging to a face
struct face_halfedge_ring
{
    face_handle face;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(halfedge_handle h) const;

    // Iteration:
    face_halfedge_circulator begin() const { return {face, false}; }
    face_halfedge_circulator end() const { return {face, true}; }
};

/// all edges belonging to a face
struct face_edge_ring
{
    face_handle face;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(edge_handle e) const;

    // Iteration:
    face_edge_circulator begin() const { return {face, false}; }
    face_edge_circulator end() const { return {face, true}; }
};

/// all adjacent faces belonging to a face
struct face_face_ring
{
    face_handle face;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(face_handle f) const;

    // Iteration:
    face_face_circulator begin() const { return {face, false}; }
    face_face_circulator end() const { return {face, true}; }
};

/// all outgoing half-edges from a vertex
struct vertex_halfedge_out_ring
{
    vertex_handle vertex;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(halfedge_handle h) const;

    // Iteration:
    vertex_halfedge_out_circulator begin() const { return {vertex, vertex.is_isolated()}; }
    vertex_halfedge_out_circulator end() const { return {vertex, true}; }
};

/// all incoming half-edges from a vertex
struct vertex_halfedge_in_ring
{
    vertex_handle vertex;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(halfedge_handle h) const;

    // Iteration:
    vertex_halfedge_in_circulator begin() const { return {vertex, vertex.is_isolated()}; }
    vertex_halfedge_in_circulator end() const { return {vertex, true}; }
};

/// all adjacent vertices of a vertex
struct vertex_vertex_ring
{
    vertex_handle vertex;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(vertex_handle v) const;

    // Iteration:
    vertex_vertex_circulator begin() const { return {vertex, vertex.is_isolated()}; }
    vertex_vertex_circulator end() const { return {vertex, true}; }
};

/// all adjacent edges of a vertex
struct vertex_edge_ring
{
    vertex_handle vertex;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(edge_handle e) const;

    // Iteration:
    vertex_edge_circulator begin() const { return {vertex, vertex.is_isolated()}; }
    vertex_edge_circulator end() const { return {vertex, true}; }
};

/// all adjacent faces of a vertex (INCLUDES invalid ones for boundaries)
struct vertex_face_ring
{
    vertex_handle vertex;

    /// Number of vertices
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(face_handle f) const;

    // Iteration:
    vertex_face_circulator begin() const { return {vertex, vertex.is_isolated()}; }
    vertex_face_circulator end() const { return {vertex, true}; }
};


/// ======== IMPLEMENTATION ========

inline int face_vertex_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}
inline int face_edge_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}
inline int face_halfedge_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}
inline int face_face_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

inline int vertex_halfedge_out_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

inline int vertex_halfedge_in_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

inline int vertex_vertex_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

inline int vertex_edge_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

inline int vertex_face_ring::size() const
{
    auto cnt = 0;
    for (auto v : *this)
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

inline bool face_vertex_ring::contains(vertex_handle v) const
{
    for (auto v2 : *this)
        if (v == v2)
            return true;
    return false;
}

inline bool face_edge_ring::contains(edge_handle e) const
{
    for (auto e2 : *this)
        if (e == e2)
            return true;
    return false;
}

inline bool face_halfedge_ring::contains(halfedge_handle h) const
{
    for (auto h2 : *this)
        if (h == h2)
            return true;
    return false;
}

inline bool face_face_ring::contains(face_handle f) const
{
    for (auto f2 : *this)
        if (f == f2)
            return true;
    return false;
}

inline bool vertex_halfedge_out_ring::contains(halfedge_handle h) const
{
    for (auto h2 : *this)
        if (h == h2)
            return true;
    return false;
}

inline bool vertex_halfedge_in_ring::contains(halfedge_handle h) const
{
    for (auto h2 : *this)
        if (h == h2)
            return true;
    return false;
}

inline bool vertex_vertex_ring::contains(vertex_handle v) const
{
    for (auto v2 : *this)
        if (v == v2)
            return true;
    return false;
}

inline bool vertex_edge_ring::contains(edge_handle e) const
{
    for (auto e2 : *this)
        if (e == e2)
            return true;
    return false;
}

inline bool vertex_face_ring::contains(face_handle f) const
{
    for (auto f2 : *this)
        if (f == f2)
            return true;
    return false;
}
}
