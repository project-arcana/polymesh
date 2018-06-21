#pragma once

#include <cstddef>
#include <vector>

#include "EdgeHandle.hh"
#include "FaceHandle.hh"
#include "HalfedgeHandle.hh"
#include "VertexHandle.hh"

#include "Ranges.hh"

namespace polymesh
{
/**
 * @brief Half-Edge Mesh Datastructure
 *
 */
struct Mesh
{
    // accessors and iterators
public:
    /// TODO:
    /// vertices(), faces(), ...

    /// get handle from index
    FaceHandle handle_of(FaceIndex idx) const { return {this, idx}; }
    EdgeHandle handle_of(EdgeIndex idx) const { return {this, idx}; }
    VertexHandle handle_of(VertexIndex idx) const { return {this, idx}; }
    HalfedgeHandle handle_of(HalfedgeIndex idx) const { return {this, idx}; }

    /// number of primitives
    /// CAUTION: includes deleted ones!
    int size_faces() const { return mFaces.size(); }
    int size_vertices() const { return mVertices.size(); }
    int size_halfedges() const { return mHalfedges.size(); }
    int size_edges() const { return mHalfedges.size() >> 1; }

    /// iterator ranges for primitives
    ///
    /// CAUTION: includes deleted ones!
    ///   use compactify() to ensure that no deleted ones exist
    ///   use valid_xyz() to skip deleted ones during iteration (slower)
    ///
    /// NOTE: adding primitives does NOT invalidate ranges. (newly added ones are processed at the end)
    ///       deleting primitives does NOT invalidate ranges.
    vertex_range vertices() const { return {vertices_begin(), vertices_end()}; }
    skipping_vertex_range valid_vertices() const { return {valid_vertices_begin(), valid_vertices_end()}; }

    /// begin/end for ranges
    vertex_iterator vertices_begin() const { return {{this, VertexIndex(0)}}; }
    vertex_iterator vertices_end() const { return {{this, VertexIndex::invalid()}}; }
    skipping_vertex_iterator valid_vertices_begin() const { return {{this, VertexIndex(0)}}; }
    skipping_vertex_iterator valid_vertices_end() const { return {{this, VertexIndex::invalid()}}; }

    // modifiers
public:
    /// TODO:
    /// add_vertex/he/h/...

    /// Adds a single non-connected vertex
    /// Does NOT invalidate iterators!
    VertexHandle add_vertex();

    /// Adds a face consisting of N vertices
    template <size_t N>
    FaceHandle add_face(const VertexHandle (&vhandles)[N]);
    FaceHandle add_face(VertexHandle v0, VertexHandle v1, VertexHandle v2);
    FaceHandle add_face(VertexHandle v0, VertexHandle v1, VertexHandle v2, VertexHandle v3);
    FaceHandle add_face(std::vector<VertexHandle> vhandles);
    FaceHandle add_face(VertexHandle const *vhandles, size_t vcnt);

    // helper
public:
    /// Returns true if the mesh is guaranteed compact, otherwise call compactify() to be sure
    bool is_compact() const { return mCompact; }
    /// Removes all invalid/deleted primitives
    /// NOTE: cheap no-op if already compact
    void compactify();

    /// Asserts that mesh invariants hold, e.g. that the half-edge stored in a face actually bounds that face
    void assert_consistency() const;

    // reserves a certain number of primitives
    void reserve_faces(size_t capacity) { mFaces.reserve(capacity); }
    void reserve_vertices(size_t capacity) { mVertices.reserve(capacity); }
    void reserve_halfedges(size_t capacity) { mHalfedges.reserve(capacity); }

    // returns the next valid idx starting AFTER the given one
    // NOTE: the result can be invalid if the input was the last valid one
    VertexIndex next_valid_idx_after(VertexIndex idx) const;
    EdgeIndex next_valid_idx_after(EdgeIndex idx) const;
    FaceIndex next_valid_idx_after(FaceIndex idx) const;
    HalfedgeIndex next_valid_idx_after(HalfedgeIndex idx) const;
    // returns the next valid idx starting BEFORE the given one counting downwards
    VertexIndex prev_valid_idx_before(VertexIndex idx) const;
    EdgeIndex prev_valid_idx_before(EdgeIndex idx) const;
    FaceIndex prev_valid_idx_before(FaceIndex idx) const;
    HalfedgeIndex prev_valid_idx_before(HalfedgeIndex idx) const;

    // internal datastructures
private:
    struct Face
    {
        HalfedgeIndex halfedge; ///< one half-edge bounding this face

        bool is_valid() const { return halfedge.is_valid(); }
        void set_deleted() { halfedge = HalfedgeIndex::invalid(); }
    };

    struct Vertex
    {
        HalfedgeIndex outgoing_halfedge;

