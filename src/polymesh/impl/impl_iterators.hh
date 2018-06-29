#pragma once

#include "../Mesh.hh"

namespace polymesh
{

inline valid_vertex_iterator &valid_vertex_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
inline all_vertex_iterator &all_vertex_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}
inline void valid_vertex_iterator::move_to_valid()
{
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
}

inline valid_face_iterator &valid_face_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
inline face_iterator &face_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}
inline void valid_face_iterator::move_to_valid()
{
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
}

inline valid_edge_iterator &valid_edge_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
inline edge_iterator &edge_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}
inline void valid_edge_iterator::move_to_valid()
{
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
}

inline valid_halfedge_iterator &valid_halfedge_iterator::operator++()
{
    handle.idx.value++;
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
    return *this;
}
inline halfedge_iterator &halfedge_iterator::operator++()
{
    handle.idx.value++;
    return *this;
}
inline void valid_halfedge_iterator::move_to_valid()
{
    handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
}

}
