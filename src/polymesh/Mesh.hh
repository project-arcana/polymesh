#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "cursors.hh"
#include "properties.hh"
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
class Mesh
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
    // const versions:
    const_vertex_collection vertices() const { return {this}; }
    const_face_collection faces() const { return {this}; }
    const_edge_collection edges() const { return {this}; }
    const_halfedge_collection halfedges() const { return {this}; }

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
    vertex_index to_vertex_of(halfedge_index idx) const { return halfedge(idx).to_vertex; }
    /// returns the vertex that this half-edge is leaving from
    vertex_index from_vertex_of(halfedge_index idx) const { return halfedge(opposite(idx)).to_vertex; }

    // internal datastructures
private:
    // 4 byte per face
    struct face_info
    {
        halfedge_index halfedge; ///< one half-edge bounding this face

        bool is_valid() const { return halfedge.is_valid(); }
        void set_deleted() { halfedge = halfedge_index::invalid(); }
    };

    // 4 byte per vertex
    struct vertex_info
    {
        halfedge_index outgoing_halfedge;

        /// a vertex can be valid even without outgoing halfedge
        bool is_valid() const { return outgoing_halfedge.value >= -1; }
        bool is_isolated() const { return !outgoing_halfedge.is_valid(); }
        void set_deleted() { outgoing_halfedge = halfedge_index(-2); }
        // is_boundary: check if outgoing_halfedge is boundary
    };

    // 32 byte per edge
    struct halfedge_info
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
    std::vector<face_info> mFaces;
    std::vector<vertex_info> mVertices;
    std::vector<halfedge_info> mHalfedges;

    struct face_info &face(face_index i) { return mFaces[i.value]; }
    struct face_info const &face(face_index i) const { return mFaces[i.value]; }
    struct vertex_info &vertex(vertex_index i) { return mVertices[i.value]; }
    struct vertex_info const &vertex(vertex_index i) const { return mVertices[i.value]; }
    struct halfedge_info &halfedge(halfedge_index i) { return mHalfedges[i.value]; }
    struct halfedge_info const &halfedge(halfedge_index i) const { return mHalfedges[i.value]; }
    struct halfedge_info &halfedge(edge_index i, int h) { return mHalfedges[(i.value >> 1) + h]; }
    struct halfedge_info const &halfedge(edge_index i, int h) const { return mHalfedges[(i.value >> 1) + h]; }

    // internal state
private:
    bool mCompact = true;
    int mDeletedFaces = 0;
    int mDeletedVertices = 0;
    int mDeletedHalfedges = 0;

    std::vector<halfedge_index> mFaceInsertCache;

    // properties
private:
    // linked lists of all properties
    mutable vertex_property_base *mVertexProps = nullptr;
    mutable face_property_base *mFaceProps = nullptr;
    mutable edge_property_base *mEdgeProps = nullptr;
    mutable halfedge_property_base *mHalfedgeProps = nullptr;

    void register_prop(vertex_property_base *prop) const;
    void deregister_prop(vertex_property_base *prop) const;
    void register_prop(face_property_base *prop) const;
    void deregister_prop(face_property_base *prop) const;
    void register_prop(edge_property_base *prop) const;
    void deregister_prop(edge_property_base *prop) const;
    void register_prop(halfedge_property_base *prop) const;
    void deregister_prop(halfedge_property_base *prop) const;

    // friends
private:
    friend struct vertex_handle;
    friend struct vertex_collection;
    friend struct vertex_iterator;
    friend struct valid_vertex_iterator;
    friend struct valid_vertex_collection;
    friend struct const_vertex_collection;
    friend struct vertex_property_base;

    friend struct face_handle;
    friend struct face_collection;
    friend struct face_iterator;
    friend struct valid_face_iterator;
    friend struct valid_face_collection;
    friend struct const_face_collection;
    friend struct face_property_base;

    friend struct edge_handle;
    friend struct edge_collection;
    friend struct edge_iterator;
    friend struct valid_edge_iterator;
    friend struct valid_edge_collection;
    friend struct const_edge_collection;
    friend struct edge_property_base;

    friend struct halfedge_handle;
    friend struct halfedge_collection;
    friend struct halfedge_iterator;
    friend struct valid_halfedge_iterator;
    friend struct valid_halfedge_collection;
    friend struct const_halfedge_collection;
    friend struct halfedge_property_base;
};

