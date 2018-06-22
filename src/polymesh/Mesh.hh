#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "cursors.hh"
#include "ranges.hh"

namespace polymesh
{
using SharedMesh = std::shared_ptr<Mesh>;

/**
 * @brief Half-edge Mesh Datastructure
 *
 * * Primitives are accessed via the smart collections mesh.<primitive>()
 *   (where <primitive> can be vertices, edges, faces, or halfedges)
 *
 * * Primitives can be added via <primitive>().add()
 *   (added primitives are at the end of the collection)
 *
 * * Primitives can be deleted via <primitive>().delete(...)
 *   (deleted primitives are invalidated (flagged for removal). call compactify() to remove them)
 *
 * * `for (auto h : <primitive>())` iterates over _all_ primitives, including invalid ones
 *   (`for (auto h : valid_<primitive>())` skips over invalid ones)
 */
struct Mesh
{
    // accessors and iterators
public:
    /// TODO:
    /// vertices(), faces(), ...

    /// get handle from index
    face_handle handle_of(face_index idx) const { return {this, idx}; }
    edge_handle handle_of(edge_index idx) const { return {this, idx}; }
    vertex_handle handle_of(vertex_index idx) const { return {this, idx}; }
    halfedge_handle handle_of(halfedge_index idx) const { return {this, idx}; }

    /// smart collections for primitives INCLUDING deleted ones
    /// Also primary interfaces for querying size and adding primitives
    ///
    /// CAUTION: includes deleted ones!
    ///   use compactify() to ensure that no deleted ones exist
    ///   use valid_<primitive>() to skip deleted ones during iteration (slower)
    ///
    /// NOTE: adding primitives does NOT invalidate ranges. (newly added ones are NOT processed though)
    ///       deleting primitives does NOT invalidate ranges.
    vertex_collection vertices() { return {this}; }
    face_collection faces() { return {this}; }
    edge_collection edges() { return {this}; }
    halfedge_collection halfedges() { return {this}; }

    /// smart collections for VALID primitives (EXCLUDING deleted ones)
    ///
    /// NOTE: if mesh.is_compact() is guaranteed, <primitive>() is faster than valid_<primitive>()
    ///
    /// NOTE: adding primitives does NOT invalidate ranges. (newly added ones are NOT processed though)
    ///       deleting primitives does NOT invalidate ranges. (they will be skipped)
    valid_vertex_collection valid_vertices() const { return {this}; }
    valid_face_collection valid_faces() const { return {this}; }
    valid_edge_collection valid_edges() const { return {this}; }
    valid_halfedge_collection valid_halfedges() const { return {this}; }

    // helper
public:
    /// Returns true if the mesh is guaranteed compact, otherwise call compactify() to be sure
    bool is_compact() const { return mCompact; }
    /// Removes all invalid/deleted primitives
    /// NOTE: cheap no-op if already compact
    void compactify();

    /// Asserts that mesh invariants hold, e.g. that the half-edge stored in a face actually bounds that face
    void assert_consistency() const;

    // ctor
public:
    Mesh() = default;

    /// Meshes can be neither moved nor copied because properties depend on the Mesh address
    Mesh(Mesh const &) = delete;
    Mesh(Mesh &&) = delete;
    Mesh &operator=(Mesh const &) = delete;
    Mesh &operator=(Mesh &&) = delete;

    /// Creates a new mesh and returns a shared_ptr to it
    static SharedMesh create() { return std::make_shared<Mesh>(); }

    // internal helper
private:
    // reserves a certain number of primitives
    void reserve_faces(size_t capacity) { mFaces.reserve(capacity); }
    void reserve_vertices(size_t capacity) { mVertices.reserve(capacity); }
    void reserve_edges(size_t capacity) { mHalfedges.reserve(capacity * 2); }
    void reserve_halfedges(size_t capacity) { mHalfedges.reserve(capacity); }

