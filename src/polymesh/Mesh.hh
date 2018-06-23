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
 * @brief Half-edge Mesh Data Structure
 *
 *  * Primitives are accessed via the smart collections mesh.<primitive>()
 *    (where <primitive> can be vertices, edges, faces, or halfedges)
 *
 *  * Primitives can be added via <primitive>().add()
 *    (added primitives are at the end of the collection)
 *
 *  * Primitives can be deleted via <primitive>().delete(...)
 *    (deleted primitives are invalidated (flagged for removal). call compactify() to remove them)
 *
 *  * `for (auto h : <primitive>())` iterates over _all_ primitives, including invalid ones
 *    (`for (auto h : valid_<primitive>())` skips over invalid ones)
 *
 * For more concept documents see:
 *  * http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
 *  * https://www.openmesh.org/media/Documentations/OpenMesh-Doc-Latest/a03930.html
 */
struct Mesh
{
    // accessors and iterators
public:
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

    /// get handle from index
    face_handle handle_of(face_index idx) const { return {this, idx}; }
    edge_handle handle_of(edge_index idx) const { return {this, idx}; }
    vertex_handle handle_of(vertex_index idx) const { return {this, idx}; }
    halfedge_handle handle_of(halfedge_index idx) const { return {this, idx}; }

    /// get handle from index, subscript version
    face_handle operator[](face_index idx) const { return handle_of(idx); }
    edge_handle operator[](edge_index idx) const { return handle_of(idx); }
    vertex_handle operator[](vertex_index idx) const { return handle_of(idx); }
    halfedge_handle operator[](halfedge_index idx) const { return handle_of(idx); }

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

    /// Adds a single non-connected vertex
    /// Does NOT invalidate iterators!
    vertex_index add_vertex();

    /// Adds a face consisting of N vertices
    /// The vertices must already be sorted in CCW order
    /// (note: trying to add already existing halfedges triggers assertions)
    face_index add_face(vertex_handle const *v_handles, size_t vcnt);
    face_index add_face(vertex_index const *v_indices, size_t vcnt);
    face_index add_face(halfedge_handle const *half_loop, size_t vcnt);
    face_index add_face(halfedge_index const *half_loop, size_t vcnt);

    /// Adds an edge between two existing, distinct vertices
    /// if edge already exists, returns it
    edge_index add_or_get_edge(vertex_index v_from, vertex_index v_to);

    /// same as add_or_get_edge but returns the appropriate half-edge
    halfedge_index add_or_get_halfedge(vertex_index v_from, vertex_index v_to);

    // Properties
    bool is_boundary(vertex_index idx) const;
    bool is_boundary(halfedge_index idx) const;

    /// Returns the opposite of a given valid half-edge
    halfedge_index opposite(halfedge_index he) const;

    /// Makes two half-edges adjacent
    /// Ensures:
    ///     * he_in->next == he_out
    ///     * he_out->prev == he_in
    /// Requires:
    ///     * he_in->is_free()
    ///     * he_out->is_free()
    /// Only works if a free incident half-edge is available
    void make_adjacent(halfedge_index he_in, halfedge_index he_out);

    /// finds the next free incoming half-edge around a certain vertex
    /// starting from in_begin, EXCLUDING in_end (if in_begin == in_end, the whole vertex is searched)
    /// returns invalid index if no edge is found
    halfedge_index find_free_incident(halfedge_index in_begin, halfedge_index in_end) const;
    /// finds a free incident incoming half-edge around a given vertex
    halfedge_index find_free_incident(vertex_index v) const;

    /// returns half-edge going from `from`, point to `to`
    /// returns invalid index if not exists
    halfedge_index find_halfedge(vertex_index from, vertex_index to) const;

    /// returns edge index belonging to a half-edge
    edge_index edge_of(halfedge_index idx) const { return edge_index(idx.value >> 1); }
    /// returns a half-edge belonging to an edge
    halfedge_index halfedge_of(edge_index idx, int i) const { return halfedge_index((idx.value << 1) + i); }