/// ======== IMPLEMENTATION ========

inline vertex_index Mesh::add_vertex()
{
    auto idx = vertex_index((int)mVertices.size());
    mVertices.push_back(vertex_info());

    // notify properties
    auto vCnt = mVertices.size();
    for (auto p = mVertexProps; p; p = p->mNextProperty)
        p->resize(vCnt);

    return idx;
}

inline face_index Mesh::add_face(const vertex_handle *v_handles, size_t vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0u; i < vcnt; ++i)
        mFaceInsertCache[i] = find_halfedge(v_handles[i].idx, v_handles[(i + 1) % vcnt].idx);
    return add_face(mFaceInsertCache.data(), vcnt);
}

inline face_index Mesh::add_face(const vertex_index *v_indices, size_t vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0u; i < vcnt; ++i)
        mFaceInsertCache[i] = find_halfedge(v_indices[i], v_indices[(i + 1) % vcnt]);
    return add_face(mFaceInsertCache.data(), vcnt);
}

inline face_index Mesh::add_face(const halfedge_handle *half_loop, size_t vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0u; i < vcnt; ++i)
        mFaceInsertCache[i] = half_loop[i].idx;
    return add_face(mFaceInsertCache.data(), vcnt);
}

inline face_index Mesh::add_face(const halfedge_index *half_loop, size_t vcnt)
{
    assert(vcnt >= 3 && "no support for less-than-triangular faces");
    /// TODO: properties

    auto fidx = face_index((int)mFaces.size());

    // ensure that half-edges are adjacent at each vertex
    for (auto i = 0u; i < vcnt; ++i)
    {
        auto h0 = half_loop[i];
        auto h1 = half_loop[(i + 1) % vcnt];

        // half-edge must form a chain
        assert(to_vertex_of(h0) == from_vertex_of(h1));
        // half-edge must be free, i.e. allow a new polygon
        assert(halfedge(h0).is_free());

        // make them adjacent
        make_adjacent(h0, h1);

        // link face
        halfedge(h0).face = fidx;
    }

    // set up face data
    face_info f;
    f.halfedge = half_loop[0];
    mFaces.push_back(f);

    // notify properties
    auto fCnt = mFaces.size();
    for (auto p = mFaceProps; p; p = p->mNextProperty)
        p->resize(fCnt);

    return fidx;
}

inline edge_index Mesh::add_or_get_edge(vertex_index v_from, vertex_index v_to)
{
    assert(v_from != v_to);

    // already exists?
    auto he = find_halfedge(v_from, v_to);
    if (he.is_valid())
        return edge_of(he);

    auto &vd_from = vertex(v_from);
    auto &vd_to = vertex(v_to);

    // allocate new
    auto he_size = (int)mHalfedges.size();
    auto h_from_to_idx = halfedge_index(he_size + 0);
    auto h_to_from_idx = halfedge_index(he_size + 1);
    auto eidx = edge_index(he_size >> 1);
    halfedge_info h_from_to;
    halfedge_info h_to_from;

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
        auto &from_in = halfedge(from_in_idx);
        auto from_out_idx = from_in.next_halfedge;
        auto &from_out = halfedge(from_out_idx);

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
        auto &to_in = halfedge(to_in_idx);
        auto to_out_idx = to_in.next_halfedge;
        auto &to_out = halfedge(to_out_idx);

        to_in.next_halfedge = h_to_from_idx;
        h_to_from.prev_halfedge = to_in_idx;

        h_from_to.next_halfedge = to_out_idx;
        to_out.prev_halfedge = h_from_to_idx;
    }

    // finalize
    mHalfedges.push_back(h_from_to);
    mHalfedges.push_back(h_to_from);

    // notify properties
    auto hCnt = mHalfedges.size();
    auto eCnt = hCnt >> 1;
    for (auto p = mHalfedgeProps; p; p = p->mNextProperty)
        p->resize(hCnt);
    for (auto p = mEdgeProps; p; p = p->mNextProperty)
        p->resize(eCnt);

    return eidx;
}