    int size_faces() const { return mFaces.size(); }
    int size_vertices() const { return mVertices.size(); }
    int size_edges() const { return mHalfedges.size() >> 1; }
    int size_halfedges() const { return mHalfedges.size(); }

    int size_valid_faces() const { return mFaces.size() - mDeletedFaces; }
    int size_valid_vertices() const { return mVertices.size() - mDeletedVertices; }
    int size_valid_edges() const { return (mHalfedges.size() - mDeletedHalfedges) >> 1; }
    int size_valid_halfedges() const { return mHalfedges.size() - mDeletedHalfedges; }

    // returns the next valid idx (returns the given one if valid)
    // NOTE: the result can be invalid if no valid one was found
    vertex_index next_valid_idx_from(vertex_index idx) const;
    edge_index next_valid_idx_from(edge_index idx) const;
    face_index next_valid_idx_from(face_index idx) const;
    halfedge_index next_valid_idx_from(halfedge_index idx) const;
    // returns the next valid idx (returns the given one if valid) counting DOWNWARDS
    vertex_index prev_valid_idx_from(vertex_index idx) const;
    edge_index prev_valid_idx_from(edge_index idx) const;
    face_index prev_valid_idx_from(face_index idx) const;
    halfedge_index prev_valid_idx_from(halfedge_index idx) const;

    /// Adds a single non-connected vertex
    /// Does NOT invalidate iterators!
    vertex_handle add_vertex();

    /// Adds a face consisting of N vertices
    /// The vertices must already be sorted in CCW order
    /// (note: trying to add already existing halfedges triggers assertions)
    face_handle add_face(vertex_handle const *vhandles, size_t vcnt);

    // Iterators
    vertex_iterator vertices_begin() const { return {{this, vertex_index(0)}}; }
    vertex_iterator vertices_end() const { return {{this, vertex_index(mVertices.size())}}; }
    valid_vertex_iterator valid_vertices_begin() const { return {{this, vertex_index(0)}}; }
    valid_vertex_iterator valid_vertices_end() const { return {{this, vertex_index(mVertices.size())}}; }

    face_iterator faces_begin() const { return {{this, face_index(0)}}; }
    face_iterator faces_end() const { return {{this, face_index(mFaces.size())}}; }
    valid_face_iterator valid_faces_begin() const { return {{this, face_index(0)}}; }
    valid_face_iterator valid_faces_end() const { return {{this, face_index(mFaces.size())}}; }

    edge_iterator edges_begin() const { return {{this, edge_index(0)}}; }
    edge_iterator edges_end() const { return {{this, edge_index(mHalfedges.size() >> 1)}}; }
    valid_edge_iterator valid_edges_begin() const { return {{this, edge_index(0)}}; }
    valid_edge_iterator valid_edges_end() const { return {{this, edge_index(mHalfedges.size() >> 1)}}; }

    halfedge_iterator halfedges_begin() const { return {{this, halfedge_index(0)}}; }
    halfedge_iterator halfedges_end() const { return {{this, halfedge_index(mHalfedges.size())}}; }
    valid_halfedge_iterator valid_halfedges_begin() const { return {{this, halfedge_index(0)}}; }
    valid_halfedge_iterator valid_halfedges_end() const { return {{this, halfedge_index(mHalfedges.size())}}; }

    // internal datastructures
private:
    struct face
    {
        halfedge_index halfedge; ///< one half-edge bounding this face

        bool is_valid() const { return halfedge.is_valid(); }
        void set_deleted() { halfedge = halfedge_index::invalid(); }
    };

    struct vertex
    {
        halfedge_index outgoing_halfedge;

        /// a vertex can be valid even without outgoing halfedge
        bool is_valid() const { return outgoing_halfedge.value >= -1; }
        void set_deleted() { outgoing_halfedge = halfedge_index(-2); }
    };

    struct halfedge
    {
        vertex_index vertex;
        face_index face;              ///< might be invalid if boundary
        halfedge_index next_halfedge; ///< CCW
        halfedge_index prev_halfedge; ///< CW
        // opposite half-edge idx is "idx ^ 1"
        // edge idx is "idx >> 1"

