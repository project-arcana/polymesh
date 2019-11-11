#include "Mesh.hh"

#include <map>
#include <set>

#include "assert.hh"
#include "debug.hh"

#include "detail/permutation.hh"
#include "detail/split_vector.hh"

using namespace polymesh;


void Mesh::reserve_faces(int capacity)
{
    if (mFacesCapacity >= capacity)
        return;

    auto old_size = mFacesSize;
    mFacesCapacity = capacity;
    detail::reserve(mFacesSize, mFacesCapacity, mFaceToHalfedge);

    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_size);
}

void Mesh::reserve_vertices(int capacity)
{
    if (mVerticesCapacity >= capacity)
        return;

    auto old_size = mVerticesCapacity;
    mVerticesCapacity = capacity;
    detail::reserve(mVerticesSize, mVerticesCapacity, mVertexToOutgoingHalfedge);

    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->resize_from(old_size);
}

void Mesh::reserve_edges(int capacity) { reserve_halfedges(capacity << 1); }

void Mesh::reserve_halfedges(int capacity)
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

void Mesh::alloc_primitives(int vertices, int faces, int halfedges)
{
    POLYMESH_ASSERT(vertices >= 0 && faces >= 0 && halfedges >= 0);

    POLYMESH_ASSERT(size_all_vertices() + int64_t(vertices) <= int64_t(std::numeric_limits<int>::max()) && "polymesh only supports 2^31 primitives");
    POLYMESH_ASSERT(size_all_faces() + int64_t(faces) <= int64_t(std::numeric_limits<int>::max()) && "polymesh only supports 2^31 primitives");
    POLYMESH_ASSERT(size_all_halfedges() + int64_t(halfedges) <= int64_t(std::numeric_limits<int>::max()) && "polymesh only supports 2^31 primitives");

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


void Mesh::permute_vertices(std::vector<int> const& p)
{
    POLYMESH_ASSERT(detail::is_valid_permutation(p));
    POLYMESH_ASSERT(int(p.size()) == mVerticesSize);

    // calculate transpositions
    auto ts = detail::transpositions_of(p);

    // apply them
    for (auto t : ts)
        std::swap(mVertexToOutgoingHalfedge[t.first], mVertexToOutgoingHalfedge[t.second]);

    // fix half-edges
    for (auto& h_to : detail::range(mHalfedgesSize, mHalfedgeToVertex))
        if (h_to.is_valid())
            h_to.value = p[h_to.value];

    // update attributes
    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->apply_transpositions(ts);
}

void Mesh::permute_faces(std::vector<int> const& p)
{
    POLYMESH_ASSERT(detail::is_valid_permutation(p));
    POLYMESH_ASSERT(int(p.size()) == mFacesSize);

    // calculate transpositions
    auto ts = detail::transpositions_of(p);

    // apply them
    for (auto t : ts)
        std::swap(mFaceToHalfedge[t.first], mFaceToHalfedge[t.second]);

    // fix half-edges
    for (auto& h_f : detail::range(mHalfedgesSize, mHalfedgeToFace))
        if (h_f.is_valid())
            h_f.value = p[h_f.value];

    // update attributes
    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->apply_transpositions(ts);
}

void Mesh::permute_edges(std::vector<int> const& p)
{
    POLYMESH_ASSERT(detail::is_valid_permutation(p));
    POLYMESH_ASSERT(int(p.size() * 2) == mHalfedgesSize);

    std::vector<int> hp(p.size() * 2);
    for (auto i = 0u; i < p.size(); ++i)
    {
        hp[i * 2 + 0] = p[i] * 2 + 0;
        hp[i * 2 + 1] = p[i] * 2 + 1;
    }
    POLYMESH_ASSERT(detail::is_valid_permutation(hp));

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
    for (auto& v_out : detail::range(mVerticesSize, mVertexToOutgoingHalfedge))
        if (v_out.value >= 0)
            v_out.value = hp[v_out.value];

    for (auto& f_h : detail::range(mFacesSize, mFaceToHalfedge))
        if (f_h.value >= 0)
            f_h.value = hp[f_h.value];

    for (auto& h_next : detail::range(mHalfedgesSize, mHalfedgeToNextHalfedge))
        if (h_next.value >= 0)
            h_next.value = hp[h_next.value];

    for (auto& h_prev : detail::range(mHalfedgesSize, mHalfedgeToPrevHalfedge))
        if (h_prev.value >= 0)
            h_prev.value = hp[h_prev.value];

    // update attributes
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->apply_transpositions(edge_ts);
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->apply_transpositions(halfedge_ts);
}

void Mesh::compactify()
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

    detail::resize(mVerticesSize, mVerticesCapacity, int(v_new_to_old.size()), mVertexToOutgoingHalfedge);
    detail::resize(mFacesSize, mFacesCapacity, int(f_new_to_old.size()), mFaceToHalfedge);
    detail::resize(mHalfedgesSize, mHalfedgesCapacity, int(h_new_to_old.size()), mHalfedgeToFace, mHalfedgeToVertex, mHalfedgeToNextHalfedge, mHalfedgeToPrevHalfedge);

    for (auto& v_out : detail::range(mVerticesSize, mVertexToOutgoingHalfedge))
        if (v_out.value >= 0)
            v_out.value = h_old_to_new[v_out.value];

    for (auto& f_h : detail::range(mFacesSize, mFaceToHalfedge))
        if (f_h.value >= 0)
            f_h.value = h_old_to_new[f_h.value];

    for (auto& h_next : detail::range(mHalfedgesSize, mHalfedgeToNextHalfedge))
        if (h_next.value >= 0)
            h_next.value = h_old_to_new[h_next.value];
    for (auto& h_prev : detail::range(mHalfedgesSize, mHalfedgeToPrevHalfedge))
        if (h_prev.value >= 0)
            h_prev.value = h_old_to_new[h_prev.value];
    for (auto& h_f : detail::range(mHalfedgesSize, mHalfedgeToFace))
        if (h_f.value >= 0)
            h_f.value = f_old_to_new[h_f.value];
    for (auto& h_v : detail::range(mHalfedgesSize, mHalfedgeToVertex))
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

void Mesh::clear()
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

void Mesh::shrink_to_fit()
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

void Mesh::reset()
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

void Mesh::copy_from(const Mesh& m)
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

void Mesh::assert_consistency() const
{
    // check sizes
    POLYMESH_ASSERT(mHalfedgesSize % 2 == 0); ///< even number of halfedges

    // check correct counts
    {
        auto vertex_cnt = 0;
        auto face_cnt = 0;
        auto edge_cnt = 0;
        auto halfedge_cnt = 0;

        auto valid_vertex_cnt = 0;
        auto valid_face_cnt = 0;
        auto valid_edge_cnt = 0;
        auto valid_halfedge_cnt = 0;

        auto invalid_vertex_cnt = 0;
        auto invalid_face_cnt = 0;
        auto invalid_edge_cnt = 0;
        auto invalid_halfedge_cnt = 0;

        for (auto h : all_vertices())
        {
            ++vertex_cnt;
            if (h.is_removed())
                ++invalid_vertex_cnt;
        }

        for (auto h : all_faces())
        {
            ++face_cnt;
            if (h.is_removed())
                ++invalid_face_cnt;
        }

        for (auto h : all_edges())
        {
            ++edge_cnt;
            if (h.is_removed())
                ++invalid_edge_cnt;
        }

        for (auto h : all_halfedges())
        {
            ++halfedge_cnt;
            if (h.is_removed())
                ++invalid_halfedge_cnt;
        }

        for (auto h : vertices())
        {
            POLYMESH_ASSERT(h.is_valid());
            POLYMESH_ASSERT(!h.is_removed());
            ++valid_vertex_cnt;
        }
        for (auto h : faces())
        {
            POLYMESH_ASSERT(h.is_valid());
            POLYMESH_ASSERT(!h.is_removed());
            ++valid_face_cnt;
        }
        for (auto h : edges())
        {
            POLYMESH_ASSERT(h.is_valid());
            POLYMESH_ASSERT(!h.is_removed());
            ++valid_edge_cnt;
        }
        for (auto h : halfedges())
        {
            POLYMESH_ASSERT(h.is_valid());
            POLYMESH_ASSERT(!h.is_removed());
            ++valid_halfedge_cnt;
        }

        POLYMESH_ASSERT(vertex_cnt == all_vertices().size());
        POLYMESH_ASSERT(face_cnt == all_faces().size());
        POLYMESH_ASSERT(edge_cnt == all_edges().size());
        POLYMESH_ASSERT(halfedge_cnt == all_halfedges().size());

        POLYMESH_ASSERT(valid_vertex_cnt == vertices().size());
        POLYMESH_ASSERT(valid_face_cnt == faces().size());
        POLYMESH_ASSERT(valid_edge_cnt == edges().size());
        POLYMESH_ASSERT(valid_halfedge_cnt == halfedges().size());

        POLYMESH_ASSERT(vertex_cnt == valid_vertex_cnt + invalid_vertex_cnt);
        POLYMESH_ASSERT(face_cnt == valid_face_cnt + invalid_face_cnt);
        POLYMESH_ASSERT(edge_cnt == valid_edge_cnt + invalid_edge_cnt);
        POLYMESH_ASSERT(halfedge_cnt == valid_halfedge_cnt + invalid_halfedge_cnt);

        POLYMESH_ASSERT(mRemovedFaces == invalid_face_cnt);
        POLYMESH_ASSERT(mRemovedVertices == invalid_vertex_cnt);
        POLYMESH_ASSERT(mRemovedHalfedges == invalid_halfedge_cnt);
        POLYMESH_ASSERT(invalid_edge_cnt * 2 == invalid_halfedge_cnt);
        POLYMESH_ASSERT(valid_edge_cnt * 2 == valid_halfedge_cnt);
        POLYMESH_ASSERT(edge_cnt * 2 == halfedge_cnt);
    }

    // check validity
    for (auto f : faces())
    {
        POLYMESH_ASSERT(f.any_halfedge().is_valid());
    }
    for (auto f : halfedges())
    {
        POLYMESH_ASSERT(f.vertex_to().is_valid());
    }

    // check only non-removed can be accessed topologically
    for (auto f : faces())
    {
        POLYMESH_ASSERT(!f.any_halfedge().is_removed());
        POLYMESH_ASSERT(!f.any_vertex().is_removed());

        for (auto v : f.vertices())
            POLYMESH_ASSERT(!v.is_removed());

        for (auto h : f.halfedges())
            POLYMESH_ASSERT(!h.is_removed());

        for (auto f : f.adjacent_faces())
            POLYMESH_ASSERT(!f.is_removed());

        for (auto f : f.edges())
            POLYMESH_ASSERT(!f.is_removed());
    }
    for (auto v : vertices())
    {
        POLYMESH_ASSERT(!v.any_face().is_removed());
        POLYMESH_ASSERT(!v.any_edge().is_removed());
        POLYMESH_ASSERT(!v.any_incoming_halfedge().is_removed());
        POLYMESH_ASSERT(!v.any_outgoing_halfedge().is_removed());

        for (auto v : v.adjacent_vertices())
            POLYMESH_ASSERT(!v.is_removed());

        for (auto h : v.incoming_halfedges())
            POLYMESH_ASSERT(!h.is_removed());

        for (auto h : v.outgoing_halfedges())
            POLYMESH_ASSERT(!h.is_removed());

        for (auto f : v.faces())
            POLYMESH_ASSERT(!f.is_removed());

        for (auto f : v.edges())
            POLYMESH_ASSERT(!f.is_removed());
    }
    for (auto e : edges())
    {
        POLYMESH_ASSERT(!e.faceA().is_removed());
        POLYMESH_ASSERT(!e.faceB().is_removed());

        POLYMESH_ASSERT(!e.vertexA().is_removed());
        POLYMESH_ASSERT(!e.vertexB().is_removed());

        POLYMESH_ASSERT(!e.halfedgeA().is_removed());
        POLYMESH_ASSERT(!e.halfedgeB().is_removed());
    }
    for (auto h : halfedges())
    {
        POLYMESH_ASSERT(!h.prev().is_removed());
        POLYMESH_ASSERT(!h.next().is_removed());
        POLYMESH_ASSERT(!h.edge().is_removed());
        POLYMESH_ASSERT(!h.vertex_from().is_removed());
        POLYMESH_ASSERT(!h.vertex_to().is_removed());
        POLYMESH_ASSERT(!h.face().is_removed());
        POLYMESH_ASSERT(!h.opposite().is_removed());
        POLYMESH_ASSERT(!h.opposite_face().is_removed());
    }

    // check half-edge consistencies
    for (auto h : halfedges())
    {
        POLYMESH_ASSERT(h.next().is_valid());
        POLYMESH_ASSERT(h.prev().is_valid());
        POLYMESH_ASSERT(h.opposite().is_valid());
        POLYMESH_ASSERT(h.vertex_to().is_valid());
        POLYMESH_ASSERT(h.vertex_from().is_valid());
        // face can be invalid

        POLYMESH_ASSERT(h.next().prev() == h);
        POLYMESH_ASSERT(h.prev().next() == h);
        POLYMESH_ASSERT(h.opposite().opposite() == h);

        if (!h.is_boundary())
            POLYMESH_ASSERT(h.face().halfedges().contains(h));
        POLYMESH_ASSERT(h.vertex_to().incoming_halfedges().contains(h));
        POLYMESH_ASSERT(h.vertex_from().outgoing_halfedges().contains(h));

        POLYMESH_ASSERT(h.edge().halfedgeA() == h || h.edge().halfedgeB() == h);

        POLYMESH_ASSERT(h.next().vertex_from() == h.vertex_to());
        POLYMESH_ASSERT(h.prev().vertex_to() == h.vertex_from());

        auto ref_face = h.face();
        if (ref_face.is_valid())
            for (auto h : h.ring())
                POLYMESH_ASSERT(h.face() == ref_face);
    }

    // check vertex consistencies
    for (auto v : vertices())
    {
        if (!v.is_isolated())
        {
            POLYMESH_ASSERT(v.any_incoming_halfedge().is_valid());
            POLYMESH_ASSERT(v.any_outgoing_halfedge().is_valid());
            // POLYMESH_ASSERT(v.any_valid_face().is_valid()); -> wiremeshes are non-isolated but have no faces
            POLYMESH_ASSERT(v.any_edge().is_valid());

            POLYMESH_ASSERT(v.any_incoming_halfedge().vertex_to() == v);
            POLYMESH_ASSERT(v.any_outgoing_halfedge().vertex_from() == v);

            for (auto f : v.faces())
            {
                POLYMESH_ASSERT(f.is_valid());
                POLYMESH_ASSERT(f.vertices().contains(v));
            }

            for (auto h : v.outgoing_halfedges())
                POLYMESH_ASSERT(h.vertex_from() == v);

            for (auto h : v.incoming_halfedges())
                POLYMESH_ASSERT(h.vertex_to() == v);

            for (auto vv : v.adjacent_vertices())
                POLYMESH_ASSERT(vv.adjacent_vertices().contains(v));

            for (auto e : v.edges())
                POLYMESH_ASSERT(e.vertexA() == v || e.vertexB() == v);
        }
        else
        {
            POLYMESH_ASSERT(v.any_face().is_invalid());
            POLYMESH_ASSERT(v.any_valid_face().is_invalid());
            POLYMESH_ASSERT(v.any_incoming_halfedge().is_invalid());
            POLYMESH_ASSERT(v.any_outgoing_halfedge().is_invalid());
            POLYMESH_ASSERT(v.any_edge().is_invalid());

            POLYMESH_ASSERT(v.faces().size() == 0);
            POLYMESH_ASSERT(v.edges().size() == 0);
            POLYMESH_ASSERT(v.adjacent_vertices().size() == 0);
            POLYMESH_ASSERT(v.outgoing_halfedges().size() == 0);
            POLYMESH_ASSERT(v.incoming_halfedges().size() == 0);
        }
    }

    // check face consistencies
    for (auto f : faces())
    {
        POLYMESH_ASSERT(f.any_halfedge().is_valid());
        POLYMESH_ASSERT(f.any_vertex().is_valid());

        POLYMESH_ASSERT(f.any_halfedge().face() == f);
        POLYMESH_ASSERT(f.any_vertex().faces().contains(f));

        for (auto h : f.halfedges())
            POLYMESH_ASSERT(h.face() == f);

        for (auto v : f.vertices())
            POLYMESH_ASSERT(v.faces().contains(f));

        for (auto ff : f.adjacent_faces())
        {
            POLYMESH_ASSERT(ff.is_valid());
            POLYMESH_ASSERT(ff.adjacent_faces().contains(f));
        }

        for (auto e : f.edges())
            POLYMESH_ASSERT(e.faceA() == f || e.faceB() == f);
    }

    // check edge consistencies
    for (auto e : edges())
    {
        POLYMESH_ASSERT(e.vertexA().is_valid());
        POLYMESH_ASSERT(e.vertexB().is_valid());
        POLYMESH_ASSERT(e.halfedgeA().is_valid());
        POLYMESH_ASSERT(e.halfedgeB().is_valid());
        // faces can be invalid

        POLYMESH_ASSERT(e.faceA().is_invalid() || e.faceA().edges().contains(e));
        POLYMESH_ASSERT(e.faceB().is_invalid() || e.faceB().edges().contains(e));

        POLYMESH_ASSERT(e.vertexA().edges().contains(e));
        POLYMESH_ASSERT(e.vertexB().edges().contains(e));

        POLYMESH_ASSERT(e.halfedgeA().edge() == e);
        POLYMESH_ASSERT(e.halfedgeB().edge() == e);
    }

    // check boundaries
    for (auto h : halfedges())
        if (h.is_boundary())
        {
            POLYMESH_ASSERT(h.face().is_invalid());
            POLYMESH_ASSERT(h.edge().is_boundary());

            if (h.opposite().is_boundary())
            {
                POLYMESH_ASSERT(h.edge().is_isolated());
                POLYMESH_ASSERT(h.opposite_face().is_invalid());
            }
            else
            {
                POLYMESH_ASSERT(h.opposite_face().is_boundary());
            }

            POLYMESH_ASSERT(h.vertex_to().is_boundary());
            POLYMESH_ASSERT(h.vertex_from().is_boundary());
        }

    // check derived counts
    {
        auto v_e_sum = 0;
        auto f_h_sum = 0;

        for (auto v : vertices())
        {
            v_e_sum += v.edges().size();
        }
        for (auto f : faces())
        {
            f_h_sum += f.halfedges().size();
        }

        POLYMESH_ASSERT(v_e_sum == 2 * size_valid_edges());
        // WRONG: POLYMESH_ASSERT(f_h_sum == size_valid_halfedges());

        // TODO: more?
    }

    // compactness
    if (is_compact())
    {
        for (auto v : all_vertices())
        {
            POLYMESH_ASSERT(v.is_valid());
            POLYMESH_ASSERT(!v.is_removed());
        }

        for (auto f : all_faces())
        {
            POLYMESH_ASSERT(f.is_valid());
            POLYMESH_ASSERT(!f.is_removed());
        }

        for (auto e : all_edges())
        {
            POLYMESH_ASSERT(e.is_valid());
            POLYMESH_ASSERT(!e.is_removed());
        }

        for (auto h : all_halfedges())
        {
            POLYMESH_ASSERT(h.is_valid());
            POLYMESH_ASSERT(!h.is_removed());
        }
    }

    // check half-edge uniqueness
    std::map<int, std::set<int>> hes;
    for (auto h : halfedges())
    {
        auto v0 = h.vertex_from().idx.value;
        auto v1 = h.vertex_to().idx.value;

        if (!hes[v0].insert(v1).second)
            POLYMESH_ASSERT(false && "duplicated half-edge");
    }
}