inline halfedge_index Mesh::add_or_get_halfedge(vertex_index v_from, vertex_index v_to)
{
    auto e = add_or_get_edge(v_from, v_to);
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    return halfedge(h0).to_vertex == v_to ? h0 : h1;
}

inline void Mesh::make_adjacent(halfedge_index he_in, halfedge_index he_out)
{
    // see http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm ::makeAdjacent
    auto &in = halfedge(he_in);
    auto &out = halfedge(he_out);

    auto he_b = in.next_halfedge;
    auto he_d = out.prev_halfedge;

    // already correct
    if (he_b == he_out)
        return;

    // find free half-edge after `out` but before `in`
    auto he_g = find_free_incident(opposite(he_out), he_in);
    assert(he_g.is_valid()); // unable to make adjacent

    auto &b = halfedge(he_b);
    auto &d = halfedge(he_d);
    auto &g = halfedge(he_g);

    auto he_h = g.next_halfedge;
    auto &h = halfedge(he_d);

    // properly rewire
    in.next_halfedge = he_out;
    out.prev_halfedge = he_in;

    g.next_halfedge = he_b;
    b.prev_halfedge = he_g;

    d.next_halfedge = he_h;
    h.prev_halfedge = he_d;
}

inline halfedge_index Mesh::find_free_incident(halfedge_index in_begin, halfedge_index in_end) const
{
    assert(halfedge(in_begin).to_vertex == halfedge(in_end).to_vertex);

    auto he = in_begin;
    do
    {
        auto const &h = halfedge(he);
        assert(h.to_vertex == halfedge(in_end).to_vertex);

        // free? found one!
        if (h.is_free())
            return he;

        // next half-edge of vertex
        he = opposite(h.next_halfedge);
    } while (he != in_end);

    return halfedge_index::invalid();
}

inline halfedge_index Mesh::find_free_incident(vertex_index v) const
{
    auto in_begin = opposite(vertex(v).outgoing_halfedge);
    return find_free_incident(in_begin, in_begin);
}

inline halfedge_index Mesh::find_halfedge(vertex_index from, vertex_index to) const
{
    auto he_begin = vertex(from).outgoing_halfedge;
    if (!he_begin.is_valid())
        return halfedge_index::invalid(); // isolated vertex

    auto he = he_begin;
    do
    {
        auto const &h = halfedge(he);

        // found?
        if (h.to_vertex == to)
            return he;

        // advance
        he = opposite(h.next_halfedge);

    } while (he != he_begin);

    return halfedge_index::invalid(); // not found
}

inline bool Mesh::is_boundary(vertex_index idx) const
{
    auto const &v = vertex(idx);
    return v.outgoing_halfedge.is_valid() && is_boundary(v.outgoing_halfedge);
}

inline bool Mesh::is_boundary(halfedge_index idx) const
{
    return halfedge(idx).is_free();
}

inline halfedge_index Mesh::opposite(halfedge_index he) const
{
    return halfedge_index(he.value ^ 1);
}

inline vertex_index Mesh::next_valid_idx_from(vertex_index idx) const
{
    for (auto i = idx.value; i < (int)mVertices.size(); ++i)
        if (mVertices[i].is_valid())
            return vertex_index(i);
    return vertex_index(mVertices.size()); // end index
}