        bool is_valid() const { return vertex.is_valid(); }
        bool is_boundary() const { return !face.is_valid(); }
        void set_deleted() { vertex = vertex_index::invalid(); }
    };

    // internal primitives
private:
    std::vector<face> mFaces;
    std::vector<vertex> mVertices;
    std::vector<halfedge> mHalfedges;

    // internal state
private:
    bool mCompact = true;
    int mDeletedFaces = 0;
    int mDeletedVertices = 0;
    int mDeletedHalfedges = 0;

    // friends
private:
    friend struct vertex_handle;
    friend struct vertex_collection;
    friend struct vertex_iterator;
    friend struct valid_vertex_iterator;
    friend struct valid_vertex_collection;

    friend struct face_handle;
    friend struct face_collection;
    friend struct face_iterator;
    friend struct valid_face_iterator;
    friend struct valid_face_collection;

    friend struct edge_handle;
    friend struct edge_collection;
    friend struct edge_iterator;
    friend struct valid_edge_iterator;
    friend struct valid_edge_collection;

    friend struct halfedge_handle;
    friend struct halfedge_collection;
    friend struct halfedge_iterator;
    friend struct valid_halfedge_iterator;
    friend struct valid_halfedge_collection;
};

/// ======== IMPLEMENTATION ========

vertex_handle Mesh::add_vertex()
{
    auto idx = (int)mVertices.size();
    mVertices.push_back(vertex());
    /// TODO: properties
    return handle_of(vertex_index(idx));
}

face_handle Mesh::add_face(const vertex_handle *vhandles, size_t vcnt)
{
    auto fidx = (int)mFaces.size();
    face f;
    /// TODO: properties
    assert(0 && "implement me"); /// TODO
    mFaces.push_back(f);
    return handle_of(face_index(fidx));
}

vertex_index Mesh::next_valid_idx_from(vertex_index idx) const
{
    for (auto i = idx.value; i < (int)mVertices.size(); ++i)
        if (mVertices[i].is_valid())
            return vertex_index(i);
    return vertex_index(mVertices.size()); // end index
}

vertex_index Mesh::prev_valid_idx_from(vertex_index idx) const
{
    for (auto i = idx.value; i >= 0; --i)
        if (mVertices[i].is_valid())
            return vertex_index(i);
    return {}; // invalid
}

edge_index Mesh::next_valid_idx_from(edge_index idx) const
{
    for (auto i = idx.value << 1; i < (int)mHalfedges.size(); i += 2)
        if (mHalfedges[i].is_valid())
            return edge_index(i >> 1);
    return edge_index(mHalfedges.size() >> 1); // end index
}

edge_index Mesh::prev_valid_idx_from(edge_index idx) const
{
    for (auto i = idx.value << 1; i >= 0; i -= 2)
        if (mHalfedges[i].is_valid())
            return edge_index(i >> 1);
    return {}; // invalid
}

face_index Mesh::next_valid_idx_from(face_index idx) const
{
    for (auto i = idx.value; i < (int)mFaces.size(); ++i)
        if (mFaces[i].is_valid())
            return face_index(i);
    return face_index(mFaces.size()); // end index
}

face_index Mesh::prev_valid_idx_from(face_index idx) const
{
    for (auto i = idx.value; i >= 0; --i)
        if (mFaces[i].is_valid())
            return face_index(i);
    return {}; // invalid
}

halfedge_index Mesh::next_valid_idx_from(halfedge_index idx) const
{
    for (auto i = idx.value; i < (int)mHalfedges.size(); ++i)
        if (mHalfedges[i].is_valid())
            return halfedge_index(i);
    return halfedge_index(mHalfedges.size()); // end index
}

halfedge_index Mesh::prev_valid_idx_from(halfedge_index idx) const
{
    for (auto i = idx.value; i >= 0; --i)
        if (mHalfedges[i].is_valid())
            return halfedge_index(i);
    return {}; // invalid
}

/// ======== ITERATOR IMPLEMENTATION ========

valid_vertex_iterator &valid_vertex_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
vertex_iterator &vertex_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}