    /// returns the vertex that this half-edge is pointing to
    vertex_index to_vertex_of(halfedge_index idx) const { return mHalfedges[idx.value].to_vertex; }
    /// returns the vertex that this half-edge is leaving from
    vertex_index from_vertex_of(halfedge_index idx) const { return mHalfedges[opposite(idx).value].to_vertex; }

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
        bool is_isolated() const { return !outgoing_halfedge.is_valid(); }
        void set_deleted() { outgoing_halfedge = halfedge_index(-2); }
        // is_boundary: check if outgoing_halfedge is boundary
    };

    struct halfedge
    {
        vertex_index to_vertex;       ///< half-edge points towards this vertex
        face_index face;              ///< might be invalid if boundary
        halfedge_index next_halfedge; ///< CCW
        halfedge_index prev_halfedge; ///< CW
        // opposite half-edge idx is "idx ^ 1"
        // edge idx is "idx >> 1"

        bool is_valid() const { return to_vertex.is_valid(); }

        /// a half-edge is free if it is a boundary, aka has no associated face
        bool is_free() const { return !face.is_valid(); }

        // CAUTION: delete both HE belonging to an edge
        void set_deleted() { to_vertex = vertex_index::invalid(); }
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

    std::vector<halfedge_index> mFaceInsertCache;

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

vertex_index Mesh::add_vertex()
{
    /// TODO: properties

    auto idx = vertex_index((int)mVertices.size());
    mVertices.push_back(vertex());
    return idx;
}

face_index Mesh::add_face(const vertex_handle *v_handles, size_t vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0u; i < vcnt; ++i)
        mFaceInsertCache[i] = find_halfedge(v_handles[i].idx, v_handles[(i + 1) % vcnt].idx);
    return add_face(mFaceInsertCache.data(), vcnt);
}

face_index Mesh::add_face(const vertex_index *v_indices, size_t vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0u; i < vcnt; ++i)
        mFaceInsertCache[i] = find_halfedge(v_indices[i], v_indices[(i + 1) % vcnt]);
    return add_face(mFaceInsertCache.data(), vcnt);
}

face_index Mesh::add_face(const halfedge_handle *half_loop, size_t vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0u; i < vcnt; ++i)
        mFaceInsertCache[i] = half_loop[i].idx;
    return add_face(mFaceInsertCache.data(), vcnt);
}

face_index Mesh::add_face(const halfedge_index *half_loop, size_t vcnt)
{
    assert(vcnt >= 3 && "no support for less-than-triangular faces");
    /// TODO: properties

    auto fidx = face_index((int)mFaces.size());

    // ensure that half-edges are adjacent at each vertex
    for (auto i = 0u; i <= vcnt; ++i)
    {
        auto h0 = half_loop[i];
        auto h1 = half_loop[(i + 1) % vcnt];

        // half-edge must form a chain
        assert(to_vertex_of(h0) == from_vertex_of(h1));
        // half-edge must be free, i.e. allow a new polygon
        assert(mHalfedges[h0.value].is_free());

        // make them adjacent
        make_adjacent(h0, h1);

        // link face
        mHalfedges[h0.value].face = fidx;
    }

    // set up face data
    face f;
    f.halfedge = half_loop[0];
    mFaces.push_back(f);

    return fidx;
}