inline vertex_index Mesh::prev_valid_idx_from(vertex_index idx) const
{
    for (auto i = idx.value; i >= 0; --i)
        if (mVertices[i].is_valid())
            return vertex_index(i);
    return {}; // invalid
}

inline edge_index Mesh::next_valid_idx_from(edge_index idx) const
{
    for (auto i = idx.value << 1; i < (int)mHalfedges.size(); i += 2)
        if (mHalfedges[i].is_valid())
            return edge_index(i >> 1);
    return edge_index(mHalfedges.size() >> 1); // end index
}

inline edge_index Mesh::prev_valid_idx_from(edge_index idx) const
{
    for (auto i = idx.value << 1; i >= 0; i -= 2)
        if (mHalfedges[i].is_valid())
            return edge_index(i >> 1);
    return {}; // invalid
}

inline face_index Mesh::next_valid_idx_from(face_index idx) const
{
    for (auto i = idx.value; i < (int)mFaces.size(); ++i)
        if (mFaces[i].is_valid())
            return face_index(i);
    return face_index(mFaces.size()); // end index
}

inline face_index Mesh::prev_valid_idx_from(face_index idx) const
{
    for (auto i = idx.value; i >= 0; --i)
        if (mFaces[i].is_valid())
            return face_index(i);
    return {}; // invalid
}

inline halfedge_index Mesh::next_valid_idx_from(halfedge_index idx) const
{
    for (auto i = idx.value; i < (int)mHalfedges.size(); ++i)
        if (mHalfedges[i].is_valid())
            return halfedge_index(i);
    return halfedge_index(mHalfedges.size()); // end index
}

inline halfedge_index Mesh::prev_valid_idx_from(halfedge_index idx) const
{
    for (auto i = idx.value; i >= 0; --i)
        if (mHalfedges[i].is_valid())
            return halfedge_index(i);
    return {}; // invalid
}

/// ======== ITERATOR IMPLEMENTATION ========

inline valid_vertex_iterator &valid_vertex_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
inline vertex_iterator &vertex_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}

inline valid_face_iterator &valid_face_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
inline face_iterator &face_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}

inline valid_edge_iterator &valid_edge_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
inline edge_iterator &edge_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}

inline valid_halfedge_iterator &valid_halfedge_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
inline halfedge_iterator &halfedge_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}

/// ======== RANGES IMPLEMENTATION ========

// - Vertices -

inline int vertex_collection::size() const
{
    return mesh->size_vertices();
}

inline void vertex_collection::reserve(int capacity) const
{
    mesh->reserve_vertices(capacity);
}

inline vertex_handle vertex_collection::add() const
{
    return mesh->handle_of(mesh->add_vertex());
}

inline vertex_iterator vertex_collection::begin() const
{
    return mesh->vertices_begin();
}

inline vertex_iterator vertex_collection::end() const
{
    return mesh->vertices_end();
}

inline int const_vertex_collection::size() const
{
    return mesh->size_vertices();
}

inline vertex_iterator const_vertex_collection::begin() const
{
    return mesh->vertices_begin();
}

inline vertex_iterator const_vertex_collection::end() const
{
    return mesh->vertices_end();
}

inline int valid_vertex_collection::size() const
{
    return mesh->size_valid_vertices();
}

inline valid_vertex_iterator valid_vertex_collection::begin() const
{
    return mesh->valid_vertices_begin();
}

inline valid_vertex_iterator valid_vertex_collection::end() const
{
    return mesh->valid_vertices_end();
}

// - Faces -

inline int face_collection::size() const
{
    return mesh->size_faces();
}

inline void face_collection::reserve(int capacity) const
{
    mesh->reserve_faces(capacity);
}

inline face_handle face_collection::add(const vertex_handle *v_handles, size_t vcnt) const
{
    return mesh->handle_of(mesh->add_face(v_handles, vcnt));
}

inline face_handle face_collection::add(const halfedge_handle *half_loop, size_t vcnt) const
{
    return mesh->handle_of(mesh->add_face(half_loop, vcnt));
}