        /// a vertex can be valid even without outgoing halfedge
        bool is_valid() const { return outgoing_halfedge.value >= -1; }
        void set_deleted() { outgoing_halfedge = HalfedgeIndex(-2); }
    };

    struct Halfedge
    {
        VertexIndex vertex;
        FaceIndex face;              ///< might be invalid if boundary
        HalfedgeIndex next_halfedge; ///< CCW
        HalfedgeIndex prev_halfedge; ///< CW
        // opposite half-edge idx is "idx ^ 1"
        // edge idx is "idx >> 1"

        bool is_valid() const { return vertex.is_valid(); }
        bool is_boundary() const { return !face.is_valid(); }
        void set_deleted() { vertex = VertexIndex::invalid(); }
    };

    // internal primitives
private:
    std::vector<Face> mFaces;
    std::vector<Vertex> mVertices;
    std::vector<Halfedge> mHalfedges;

    // internal state
private:
    bool mCompact = true;
};

/// ======== IMPLEMENTATION ========

VertexHandle Mesh::add_vertex()
{
    auto idx = (int)mVertices.size();
    mVertices.push_back(Vertex());
    return handle_of(VertexIndex(idx));
}

FaceHandle Mesh::add_face(const VertexHandle *vhandles, size_t vcnt)
{
    auto fidx = (int)mFaces.size();
    Face f;
    assert(0 && "implement me"); /// TODO
    mFaces.push_back(f);
    return handle_of(FaceIndex(fidx));
}

FaceHandle Mesh::add_face(VertexHandle v0, VertexHandle v1, VertexHandle v2)
{
    VertexHandle vs[3] = {v0, v1, v2};
    return add_face(vs);
}

FaceHandle Mesh::add_face(VertexHandle v0, VertexHandle v1, VertexHandle v2, VertexHandle v3)
{
    VertexHandle vs[4] = {v0, v1, v2, v3};
    return add_face(vs);
}

FaceHandle Mesh::add_face(std::vector<VertexHandle> vhandles)
{
    return add_face(vhandles.data(), vhandles.size());
}

template <size_t N>
FaceHandle Mesh::add_face(const VertexHandle (&vhandles)[N])
{
    return add_face(vhandles, N);
}

VertexIndex Mesh::next_valid_idx_after(VertexIndex idx) const
{
    for (auto i = idx.value + 1; i < (int)mVertices.size(); ++i)
        if (mVertices[i].is_valid())
            return VertexIndex(i);
    return {}; // invalid
}

VertexIndex Mesh::prev_valid_idx_before(VertexIndex idx) const
{
    for (auto i = idx.value - 1; i >= 0; --i)
        if (mVertices[i].is_valid())
            return VertexIndex(i);
    return {}; // invalid
}

EdgeIndex Mesh::next_valid_idx_after(EdgeIndex idx) const
{
    for (auto i = (idx.value + 1) << 1; i < (int)mHalfedges.size(); i += 2)
        if (mHalfedges[i].is_valid())
            return EdgeIndex(i >> 1);
    return {}; // invalid
}

EdgeIndex Mesh::prev_valid_idx_before(EdgeIndex idx) const
{
    for (auto i = (idx.value - 1) << 1; i >= 0; i -= 2)
        if (mHalfedges[i].is_valid())
            return EdgeIndex(i >> 1);
    return {}; // invalid
}

FaceIndex Mesh::next_valid_idx_after(FaceIndex idx) const
{
    for (auto i = idx.value + 1; i < (int)mFaces.size(); ++i)
        if (mFaces[i].is_valid())
            return FaceIndex(i);
    return {}; // invalid
}

FaceIndex Mesh::prev_valid_idx_before(FaceIndex idx) const
{
    for (auto i = idx.value - 1; i >= 0; --i)
        if (mFaces[i].is_valid())
            return FaceIndex(i);
    return {}; // invalid
}

HalfedgeIndex Mesh::next_valid_idx_after(HalfedgeIndex idx) const
{
    for (auto i = idx.value + 1; i < (int)mHalfedges.size(); ++i)
        if (mHalfedges[i].is_valid())
            return HalfedgeIndex(i);
    return {}; // invalid
}

HalfedgeIndex Mesh::prev_valid_idx_before(HalfedgeIndex idx) const
{
    for (auto i = idx.value - 1; i >= 0; --i)
        if (mHalfedges[i].is_valid())
            return HalfedgeIndex(i);
    return {}; // invalid
}

/// ======== ITERATOR IMPLEMENTATION ========

skipping_vertex_iterator &skipping_vertex_iterator::operator++()
{
    handle.idx = handle.mesh->next_valid_idx_after(handle.idx);
    return *this;
}
vertex_iterator &vertex_iterator::operator++()
{
    handle.idx.value++;
    if (handle.idx.value >= handle.mesh->size_vertices())
        handle.idx.value = -1;
    return *this;
}
}
