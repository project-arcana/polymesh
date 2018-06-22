#pragma once

namespace polymesh
{
struct Mesh;

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

    // TODO:
    // vertex_to
    // vertex_from
    // face
    // opposite_face
    // opposite
};
}