inline face_handle face_collection::add(std::vector<vertex_handle> const &v_handles) const
{
    return add(v_handles.data(), v_handles.size());
}

inline face_handle face_collection::add(std::vector<halfedge_handle> const &half_loop) const
{
    return add(half_loop.data(), half_loop.size());
}

inline face_handle face_collection::add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    halfedge_index hs[3] = {
        mesh->add_or_get_halfedge(v0.idx, v1.idx), //
        mesh->add_or_get_halfedge(v1.idx, v2.idx), //
        mesh->add_or_get_halfedge(v2.idx, v0.idx), //
    };
    return mesh->handle_of(mesh->add_face(hs, 3));
}

inline face_handle face_collection::add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    halfedge_index hs[4] = {
        mesh->add_or_get_halfedge(v0.idx, v1.idx), //
        mesh->add_or_get_halfedge(v1.idx, v2.idx), //
        mesh->add_or_get_halfedge(v2.idx, v3.idx), //
        mesh->add_or_get_halfedge(v3.idx, v0.idx), //
    };
    return mesh->handle_of(mesh->add_face(hs, 4));
}

inline face_handle face_collection::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const
{
    halfedge_index hs[3] = {h0.idx, h1.idx, h2.idx};
    return mesh->handle_of(mesh->add_face(hs, 3));
}

inline face_handle face_collection::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const
{
    halfedge_index hs[4] = {h0.idx, h1.idx, h2.idx, h3.idx};
    return mesh->handle_of(mesh->add_face(hs, 4));
}

template <size_t N>
inline face_handle face_collection::add(const vertex_handle (&v_handles)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0u; i < N; ++i)
        hs[i] = mesh->find_halfedge(v_handles[i].idx, v_handles[(i + 1) % N].idx);
    return mesh->handle_of(mesh->add_face(hs, N));
}

template <size_t N>
inline face_handle face_collection::add(const halfedge_handle (&half_loop)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0u; i < N; ++i)
        hs[i] = half_loop[i].idx;
    return mesh->handle_of(mesh->add_face(hs, N));
}

inline face_iterator face_collection::begin() const
{
    return mesh->faces_begin();
}

inline face_iterator face_collection::end() const
{
    return mesh->faces_end();
}

inline int const_face_collection::size() const
{
    return mesh->size_faces();
}

inline face_iterator const_face_collection::begin() const
{
    return mesh->faces_begin();
}

inline face_iterator const_face_collection::end() const
{
    return mesh->faces_end();
}

inline int valid_face_collection::size() const
{
    return mesh->size_valid_faces();
}

inline valid_face_iterator valid_face_collection::begin() const
{
    return mesh->valid_faces_begin();
}

inline valid_face_iterator valid_face_collection::end() const
{
    return mesh->valid_faces_end();
}

// - Edges -

inline int edge_collection::size() const
{
    return mesh->size_edges();
}

inline void edge_collection::reserve(int capacity) const
{
    mesh->reserve_edges(capacity);
}

inline edge_handle edge_collection::add_or_get(vertex_handle v_from, vertex_handle v_to)
{
    return mesh->handle_of(mesh->add_or_get_edge(v_from.idx, v_to.idx));
}

inline edge_iterator edge_collection::begin() const
{
    return mesh->edges_begin();
}

inline edge_iterator edge_collection::end() const
{
    return mesh->edges_end();
}

inline int const_edge_collection::size() const
{
    return mesh->size_edges();
}

inline edge_iterator const_edge_collection::begin() const
{
    return mesh->edges_begin();
}

inline edge_iterator const_edge_collection::end() const
{
    return mesh->edges_end();
}

inline int valid_edge_collection::size() const
{
    return mesh->size_valid_edges();
}

inline valid_edge_iterator valid_edge_collection::begin() const
{
    return mesh->valid_edges_begin();
}

