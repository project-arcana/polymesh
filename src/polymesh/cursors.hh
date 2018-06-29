#pragma once

#include <iostream>

#include "primitives.hh"

namespace polymesh
{
// ======================== INDICES ========================

template <typename tag>
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
    static index_t invalid() { return {}; }

    bool operator==(index_t const& rhs) const { return value == rhs.value; }
    bool operator!=(index_t const& rhs) const { return value != rhs.value; }

    template <class AttrT>
    AttrT& operator[](attribute<AttrT>& prop) const;
    template <class AttrT>
    AttrT const& operator[](attribute<AttrT> const& prop) const;
    template <class AttrT>
    AttrT& operator[](attribute<AttrT>* prop) const;
    template <class AttrT>
    AttrT const& operator[](attribute<AttrT> const* prop) const;
};

struct face_index : primitive_index<face_tag>
{
    using primitive_index::primitive_index;
    using primitive_index::operator=;
};
struct edge_index : primitive_index<edge_tag>
{
    using primitive_index::primitive_index;
    using primitive_index::operator=;
};
struct halfedge_index : primitive_index<halfedge_tag>
{
    using primitive_index::primitive_index;
    using primitive_index::operator=;
};
struct vertex_index : primitive_index<vertex_tag>
{
    using primitive_index::primitive_index;
    using primitive_index::operator=;
};


// ===========================================
// OLD CODE:


// ======================== HANDLES ========================

struct face_handle
{
    Mesh const* mesh = nullptr;
    face_index idx;

    face_handle() = default;
    face_handle(Mesh const* mesh, face_index idx) : mesh(mesh), idx(idx) {}

