#pragma once

#include <cassert>

#include "cursors.hh"

// For iterator interfaces, see http://anderberg.me/2016/07/04/c-custom-iterators/
// Note: some iterator methods are implemented at the end of Mesh.hh to ensure inlining

namespace polymesh
{

// struct valid_primitive_iterator
// {
//
// };





// ===========================================
// OLD CODE:

struct valid_vertex_iterator
{
    valid_vertex_iterator() = default;
    valid_vertex_iterator(vertex_handle handle) : handle(handle) { move_to_valid(); }

    vertex_handle operator*() const { return handle; }
    valid_vertex_iterator& operator++();
    valid_vertex_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(valid_vertex_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(valid_vertex_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    vertex_handle handle;

    void move_to_valid();
};

struct vertex_iterator
{
    vertex_iterator() = default;
    vertex_iterator(vertex_handle handle) : handle(handle) {}

    vertex_handle operator*() const { return handle; }
    vertex_iterator& operator++();
    vertex_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(vertex_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(vertex_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    vertex_handle handle;
};

struct valid_face_iterator
{
    valid_face_iterator() = default;
    valid_face_iterator(face_handle handle) : handle(handle) { move_to_valid(); }

    face_handle operator*() const { return handle; }
    valid_face_iterator& operator++();
    valid_face_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(valid_face_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(valid_face_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    face_handle handle;

    void move_to_valid();
};

struct face_iterator
{
    face_iterator() = default;
    face_iterator(face_handle handle) : handle(handle) {}

    face_handle operator*() const { return handle; }
    face_iterator& operator++();
    face_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(face_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(face_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    face_handle handle;
};

struct valid_edge_iterator
{
    valid_edge_iterator() = default;
    valid_edge_iterator(edge_handle handle) : handle(handle) { move_to_valid(); }

    edge_handle operator*() const { return handle; }
    valid_edge_iterator& operator++();
    valid_edge_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(valid_edge_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(valid_edge_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    edge_handle handle;

    void move_to_valid();
};

struct edge_iterator
{
    edge_iterator() = default;
    edge_iterator(edge_handle handle) : handle(handle) {}

    edge_handle operator*() const { return handle; }
    edge_iterator& operator++();
    edge_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(edge_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(edge_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    edge_handle handle;
};

struct valid_halfedge_iterator
{
    valid_halfedge_iterator() = default;
    valid_halfedge_iterator(halfedge_handle handle) : handle(handle) { move_to_valid(); }

    halfedge_handle operator*() const { return handle; }
    valid_halfedge_iterator& operator++();
    valid_halfedge_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(valid_halfedge_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(valid_halfedge_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;

    void move_to_valid();
};

struct halfedge_iterator
{
    halfedge_iterator() = default;
    halfedge_iterator(halfedge_handle handle) : handle(handle) {}

    halfedge_handle operator*() const { return handle; }
    halfedge_iterator& operator++();
    halfedge_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(halfedge_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(halfedge_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    halfedge_handle handle;
};

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