inline valid_edge_iterator valid_edge_collection::end() const
{
    return mesh->valid_edges_end();
}

// - Halfedges -

inline int halfedge_collection::size() const
{
    return mesh->size_halfedges();
}

inline void halfedge_collection::reserve(int capacity) const
{
    mesh->reserve_halfedges(capacity);
}

inline halfedge_handle halfedge_collection::add_or_get(vertex_handle v_from, vertex_handle v_to)
{
    return mesh->handle_of(mesh->add_or_get_halfedge(v_from.idx, v_to.idx));
}

inline halfedge_iterator halfedge_collection::begin() const
{
    return mesh->halfedges_begin();
}

inline halfedge_iterator halfedge_collection::end() const
{
    return mesh->halfedges_end();
}

inline int const_halfedge_collection::size() const
{
    return mesh->size_halfedges();
}

inline halfedge_iterator const_halfedge_collection::begin() const
{
    return mesh->halfedges_begin();
}

inline halfedge_iterator const_halfedge_collection::end() const
{
    return mesh->halfedges_end();
}

inline int valid_halfedge_collection::size() const
{
    return mesh->size_valid_halfedges();
}

inline valid_halfedge_iterator valid_halfedge_collection::begin() const
{
    return mesh->valid_halfedges_begin();
}

inline valid_halfedge_iterator valid_halfedge_collection::end() const
{
    return mesh->valid_halfedges_end();
}

/// ======== HANDLES IMPLEMENTATION ========

inline bool vertex_handle::is_valid() const
{
    return idx.is_valid() && mesh->vertex(idx).is_valid();
}

inline bool vertex_handle::is_deleted() const
{
    return !idx.is_valid() || !mesh->vertex(idx).is_valid();
}

inline bool face_handle::is_valid() const
{
    return idx.is_valid() && mesh->face(idx).is_valid();
}

inline bool face_handle::is_deleted() const
{
    return !idx.is_valid() || !mesh->face(idx).is_valid();
}

inline bool edge_handle::is_valid() const
{
    return idx.is_valid() && mesh->halfedge(idx, 0).is_valid();
}

inline bool edge_handle::is_deleted() const
{
    return !idx.is_valid() || !mesh->halfedge(idx, 0).is_valid();
}

inline bool halfedge_handle::is_valid() const
{
    return idx.is_valid() && mesh->halfedge(idx).is_valid();
}

inline bool halfedge_handle::is_deleted() const
{
    return !idx.is_valid() || !mesh->halfedge(idx).is_valid();
}

inline vertex_handle halfedge_handle::vertex_to() const
{
    return mesh->handle_of(mesh->halfedge(idx).to_vertex);
}

inline vertex_handle halfedge_handle::vertex_from() const
{
    return mesh->handle_of(mesh->halfedge(mesh->opposite(idx)).to_vertex);
}

inline face_handle halfedge_handle::face() const
{
    return mesh->handle_of(mesh->halfedge(idx).face);
}

inline halfedge_handle halfedge_handle::next() const
{
    return mesh->handle_of(mesh->halfedge(idx).next_halfedge);
}

inline halfedge_handle halfedge_handle::prev() const
{
    return mesh->handle_of(mesh->halfedge(idx).prev_halfedge);
}

inline halfedge_handle halfedge_handle::opposite() const
{
    return mesh->handle_of(mesh->opposite(idx));
}

inline face_handle halfedge_handle::opposite_face() const
{
    return mesh->handle_of(mesh->halfedge(mesh->opposite(idx)).face);
}

inline halfedge_handle edge_handle::halfedgeA() const
{
    return mesh->handle_of(mesh->halfedge_of(idx, 0));
}

inline halfedge_handle edge_handle::halfedgeB() const
{
    return mesh->handle_of(mesh->halfedge_of(idx, 1));
}

inline face_handle vertex_handle::any_face() const
{
    return mesh->handle_of(mesh->halfedge(mesh->vertex(idx).outgoing_halfedge).face);
}

