#pragma once

#include <polymesh/Mesh.hh>
#include "../assert.hh"
#include "../detail/split_vector.hh"

namespace polymesh
{
inline face_index& Mesh::face_of(halfedge_index idx)
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mHalfedgesSize && "out of bounds");
    return mHalfedgeToFace[(int)idx];
}
inline vertex_index& Mesh::to_vertex_of(halfedge_index idx)
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mHalfedgesSize && "out of bounds");
    return mHalfedgeToVertex[(int)idx];
}
inline halfedge_index& Mesh::next_halfedge_of(halfedge_index idx)
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mHalfedgesSize && "out of bounds");
    return mHalfedgeToNextHalfedge[(int)idx];
}
inline halfedge_index& Mesh::prev_halfedge_of(halfedge_index idx)
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mHalfedgesSize && "out of bounds");
    return mHalfedgeToPrevHalfedge[(int)idx];
}
inline halfedge_index& Mesh::halfedge_of(face_index idx)
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mFacesSize && "out of bounds");
    return mFaceToHalfedge[(int)idx];
}
inline halfedge_index& Mesh::outgoing_halfedge_of(vertex_index idx)
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mVerticesSize && "out of bounds");
    return mVertexToOutgoingHalfedge[(int)idx];
}

inline face_index const& Mesh::face_of(halfedge_index idx) const
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mHalfedgesSize && "out of bounds");
    return mHalfedgeToFace[(int)idx];
}
inline vertex_index const& Mesh::to_vertex_of(halfedge_index idx) const
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mHalfedgesSize && "out of bounds");
    return mHalfedgeToVertex[(int)idx];
}
inline halfedge_index const& Mesh::next_halfedge_of(halfedge_index idx) const
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mHalfedgesSize && "out of bounds");
    return mHalfedgeToNextHalfedge[(int)idx];
}
inline halfedge_index const& Mesh::prev_halfedge_of(halfedge_index idx) const
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mHalfedgesSize && "out of bounds");
    return mHalfedgeToPrevHalfedge[(int)idx];
}
inline halfedge_index const& Mesh::halfedge_of(face_index idx) const
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mFacesSize && "out of bounds");
    return mFaceToHalfedge[(int)idx];
}
inline halfedge_index const& Mesh::outgoing_halfedge_of(vertex_index idx) const
{
    POLYMESH_ASSERT(0 <= idx.value && idx.value < mVerticesSize && "out of bounds");
    return mVertexToOutgoingHalfedge[(int)idx];
}

inline vertex_index Mesh::alloc_vertex()
{
    auto idx = vertex_index(size_all_vertices());

    auto old_size = mVerticesSize;
    auto capacity_changed = detail::alloc_back(mVerticesSize, mVerticesCapacity, mVertexToOutgoingHalfedge);
    mVertexToOutgoingHalfedge[mVerticesSize - 1] = halfedge_index::invalid;

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
    mFaceToHalfedge[mFacesSize - 1] = halfedge_index::invalid;

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
        mHalfedgeToFace[mHalfedgesSize - 1] = face_index::invalid;
        mHalfedgeToVertex[mHalfedgesSize - 1] = vertex_index::invalid;
        mHalfedgeToNextHalfedge[mHalfedgesSize - 1] = halfedge_index::invalid;
        mHalfedgeToPrevHalfedge[mHalfedgesSize - 1] = halfedge_index::invalid;
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

inline unique_ptr<Mesh> Mesh::copy() const
{
    auto m = create();
    m->copy_from(*this);
    return m;
}
} // namespace polymesh
