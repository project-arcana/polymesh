#pragma once

#include <cassert>

#include "EdgeHandle.hh"
#include "FaceHandle.hh"
#include "HalfedgeHandle.hh"
#include "VertexHandle.hh"

// For iterator interfaces, see http://anderberg.me/2016/07/04/c-custom-iterators/
// Note: some iterator methods are implemented at the end of Mesh.hh to ensure inlining

namespace polymesh
{
struct skipping_vertex_iterator
{
    skipping_vertex_iterator() = default;
    skipping_vertex_iterator(VertexHandle handle) : handle(handle) {}

    VertexHandle operator*() const { return handle; }
    skipping_vertex_iterator& operator++();
    skipping_vertex_iterator operator++(int)
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(skipping_vertex_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(skipping_vertex_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

private:
    VertexHandle handle;
};

struct vertex_iterator
{
    vertex_iterator() = default;
    vertex_iterator(VertexHandle handle) : handle(handle) {}

    VertexHandle operator*() const { return handle; }
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
    VertexHandle handle;
};
}