inline halfedge_handle vertex_handle::any_halfedge() const
{
    return mesh->handle_of(mesh->vertex(idx).outgoing_halfedge);
}

inline vertex_handle face_handle::any_vertex() const
{
    return mesh->handle_of(mesh->halfedge(mesh->face(idx).halfedge).to_vertex);
}

inline halfedge_handle face_handle::any_halfedge() const
{
    return mesh->handle_of(mesh->face(idx).halfedge);
}

inline face_vertex_ring face_handle::vertices() const
{
    return {*this};
}

/// ======== PROPERTIES IMPLEMENTATION ========

template <typename PropT>
vertex_property<PropT> vertex_collection::make_property(const PropT &def_value)
{
    return vertex_property<PropT>(mesh, def_value);
}

template <typename PropT>
face_property<PropT> face_collection::make_property(const PropT &def_value)
{
    return face_property<PropT>(mesh, def_value);
}

template <typename PropT>
edge_property<PropT> edge_collection::make_property(const PropT &def_value)
{
    return edge_property<PropT>(mesh, def_value);
}

template <typename PropT>
halfedge_property<PropT> halfedge_collection::make_property(const PropT &def_value)
{
    return halfedge_property<PropT>(mesh, def_value);
}

inline void Mesh::register_prop(vertex_property_base *prop) const
{
    // insert in front
    auto nextProps = mVertexProps;
    mVertexProps = prop;
    prop->mNextProperty = nextProps;
    if (nextProps)
        nextProps->mPrevProperty = prop;

    // resize prop
    prop->resize(vertices().size());
}

inline void Mesh::deregister_prop(vertex_property_base *prop) const
{
    if (prop->mPrevProperty)
        prop->mPrevProperty->mNextProperty = prop->mNextProperty;

    if (prop->mNextProperty)
        prop->mNextProperty = prop->mPrevProperty;

    if (mVertexProps == prop)
        mVertexProps = prop->mNextProperty;
}

inline void Mesh::register_prop(face_property_base *prop) const
{
    // insert in front
    auto nextProps = mFaceProps;
    mFaceProps = prop;
    prop->mNextProperty = nextProps;
    if (nextProps)
        nextProps->mPrevProperty = prop;

    // resize prop
    prop->resize(faces().size());
}

inline void Mesh::deregister_prop(face_property_base *prop) const
{
    if (prop->mPrevProperty)
        prop->mPrevProperty->mNextProperty = prop->mNextProperty;

    if (prop->mNextProperty)
        prop->mNextProperty = prop->mPrevProperty;

    if (mFaceProps == prop)
        mFaceProps = prop->mNextProperty;
}

inline void Mesh::register_prop(edge_property_base *prop) const
{
    // insert in front
    auto nextProps = mEdgeProps;
    mEdgeProps = prop;
    prop->mNextProperty = nextProps;
    if (nextProps)
        nextProps->mPrevProperty = prop;

    // resize prop
    prop->resize(edges().size());
}

inline void Mesh::deregister_prop(edge_property_base *prop) const
{
    if (prop->mPrevProperty)
        prop->mPrevProperty->mNextProperty = prop->mNextProperty;

    if (prop->mNextProperty)
        prop->mNextProperty = prop->mPrevProperty;

    if (mEdgeProps == prop)
        mEdgeProps = prop->mNextProperty;
}

inline void Mesh::register_prop(halfedge_property_base *prop) const
{
    // insert in front
    auto nextProps = mHalfedgeProps;
    mHalfedgeProps = prop;
    prop->mNextProperty = nextProps;
    if (nextProps)
        nextProps->mPrevProperty = prop;

    // resize prop
    prop->resize(halfedges().size());
}

inline void Mesh::deregister_prop(halfedge_property_base *prop) const
{
    if (prop->mPrevProperty)
        prop->mPrevProperty->mNextProperty = prop->mNextProperty;

    if (prop->mNextProperty)
        prop->mNextProperty = prop->mPrevProperty;

    if (mHalfedgeProps == prop)
        mHalfedgeProps = prop->mNextProperty;
}

