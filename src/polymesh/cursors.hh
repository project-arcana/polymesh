#pragma once

namespace polymesh
{
class Mesh;

template <typename PropT>
struct vertex_property;
template <typename PropT>
struct face_property;
template <typename PropT>
struct edge_property;
template <typename PropT>
struct halfedge_property;

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
    PropT& operator[](face_property<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](face_property<PropT> const& prop) const;
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
    PropT& operator[](vertex_property<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](vertex_property<PropT> const& prop) const;
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
    PropT& operator[](edge_property<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](edge_property<PropT> const& prop) const;
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
    PropT& operator[](halfedge_property<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](halfedge_property<PropT> const& prop) const;
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
    PropT& operator[](face_property<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](face_property<PropT> const& prop) const;

    bool is_valid() const;   ///< valid idx and not deleted
    bool is_deleted() const; ///< marked for deletion (or invalid idx)
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
    PropT& operator[](vertex_property<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](vertex_property<PropT> const& prop) const;

    bool is_valid() const;   ///< valid idx and not deleted
    bool is_deleted() const; ///< marked for deletion (or invalid idx)
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
    PropT& operator[](edge_property<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](edge_property<PropT> const& prop) const;

    bool is_valid() const;   ///< valid idx and not deleted
    bool is_deleted() const; ///< marked for deletion (or invalid idx)
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
    PropT& operator[](halfedge_property<PropT>& prop) const;
    template <typename PropT>
    PropT const& operator[](halfedge_property<PropT> const& prop) const;

    bool is_valid() const;   ///< valid idx and not deleted
    bool is_deleted() const; ///< marked for deletion (or invalid idx)

    // TODO:
    // vertex_to
    // vertex_from
    // face
    // opposite_face
    // opposite
};

}
