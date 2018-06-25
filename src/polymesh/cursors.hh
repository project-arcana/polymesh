#pragma once

namespace polymesh
{
class Mesh;

template <typename PropT>
struct vertex_attribute;
template <typename PropT>
struct face_attribute;
template <typename PropT>
struct edge_attribute;
template <typename PropT>
struct halfedge_attribute;

struct vertex_handle;
struct face_handle;
struct edge_handle;
struct halfedge_handle;

struct face_vertex_ring;

// ======================== INDICES ========================

struct face_index
{
    int value = -1;

    face_index() = default;
    explicit face_index(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    static face_index invalid() { return {}; }

    bool operator==(face_index const& rhs) const { return value == rhs.value; }
    bool operator!=(face_index const& rhs) const { return value != rhs.value; }

    template <typename PropT>
    PropT& operator[](face_attribute<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](face_attribute<PropT> const& prop) const;
};

struct vertex_index
{
    int value = -1;

    vertex_index() = default;
    explicit vertex_index(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    static vertex_index invalid() { return {}; }

    bool operator==(vertex_index const& rhs) const { return value == rhs.value; }
    bool operator!=(vertex_index const& rhs) const { return value != rhs.value; }

    template <typename PropT>
    PropT& operator[](vertex_attribute<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](vertex_attribute<PropT> const& prop) const;
};

struct edge_index
{
    int value = -1;

    edge_index() = default;
    explicit edge_index(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    static edge_index invalid() { return {}; }

    bool operator==(edge_index const& rhs) const { return value == rhs.value; }
    bool operator!=(edge_index const& rhs) const { return value != rhs.value; }

    template <typename PropT>
    PropT& operator[](edge_attribute<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](edge_attribute<PropT> const& prop) const;
};

struct halfedge_index
{
    int value = -1;

    halfedge_index() = default;
    explicit halfedge_index(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    static halfedge_index invalid() { return {}; }

    bool operator==(halfedge_index const& rhs) const { return value == rhs.value; }
    bool operator!=(halfedge_index const& rhs) const { return value != rhs.value; }

    template <typename PropT>
    PropT& operator[](halfedge_attribute<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](halfedge_attribute<PropT> const& prop) const;
};

// ======================== HANDLES ========================

struct face_handle
{
    Mesh const* mesh;
    face_index idx;

    face_handle(Mesh const* mesh, face_index idx) : mesh(mesh), idx(idx) {}

    bool operator==(face_index const& rhs) const { return idx == rhs; }
    bool operator!=(face_index const& rhs) const { return idx != rhs; }
    bool operator==(face_handle const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(face_handle const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    template <typename PropT>
    PropT& operator[](face_attribute<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](face_attribute<PropT> const& prop) const;

    bool is_valid() const;   ///< valid idx and not deleted
    bool is_deleted() const; ///< marked for deletion (or invalid idx)

    vertex_handle any_vertex() const;
    halfedge_handle any_halfedge() const;

    face_vertex_ring vertices() const;

    // TODO:
    // faces (1-ring)
    // edges
    // halfedges
    // vertices
};

struct vertex_handle
{
    Mesh const* mesh;
    vertex_index idx;

    vertex_handle(Mesh const* mesh, vertex_index idx) : mesh(mesh), idx(idx) {}

    bool operator==(vertex_index const& rhs) const { return idx == rhs; }
    bool operator!=(vertex_index const& rhs) const { return idx != rhs; }
    bool operator==(vertex_handle const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(vertex_handle const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    template <typename PropT>
    PropT& operator[](vertex_attribute<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](vertex_attribute<PropT> const& prop) const;

    bool is_valid() const;   ///< valid idx and not deleted
    bool is_deleted() const; ///< marked for deletion (or invalid idx)

    face_handle any_face() const;
    halfedge_handle any_halfedge() const;
};

struct edge_handle
{
    Mesh const* mesh;
    edge_index idx;

    edge_handle(Mesh const* mesh, edge_index idx) : mesh(mesh), idx(idx) {}

    bool operator==(edge_index const& rhs) const { return idx == rhs; }
    bool operator!=(edge_index const& rhs) const { return idx != rhs; }
    bool operator==(edge_handle const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(edge_handle const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    template <typename PropT>
    PropT& operator[](edge_attribute<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](edge_attribute<PropT> const& prop) const;

    bool is_valid() const;   ///< valid idx and not deleted
    bool is_deleted() const; ///< marked for deletion (or invalid idx)

    halfedge_handle halfedgeA() const;
    halfedge_handle halfedgeB() const;
};

struct halfedge_handle
{
    Mesh const* mesh;
    halfedge_index idx;

    halfedge_handle(Mesh const* mesh, halfedge_index idx) : mesh(mesh), idx(idx) {}

    bool operator==(halfedge_index const& rhs) const { return idx == rhs; }
    bool operator!=(halfedge_index const& rhs) const { return idx != rhs; }
    bool operator==(halfedge_handle const& rhs) const { return mesh == rhs.mesh && idx == rhs.idx; }
    bool operator!=(halfedge_handle const& rhs) const { return mesh != rhs.mesh || idx != rhs.idx; }

    template <typename PropT>
    PropT& operator[](halfedge_attribute<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](halfedge_attribute<PropT> const& prop) const;

    bool is_valid() const;   ///< valid idx and not deleted
    bool is_deleted() const; ///< marked for deletion (or invalid idx)

    vertex_handle vertex_to() const;
    vertex_handle vertex_from() const;
    face_handle face() const;
    halfedge_handle next() const;
    halfedge_handle prev() const;
    halfedge_handle opposite() const;
    face_handle opposite_face() const;
};

}
