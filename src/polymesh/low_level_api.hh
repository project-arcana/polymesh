#pragma once

#include <type_traits>
#include <vector> // TODO: replace me by span

#include "cursors.hh"
#include "tmp.hh"

namespace polymesh
{
class Mesh;

struct low_level_api_const;
struct low_level_api_mutable;

inline low_level_api_const low_level_api(Mesh const& m);
inline low_level_api_const low_level_api(Mesh const* m);
inline low_level_api_mutable low_level_api(Mesh& m);
inline low_level_api_mutable low_level_api(Mesh* m);

/**
 * The Low-Level API allows access to the internal datastructure of a Mesh
 *
 * CAUTION: should only be used if you know what you do!
 *
 * Usage:
 *      auto ll = low_level_api(myMesh);
 *      ll.something();
 */
template <class MeshT>
struct low_level_api_base
{
    // primitive access
public:
    tmp::cond_const_ref<vertex_index, MeshT> to_vertex_of(halfedge_index idx) const;
    tmp::cond_const_ref<face_index, MeshT> face_of(halfedge_index idx) const;
    tmp::cond_const_ref<halfedge_index, MeshT> next_halfedge_of(halfedge_index idx) const;
    tmp::cond_const_ref<halfedge_index, MeshT> prev_halfedge_of(halfedge_index idx) const;
    tmp::cond_const_ref<halfedge_index, MeshT> halfedge_of(face_index idx) const;
    tmp::cond_const_ref<halfedge_index, MeshT> outgoing_halfedge_of(vertex_index idx) const;

    // number of primitives
public:
    int capacity_faces() const;
    int capacity_vertices() const;
    int capacity_halfedges() const;

    int size_all_faces() const;
    int size_all_vertices() const;
    int size_all_edges() const;
    int size_all_halfedges() const;

    int size_valid_faces() const;
    int size_valid_vertices() const;
    int size_valid_edges() const;
    int size_valid_halfedges() const;

    int size_removed_faces() const;
    int size_removed_vertices() const;
    int size_removed_edges() const;
    int size_removed_halfedges() const;

    // byte size information
public:
    size_t byte_size_topology() const;
    size_t byte_size_attributes() const;

    size_t allocated_byte_size_topology() const;
    size_t allocated_byte_size_attributes() const;

    // traversal helper
public:
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

    // modification checks
public:
    /// Returns true iff the face can be added
    bool can_add_face(vertex_handle const* v_handles, int vcnt) const;
    bool can_add_face(vertex_index const* v_indices, int vcnt) const;
    bool can_add_face(halfedge_handle const* half_loop, int vcnt) const;
    bool can_add_face(halfedge_index const* half_loop, int vcnt) const;

    // topology helper
public:
    /// Returns the opposite of a given valid half-edge
    halfedge_index opposite(halfedge_index he) const;
    /// Returns the opposite face of a given valid half-edge
    face_index opposite_face_of(halfedge_index he) const;
    /// Returns the from-vertex of a given valid half-edge
    vertex_index from_vertex_of(halfedge_index idx) const;

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
    /// returns the to-vertex belonging to the halfedge of an edge
    vertex_index to_vertex_of(edge_index idx, int i) const { return to_vertex_of(halfedge_of(idx, i)); }
    /// returns the face belonging to the halfedge of an edge
    face_index face_of(edge_index idx, int i) const { return face_of(halfedge_of(idx, i)); }

    // properties
public:
    /// a half-edge is free if it has no adjacent face
    bool is_free(halfedge_index idx) const;

    /// a vertex is boundary if any outgoing half-edges is a boundary (isolated vertices are boundary)
    bool is_boundary(vertex_index idx) const;
    /// a half-edge is boundary if it has no face
    bool is_boundary(halfedge_index idx) const;
    /// an edge is boundary if at least one half-edge is boundary
    bool is_boundary(edge_index idx) const;
    /// a face is boundary if at least one half-edge is boundary
    bool is_boundary(face_index idx) const;

    /// true iff vertex is marked for removal
    bool is_removed(vertex_index idx) const;
    /// true iff face is marked for removal
    bool is_removed(face_index idx) const;
    /// true iff edge is marked for removal
    bool is_removed(edge_index idx) const;
    /// true iff half-edge is marked for removal
    bool is_removed(halfedge_index idx) const;

    /// vertices without outgoing half-edges are isolated
    bool is_isolated(vertex_index idx) const;
    /// edges without faces are isolated
    bool is_isolated(edge_index idx) const;

    // attribute related properties
public:
    // number of attached vertex attributes
    int vertex_attribute_count() const;
    // number of attached face attributes
    int face_attribute_count() const;
    // number of attached edge attributes
    int edge_attribute_count() const;
    // number of attached halfedge attributes
    int halfedge_attribute_count() const;

