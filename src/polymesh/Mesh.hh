#pragma once

#include <cstddef>
#include <vector>

#include "fwd.hh"

#include "attributes.hh"
#include "cursors.hh"
#include "detail/unique_array.hh"
#include "detail/unique_ptr.hh"
#include "ranges.hh"

// often used helper
#include "low_level_api.hh"

namespace polymesh
{
/**
 * @brief Half-edge Mesh Data Structure
 *
 *  * Primitives are accessed via the smart collections mesh.[primitive]()
 *    (where [primitive] can be vertices, edges, faces, or halfedges)
 *
 *  * Primitives can be added via [primitive]().add()
 *    (added primitives are at the end of the collection)
 *
 *  * Primitives can be removed via [primitive]().remove(...)
 *    (removed primitives are invalidated (flagged for removal). call compactify() to remove them)
 *
 *  * `for (auto h : [primitive]())` iterates over primitives (skipping invalid ones)
 *    (`for (auto h : valid_[primitive]())` is slightly faster and includes invalid ones)
 *
 *  * low-level operations can be performed by accessing low_level_api(mesh)
 *
 * For more concept documents see:
 *  * http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
 *  * https://www.openmesh.org/media/Documentations/OpenMesh-Doc-Latest/a03930.html
 *
 * Currently a mesh consumes 120 bytes without any data
 */
class Mesh
{
    // accessors and iterators
public:
    /// smart collections for primitives
    /// Also primary interfaces for querying size and adding primitives
    ///
    /// [primitive]() contains only valid (non-removed) primitives
    /// all_[primitive]() contains also primitives that are flagged for removal (but is faster)
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
    /// NOTE: similar to std::vector, this does NOT free the memory (use shrink_to_fit)
    void clear();

    /// reallocate primitives and attributes such that the memory is a tight fit (capacity == size)
    /// NOTE: An empty mesh has no allocated memory
    void shrink_to_fit();

    /// In behavior similar to clear() followed by shrink_to_fit()
    /// Basically frees all associated memory immediately
    void reset();

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
    Mesh(Mesh const&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh const&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    /// Creates a new mesh and returns a unique_ptr to it
    static unique_ptr<Mesh> create() { return make_unique<Mesh>(); }

    /// Clears this mesh and copies mesh topology, NOT attributes!
    void copy_from(Mesh const& m);
    /// Creates a new mesh and calls copy_from(*this);
    /// Note: does NOT copy attributes!
    unique_ptr<Mesh> copy() const;

    // internal primitives
private:
    unique_array<halfedge_index> mFaceToHalfedge;
    int mFacesSize = 0;
    int mFacesCapacity = 0;

    unique_array<halfedge_index> mVertexToOutgoingHalfedge;
    int mVerticesSize = 0;
    int mVerticesCapacity = 0;

    unique_array<vertex_index> mHalfedgeToVertex;
    unique_array<face_index> mHalfedgeToFace;
    unique_array<halfedge_index> mHalfedgeToNextHalfedge;
    unique_array<halfedge_index> mHalfedgeToPrevHalfedge;
    int mHalfedgesSize = 0;
    int mHalfedgesCapacity = 0;

    // primitive size
private:
    int size_all_faces() const { return (int)mFacesSize; }
    int size_all_vertices() const { return (int)mVerticesSize; }
    int size_all_edges() const { return (int)mHalfedgesSize >> 1; }
    int size_all_halfedges() const { return (int)mHalfedgesSize; }

    int size_valid_faces() const { return (int)mFacesSize - mRemovedFaces; }
    int size_valid_vertices() const { return (int)mVerticesSize - mRemovedVertices; }
    int size_valid_edges() const { return ((int)mHalfedgesSize - mRemovedHalfedges) >> 1; }
    int size_valid_halfedges() const { return (int)mHalfedgesSize - mRemovedHalfedges; }

    // primitive access
private:
    vertex_index& to_vertex_of(halfedge_index idx);
    face_index& face_of(halfedge_index idx);
    halfedge_index& next_halfedge_of(halfedge_index idx);
    halfedge_index& prev_halfedge_of(halfedge_index idx);
    halfedge_index& halfedge_of(face_index idx);
    halfedge_index& outgoing_halfedge_of(vertex_index idx);

    vertex_index const& to_vertex_of(halfedge_index idx) const;
    face_index const& face_of(halfedge_index idx) const;
    halfedge_index const& next_halfedge_of(halfedge_index idx) const;
    halfedge_index const& prev_halfedge_of(halfedge_index idx) const;
    halfedge_index const& halfedge_of(face_index idx) const;
    halfedge_index const& outgoing_halfedge_of(vertex_index idx) const;

    // primitive allocation
private:
    /// Allocates a new vertex
    vertex_index alloc_vertex();
    /// Allocates a new face
    face_index alloc_face();
    /// Allocates a new edge
    edge_index alloc_edge();
    /// Allocates a given amount of vertices, faces, and halfedges
    /// NOTE: leaves ALL of them in an unspecified state
    void alloc_primitives(int vertices, int faces, int halfedges);

    // reserves a certain number of primitives
    void reserve_faces(int capacity);
    void reserve_vertices(int capacity);
    void reserve_edges(int capacity);
    void reserve_halfedges(int capacity);

    // primitive reordering
private:
    /// applies an index remapping to all face indices (p[curr_idx] = new_idx)
    void permute_faces(std::vector<int> const& p);
    /// applies an index remapping to all edge (and half-edge) indices (p[curr_idx] = new_idx)
    void permute_edges(std::vector<int> const& p);
    /// applies an index remapping to all vertices indices (p[curr_idx] = new_idx)
    void permute_vertices(std::vector<int> const& p);

    // internal state
private:
    bool mCompact = true;
    int mRemovedFaces = 0;
    int mRemovedVertices = 0;
    int mRemovedHalfedges = 0;

    // attributes
private:
    // linked lists of all attributes
    mutable primitive_attribute_base<vertex_tag>* mVertexAttrs = nullptr;
    mutable primitive_attribute_base<face_tag>* mFaceAttrs = nullptr;
    mutable primitive_attribute_base<edge_tag>* mEdgeAttrs = nullptr;
    mutable primitive_attribute_base<halfedge_tag>* mHalfedgeAttrs = nullptr;

    void register_attr(primitive_attribute_base<vertex_tag>* attr) const;
    void deregister_attr(primitive_attribute_base<vertex_tag>* attr) const;
    void register_attr(primitive_attribute_base<face_tag>* attr) const;
    void deregister_attr(primitive_attribute_base<face_tag>* attr) const;
    void register_attr(primitive_attribute_base<edge_tag>* attr) const;
    void deregister_attr(primitive_attribute_base<edge_tag>* attr) const;
    void register_attr(primitive_attribute_base<halfedge_tag>* attr) const;
    void deregister_attr(primitive_attribute_base<halfedge_tag>* attr) const;

    // friends
private:
    // for attribute registration
    template <class tag>
    friend struct primitive_attribute_base;

    // for low level access
    template <class MeshT>
    friend struct low_level_api_base;
    friend struct low_level_api_const;
    friend struct low_level_api_mutable;
};
} // namespace polymesh

// ======== IMPLEMENTATIONS ========

#include "impl/impl_attributes.hh"
#include "impl/impl_cursors.hh"
#include "impl/impl_low_level_api_base.hh"
#include "impl/impl_low_level_api_mutable.hh"
#include "impl/impl_mesh.hh"
#include "impl/impl_primitive.hh"
#include "impl/impl_ranges.hh"
