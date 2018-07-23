#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "attributes.hh"
#include "cursors.hh"
#include "ranges.hh"

// often used helper
#include "attribute_collection.hh"

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
 *  * Primitives can be removed via <primitive>().remove(...)
 *    (removed primitives are invalidated (flagged for removal). call compactify() to remove them)
 *
 *  * `for (auto h : <primitive>())` iterates over _all_ primitives, including invalid ones
 *    (`for (auto h : valid_<primitive>())` skips over invalid ones)
 *
 *  * low-level operations can be performed by accessing low_level_api(mesh)
 *
 * For more concept documents see:
 *  * http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
 *  * https://www.openmesh.org/media/Documentations/OpenMesh-Doc-Latest/a03930.html
 */
class Mesh
{
    // accessors and iterators
public:
    /// smart collections for primitives
    /// Also primary interfaces for querying size and adding primitives
    ///
    /// <primitive>() contains only valid (non-removed) primitives
    /// all_<primitive>() contains also primitives that are flagged for removal (but is faster)
    /// use compactify() to guarantee that no removed ones exist
    ///
    /// NOTE: adding primitives does NOT invalidate ranges. (newly added ones are NOT processed though)
    ///       deleting primitives does NOT invalidate ranges.
    valid_vertex_collection vertices();
    valid_face_collection faces();
    valid_edge_collection edges();
    valid_halfedge_collection halfedges();

    valid_vertex_const_collection vertices() const;
    valid_face_const_collection faces() const;
    valid_edge_const_collection edges() const;
    valid_halfedge_const_collection halfedges() const;

    all_vertex_collection all_vertices();
    all_face_collection all_faces();
    all_edge_collection all_edges();
    all_halfedge_collection all_halfedges();

    all_vertex_const_collection all_vertices() const;
    all_face_const_collection all_faces() const;
    all_edge_const_collection all_edges() const;
    all_halfedge_const_collection all_halfedges() const;

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

    /// deletes all faces, vertices, edges, and halfedges
    void clear();

    // helper
public:
    /// Returns true if the mesh is guaranteed compact, otherwise call compactify() to be sure
    bool is_compact() const { return mCompact; }
    /// Removes all invalid/removed primitives
    /// NOTE: cheap no-op if already compact
    void compactify();

    /// Asserts that mesh invariants hold, e.g. that the half-edge stored in a face actually bounds that face
    void assert_consistency() const;

    // ctor
public:
    Mesh() = default;

    /// Meshes can be neither moved nor copied because attributes depend on the Mesh address
    Mesh(Mesh const &) = delete;
    Mesh(Mesh &&) = delete;
    Mesh &operator=(Mesh const &) = delete;
    Mesh &operator=(Mesh &&) = delete;

    /// Creates a new mesh and returns a shared_ptr to it
    static SharedMesh create() { return std::make_shared<Mesh>(); }

    /// Clears this mesh and copies mesh topology, NOT attributes!
    void copy_from(Mesh const &m);
    /// Creates a new mesh and calls copy_from(*this);
    SharedMesh copy() const;

    // internal helper
private:
    // reserves a certain number of primitives
    void reserve_faces(int capacity);
    void reserve_vertices(int capacity);
    void reserve_edges(int capacity);
    void reserve_halfedges(int capacity);

    int size_all_faces() const { return (int)mFaces.size(); }
    int size_all_vertices() const { return (int)mVertices.size(); }
    int size_all_edges() const { return (int)mHalfedges.size() >> 1; }
    int size_all_halfedges() const { return (int)mHalfedges.size(); }

    int size_valid_faces() const { return (int)mFaces.size() - mRemovedFaces; }
    int size_valid_vertices() const { return (int)mVertices.size() - mRemovedVertices; }
    int size_valid_edges() const { return ((int)mHalfedges.size() - mRemovedHalfedges) >> 1; }
    int size_valid_halfedges() const { return (int)mHalfedges.size() - mRemovedHalfedges; }

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
    vertex_index add_vertex();

    /// Allocates a new vertex
    vertex_index alloc_vertex();
    /// Allocates a new face
    face_index alloc_face();
    /// Allocates a new edge
    edge_index alloc_edge();

    /// Adds a face consisting of N vertices
    /// The vertices must already be sorted in CCW order
    /// (note: trying to add already existing halfedges triggers assertions)
    face_index add_face(vertex_handle const *v_handles, int vcnt);
    face_index add_face(vertex_index const *v_indices, int vcnt);
    face_index add_face(halfedge_handle const *half_loop, int vcnt);
    face_index add_face(halfedge_index const *half_loop, int vcnt);

    /// Adds an edge between two existing, distinct vertices
    /// if edge already exists, returns it
    edge_index add_or_get_edge(vertex_index v_from, vertex_index v_to);
    /// Adds an edge between to existing, distinct vertices that are pointed to by two given halfedges
    /// if edge already exists, returns it
    edge_index add_or_get_edge(halfedge_index h_from, halfedge_index h_to);

