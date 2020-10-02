#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class MeshT>
tmp::cond_const_ref<vertex_index, MeshT> low_level_api_base<MeshT>::to_vertex_of(halfedge_index idx) const
{
    return m.to_vertex_of(idx);
}

template <class MeshT>
tmp::cond_const_ref<face_index, MeshT> low_level_api_base<MeshT>::face_of(halfedge_index idx) const
{
    return m.face_of(idx);
}

template <class MeshT>
tmp::cond_const_ref<halfedge_index, MeshT> low_level_api_base<MeshT>::next_halfedge_of(halfedge_index idx) const
{
    return m.next_halfedge_of(idx);
}

template <class MeshT>
tmp::cond_const_ref<halfedge_index, MeshT> low_level_api_base<MeshT>::prev_halfedge_of(halfedge_index idx) const
{
    return m.prev_halfedge_of(idx);
}

template <class MeshT>
tmp::cond_const_ref<halfedge_index, MeshT> low_level_api_base<MeshT>::halfedge_of(face_index idx) const
{
    return m.halfedge_of(idx);
}

template <class MeshT>
tmp::cond_const_ref<halfedge_index, MeshT> low_level_api_base<MeshT>::outgoing_halfedge_of(vertex_index idx) const
{
    return m.outgoing_halfedge_of(idx);
}


template <class MeshT>
int low_level_api_base<MeshT>::capacity_faces() const
{
    return m.mFacesCapacity;
}

template <class MeshT>
int low_level_api_base<MeshT>::capacity_vertices() const
{
    return m.mVerticesCapacity;
}

template <class MeshT>
int low_level_api_base<MeshT>::capacity_halfedges() const
{
    return m.mHalfedgesCapacity;
}