edge_index Mesh::add_or_get_edge(vertex_index v_from, vertex_index v_to)
{
    assert(v_from != v_to);

    // already exists?
    auto he = find_halfedge(v_from, v_to);
    if (he.is_valid())
        return edge_of(he);

    auto &vd_from = mVertices[v_from.value];
    auto &vd_to = mVertices[v_to.value];

    // allocate new
    auto he_size = (int)mHalfedges.size();
    auto h_from_to_idx = halfedge_index(he_size + 0);
    auto h_to_from_idx = halfedge_index(he_size + 1);
    auto eidx = edge_index(he_size >> 1);
    halfedge h_from_to;
    halfedge h_to_from;

    // setup data (self-connected edge)
    h_from_to.to_vertex = v_to;
    h_to_from.to_vertex = v_from;
    h_from_to.next_halfedge = h_to_from_idx;
    h_to_from.next_halfedge = h_from_to_idx;

    // link from vertex
    if (vd_from.is_isolated())
        vd_from.outgoing_halfedge = h_from_to_idx;
    else
    {
        auto from_in_idx = find_free_incident(v_from);
        assert(from_in_idx.is_valid() && "vertex is already fully connected");
        auto &from_in = mHalfedges[from_in_idx.value];
        auto from_out_idx = from_in.next_halfedge;
        auto &from_out = mHalfedges[from_out_idx.value];

        from_in.next_halfedge = h_from_to_idx;
        h_from_to.prev_halfedge = from_in_idx;

        h_to_from.next_halfedge = from_out_idx;
        from_out.prev_halfedge = h_to_from_idx;
    }

    // link to vertex
    if (vd_to.is_isolated())
        vd_to.outgoing_halfedge = h_from_to_idx;
    else
    {
        auto to_in_idx = find_free_incident(v_to);
        assert(to_in_idx.is_valid() && "vertex is already fully connected");
        auto &to_in = mHalfedges[to_in_idx.value];
        auto to_out_idx = to_in.next_halfedge;
        auto &to_out = mHalfedges[to_out_idx.value];

        to_in.next_halfedge = h_to_from_idx;
        h_to_from.prev_halfedge = to_in_idx;

        h_from_to.next_halfedge = to_out_idx;
        to_out.prev_halfedge = h_from_to_idx;
    }

    // finalize
    mHalfedges.push_back(h_from_to);
    mHalfedges.push_back(h_to_from);
    return eidx;
}

halfedge_index Mesh::add_or_get_halfedge(vertex_index v_from, vertex_index v_to)
{
    auto e = add_or_get_edge(v_from, v_to);
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    return mHalfedges[h0.value].to_vertex == v_to ? h0 : h1;
}

void Mesh::make_adjacent(halfedge_index he_in, halfedge_index he_out)
{
    // see http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm ::makeAdjacent
    auto &in = mHalfedges[he_in.value];
    auto &out = mHalfedges[he_out.value];

    auto he_b = in.next_halfedge;
    auto he_d = out.prev_halfedge;

    // already correct
    if (he_b == he_out)
        return;

    // find free half-edge after `out` but before `in`
    auto he_g = find_free_incident(opposite(he_out), he_in);
    assert(he_g.is_valid()); // unable to make adjacent

    auto &b = mHalfedges[he_b.value];
    auto &d = mHalfedges[he_d.value];
    auto &g = mHalfedges[he_g.value];

    auto he_h = g.next_halfedge;
    auto &h = mHalfedges[he_d.value];

    // properly rewire
    in.next_halfedge = he_out;
    out.prev_halfedge = he_in;

    g.next_halfedge = he_b;
    b.prev_halfedge = he_g;

    d.next_halfedge = he_h;
    h.prev_halfedge = he_d;
}

halfedge_index Mesh::find_free_incident(halfedge_index in_begin, halfedge_index in_end) const
{
    assert(mHalfedges[in_begin.value].to_vertex == mHalfedges[in_end.value].to_vertex);

    auto he = in_begin;
    do
    {
        auto const &h = mHalfedges[he.value];
        assert(h.to_vertex == mHalfedges[in_end.value].to_vertex);

        // free? found one!
        if (h.is_free())
            return he;

        // next half-edge of vertex
        he = opposite(h.next_halfedge);
    } while (he != in_end);

    return halfedge_index::invalid();
}

