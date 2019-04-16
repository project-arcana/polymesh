#pragma once

#include <utility>

#include <polymesh/Mesh.hh>

namespace std
{
template <class tag>
struct hash<polymesh::primitive_index<tag>>
{
    size_t operator()(polymesh::primitive_index<tag> const& i) const noexcept { return std::hash<int>()(i.value); }
};
template <>
struct hash<polymesh::face_index>
{
    size_t operator()(polymesh::face_index const& i) const noexcept { return std::hash<int>()(i.value); }
};
template <>
struct hash<polymesh::vertex_index>
{
    size_t operator()(polymesh::vertex_index const& i) const noexcept { return std::hash<int>()(i.value); }
};
template <>
struct hash<polymesh::edge_index>
{
    size_t operator()(polymesh::edge_index const& i) const noexcept { return std::hash<int>()(i.value); }
};
template <>
struct hash<polymesh::halfedge_index>
{
    size_t operator()(polymesh::halfedge_index const& i) const noexcept { return std::hash<int>()(i.value); }
};

template <class tag>
struct hash<polymesh::primitive_handle<tag>>
{
    size_t operator()(polymesh::primitive_handle<tag> const& i) const noexcept { return std::hash<int>()(i.idx.value); }
};
template <>
struct hash<polymesh::face_handle>
{
    size_t operator()(polymesh::face_handle const& i) const noexcept { return std::hash<int>()(i.idx.value); }
};
template <>
struct hash<polymesh::vertex_handle>
{
    size_t operator()(polymesh::vertex_handle const& i) const noexcept { return std::hash<int>()(i.idx.value); }
};
template <>
struct hash<polymesh::edge_handle>
{
    size_t operator()(polymesh::edge_handle const& i) const noexcept { return std::hash<int>()(i.idx.value); }
};
template <>
struct hash<polymesh::halfedge_handle>
{
    size_t operator()(polymesh::halfedge_handle const& i) const noexcept { return std::hash<int>()(i.idx.value); }
};
}