inline vertex_property_base::vertex_property_base(const Mesh *mesh) : mMesh(mesh)
{
    // mMesh->register_prop(this); TOO EARLY!
}

inline face_property_base::face_property_base(const Mesh *mesh) : mMesh(mesh)
{
    // mMesh->register_prop(this); TOO EARLY!
}

inline edge_property_base::edge_property_base(const Mesh *mesh) : mMesh(mesh)
{
    // mMesh->register_prop(this); TOO EARLY!
}

inline halfedge_property_base::halfedge_property_base(const Mesh *mesh) : mMesh(mesh)
{
    // mMesh->register_prop(this); TOO EARLY!
}

inline vertex_property_base::~vertex_property_base()
{
    mMesh->deregister_prop(this);
}

inline face_property_base::~face_property_base()
{
    mMesh->deregister_prop(this);
}

inline edge_property_base::~edge_property_base()
{
    mMesh->deregister_prop(this);
}

inline halfedge_property_base::~halfedge_property_base()
{
    mMesh->deregister_prop(this);
}

inline void vertex_property_base::register_prop()
{
    mMesh->register_prop(this);
}

inline void face_property_base::register_prop()
{
    mMesh->register_prop(this);
}

inline void edge_property_base::register_prop()
{
    mMesh->register_prop(this);
}

inline void halfedge_property_base::register_prop()
{
    mMesh->register_prop(this);
}

template <typename PropT>
vertex_property<PropT>::vertex_property(const Mesh *mesh, const PropT &def_value)
  : vertex_property_base(mesh), mDefaultValue(def_value)
{
    register_prop();
}

template <typename PropT>
face_property<PropT>::face_property(const Mesh *mesh, const PropT &def_value)
  : face_property_base(mesh), mDefaultValue(def_value)
{
    register_prop();
}

template <typename PropT>
edge_property<PropT>::edge_property(const Mesh *mesh, const PropT &def_value)
  : edge_property_base(mesh), mDefaultValue(def_value)
{
    register_prop();
}

template <typename PropT>
halfedge_property<PropT>::halfedge_property(const Mesh *mesh, const PropT &def_value)
  : halfedge_property_base(mesh), mDefaultValue(def_value)
{
    register_prop();
}

template <typename PropT>
size_t vertex_property<PropT>::size() const
{
    return mMesh->vertices().size();
}

template <typename PropT>
void vertex_property<PropT>::clear(PropT const &value)
{
    mData.clear();
    mData.resize(mMesh->vertices().size(), value);
}
template <typename PropT>
void vertex_property<PropT>::clear()
{
    clear(mDefaultValue);
}

template <typename PropT>
size_t face_property<PropT>::size() const
{
    return mMesh->vertices().size();
}

template <typename PropT>
void face_property<PropT>::clear(PropT const &value)
{
    mData.clear();
    mData.resize(mMesh->vertices().size(), value);
}
template <typename PropT>
void face_property<PropT>::clear()
{
    clear(mDefaultValue);
}

template <typename PropT>
size_t edge_property<PropT>::size() const
{
    return mMesh->vertices().size();
}

template <typename PropT>
void edge_property<PropT>::clear(PropT const &value)
{
    mData.clear();
    mData.resize(mMesh->vertices().size(), value);
}
template <typename PropT>
void edge_property<PropT>::clear()
{
    clear(mDefaultValue);
}

template <typename PropT>
size_t halfedge_property<PropT>::size() const
{
    return mMesh->vertices().size();
}

template <typename PropT>
void halfedge_property<PropT>::clear(PropT const &value)
{
    mData.clear();
    mData.resize(mMesh->vertices().size(), value);
}
template <typename PropT>
void halfedge_property<PropT>::clear()
{
    clear(mDefaultValue);
}
}