    /// same as add_or_get_edge but returns the apattrriate half-edge
    /// Assures:
    ///     return_value.from_vertex == v_from
    ///     return_value.to_vertex == v_to
    halfedge_index add_or_get_halfedge(vertex_index v_from, vertex_index v_to);
    /// same as add_or_get_edge but returns the apattrriate half-edge
    /// Assures:
    ///     return_value.from_vertex == h_from.to_vertex
    ///     return_value.to_vertex == h_to.to_vertex
    halfedge_index add_or_get_halfedge(halfedge_index h_from, halfedge_index h_to);

    /// connect two half-edges in a prev-next relation
    void connect_prev_next(halfedge_index prev, halfedge_index next);

    /// splits a face
    vertex_index face_split(face_index f);
    /// splits an edge
    vertex_index edge_split(edge_index e);
    /// splits a half-edge
    vertex_index halfedge_split(halfedge_index h);

    /// fills a face
    face_index face_fill(halfedge_index h);

    /// attaches a given vertex to the to-vertex of a given half-edge
    void halfedge_attach(halfedge_index h, vertex_index v);

    /// collapse a vertex
    void vertex_collapse(vertex_index v);
    /// collapse a half-edge
    void halfedge_collapse(halfedge_index h);

    /// rotates an edge to next
    void edge_rotate_next(edge_index e);
    /// rotates an edge to prev
    void edge_rotate_prev(edge_index e);
    /// rotates a half-edge to next
    void halfedge_rotate_next(halfedge_index h);
    /// rotates a half-edge to prev
    void halfedge_rotate_prev(halfedge_index h);

    /// removes a face (actually sets the removed status)
    /// modifies all adjacent vertices so that they correctly report is_boundary true
    void remove_face(face_index f_idx);
    /// removes both adjacent faces, then removes both half edges
    void remove_edge(edge_index e_idx);
    /// removes all adjacent edges, then the vertex
    void remove_vertex(vertex_index v_idx);

    /// choses a new outgoing half-edge for a given vertex, prefers boundary ones
    /// assumes non-isolated vertex
    void fix_boundary_state_of(vertex_index v_idx);
    /// choses a new half-edge for a given face, prefers boundary ones
    void fix_boundary_state_of(face_index f_idx);
    /// choses a new half-edge for all vertices of a face, prefers boundary ones
    void fix_boundary_state_of_vertices(face_index f_idx);

    // attributes
    bool is_free(halfedge_index idx) const;

    bool is_boundary(vertex_index idx) const;
    bool is_boundary(halfedge_index idx) const;

    bool is_removed(vertex_index idx) const;
    bool is_removed(face_index idx) const;
    bool is_removed(edge_index idx) const;
    bool is_removed(halfedge_index idx) const;

    bool is_isolated(vertex_index idx) const;

    vertex_index &to_vertex_of(halfedge_index idx);
    face_index &face_of(halfedge_index idx);
    halfedge_index &next_halfedge_of(halfedge_index idx);
    halfedge_index &prev_halfedge_of(halfedge_index idx);
    halfedge_index &halfedge_of(face_index idx);
    halfedge_index &outgoing_halfedge_of(vertex_index idx);

    vertex_index to_vertex_of(halfedge_index idx) const;
    face_index face_of(halfedge_index idx) const;
    halfedge_index next_halfedge_of(halfedge_index idx) const;
    halfedge_index prev_halfedge_of(halfedge_index idx) const;
    halfedge_index halfedge_of(face_index idx) const;
    halfedge_index outgoing_halfedge_of(vertex_index idx) const;

    void set_removed(vertex_index idx);
    void set_removed(face_index idx);
    void set_removed(edge_index idx);

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

    vertex_index &from_vertex_of(halfedge_index idx);
    vertex_index from_vertex_of(halfedge_index idx) const;

    /// applies an index remapping to all face indices (p[curr_idx] = new_idx)
    void permute_faces(std::vector<int> const &p);
    /// applies an index remapping to all edge (and half-edge) indices (p[curr_idx] = new_idx)
    void permute_edges(std::vector<int> const &p);
    /// applies an index remapping to all vertices indices (p[curr_idx] = new_idx)
    void permute_vertices(std::vector<int> const &p);

    // internal datastructures
private:
    // 4 byte per face
    struct face_info
    {
        halfedge_index halfedge; ///< one half-edge bounding this face

        bool is_valid() const { return halfedge.is_valid(); }
        void set_removed() { halfedge = halfedge_index::invalid(); }
        // is_boundary: check if half-edge is boundary
    };

    // 4 byte per vertex
    struct vertex_info
    {
        halfedge_index outgoing_halfedge;

        /// a vertex can be valid even without outgoing halfedge
        bool is_valid() const { return outgoing_halfedge.value >= -1; }
        bool is_isolated() const { return !outgoing_halfedge.is_valid(); }
        void set_removed() { outgoing_halfedge = halfedge_index(-2); }
        // is_boundary: check if outgoing_halfedge is boundary
    };

    // 16 byte per edge
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

