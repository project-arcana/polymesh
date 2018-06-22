#pragma once

#include <cassert>

#include "cursors.hh"

// For iterator interfaces, see http://anderberg.me/2016/07/04/c-custom-iterators/
// Note: some iterator methods are implemented at the end of Mesh.hh to ensure inlining

namespace polymesh
{
struct valid_vertex_iterator
{
    valid_vertex_iterator() = default;
    valid_vertex_iterator(vertex_handle handle) : handle(handle) {}

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
    valid_face_iterator(face_handle handle) : handle(handle) {}

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
    valid_edge_iterator(edge_handle handle) : handle(handle) {}

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
    valid_halfedge_iterator(halfedge_handle handle) : handle(handle) {}

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
}
