#pragma once

#include <cassert>

#include "cursors.hh"

// For iterator interfaces, see http://anderberg.me/2016/07/04/c-custom-iterators/
// Note: some iterator methods are implemented at the end of Mesh.hh to ensure inlining

namespace polymesh
{

template <typename tag>
struct valid_primitive_iterator
{
    using handle_t = typename primitive<tag>::handle;

    valid_primitive_iterator() = default;
    valid_primitive_iterator(handle_t handle) : handle(handle)
    {
        handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    }

    handle_t operator*() const { return handle; }
    valid_primitive_iterator& operator++()
    {
        handle.idx.value++;
        handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
        return *this;
    }
    valid_primitive_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(valid_primitive_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(valid_primitive_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    handle_t handle;
};

template <typename tag>
struct all_primitive_iterator
{
    using handle_t = typename primitive<tag>::handle;

    all_primitive_iterator() = default;
    all_primitive_iterator(handle_t handle) : handle(handle) {}

    handle_t operator*() const { return handle; }
    all_primitive_iterator& operator++()
    {
        handle.idx.value++;
        return *this;
    }
    all_primitive_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(all_primitive_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(all_primitive_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    handle_t handle;
};

// ===========================================
// OLD CODE:

/// Iterates over all vertices of a given face
struct face_vertex_circulator
{
    face_vertex_circulator() = default;
    face_vertex_circulator(face_handle handle, bool not_at_begin)
      : handle(handle.any_halfedge()), not_at_begin(not_at_begin)
    {
    }

    vertex_handle operator*() const { return handle.vertex_to(); }
    face_vertex_circulator& operator++()
    {
        handle = handle.next();
        not_at_begin = true;
        return *this;
    }
    face_vertex_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(face_vertex_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(face_vertex_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};

/// Iterates over all halfedges of a given face
struct face_halfedge_circulator
{
    face_halfedge_circulator() = default;
    face_halfedge_circulator(face_handle handle, bool not_at_begin)
      : handle(handle.any_halfedge()), not_at_begin(not_at_begin)
    {
    }

    halfedge_handle operator*() const { return handle; }
    face_halfedge_circulator& operator++()
    {
        handle = handle.next();
        not_at_begin = true;
        return *this;
    }
    face_halfedge_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(face_halfedge_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(face_halfedge_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};

/// Iterates over all edges of a given face
struct face_edge_circulator
{
    face_edge_circulator() = default;
    face_edge_circulator(face_handle handle, bool not_at_begin)
      : handle(handle.any_halfedge()), not_at_begin(not_at_begin)
    {
    }

    edge_handle operator*() const { return handle.edge(); }
    face_edge_circulator& operator++()
    {
        handle = handle.next();
        not_at_begin = true;
        return *this;
    }
    face_edge_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(face_edge_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(face_edge_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};

/// Iterates over all adjacent faces of a given face
struct face_face_circulator
{
    face_face_circulator() = default;
    face_face_circulator(face_handle handle, bool not_at_begin)
      : handle(handle.any_halfedge()), not_at_begin(not_at_begin)
    {
    }

    face_handle operator*() const { return handle.opposite_face(); }
    face_face_circulator& operator++()
    {
        handle = handle.next();
        not_at_begin = true;
        return *this;
    }
    face_face_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(face_face_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(face_face_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};

/// Iterates over all outgoing halfedges of a given vertex
struct vertex_halfedge_out_circulator
{
    vertex_halfedge_out_circulator() = default;
    vertex_halfedge_out_circulator(vertex_handle handle, bool not_at_begin)
      : handle(handle.any_outgoing_halfedge()), not_at_begin(not_at_begin)
    {
    }

    halfedge_handle operator*() const { return handle; }
    vertex_halfedge_out_circulator& operator++()
    {
        handle = handle.opposite().next();
        not_at_begin = true;
        return *this;
    }
    vertex_halfedge_out_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(vertex_halfedge_out_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(vertex_halfedge_out_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};

/// Iterates over all incoming halfedges of a given vertex
struct vertex_halfedge_in_circulator
{
    vertex_halfedge_in_circulator() = default;
    vertex_halfedge_in_circulator(vertex_handle handle, bool not_at_begin)
      : handle(handle.any_outgoing_halfedge()), not_at_begin(not_at_begin)
    {
    }

    halfedge_handle operator*() const { return handle.opposite(); }
    vertex_halfedge_in_circulator& operator++()
    {
        handle = handle.opposite().next();
        not_at_begin = true;
        return *this;
    }
    vertex_halfedge_in_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(vertex_halfedge_in_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(vertex_halfedge_in_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};

/// Iterates over all adjacent faces of a given vertex INCLUDING invalid ones
struct vertex_face_circulator
{
    vertex_face_circulator() = default;
    vertex_face_circulator(vertex_handle handle, bool not_at_begin)
      : handle(handle.any_outgoing_halfedge()), not_at_begin(not_at_begin)
    {
    }

    face_handle operator*() const { return handle.face(); }
    vertex_face_circulator& operator++()
    {
        handle = handle.opposite().next();
        not_at_begin = true;
        return *this;
    }
    vertex_face_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(vertex_face_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(vertex_face_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};

/// Iterates over all adjacent vertices of a given vertex
struct vertex_vertex_circulator
{
    vertex_vertex_circulator() = default;
    vertex_vertex_circulator(vertex_handle handle, bool not_at_begin)
      : handle(handle.any_outgoing_halfedge()), not_at_begin(not_at_begin)
    {
    }

    vertex_handle operator*() const { return handle.vertex_to(); }
    vertex_vertex_circulator& operator++()
    {
        handle = handle.opposite().next();
        not_at_begin = true;
        return *this;
    }
    vertex_vertex_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(vertex_vertex_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(vertex_vertex_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};

/// Iterates over all adjacent edges of a given vertex
struct vertex_edge_circulator
{
    vertex_edge_circulator() = default;
    vertex_edge_circulator(vertex_handle handle, bool not_at_begin)
      : handle(handle.any_outgoing_halfedge()), not_at_begin(not_at_begin)
    {
    }

    edge_handle operator*() const { return handle.edge(); }
    vertex_edge_circulator& operator++()
    {
        handle = handle.opposite().next();
        not_at_begin = true;
        return *this;
    }
    vertex_edge_circulator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(vertex_edge_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(vertex_edge_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
    bool not_at_begin;
};
}