    // offsets in binary representation (useful for wrappers in other languages)
public:
    constexpr static int offset_mVerticesSize() { return offsetof(MeshT, mVerticesSize); }
    constexpr static int offset_mVerticesCapacity() { return offsetof(MeshT, mVerticesCapacity); }
    constexpr static int offset_mVertexToOutgoingHalfedge() { return offsetof(MeshT, mVertexToOutgoingHalfedge); }

    constexpr static int offset_mFacesSize() { return offsetof(MeshT, mFacesSize); }
    constexpr static int offset_mFacesCapacity() { return offsetof(MeshT, mFacesCapacity); }
    constexpr static int offset_mFaceToHalfedge() { return offsetof(MeshT, mFaceToHalfedge); }

    constexpr static int offset_mHalfedgesSize() { return offsetof(MeshT, mHalfedgesSize); }
    constexpr static int offset_mHalfedgesCapacity() { return offsetof(MeshT, mHalfedgesCapacity); }

    constexpr static int offset_mHalfedgeToVertex() { return offsetof(MeshT, mHalfedgeToVertex); }
    constexpr static int offset_mHalfedgeToFace() { return offsetof(MeshT, mHalfedgeToFace); }
    constexpr static int offset_mHalfedgeToPrevHalfedge() { return offsetof(MeshT, mHalfedgeToPrevHalfedge); }
    constexpr static int offset_mHalfedgeToNextHalfedge() { return offsetof(MeshT, mHalfedgeToNextHalfedge); }

protected:
    MeshT& m;

    low_level_api_base(MeshT& m) : m(m) {}
};

template <class MeshT>
struct low_level_attribute_api
{
    static int offset_attribute_dataptr()
    {
        MeshT mesh;
        auto attr = mesh.vertices().make_attribute_(0.f);
        auto offset = int(size_t(&(attr.mData)) - size_t(&attr));
        return offset;
    }
};

struct low_level_api_mutable : low_level_api_base<Mesh>
{
    // allocation
public:
    // reserves a certain number of primitives
    void reserve_faces(int capacity) const;
    void reserve_vertices(int capacity) const;
    void reserve_edges(int capacity) const;
    void reserve_halfedges(int capacity) const;

    /// Allocates a new vertex
    vertex_index alloc_vertex() const;
    /// Allocates a new face
    face_index alloc_face() const;
    /// Allocates a new edge
    edge_index alloc_edge() const;
    /// Allocates a given amount of vertices, faces, and halfedges
    /// NOTE: leaves ALL of them in an unspecified state
    void alloc_primitives(int vertices, int faces, int halfedges) const;

    // adding primitives
public:
    /// Adds a single non-connected vertex
    /// Does NOT invalidate iterators!
    vertex_index add_vertex() const;

    /// Adds a face consisting of N vertices
    /// The vertices must already be sorted in CCW order
    /// (note: trying to add already existing halfedges triggers assertions)
    /// Optional: provide an index that should be "resurrected" (must be a removed slot)
    face_index add_face(vertex_handle const* v_handles, int vcnt, face_index res_idx = {}) const;
    face_index add_face(vertex_index const* v_indices, int vcnt, face_index res_idx = {}) const;
    face_index add_face(halfedge_handle const* half_loop, int vcnt, face_index res_idx = {}) const;
    face_index add_face(halfedge_index const* half_loop, int vcnt, face_index res_idx = {}) const;

    /// Adds an edge between two existing, distinct vertices
    /// if edge already exists, returns it
    edge_index add_or_get_edge(vertex_index v_from, vertex_index v_to) const;
    /// Adds an edge between to existing, distinct vertices that are pointed to by two given halfedges
    /// if edge already exists, returns it
    edge_index add_or_get_edge(halfedge_index h_from, halfedge_index h_to) const;

    /// same as add_or_get_edge but returns the appropriate half-edge
    /// Assures:
    ///     return_value.from_vertex == v_from
    ///     return_value.to_vertex == v_to
    halfedge_index add_or_get_halfedge(vertex_index v_from, vertex_index v_to) const;
    /// same as add_or_get_edge but returns the appropriate half-edge
    /// Assures:
    ///     return_value.from_vertex == h_from.to_vertex
    ///     return_value.to_vertex == h_to.to_vertex
    halfedge_index add_or_get_halfedge(halfedge_index h_from, halfedge_index h_to) const;

    // removing primitives
public:
    /// Marks this vertex as removed
    /// (Also updates remove counts and compact status)
    void set_removed(vertex_index idx) const;
    /// Marks this face as removed
    /// (Also updates remove counts and compact status)
    void set_removed(face_index idx) const;
    /// Marks this edge as removed
    /// (Also updates remove counts and compact status)
    void set_removed(edge_index idx) const;