template <class MeshT>
int low_level_api_base<MeshT>::size_all_faces() const
{
    return m.size_all_faces();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_all_vertices() const
{
    return m.size_all_vertices();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_all_edges() const
{
    return m.size_all_edges();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_all_halfedges() const
{
    return m.size_all_halfedges();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_valid_faces() const
{
    return m.size_valid_faces();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_valid_vertices() const
{
    return m.size_valid_vertices();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_valid_edges() const
{
    return m.size_valid_edges();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_valid_halfedges() const
{
    return m.size_valid_halfedges();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_removed_faces() const
{
    return m.size_all_faces() - m.size_removed_faces();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_removed_vertices() const
{
    return m.size_all_vertices() - m.size_removed_vertices();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_removed_edges() const
{
    return m.size_all_edges() - m.size_removed_edges();
}

template <class MeshT>
int low_level_api_base<MeshT>::size_removed_halfedges() const
{
    return m.size_all_halfedges() - m.size_removed_halfedges();
}

template <class MeshT>
size_t low_level_api_base<MeshT>::byte_size_topology() const
{
    size_t s = 0;

    s += size_all_vertices() * sizeof(halfedge_index);
    s += size_all_faces() * sizeof(halfedge_index);
    s += size_all_halfedges() * sizeof(vertex_index);
    s += size_all_halfedges() * sizeof(face_index);
    s += size_all_halfedges() * sizeof(halfedge_index);
    s += size_all_halfedges() * sizeof(halfedge_index);

    return s;
}

template <class MeshT>
size_t low_level_api_base<MeshT>::byte_size_attributes() const
{
    size_t s = 0;

    for (auto a = m.mVertexAttrs; a; a = a->mNextAttribute)
        s += a->byte_size();
    for (auto a = m.mFaceAttrs; a; a = a->mNextAttribute)
        s += a->byte_size();
    for (auto a = m.mEdgeAttrs; a; a = a->mNextAttribute)
        s += a->byte_size();
    for (auto a = m.mHalfedgeAttrs; a; a = a->mNextAttribute)
        s += a->byte_size();

    return s;
}


template <class MeshT>
size_t low_level_api_base<MeshT>::allocated_byte_size_topology() const
{
    size_t s = 0;

    s += capacity_vertices() * sizeof(halfedge_index);
    s += capacity_faces() * sizeof(halfedge_index);
    s += capacity_halfedges() * sizeof(vertex_index);
    s += capacity_halfedges() * sizeof(face_index);
    s += capacity_halfedges() * sizeof(halfedge_index);
    s += capacity_halfedges() * sizeof(halfedge_index);

    return s;
}

template <class MeshT>
size_t low_level_api_base<MeshT>::allocated_byte_size_attributes() const
{
    size_t s = 0;

    for (auto a = m.mVertexAttrs; a; a = a->mNextAttribute)
        s += a->allocated_byte_size();
    for (auto a = m.mFaceAttrs; a; a = a->mNextAttribute)
        s += a->allocated_byte_size();
    for (auto a = m.mEdgeAttrs; a; a = a->mNextAttribute)
        s += a->allocated_byte_size();
    for (auto a = m.mHalfedgeAttrs; a; a = a->mNextAttribute)
        s += a->allocated_byte_size();

    return s;
}

template <class MeshT>
bool low_level_api_base<MeshT>::can_add_face(const vertex_handle* v_handles, int vcnt) const
{
    if (vcnt < 3)
        return false; // too few vertices

    // check duplicated vertices
    // TODO: more performant for high number of vertices
    for (auto i = 0; i < vcnt; ++i)
        for (auto j = i + 1; j < vcnt; ++j)
            if (v_handles[i] == v_handles[j])
                return false;

    // ensure that half-edges are adjacent at each vertex
    for (auto i = 0; i < vcnt; ++i)
    {
        if (!is_boundary(v_handles[i]))
            return false; // must be boundary

        auto v0 = v_handles[i];
        auto v1 = v_handles[(i + 1) % vcnt];
        auto v2 = v_handles[(i + 2) % vcnt];

        auto h0 = find_halfedge(v0, v1);
        auto h1 = find_halfedge(v1, v2);

        if (h0.is_valid() && !is_boundary(h0))
            return false; // must be boundary

        if (h0.is_invalid())
            continue; // will be added
        if (h1.is_invalid())
            continue; // will be added

        if (to_vertex_of(h0) != from_vertex_of(h1))
            return false; // not a chain

        if (next_halfedge_of(h0) == h1)
            continue; // correctly wired

        if (find_free_incident(opposite(h1), h0).is_invalid())
            return false; // non-manifold
    }

    return true;
}

template <class MeshT>
bool low_level_api_base<MeshT>::can_add_face(const vertex_index* v_indices, int vcnt) const
{
    if (vcnt < 3)
        return false; // too few vertices

    // check duplicated vertices
    // TODO: more performant for high number of vertices
    for (auto i = 0; i < vcnt; ++i)
        for (auto j = i + 1; j < vcnt; ++j)
            if (v_indices[i] == v_indices[j])
                return false;

    // ensure that half-edges are adjacent at each vertex
    for (auto i = 0; i < vcnt; ++i)
    {
        if (!is_boundary(v_indices[i]))
            return false; // must be boundary

        auto v0 = v_indices[i];
        auto v1 = v_indices[(i + 1) % vcnt];
        auto v2 = v_indices[(i + 2) % vcnt];

        auto h0 = find_halfedge(v0, v1);
        auto h1 = find_halfedge(v1, v2);

        if (h0.is_valid() && !is_boundary(h0))
            return false; // must be boundary

        if (h0.is_invalid())
            continue; // will be added
        if (h1.is_invalid())
            continue; // will be added

        if (to_vertex_of(h0) != from_vertex_of(h1))
            return false; // not a chain

        if (!is_free(h0))
            return false; // already contains a face

        if (next_halfedge_of(h0) == h1)
            continue; // correctly wired

        if (find_free_incident(opposite(h1), h0).is_invalid())
            return false; // non-manifold
    }

    return true;
}

template <class MeshT>
bool low_level_api_base<MeshT>::can_add_face(const halfedge_handle* half_loop, int vcnt) const
{
    if (vcnt < 3)
        return false; // too few vertices

    // check duplicated vertices
    // TODO: a bit more performant
    for (auto i = 0; i < vcnt; ++i)
        for (auto j = i + 1; j < vcnt; ++j)
            if (to_vertex_of(half_loop[i]) == to_vertex_of(half_loop[j]))
                return false;

    // ensure that half-edges are adjacent at each vertex
    for (auto i = 0; i < vcnt; ++i)
    {
        auto h0 = half_loop[i].idx;
        auto h1 = half_loop[(i + 1) % vcnt].idx;

        if (to_vertex_of(h0) != from_vertex_of(h1))
            return false; // not a chain

        if (!is_free(h0))
            return false; // already contains a face

        if (next_halfedge_of(h0) == h1)
            continue; // correctly wired

        if (find_free_incident(opposite(h1), h0).is_invalid())
            return false; // non-manifold
    }

    return true;
}

template <class MeshT>
bool low_level_api_base<MeshT>::can_add_face(const halfedge_index* half_loop, int vcnt) const
{
    if (vcnt < 3)
        return false; // too few vertices

    // check duplicated vertices
    // TODO: a bit more performant
    for (auto i = 0; i < vcnt; ++i)
        for (auto j = i + 1; j < vcnt; ++j)
            if (to_vertex_of(half_loop[i]) == to_vertex_of(half_loop[j]))
                return false;

    // ensure that half-edges are adjacent at each vertex
    for (auto i = 0; i < vcnt; ++i)
    {
        auto h0 = half_loop[i];
        auto h1 = half_loop[(i + 1) % vcnt];

        if (to_vertex_of(h0) != from_vertex_of(h1))
            return false; // not a chain

        if (!is_free(h0))
            return false; // already contains a face

        if (next_halfedge_of(h0) == h1)
            continue; // correctly wired

        if (find_free_incident(opposite(h1), h0).is_invalid())
            return false; // non-manifold
    }

    return true;
}

template <class MeshT>
halfedge_index low_level_api_base<MeshT>::find_free_incident(halfedge_index in_begin, halfedge_index in_end) const
{
    POLYMESH_ASSERT(to_vertex_of(in_begin) == to_vertex_of(in_end));

    auto he = in_begin;
    do
    {
        POLYMESH_ASSERT(to_vertex_of(he) == to_vertex_of(in_end));

        // free? found one!
        if (is_free(he))
            return he;

        // next half-edge of vertex
        he = opposite(next_halfedge_of(he));
    } while (he != in_end);

    return halfedge_index::invalid;
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
        return halfedge_index::invalid; // isolated vertex

    auto he = he_begin;
    do
    {
        // found?
        if (to_vertex_of(he) == to)
            return he;

        // advance
        he = next_halfedge_of(opposite(he));

    } while (he != he_begin);

    return halfedge_index::invalid; // not found
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
int low_level_api_base<MeshT>::vertex_attribute_count() const
{
    auto cnt = 0;
    auto attr = m.mVertexAttrs;
    while (attr)
    {
        ++cnt;
        attr = attr->mNextAttribute;
    }
    return cnt;
}
template <class MeshT>
int low_level_api_base<MeshT>::face_attribute_count() const
{
    auto cnt = 0;
    auto attr = m.mFaceAttrs;
    while (attr)
    {
        ++cnt;
        attr = attr->mNextAttribute;
    }
    return cnt;
}
template <class MeshT>
int low_level_api_base<MeshT>::edge_attribute_count() const
{
    auto cnt = 0;
    auto attr = m.mEdgeAttrs;
    while (attr)
    {
        ++cnt;
        attr = attr->mNextAttribute;
    }
    return cnt;
}
template <class MeshT>
int low_level_api_base<MeshT>::halfedge_attribute_count() const
{
    auto cnt = 0;
    auto attr = m.mHalfedgeAttrs;
    while (attr)
    {
        ++cnt;
        attr = attr->mNextAttribute;
    }
    return cnt;
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
} // namespace polymesh
