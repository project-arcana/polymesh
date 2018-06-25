#pragma once

#include <cstddef>
#include <vector>

#include "iterators.hh"

namespace polymesh
{
/// Collection of all vertices of a mesh, including deleted ones
/// Basically a smart std::vector
struct vertex_collection
{
    Mesh* mesh;

    /// Number of vertices, INCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;
    /// Ensures that a given number of vertices can be stored without reallocation
    void reserve(int capacity) const;

    /// Adds a new vertex and returns its handle
    /// Does NOT invalidate any iterator!
    vertex_handle add() const;

    // TODO: delete

    // Iteration:
    vertex_iterator begin() const;
    vertex_iterator end() const;
};

/// same as vertex_collection but const
struct const_vertex_collection
{
    Mesh const* mesh;

    /// Number of vertices, INCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;

    // Iteration:
    vertex_iterator begin() const;
    vertex_iterator end() const;
};

/// Same as vertex_collection but only including valid, non-deleted vertices
/// (a bit slower than the normal collection)
/// (if mesh->is_compact(), identical to vertex_collection)
struct valid_vertex_collection
{
    Mesh const* mesh;

    /// Number of vertices, EXCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;

    // Iteration:
    valid_vertex_iterator begin() const;
    valid_vertex_iterator end() const;
};

/// Collection of all faces of a mesh, including deleted ones
/// Basically a smart std::vector
struct face_collection
{
    Mesh* mesh;

    /// Number of vertices, INCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;
    /// Ensures that a given number of faces can be stored without reallocation
    void reserve(int capacity) const;

    /// Adds a face consisting of N vertices
    /// The vertices must already be sorted in CCW order
    /// (note: trying to add already existing halfedges triggers assertions)
    template <size_t N>
    face_handle add(const vertex_handle (&v_handles)[N]) const;
    face_handle add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const;
    face_handle add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const;
    face_handle add(std::vector<vertex_handle> const& v_handles) const;
    face_handle add(vertex_handle const* v_handles, size_t vcnt) const;
    template <size_t N>
    face_handle add(const halfedge_handle (&half_loop)[N]) const;
    face_handle add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const;
    face_handle add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const;
    face_handle add(std::vector<halfedge_handle> const& half_loop) const;
    face_handle add(halfedge_handle const* half_loop, size_t vcnt) const;

    // TODO: delete

    // Iteration:
    face_iterator begin() const;
    face_iterator end() const;
};

/// same as face_collection but const
struct const_face_collection
{
    Mesh const* mesh;

    /// Number of faces, INCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;

    // Iteration:
    face_iterator begin() const;
    face_iterator end() const;
};

/// Same as face_collection but only including valid, non-deleted faces
/// (a bit slower than the normal collection)
/// (if mesh->is_compact(), identical to face_collection)
struct valid_face_collection
{
    Mesh const* mesh;

    /// Number of faces, EXCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;

    // Iteration:
    valid_face_iterator begin() const;
    valid_face_iterator end() const;
};

/// Collection of all edges of a mesh, including deleted ones
/// Basically a smart std::vector
struct edge_collection
{
    Mesh* mesh;

    /// Number of vertices, INCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;
    /// Ensures that a given number of edges can be stored without reallocation
    void reserve(int capacity) const;

    /// Adds an edge between two existing, distinct vertices
    /// if edge already exists, returns it
    edge_handle add_or_get(vertex_handle v_from, vertex_handle v_to);

    // Iteration:
    edge_iterator begin() const;
    edge_iterator end() const;
};

/// same as edge_collection but const
struct const_edge_collection
{
    Mesh const* mesh;

    /// Number of edges, INCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;

    // Iteration:
    edge_iterator begin() const;
    edge_iterator end() const;
};

/// Same as edge_collection but only including valid, non-deleted edges
/// (a bit slower than the normal collection)
/// (if mesh->is_compact(), identical to edge_collection)
struct valid_edge_collection
{
    Mesh const* mesh;

    /// Number of edges, EXCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;

    // Iteration:
    valid_edge_iterator begin() const;
    valid_edge_iterator end() const;
};

/// Collection of all half-edges of a mesh, including deleted ones
/// Basically a smart std::vector
struct halfedge_collection
{
    Mesh* mesh;

    /// Number of vertices, INCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;
    /// Ensures that a given number of half-edges can be stored without reallocation
    void reserve(int capacity) const;

    /// Adds an half-edge between two existing, distinct vertices
    /// if half-edge already exists, returns it
    /// (always adds opposite half-edge as well)
    halfedge_handle add_or_get(vertex_handle v_from, vertex_handle v_to);

    // Iteration:
    halfedge_iterator begin() const;
    halfedge_iterator end() const;
};

/// same as halfedge_collection but const
struct const_halfedge_collection
{
    Mesh const* mesh;

    /// Number of halfedges, INCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;

    // Iteration:
    halfedge_iterator begin() const;
    halfedge_iterator end() const;
};

/// Same as halfedge_collection but only including valid, non-deleted halfedges
/// (a bit slower than the normal collection)
/// (if mesh->is_compact(), identical to halfedge_collection)
struct valid_halfedge_collection
{
    Mesh const* mesh;

    /// Number of halfedges, EXCLUDING deleted/invalid ones
    /// O(1) computation
    int size() const;

    // Iteration:
    valid_halfedge_iterator begin() const;
    valid_halfedge_iterator end() const;
};
}
