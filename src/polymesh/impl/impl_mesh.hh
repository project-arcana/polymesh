#pragma once

#include "../Mesh.hh"

#include "../detail/permutation.hh"
#include "../detail/split_vector.hh"

namespace polymesh
{
inline face_index &Mesh::face_of(halfedge_index idx) { return mHalfedgeToFace[(int)idx]; }
inline vertex_index &Mesh::to_vertex_of(halfedge_index idx) { return mHalfedgeToVertex[(int)idx]; }
inline halfedge_index &Mesh::next_halfedge_of(halfedge_index idx) { return mHalfedgeToNextHalfedge[(int)idx]; }
inline halfedge_index &Mesh::prev_halfedge_of(halfedge_index idx) { return mHalfedgeToPrevHalfedge[(int)idx]; }
inline halfedge_index &Mesh::halfedge_of(face_index idx) { return mFaceToHalfedge[(int)idx]; }
inline halfedge_index &Mesh::outgoing_halfedge_of(vertex_index idx) { return mVertexToOutgoingHalfedge[(int)idx]; }

inline face_index const &Mesh::face_of(halfedge_index idx) const { return mHalfedgeToFace[(int)idx]; }
inline vertex_index const &Mesh::to_vertex_of(halfedge_index idx) const { return mHalfedgeToVertex[(int)idx]; }
inline halfedge_index const &Mesh::next_halfedge_of(halfedge_index idx) const { return mHalfedgeToNextHalfedge[(int)idx]; }
inline halfedge_index const &Mesh::prev_halfedge_of(halfedge_index idx) const { return mHalfedgeToPrevHalfedge[(int)idx]; }
inline halfedge_index const &Mesh::halfedge_of(face_index idx) const { return mFaceToHalfedge[(int)idx]; }
inline halfedge_index const &Mesh::outgoing_halfedge_of(vertex_index idx) const { return mVertexToOutgoingHalfedge[(int)idx]; }

inline vertex_index Mesh::alloc_vertex()
{
    auto idx = vertex_index(size_all_vertices());

    auto old_size = mVerticesSize;
    auto capacity_changed = detail::alloc_back(mVerticesSize, mVerticesCapacity, mVertexToOutgoingHalfedge);
    mVertexToOutgoingHalfedge[mVerticesSize - 1] = halfedge_index::invalid();

    if (capacity_changed)
    {
        // notify attributes
        for (auto p = mVertexAttrs; p; p = p->mNextAttribute)
            p->resize_from(old_size);
    }

    return idx;
}

inline face_index Mesh::alloc_face()
{
    auto idx = face_index(size_all_faces());

    auto old_size = mFacesSize;
    auto capacity_changed = detail::alloc_back(mFacesSize, mFacesCapacity, mFaceToHalfedge);
    mFaceToHalfedge[mFacesSize - 1] = halfedge_index::invalid();

    if (capacity_changed)
    {
        // notify attributes
        for (auto p = mFaceAttrs; p; p = p->mNextAttribute)
            p->resize_from(old_size);
    }

    return idx;
}

inline edge_index Mesh::alloc_edge()
{
    auto idx = edge_index(size_all_edges());

    auto capacity_changed = false;
    auto old_size = mHalfedgesSize;
    for (auto i = 0; i < 2; i++)
    {
        capacity_changed |= detail::alloc_back(mHalfedgesSize, mHalfedgesCapacity, //
                                               mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);
        mHalfedgeToFace[mHalfedgesSize - 1] = face_index::invalid();
        mHalfedgeToVertex[mHalfedgesSize - 1] = vertex_index::invalid();
        mHalfedgeToNextHalfedge[mHalfedgesSize - 1] = halfedge_index::invalid();
        mHalfedgeToPrevHalfedge[mHalfedgesSize - 1] = halfedge_index::invalid();
    }

    if (capacity_changed)
    {
        // notify attributes
        for (auto p = mEdgeAttrs; p; p = p->mNextAttribute)
            p->resize_from(old_size >> 1);
        for (auto p = mHalfedgeAttrs; p; p = p->mNextAttribute)
            p->resize_from(old_size);
    }

    return idx;
}

inline void Mesh::alloc_primitives(int vertices, int faces, int halfedges)
{
    assert(vertices >= 0 && faces >= 0 && halfedges >= 0);

    assert(size_all_vertices() + int64_t(vertices) <= int64_t(std::numeric_limits<int>::max()) && "polymesh only supports 2^31 primitives");
    assert(size_all_faces() + int64_t(faces) <= int64_t(std::numeric_limits<int>::max()) && "polymesh only supports 2^31 primitives");
    assert(size_all_halfedges() + int64_t(halfedges) <= int64_t(std::numeric_limits<int>::max()) && "polymesh only supports 2^31 primitives");

    auto vCnt = size_all_vertices() + vertices;
    auto fCnt = size_all_faces() + faces;
    auto hCnt = size_all_halfedges() + halfedges;

    // alloc space
    auto old_v_size = mVerticesSize;
    auto v_capacity_changed = detail::resize(mVerticesSize, mVerticesCapacity, vCnt, mVertexToOutgoingHalfedge);

    auto old_f_size = mFacesSize;
    auto f_capacity_changed = detail::resize(mFacesSize, mFacesCapacity, fCnt, mFaceToHalfedge);

    auto old_h_size = mHalfedgesSize;
    auto h_capacity_changed = detail::resize(mHalfedgesSize, mHalfedgesCapacity, hCnt, //
                                             mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);

    // notify attributes
    if (v_capacity_changed)
        for (auto p = mVertexAttrs; p; p = p->mNextAttribute)
            p->resize_from(old_v_size);

    if (f_capacity_changed)
        for (auto p = mFaceAttrs; p; p = p->mNextAttribute)
            p->resize_from(old_f_size);

    if (h_capacity_changed)
    {
        for (auto p = mEdgeAttrs; p; p = p->mNextAttribute)
            p->resize_from(old_h_size >> 1);
        for (auto p = mHalfedgeAttrs; p; p = p->mNextAttribute)
            p->resize_from(old_h_size);
    }
}


inline void Mesh::permute_vertices(std::vector<int> const &p)
{
    assert(detail::is_valid_permutation(p));

    // calculate transpositions
    auto ts = detail::transpositions_of(p);

    // apply them
    for (auto t : ts)
        std::swap(mVertexToOutgoingHalfedge[t.first], mVertexToOutgoingHalfedge[t.second]);

    // fix half-edges
    for (auto &h_to : detail::range(mHalfedgesSize, mHalfedgeToVertex))
        if (h_to.is_valid())
            h_to.value = p[h_to.value];

    // update attributes
    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->apply_transpositions(ts);
}

inline void Mesh::permute_faces(std::vector<int> const &p)
{
    assert(detail::is_valid_permutation(p));

    // calculate transpositions
    auto ts = detail::transpositions_of(p);

    // apply them
    for (auto t : ts)
        std::swap(mFaceToHalfedge[t.first], mFaceToHalfedge[t.second]);

    // fix half-edges
    for (auto &h_f : detail::range(mHalfedgesSize, mHalfedgeToFace))
        if (h_f.is_valid())
            h_f.value = p[h_f.value];

    // update attributes
    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->apply_transpositions(ts);
}

inline void Mesh::permute_edges(std::vector<int> const &p)
{
    assert(detail::is_valid_permutation(p));

    std::vector<int> hp(p.size() * 2);
    for (auto i = 0u; i < p.size(); ++i)
    {
        hp[i * 2 + 0] = p[i] * 2 + 0;
        hp[i * 2 + 1] = p[i] * 2 + 1;
    }
    assert(detail::is_valid_permutation(hp));

    // calculate transpositions
    std::vector<std::pair<int, int>> edge_ts;
    std::vector<std::pair<int, int>> halfedge_ts;

    detail::apply_permutation(p, [&](int i, int j) {
        edge_ts.emplace_back(i, j);
        halfedge_ts.emplace_back((i << 1) + 0, (j << 1) + 0);
        halfedge_ts.emplace_back((i << 1) + 1, (j << 1) + 1);
    });

    // apply them
    for (auto t : halfedge_ts)
    {
        std::swap(mHalfedgeToFace[t.first], mHalfedgeToFace[t.second]);
        std::swap(mHalfedgeToVertex[t.first], mHalfedgeToVertex[t.second]);
        std::swap(mHalfedgeToNextHalfedge[t.first], mHalfedgeToNextHalfedge[t.second]);
        std::swap(mHalfedgeToPrevHalfedge[t.first], mHalfedgeToPrevHalfedge[t.second]);
    }

    // fix half-edges
    for (auto &v_out : detail::range(mVerticesSize, mVertexToOutgoingHalfedge))
        if (v_out.value >= 0)
            v_out.value = hp[v_out.value];

    for (auto &f_h : detail::range(mFacesSize, mFaceToHalfedge))
        if (f_h.value >= 0)
            f_h.value = hp[f_h.value];

    for (auto &h_next : detail::range(mHalfedgesSize, mHalfedgeToNextHalfedge))
        if (h_next.value >= 0)
            h_next.value = hp[h_next.value];

    for (auto &h_prev : detail::range(mHalfedgesSize, mHalfedgeToPrevHalfedge))
        if (h_prev.value >= 0)
            h_prev.value = hp[h_prev.value];

    // update attributes
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->apply_transpositions(edge_ts);
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->apply_transpositions(halfedge_ts);
}

inline void Mesh::compactify()
{
    if (is_compact())
        return;

    auto ll = low_level_api(this);

    // calculate remappings
    int v_cnt = size_all_vertices();
    int f_cnt = size_all_faces();
    int e_cnt = size_all_edges();
    int h_cnt = size_all_halfedges();
    std::vector<int> v_new_to_old;
    std::vector<int> f_new_to_old;
    std::vector<int> e_new_to_old;
    std::vector<int> h_new_to_old;
    v_new_to_old.reserve(v_cnt);
    f_new_to_old.reserve(f_cnt);
    e_new_to_old.reserve(e_cnt);
    h_new_to_old.reserve(h_cnt);
    std::vector<int> h_old_to_new(h_cnt, -1);
    std::vector<int> v_old_to_new(v_cnt, -1);
    std::vector<int> f_old_to_new(f_cnt, -1);

    for (auto i = 0; i < v_cnt; ++i)
        if (!ll.is_removed(vertex_index(i)))
        {
            v_old_to_new[i] = (int)v_new_to_old.size();
            v_new_to_old.push_back(i);
        }

    for (auto i = 0; i < f_cnt; ++i)
        if (!ll.is_removed(face_index(i)))
        {
            f_old_to_new[i] = (int)f_new_to_old.size();
            f_new_to_old.push_back(i);
        }

    for (auto i = 0; i < e_cnt; ++i)
        if (!ll.is_removed(edge_index(i)))
            e_new_to_old.push_back(i);

    for (auto i = 0; i < h_cnt; ++i)
        if (!ll.is_removed(halfedge_index(i)))
        {
            h_old_to_new[i] = (int)h_new_to_old.size();
            h_new_to_old.push_back(i);
        }

    // apply remappings (map[new_prim_id] = old_prim_id)

    for (auto i = 0u; i < v_new_to_old.size(); ++i)
        mVertexToOutgoingHalfedge[i] = mVertexToOutgoingHalfedge[v_new_to_old[i]];
    for (auto i = 0u; i < f_new_to_old.size(); ++i)
        mFaceToHalfedge[i] = mFaceToHalfedge[f_new_to_old[i]];
    for (auto i = 0u; i < h_new_to_old.size(); ++i)
    {
        mHalfedgeToFace[i] = mHalfedgeToFace[h_new_to_old[i]];
        mHalfedgeToVertex[i] = mHalfedgeToVertex[h_new_to_old[i]];
        mHalfedgeToNextHalfedge[i] = mHalfedgeToNextHalfedge[h_new_to_old[i]];
        mHalfedgeToPrevHalfedge[i] = mHalfedgeToPrevHalfedge[h_new_to_old[i]];
    }

    detail::resize(mVerticesSize, mVerticesCapacity, v_new_to_old.size(), mVertexToOutgoingHalfedge);
    detail::resize(mFacesSize, mFacesCapacity, f_new_to_old.size(), mFaceToHalfedge);
    detail::resize(mHalfedgesSize, mHalfedgesCapacity, h_new_to_old.size(), mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);

    for (auto &v_out : detail::range(mVerticesSize, mVertexToOutgoingHalfedge))
        if (v_out.value >= 0)
            v_out.value = h_old_to_new[v_out.value];

    for (auto &f_h : detail::range(mFacesSize, mFaceToHalfedge))
        if (f_h.value >= 0)
            f_h.value = h_old_to_new[f_h.value];

    for (auto &h_next : detail::range(mHalfedgesSize, mHalfedgeToNextHalfedge))
        if (h_next.value >= 0)
            h_next.value = h_old_to_new[h_next.value];
    for (auto &h_prev : detail::range(mHalfedgesSize, mHalfedgeToPrevHalfedge))
        if (h_prev.value >= 0)
            h_prev.value = h_old_to_new[h_prev.value];
    for (auto &h_f : detail::range(mHalfedgesSize, mHalfedgeToFace))
        if (h_f.value >= 0)
            h_f.value = f_old_to_new[h_f.value];
    for (auto &h_v : detail::range(mHalfedgesSize, mHalfedgeToVertex))
        if (h_v.value >= 0)
            h_v.value = v_old_to_new[h_v.value];

    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->apply_remapping(v_new_to_old);
    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->apply_remapping(f_new_to_old);
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->apply_remapping(e_new_to_old);
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->apply_remapping(h_new_to_old);

    // shrink to fit
    auto old_v_size = mVerticesSize;
    auto old_f_size = mFacesSize;
    auto old_h_size = mHalfedgesSize;

    detail::shrink_to_fit(mVerticesSize, mVerticesCapacity, mVertexToOutgoingHalfedge);
    detail::shrink_to_fit(mFacesSize, mFacesCapacity, mFaceToHalfedge);
    detail::shrink_to_fit(mHalfedgesSize, mHalfedgesCapacity, mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);

    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_v_size);
    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_f_size);
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_h_size >> 1);
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_h_size);

    mRemovedFaces = 0;
    mRemovedHalfedges = 0;
    mRemovedVertices = 0;
    mCompact = true;
}

