#pragma once

#include <cstddef>

#include <polymesh/cursors.hh>
#include <polymesh/fwd.hh>

namespace polymesh
{
struct hash
{
    template <class tag>
    size_t operator()(polymesh::primitive_index<tag> const& i) const noexcept
    {
        return i.value;
    }
    size_t operator()(polymesh::face_index const& i) const noexcept { return i.value; }
    size_t operator()(polymesh::vertex_index const& i) const noexcept { return i.value; }
    size_t operator()(polymesh::edge_index const& i) const noexcept { return i.value; }
    size_t operator()(polymesh::halfedge_index const& i) const noexcept { return i.value; }

    template <class tag>
    size_t operator()(polymesh::primitive_handle<tag> const& i) const noexcept
    {
        return i.idx.value;
    }
    size_t operator()(polymesh::face_handle const& i) const noexcept { return i.idx.value; }
    size_t operator()(polymesh::vertex_handle const& i) const noexcept { return i.idx.value; }
    size_t operator()(polymesh::edge_handle const& i) const noexcept { return i.idx.value; }
    size_t operator()(polymesh::halfedge_handle const& i) const noexcept { return i.idx.value; }
};
}