    bool operator==(face_index const& rhs) const { return idx == rhs; }
    bool operator!=(face_index const& rhs) const { return idx != rhs; }
    bool operator==(face_handle const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(face_handle const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    template <class AttrT>
    AttrT& operator[](face_attribute<AttrT>& prop) const;
    template <class AttrT>
    AttrT const& operator[](face_attribute<AttrT> const& prop) const;
    template <class AttrT>
    AttrT& operator[](face_attribute<AttrT>* prop) const;
    template <class AttrT>
    AttrT const& operator[](face_attribute<AttrT> const* prop) const;

    bool is_valid() const { return idx.is_valid(); }    ///< valid idx (but could be deleted in some iterators)
    bool is_invalid() const { return !idx.is_valid(); } ///< invalid idx
    bool is_removed() const;                            ///< marked for deletion (but valid idx)

    bool is_boundary() const; ///< true if this face lies at a boundary

    vertex_handle any_vertex() const;
    halfedge_handle any_halfedge() const;

    face_vertex_ring vertices() const;
    face_edge_ring edges() const;
    face_halfedge_ring halfedges() const;
    face_face_ring adjacent_faces() const; ///< includes invalid ones for boundaries!
};

struct vertex_handle
{
    Mesh const* mesh = nullptr;
    vertex_index idx;

    vertex_handle() = default;
    vertex_handle(Mesh const* mesh, vertex_index idx) : mesh(mesh), idx(idx) {}

    bool operator==(vertex_index const& rhs) const { return idx == rhs; }
    bool operator!=(vertex_index const& rhs) const { return idx != rhs; }
    bool operator==(vertex_handle const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(vertex_handle const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    template <class AttrT>
    AttrT& operator[](vertex_attribute<AttrT>& prop) const;
    template <class AttrT>
    AttrT const& operator[](vertex_attribute<AttrT> const& prop) const;
    template <class AttrT>
    AttrT& operator[](vertex_attribute<AttrT>* prop) const;
    template <class AttrT>
    AttrT const& operator[](vertex_attribute<AttrT> const* prop) const;

    bool is_valid() const { return idx.is_valid(); }    ///< valid idx (but could be deleted in some iterators)
    bool is_invalid() const { return !idx.is_valid(); } ///< invalid idx
    bool is_removed() const;                            ///< marked for deletion (but valid idx)

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
    vertex_face_ring faces() const; ///< includes invalid ones for boundaries!
    vertex_vertex_ring adjacent_vertices() const;
};

struct edge_handle
{
    Mesh const* mesh = nullptr;
    edge_index idx;

    edge_handle() = default;
    edge_handle(Mesh const* mesh, edge_index idx) : mesh(mesh), idx(idx) {}

    bool operator==(edge_index const& rhs) const { return idx == rhs; }
    bool operator!=(edge_index const& rhs) const { return idx != rhs; }
    bool operator==(edge_handle const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(edge_handle const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    template <class AttrT>
    AttrT& operator[](edge_attribute<AttrT>& prop) const;
    template <class AttrT>
    AttrT const& operator[](edge_attribute<AttrT> const& prop) const;
    template <class AttrT>
    AttrT& operator[](edge_attribute<AttrT>* prop) const;
    template <class AttrT>
    AttrT const& operator[](edge_attribute<AttrT> const* prop) const;

    bool is_valid() const { return idx.is_valid(); }    ///< valid idx (but could be deleted in some iterators)
    bool is_invalid() const { return !idx.is_valid(); } ///< invalid idx
    bool is_removed() const;                            ///< marked for deletion (but valid idx)

    bool is_isolated() const; ///< true if this edge has no faces
    bool is_boundary() const; ///< true if this edge is a boundary

    halfedge_handle halfedgeA() const;
    halfedge_handle halfedgeB() const;
    vertex_handle vertexA() const;
    vertex_handle vertexB() const;
    face_handle faceA() const; ///< can be invalid if boundary
    face_handle faceB() const; ///< can be invalid if boundary
};

struct halfedge_handle
{
    Mesh const* mesh = nullptr;
    halfedge_index idx;

    halfedge_handle() = default;
    halfedge_handle(Mesh const* mesh, halfedge_index idx) : mesh(mesh), idx(idx) {}

    bool operator==(halfedge_index const& rhs) const { return idx == rhs; }
    bool operator!=(halfedge_index const& rhs) const { return idx != rhs; }
    bool operator==(halfedge_handle const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(halfedge_handle const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    template <class AttrT>
    AttrT& operator[](halfedge_attribute<AttrT>& prop) const;
    template <class AttrT>
    AttrT const& operator[](halfedge_attribute<AttrT> const& prop) const;
    template <class AttrT>
    AttrT& operator[](halfedge_attribute<AttrT>* prop) const;
    template <class AttrT>
    AttrT const& operator[](halfedge_attribute<AttrT> const* prop) const;

    bool is_valid() const { return idx.is_valid(); }    ///< valid idx (but could be deleted in some iterators)
    bool is_invalid() const { return !idx.is_valid(); } ///< invalid idx
    bool is_removed() const;                            ///< marked for deletion (but valid idx)

    bool is_boundary() const; ///< true if this half-edge is a boundary (CAUTION: its opposite might not be)

    vertex_handle vertex_to() const;
    vertex_handle vertex_from() const;
    edge_handle edge() const;
    face_handle face() const; ///< invalid if boundary
    halfedge_handle next() const;
    halfedge_handle prev() const;
    halfedge_handle opposite() const;
    face_handle opposite_face() const; ///< invalid if opposite boundary
};

/// ======== IMPLEMENTATION ========

inline std::ostream& operator<<(std::ostream& out, vertex_index v)
{
    out << "vertex " << v.value;
    if (v.is_invalid())
        out << " (invalid)";
    return out;
}
inline std::ostream& operator<<(std::ostream& out, face_index f)
{
    out << "face " << f.value;
    if (f.is_invalid())
        out << " (invalid)";
    return out;
}
inline std::ostream& operator<<(std::ostream& out, edge_index e)
{
    out << "edge " << e.value;
    if (e.is_invalid())
        out << " (invalid)";
    return out;
}
inline std::ostream& operator<<(std::ostream& out, halfedge_index h)
{
    out << "half-edge " << h.value;
    if (h.is_invalid())
        out << " (invalid)";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, vertex_handle v)
{
    out << v.idx;
    return out;
}
inline std::ostream& operator<<(std::ostream& out, face_handle f)
{
    out << f.idx;
    return out;
}
inline std::ostream& operator<<(std::ostream& out, edge_handle e)
{
    out << e.idx;
    return out;
}
inline std::ostream& operator<<(std::ostream& out, halfedge_handle h)
{
    out << h.idx;
    return out;
}
}