inline void Mesh::clear()
{
    if (mVerticesCapacity > 0)
    {
        for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
            a->clear_with_default();

        // AFTER clear_with_default
        mVerticesSize = 0;
    }

    if (mFacesCapacity > 0)
    {
        for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
            a->clear_with_default();

        // AFTER clear_with_default
        mFacesSize = 0;
    }

    if (mHalfedgesCapacity > 0)
    {
        for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
            a->clear_with_default();
        for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
            a->clear_with_default();

        // AFTER clear_with_default
        mHalfedgesSize = 0;
    }

    mRemovedFaces = 0;
    mRemovedHalfedges = 0;
    mRemovedVertices = 0;
    mCompact = true;
}

inline void Mesh::shrink_to_fit()
{
    if (mVerticesCapacity > mVerticesSize)
    {
        detail::shrink_to_fit(mVerticesSize, mVerticesCapacity, mVertexToOutgoingHalfedge);

        for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
            a->resize_from(mVerticesSize);
    }

    if (mFacesCapacity > mFacesSize)
    {
        detail::shrink_to_fit(mFacesSize, mFacesCapacity, mFaceToHalfedge);

        for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
            a->resize_from(mFacesSize);
    }

    if (mHalfedgesCapacity > mHalfedgesSize)
    {
        detail::shrink_to_fit(mHalfedgesSize, mHalfedgesCapacity, mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);

        for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
            a->resize_from(mHalfedgesSize >> 1);
        for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
            a->resize_from(mHalfedgesSize);
    }
}

