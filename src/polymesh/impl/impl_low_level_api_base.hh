#pragma once

#include "../Mesh.hh"

namespace polymesh
{
template <class MeshT>
tmp::ref_if_mut<vertex_index, MeshT> low_level_api_base<MeshT>::to_vertex_of(halfedge_index idx) const
{
    return m.to_vertex_of(idx);
}

template <class MeshT>
tmp::ref_if_mut<face_index, MeshT> low_level_api_base<MeshT>::face_of(halfedge_index idx) const
{
    return m.face_of(idx);
}

template <class MeshT>
tmp::ref_if_mut<halfedge_index, MeshT> low_level_api_base<MeshT>::next_halfedge_of(halfedge_index idx) const
{
    return m.next_halfedge_of(idx);
}

template <class MeshT>
tmp::ref_if_mut<halfedge_index, MeshT> low_level_api_base<MeshT>::prev_halfedge_of(halfedge_index idx) const
{
    return m.prev_halfedge_of(idx);
}

template <class MeshT>
tmp::ref_if_mut<halfedge_index, MeshT> low_level_api_base<MeshT>::halfedge_of(face_index idx) const
{
    return m.halfedge_of(idx);
}

template <class MeshT>
tmp::ref_if_mut<halfedge_index, MeshT> low_level_api_base<MeshT>::outgoing_halfedge_of(vertex_index idx) const
{
    return m.outgoing_halfedge_of(idx);
}


template<class MeshT>
int low_level_api_base<MeshT>::size_all_faces() const
{
    return m.size_all_faces();
}

template<class MeshT>
int low_level_api_base<MeshT>::size_all_vertices() const
{
    return m.size_all_vertices();
}

template<class MeshT>
int low_level_api_base<MeshT>::size_all_edges() const
{
    return m.size_all_edges();
}

template<class MeshT>
int low_level_api_base<MeshT>::size_all_halfedges() const
{
    return m.size_all_halfedges();
}

template<class MeshT>
int low_level_api_base<MeshT>::size_valid_faces() const
{
    return m.size_valid_faces();
}

template<class MeshT>
int low_level_api_base<MeshT>::size_valid_vertices() const
{
    return m.size_valid_vertices();
}

template<class MeshT>
int low_level_api_base<MeshT>::size_valid_edges() const
{
    return m.size_valid_edges();
}

template<class MeshT>
int low_level_api_base<MeshT>::size_valid_halfedges() const
{
    return m.size_valid_halfedges();
}

template <class MeshT>
halfedge_index low_level_api_base<MeshT>::find_free_incident(halfedge_index in_begin, halfedge_index in_end) const
{
    assert(to_vertex_of(in_begin) == to_vertex_of(in_end));

    auto he = in_begin;
    do
    {
        assert(to_vertex_of(he) == to_vertex_of(in_end));

        // free? found one!
        if (is_free(he))
            return he;

        // next half-edge of vertex
        he = opposite(next_halfedge_of(he));
    } while (he != in_end);

    return halfedge_index::invalid();
}

template <class MeshT>
halfedge_index low_level_api_base<MeshT>::find_free_incident(vertex_index v) const
{
    auto in_begin = opposite(outgoing_halfedge_of(v));
    return find_free_incident(in_begin, in_begin);
}

template <class MeshT>
halfedge_index low_level_api_base<MeshT>::find_halfedge(vertex_index from, vertex_index to) const
{
    auto he_begin = outgoing_halfedge_of(from);
    if (!he_begin.is_valid())
        return halfedge_index::invalid(); // isolated vertex

    auto he = he_begin;
    do
    {
        // found?
        if (to_vertex_of(he) == to)
            return he;

        // advance
        he = next_halfedge_of(opposite(he));

    } while (he != he_begin);

    return halfedge_index::invalid(); // not found
}

template <class MeshT>
bool low_level_api_base<MeshT>::is_boundary(vertex_index idx) const
{
    auto oh = outgoing_halfedge_of(idx);
    return !oh.is_valid() || is_boundary(oh);
}

template <class MeshT>
bool low_level_api_base<MeshT>::is_free(halfedge_index idx) const
{
    return face_of(idx).is_invalid();
}
template <class MeshT>
bool low_level_api_base<MeshT>::is_boundary(halfedge_index idx) const
{
    return is_free(idx);
}
template <class MeshT>
bool low_level_api_base<MeshT>::is_boundary(face_index idx) const
{
    return is_free(opposite(halfedge_of(idx)));
}
template <class MeshT>
bool low_level_api_base<MeshT>::is_boundary(edge_index idx) const
{
    return is_free(halfedge_of(idx, 0)) || is_free(halfedge_of(idx, 1));
}

template <class MeshT>
bool low_level_api_base<MeshT>::is_isolated(vertex_index idx) const
{
    return outgoing_halfedge_of(idx).is_invalid();
}
template <class MeshT>
bool low_level_api_base<MeshT>::is_isolated(edge_index idx) const
{
    return is_free(halfedge_of(idx, 0)) && is_free(halfedge_of(idx, 1));
}

template <class MeshT>
bool low_level_api_base<MeshT>::is_removed(vertex_index idx) const
{
    return outgoing_halfedge_of(idx).value == -2;
}
template <class MeshT>
bool low_level_api_base<MeshT>::is_removed(face_index idx) const
{
    return halfedge_of(idx).is_invalid();
}
template <class MeshT>
bool low_level_api_base<MeshT>::is_removed(edge_index idx) const
{
    return to_vertex_of(halfedge_of(idx, 0)).is_invalid();
}
template <class MeshT>
bool low_level_api_base<MeshT>::is_removed(halfedge_index idx) const
{
    return to_vertex_of(idx).is_invalid();
}

template <class MeshT>
halfedge_index low_level_api_base<MeshT>::opposite(halfedge_index he) const
{
    return halfedge_index(he.value ^ 1);
}
template <class MeshT>
face_index low_level_api_base<MeshT>::opposite_face_of(halfedge_index he) const
{
    return face_of(opposite(he));
}

template <class MeshT>
vertex_index low_level_api_base<MeshT>::from_vertex_of(halfedge_index idx) const
{
    return to_vertex_of(opposite(idx));
}

template <class MeshT>
vertex_index low_level_api_base<MeshT>::next_valid_idx_from(vertex_index idx) const
{
    auto s = size_all_vertices();
    auto i = idx;
    while (i.value < s && is_removed(i))
        i.value++;
    return i;
}

template <class MeshT>
vertex_index low_level_api_base<MeshT>::prev_valid_idx_from(vertex_index idx) const
{
    auto i = idx;
    while (i.value >= 0 && is_removed(i))
        i.value--;
    return i;
}

template <class MeshT>
edge_index low_level_api_base<MeshT>::next_valid_idx_from(edge_index idx) const
{
    auto s = size_all_edges();
    auto i = idx;
    while (i.value < s && is_removed(i))
        i.value++;
    return i;
}

template <class MeshT>
edge_index low_level_api_base<MeshT>::prev_valid_idx_from(edge_index idx) const
{
    auto i = idx;
    while (i.value >= 0 && is_removed(i))
        i.value--;
    return i;
}

template <class MeshT>
face_index low_level_api_base<MeshT>::next_valid_idx_from(face_index idx) const
{
    auto s = size_all_faces();
    auto i = idx;
    while (i.value < s && is_removed(i))
        i.value++;
    return i;
}

template <class MeshT>
face_index low_level_api_base<MeshT>::prev_valid_idx_from(face_index idx) const
{
    auto i = idx;
    while (i.value >= 0 && is_removed(i))
        i.value--;
    return i;
}

template <class MeshT>
halfedge_index low_level_api_base<MeshT>::next_valid_idx_from(halfedge_index idx) const
{
    auto s = size_all_halfedges();
    auto i = idx;
    while (i.value < s && is_removed(i))
        i.value++;
    return i;
}

template <class MeshT>
halfedge_index low_level_api_base<MeshT>::prev_valid_idx_from(halfedge_index idx) const
{
    auto i = idx;
    while (i.value >= 0 && is_removed(i))
        i.value--;
    return i;
}
}