    /// removes a face (actually sets the removed status)
    /// modifies all adjacent vertices so that they correctly report is_boundary true
    void remove_face(face_index f_idx) const;
    /// removes both adjacent faces, then removes both half edges
    void remove_edge(edge_index e_idx) const;
    /// removes all adjacent edges, then the vertex
    void remove_vertex(vertex_index v_idx) const;

    /// special purpose function:
    /// CAUTION: only works if edges.size() == 0
    /// clears the edge vector
    void clear_removed_edge_vector() const;

    /// Overrides the saved number of removed primitives
    /// CAUTION: only use if you know what you do!
    void set_removed_counts(int r_vertices, int r_faces, int r_edges);

    // reordering
public:
    /// applies an index remapping to all face indices (p[curr_idx] = new_idx)
    void permute_faces(std::vector<int> const& p) const;
    /// applies an index remapping to all edge (and half-edge) indices (p[curr_idx] = new_idx)
    void permute_edges(std::vector<int> const& p) const;
    /// applies an index remapping to all vertices indices (p[curr_idx] = new_idx)
    void permute_vertices(std::vector<int> const& p) const;

    // topology modification
public:
    /// Makes two half-edges adjacent
    /// Ensures:
    ///     * he_in->next == he_out
    ///     * he_out->prev == he_in
    /// Requires:
    ///     * he_in->is_free()
    ///     * he_out->is_free()
    /// Only works if a free incident half-edge is available
    void make_adjacent(halfedge_index he_in, halfedge_index he_out) const;

    /// connect two half-edges in a prev-next relation
    void connect_prev_next(halfedge_index prev, halfedge_index next) const;

    /// splits a face
    vertex_index face_split(face_index f) const;
    /// splits an edge
    vertex_index edge_split(edge_index e) const;
    /// splits an edge and triangulates all adjacent faces
    vertex_index edge_split_and_triangulate(edge_index e) const;
    /// splits a half-edge
    vertex_index halfedge_split(halfedge_index h) const;

    /// splits a face at a given ISOLATED vertex
    void face_split(face_index f, vertex_index v) const;
    /// splits an edge at a given ISOLATED vertex
    void edge_split(edge_index e, vertex_index v) const;
    /// splits an edge at a given ISOLATED vertex and triangulates all adjacent faces
    void edge_split_and_triangulate(edge_index e, vertex_index v) const;
    /// splits a half-edge at a given ISOLATED vertex
    void halfedge_split(halfedge_index h, vertex_index v) const;

    /// cuts a face along two given vertices (indicated by halfedges)
    halfedge_index face_cut(face_index f, halfedge_index h0, halfedge_index h1) const;

    /// fills a face
    face_index face_fill(halfedge_index h) const;

    /// attaches a given vertex to the to-vertex of a given half-edge
    void halfedge_attach(halfedge_index h, vertex_index v) const;

    /// merges the given and the previous halfedge.
    /// The center vertex must have valence 2
    void halfedge_merge(halfedge_index h) const;

    /// collapse a vertex
    void vertex_collapse(vertex_index v) const;
    /// collapse a half-edge
    void halfedge_collapse(halfedge_index h) const;

    /// rotates an edge to next
    void edge_rotate_next(edge_index e) const;
    /// rotates an edge to prev
    void edge_rotate_prev(edge_index e) const;
    /// performs an edge flip
    void edge_flip(edge_index e) const;
    /// rotates a half-edge to next
    void halfedge_rotate_next(halfedge_index h) const;
    /// rotates a half-edge to prev
    void halfedge_rotate_prev(halfedge_index h) const;

    // boundary states
public:
    /// choses a new outgoing half-edge for a given vertex, prefers boundary ones
    /// assumes non-isolated vertex
    void fix_boundary_state_of(vertex_index v_idx) const;
    /// choses a new half-edge for a given face, prefers boundary ones
    void fix_boundary_state_of(face_index f_idx) const;
    /// choses a new half-edge for all vertices of a face, prefers boundary ones
    void fix_boundary_state_of_vertices(face_index f_idx) const;

public:
    using low_level_api_base<Mesh>::low_level_api_base;

    friend low_level_api_mutable low_level_api(Mesh& m);
    friend low_level_api_mutable low_level_api(Mesh* m);
};

struct low_level_api_const : low_level_api_base<Mesh const>
{
public:
    using low_level_api_base<Mesh const>::low_level_api_base;

    friend low_level_api_const low_level_api(Mesh const& m);
    friend low_level_api_const low_level_api(Mesh const* m);
};

// Convenience functions
inline low_level_api_const low_level_api(Mesh const& m) { return {m}; }
inline low_level_api_const low_level_api(Mesh const* m) { return {*m}; }
inline low_level_api_mutable low_level_api(Mesh& m) { return {m}; }
inline low_level_api_mutable low_level_api(Mesh* m) { return {*m}; }
} // namespace polymesh