halfedge_index Mesh::find_free_incident(vertex_index v) const
{
    auto in_begin = opposite(mVertices[v.value].outgoing_halfedge);
    return find_free_incident(in_begin, in_begin);
}

halfedge_index Mesh::find_halfedge(vertex_index from, vertex_index to) const
{
    auto he_begin = mVertices[from.value].outgoing_halfedge;
    auto he = he_begin;
    do
    {
        auto const &h = mHalfedges[he.value];

        // found?
        if (h.to_vertex == to)
            return he;

        // advance
        he = opposite(h.next_halfedge);

    } while (he != he_begin);

    return halfedge_index::invalid(); // not found
}

bool Mesh::is_boundary(vertex_index idx) const
{
    auto const &v = mVertices[idx.value];
    return v.outgoing_halfedge.is_valid() && is_boundary(v.outgoing_halfedge);
}

bool Mesh::is_boundary(halfedge_index idx) const
{
    return mHalfedges[idx.value].is_free();
}

halfedge_index Mesh::opposite(halfedge_index he) const
{
    return halfedge_index(he.value ^ 1);
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
    return mesh->handle_of(mesh->add_vertex());
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

face_handle face_collection::add_face(const vertex_handle *v_handles, size_t vcnt) const
{
    return mesh->handle_of(mesh->add_face(v_handles, vcnt));
}

face_handle face_collection::add_face(const halfedge_handle *half_loop, size_t vcnt) const
{
    return mesh->handle_of(mesh->add_face(half_loop, vcnt));
}

face_handle face_collection::add_face(std::vector<vertex_handle> const &v_handles) const
{
    return add_face(v_handles.data(), v_handles.size());
}

face_handle face_collection::add_face(std::vector<halfedge_handle> const &half_loop) const
{
    return add_face(half_loop.data(), half_loop.size());
}

face_handle face_collection::add_face(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    halfedge_index hs[3] = {
        mesh->find_halfedge(v0.idx, v1.idx), //
        mesh->find_halfedge(v1.idx, v2.idx), //
        mesh->find_halfedge(v2.idx, v0.idx), //
    };
    return mesh->handle_of(mesh->add_face(hs, 3));
}

face_handle face_collection::add_face(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    halfedge_index hs[4] = {
        mesh->find_halfedge(v0.idx, v1.idx), //
        mesh->find_halfedge(v1.idx, v2.idx), //
        mesh->find_halfedge(v2.idx, v3.idx), //
        mesh->find_halfedge(v3.idx, v0.idx), //
    };
    return mesh->handle_of(mesh->add_face(hs, 4));
}

face_handle face_collection::add_face(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const
{
    halfedge_index hs[3] = {h0.idx, h1.idx, h2.idx};
    return mesh->handle_of(mesh->add_face(hs, 3));
}

face_handle face_collection::add_face(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const
{
    halfedge_index hs[4] = {h0.idx, h1.idx, h2.idx, h3.idx};
    return mesh->handle_of(mesh->add_face(hs, 4));
}

template <size_t N>
face_handle face_collection::add_face(const vertex_handle (&v_handles)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0u; i < N; ++i)
        hs[i] = mesh->find_halfedge(v_handles[i].idx, v_handles[(i + 1) % N].idx);
    return mesh->handle_of(mesh->add_face(hs, N));
}

template <size_t N>
face_handle face_collection::add_face(const halfedge_handle (&half_loop)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0u; i < N; ++i)
        hs[i] = half_loop[i].idx;
    return mesh->handle_of(mesh->add_face(hs, N));
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

edge_handle edge_collection::add_or_get(vertex_handle v_from, vertex_handle v_to)
{
    return mesh->handle_of(mesh->add_or_get_edge(v_from.idx, v_to.idx));
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

halfedge_handle halfedge_collection::add_or_get(vertex_handle v_from, vertex_handle v_to)
{
    return mesh->handle_of(mesh->add_or_get_halfedge(v_from.idx, v_to.idx));
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