        // CAUTION: remove both HE belonging to an edge
        void set_removed() { to_vertex = vertex_index::invalid(); }
    };

    // internal primitives
private:
    // std::vector<face_info> mFaces;
    // std::vector<vertex_info> mVertices;
    // std::vector<halfedge_info> mHalfedges;

    std::vector<halfedge_index> mFaceToHalfedge;
    std::vector<halfedge_index> mVertexToOutgoingHalfedge;

    std::vector<vertex_index> mHalfedgeToVertex;
    std::vector<face_index> mHalfedgeToFace;
    std::vector<halfedge_index> mHalfedgeToNextHalfedge;
    std::vector<halfedge_index> mHalfedgeToPrevHalfedge;

    /*
    struct face_info &face(face_index i)
    {
        assert(i.is_valid() && i.value < size_all_faces());
        return mFaces[i.value];
    }
    struct face_info const &face(face_index i) const
    {
        assert(i.is_valid() && i.value < size_all_faces());
        return mFaces[i.value];
    }
    struct vertex_info &vertex(vertex_index i)
    {
        assert(i.is_valid() && i.value < size_all_vertices());
        return mVertices[i.value];
    }
    struct vertex_info const &vertex(vertex_index i) const
    {
        assert(i.is_valid() && i.value < size_all_vertices());
        return mVertices[i.value];
    }
    struct halfedge_info &halfedge(halfedge_index i)
    {
        assert(i.is_valid() && i.value < size_all_halfedges());
        return mHalfedges[i.value];
    }
    struct halfedge_info const &halfedge(halfedge_index i) const
    {
        assert(i.is_valid() && i.value < size_all_halfedges());
        return mHalfedges[i.value];
    }
    struct halfedge_info &halfedge(edge_index i, int h)
    {
        assert(i.is_valid() && i.value < size_all_edges());
        return mHalfedges[(i.value << 1) + h];
    }
    struct halfedge_info const &halfedge(edge_index i, int h) const
    {
        assert(i.is_valid() && i.value < size_all_edges());
        return mHalfedges[(i.value << 1) + h];
    }
    */

    // internal state
private:
    bool mCompact = true;
    int mRemovedFaces = 0;
    int mRemovedVertices = 0;
    int mRemovedHalfedges = 0;

    std::vector<halfedge_index> mFaceInsertCache;

    // attributes
private:
    // linked lists of all attributes
    mutable primitive_attribute_base<vertex_tag> *mVertexAttrs = nullptr;
    mutable primitive_attribute_base<face_tag> *mFaceAttrs = nullptr;
    mutable primitive_attribute_base<edge_tag> *mEdgeAttrs = nullptr;
    mutable primitive_attribute_base<halfedge_tag> *mHalfedgeAttrs = nullptr;

    void register_attr(primitive_attribute_base<vertex_tag> *attr) const;
    void deregister_attr(primitive_attribute_base<vertex_tag> *attr) const;
    void register_attr(primitive_attribute_base<face_tag> *attr) const;
    void deregister_attr(primitive_attribute_base<face_tag> *attr) const;
    void register_attr(primitive_attribute_base<edge_tag> *attr) const;
    void deregister_attr(primitive_attribute_base<edge_tag> *attr) const;
    void register_attr(primitive_attribute_base<halfedge_tag> *attr) const;
    void deregister_attr(primitive_attribute_base<halfedge_tag> *attr) const;

    // friends
private:
    friend struct vertex_handle;
    friend struct all_vertex_iterator;
    friend struct valid_vertex_iterator;
    friend struct vertex_attribute_base;

    friend struct face_handle;
    friend struct face_iterator;
    friend struct valid_face_iterator;
    friend struct face_attribute_base;

    friend struct edge_handle;
    friend struct edge_iterator;
    friend struct valid_edge_iterator;
    friend struct edge_attribute_base;

    friend struct halfedge_handle;
    friend struct halfedge_iterator;
    friend struct valid_halfedge_iterator;
    friend struct halfedge_attribute_base;

    template <class tag>
    friend struct primitive;
    template <class tag>
    friend struct primitive_handle;
    template <class tag>
    friend struct primitive_collection;
    template <class tag>
    friend struct all_primitive_iterator;
    template <class tag>
    friend struct valid_primitive_iterator;
    template <class tag>
    friend struct valid_primitive_collection;
    template <class tag>
    friend struct const_primitive_collection;
    template <class tag>
    friend struct primitive_attribute_base;
    template <class mesh_ptr, class tag, class iterator>
    friend struct smart_collection;
    template <class iterator>
    friend struct vertex_collection;
    template <class iterator>
    friend struct face_collection;
    template <class iterator>
    friend struct edge_collection;
    template <class iterator>
    friend struct halfedge_collection;

    friend struct low_level_api;
};
}

/// ======== IMPLEMENTATIONS ========

#include "impl/impl_attributes.hh"
#include "impl/impl_cursors.hh"
#include "impl/impl_iterators.hh"
#include "impl/impl_mesh.hh"
#include "impl/impl_primitive.hh"
#include "impl/impl_ranges.hh"
