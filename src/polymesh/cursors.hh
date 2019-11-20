#pragma once

#include "primitives.hh"
#include "tmp.hh"

namespace polymesh
{
// ======================== BASE ========================

template <class tag>
struct primitive_index
{
    template <class AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;
    using index_t = typename primitive<tag>::index;
    using handle_t = typename primitive<tag>::handle;

    int value = -1;

    primitive_index() = default;
    explicit primitive_index(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    bool is_invalid() const { return value < 0; }
    static const index_t invalid;

    bool operator<(index_t const& rhs) const { return value < rhs.value; }
    bool operator<=(index_t const& rhs) const { return value <= rhs.value; }
    bool operator>(index_t const& rhs) const { return value > rhs.value; }
    bool operator>=(index_t const& rhs) const { return value >= rhs.value; }
    bool operator==(index_t const& rhs) const { return value == rhs.value; }
    bool operator!=(index_t const& rhs) const { return value != rhs.value; }
    bool operator==(handle_t const& rhs) const { return value == rhs.idx.value; }
    bool operator!=(handle_t const& rhs) const { return value != rhs.idx.value; }

    explicit operator int() const { return value; }

    /// creates a handle from this idx and the given mesh
    handle_t of(Mesh const& m) const { return handle_t(&m, index_t(value)); }
    handle_t of(Mesh const* m) const { return handle_t(m, index_t(value)); }

    /// indexes this primitive by a functor
    /// also works for attributes
    /// - e.g. v[position] or f[area]
    template <class FuncT, class ResultT = tmp::result_type_of<FuncT, index_t>>
    ResultT operator[](FuncT&& f) const;

    /// indexes this primitive by an attribute pointer
    /// if attr is null, returns writeable dummy location
    /// CAUTION: always returns same writeable dummy location, this is intended as a write-only value!
    template <class AttrT>
    AttrT& operator[](attribute<AttrT>* attr) const;
    /// indexes this primitive by an attribute pointer
    /// if attr is null, returns default constructed value
    template <class AttrT>
    AttrT const& operator[](attribute<AttrT> const* attr) const;
};

template <class tag>
const typename primitive<tag>::index primitive_index<tag>::invalid = {};

template <class tag>
struct primitive_handle
{
    template <class AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;
    using index_t = typename primitive<tag>::index;
    using handle_t = typename primitive<tag>::handle;

    Mesh const* mesh = nullptr;
    index_t idx;

    primitive_handle() = default;
    primitive_handle(Mesh const* mesh, index_t idx) : mesh(mesh), idx(idx) {}

    bool operator==(index_t const& rhs) const { return idx == rhs; }
    bool operator!=(index_t const& rhs) const { return idx != rhs; }
    bool operator==(handle_t const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(handle_t const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    explicit operator int() const { return (int)idx; }
    operator index_t() const { return idx; }

    /// indexes this primitive by a functor
    /// also works for attributes
    /// - e.g. v[position] or f[area]
    template <class FuncT, class ResultT = tmp::result_type_of<FuncT, index_t>>
    ResultT operator[](FuncT&& f) const;

    /// indexes this primitive by an attribute pointer
    /// if attr is null, returns writeable dummy location
    template <class AttrT>
    AttrT& operator[](attribute<AttrT>* attr) const;
    /// indexes this primitive by an attribute pointer
    /// if attr is null, returns default constructed value
    template <class AttrT>
    AttrT const& operator[](attribute<AttrT> const* attr) const;

    bool is_valid() const { return idx.is_valid(); }    ///< valid idx (but could be deleted in some iterators)
    bool is_invalid() const { return !idx.is_valid(); } ///< invalid idx
    static const handle_t invalid;
};

template <class tag>
const typename primitive<tag>::handle primitive_handle<tag>::invalid = {};

// ======================== INDICES ========================

struct face_index : primitive_index<face_tag>
{
    using primitive_index::primitive_index;
};
struct edge_index : primitive_index<edge_tag>
{
    using primitive_index::primitive_index;
};
struct halfedge_index : primitive_index<halfedge_tag>
{
    using primitive_index::primitive_index;
};
struct vertex_index : primitive_index<vertex_tag>
{
    using primitive_index::primitive_index;
};

// ======================== HANDLES ========================

struct face_handle : primitive_handle<face_tag>
{
    using primitive_handle::primitive_handle;

    bool is_removed() const;  ///< marked for deletion (but valid idx)
    bool is_boundary() const; ///< true if this face lies at a boundary

    vertex_handle any_vertex() const;
    halfedge_handle any_halfedge() const;

    face_vertex_ring vertices() const;
    face_edge_ring edges() const;
    face_halfedge_ring halfedges() const;
    face_face_ring adjacent_faces() const;         ///< DOES NOT include invalid ones for boundaries!
    face_all_face_ring all_adjacent_faces() const; ///< includes invalid ones for boundaries!
};

struct vertex_handle : primitive_handle<vertex_tag>
{
    using primitive_handle::primitive_handle;

    bool is_removed() const;  ///< marked for deletion (but valid idx)
    bool is_isolated() const; ///< true if this vertex is not connected at all
    bool is_boundary() const; ///< true if this vertex lies at a boundary

    face_handle any_face() const;                  ///< invalid if at boundary
    face_handle any_valid_face() const;            ///< invalid if isolated (and can be if at boundary)
    halfedge_handle any_outgoing_halfedge() const; ///< invalid if isolated
    halfedge_handle any_incoming_halfedge() const; ///< invalid if isolated
    edge_handle any_edge() const;                  ///< invalid if isolated

    vertex_halfedge_in_ring incoming_halfedges() const;
    vertex_halfedge_out_ring outgoing_halfedges() const;
    vertex_edge_ring edges() const;
    vertex_face_ring faces() const;         ///< DOES NOT include invalid ones for boundaries!
    vertex_all_face_ring all_faces() const; ///< includes invalid ones for boundaries!
    vertex_vertex_ring adjacent_vertices() const;
};

struct edge_handle : primitive_handle<edge_tag>
{
    using primitive_handle::primitive_handle;

    bool is_removed() const;  ///< marked for deletion (but valid idx)
    bool is_isolated() const; ///< true if this edge has no faces
    bool is_boundary() const; ///< true if this edge is a boundary

    halfedge_handle halfedgeA() const;
    halfedge_handle halfedgeB() const;
    vertex_handle vertexA() const;
    vertex_handle vertexB() const;
    face_handle faceA() const; ///< can be invalid if boundary
    face_handle faceB() const; ///< can be invalid if boundary
};

struct halfedge_handle : primitive_handle<halfedge_tag>
{
    using primitive_handle::primitive_handle;

    bool is_removed() const;  ///< marked for deletion (but valid idx)
    bool is_boundary() const; ///< true if this half-edge is a boundary (CAUTION: its opposite might not be)

    vertex_handle vertex_to() const;
    vertex_handle vertex_from() const;
    edge_handle edge() const;
    face_handle face() const; ///< invalid if boundary
    halfedge_handle next() const;
    halfedge_handle prev() const;
    halfedge_handle opposite() const;
    face_handle opposite_face() const; ///< invalid if opposite boundary

    halfedge_ring ring() const; ///< all half-edges along the same ring
};
}