valid_face_iterator &valid_face_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
face_iterator &face_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}

valid_edge_iterator &valid_edge_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
edge_iterator &edge_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}

valid_halfedge_iterator &valid_halfedge_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
halfedge_iterator &halfedge_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}

/// ======== RANGES IMPLEMENTATION ========

// - Vertices -

int vertex_collection::size() const
{
    return mesh->size_vertices();
}

void vertex_collection::reserve(int capacity) const
{
    mesh->reserve_vertices(capacity);
}

vertex_handle vertex_collection::add() const
{
    return mesh->add_vertex();
}

vertex_iterator vertex_collection::begin() const
{
    return mesh->vertices_begin();
}

vertex_iterator vertex_collection::end() const
{
    return mesh->vertices_end();
}

int valid_vertex_collection::size() const
{
    return mesh->size_valid_vertices();
}

valid_vertex_iterator valid_vertex_collection::begin() const
{
    return mesh->valid_vertices_begin();
}

valid_vertex_iterator valid_vertex_collection::end() const
{
    return mesh->valid_vertices_end();
}

// - Faces -

int face_collection::size() const
{
    return mesh->size_faces();
}

void face_collection::reserve(int capacity) const
{
    mesh->reserve_faces(capacity);
}

face_handle face_collection::add_face(const vertex_handle *vhandles, size_t vcnt) const
{
    return mesh->add_face(vhandles, vcnt);
}

face_handle face_collection::add_face(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    vertex_handle vs[3] = {v0, v1, v2};
    return add_face(vs);
}

face_handle face_collection::add_face(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    vertex_handle vs[4] = {v0, v1, v2, v3};
    return add_face(vs);
}

face_handle face_collection::add_face(std::vector<vertex_handle> const &vhandles) const
{
    return add_face(vhandles.data(), vhandles.size());
}

template <size_t N>
face_handle face_collection::add_face(const vertex_handle (&vhandles)[N]) const
{
    return add_face(vhandles, N);
}

face_iterator face_collection::begin() const
{
    return mesh->faces_begin();
}

face_iterator face_collection::end() const
{
    return mesh->faces_end();
}

int valid_face_collection::size() const
{
    return mesh->size_valid_faces();
}

valid_face_iterator valid_face_collection::begin() const
{
    return mesh->valid_faces_begin();
}

valid_face_iterator valid_face_collection::end() const
{
    return mesh->valid_faces_end();
}

// - Edges -

int edge_collection::size() const
{
    return mesh->size_edges();
}

void edge_collection::reserve(int capacity) const
{
    mesh->reserve_edges(capacity);
}

edge_iterator edge_collection::begin() const
{
    return mesh->edges_begin();
}

edge_iterator edge_collection::end() const
{
    return mesh->edges_end();
}

int valid_edge_collection::size() const
{
    return mesh->size_valid_edges();
}

valid_edge_iterator valid_edge_collection::begin() const
{
    return mesh->valid_edges_begin();
}

valid_edge_iterator valid_edge_collection::end() const
{
    return mesh->valid_edges_end();
}

// - Halfedges -

int halfedge_collection::size() const
{
    return mesh->size_halfedges();
}

void halfedge_collection::reserve(int capacity) const
{
    mesh->reserve_halfedges(capacity);
}

halfedge_iterator halfedge_collection::begin() const
{
    return mesh->halfedges_begin();
}

halfedge_iterator halfedge_collection::end() const
{
    return mesh->halfedges_end();
}

int valid_halfedge_collection::size() const
{
    return mesh->size_valid_halfedges();
}

valid_halfedge_iterator valid_halfedge_collection::begin() const
{
    return mesh->valid_halfedges_begin();
}

valid_halfedge_iterator valid_halfedge_collection::end() const
{
    return mesh->valid_halfedges_end();
}
}