inline void Mesh::reset()
{
    if (mVerticesCapacity > 0)
    {
        detail::clear(mVerticesSize, mVerticesCapacity, mVertexToOutgoingHalfedge);

        for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
            a->resize_from(0);
    }

    if (mFacesCapacity > 0)
    {
        detail::clear(mFacesSize, mFacesCapacity, mFaceToHalfedge);

        for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
            a->resize_from(0);
    }

    if (mHalfedgesCapacity > 0)
    {
        detail::clear(mHalfedgesSize, mHalfedgesCapacity, mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);

        for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
            a->resize_from(0);
        for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
            a->resize_from(0);
    }

    mRemovedFaces = 0;
    mRemovedHalfedges = 0;
    mRemovedVertices = 0;
    mCompact = true;
}

inline void Mesh::copy_from(const Mesh &m)
{
    auto old_v_size = mVerticesSize;
    auto old_f_size = mFacesSize;
    auto old_h_size = mHalfedgesSize;

    // copy topo
    detail::resize(mVerticesSize, mVerticesCapacity, m.mVerticesSize, mVertexToOutgoingHalfedge);
    std::copy_n(m.mVertexToOutgoingHalfedge.get(), m.mVerticesSize, mVertexToOutgoingHalfedge.get());

    detail::resize(mFacesSize, mFacesCapacity, m.mFacesSize, mFaceToHalfedge);
    std::copy_n(m.mFaceToHalfedge.get(), m.mFacesSize, mFaceToHalfedge.get());

    detail::resize(mHalfedgesSize, mHalfedgesCapacity, m.mHalfedgesSize, //
                   mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);
    std::copy_n(m.mHalfedgeToFace.get(), m.mHalfedgesSize, mHalfedgeToFace.get());
    std::copy_n(m.mHalfedgeToVertex.get(), m.mHalfedgesSize, mHalfedgeToVertex.get());
    std::copy_n(m.mHalfedgeToNextHalfedge.get(), m.mHalfedgesSize, mHalfedgeToNextHalfedge.get());
    std::copy_n(m.mHalfedgeToPrevHalfedge.get(), m.mHalfedgesSize, mHalfedgeToPrevHalfedge.get());

    // copy helper data
    mRemovedFaces = m.mRemovedFaces;
    mRemovedHalfedges = m.mRemovedHalfedges;
    mRemovedVertices = m.mRemovedVertices;
    mCompact = m.mCompact;

    // resize attributes
    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_v_size);
    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_f_size);
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_h_size >> 1);
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_h_size);
}

inline SharedMesh Mesh::copy() const
{
    auto m = create();
    m->copy_from(*this);
    return m;
}

inline void Mesh::reserve_faces(int capacity)
{
    if (mFacesCapacity >= capacity)
        return;

    auto old_size = mFacesSize;
    mFacesCapacity = capacity;
    detail::reserve(mFacesSize, mFacesCapacity, mFaceToHalfedge);

    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_size);
}

inline void Mesh::reserve_vertices(int capacity)
{
    if (mVerticesCapacity >= capacity)
        return;

    auto old_size = mVerticesCapacity;
    mVerticesCapacity = capacity;
    detail::reserve(mVerticesSize, mVerticesCapacity, mVertexToOutgoingHalfedge);

    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_size);
}

inline void Mesh::reserve_edges(int capacity) { reserve_halfedges(capacity << 1); }

inline void Mesh::reserve_halfedges(int capacity)
{
    if (mHalfedgesCapacity >= capacity)
        return;

    auto old_size = mHalfedgesSize;
    mHalfedgesCapacity = capacity;
    detail::reserve(mHalfedgesSize, mHalfedgesCapacity, mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);

    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_size);
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_size >> 1);
}
} // namespace polymesh
