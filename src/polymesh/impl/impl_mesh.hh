#pragma once

#include "../Mesh.hh"

#include "../detail/permutation.hh"

namespace polymesh
{
inline vertex_index Mesh::add_vertex() { return alloc_vertex(); }

inline vertex_index Mesh::alloc_vertex()
{
    auto idx = vertex_index(size_all_vertices());
    mVertexToOutgoingHalfedge.push_back(halfedge_index::invalid());

    // notify attributes
    auto vCnt = size_all_vertices();
    for (auto p = mVertexAttrs; p; p = p->mNextAttribute)
        p->resize(vCnt, false);

    return idx;
}

inline face_index Mesh::alloc_face()
{
    auto idx = face_index(size_all_faces());
    mFaceToHalfedge.push_back(halfedge_index::invalid());

    // notify attributes
    auto fCnt = size_all_faces();
    for (auto p = mFaceAttrs; p; p = p->mNextAttribute)
        p->resize(fCnt, false);

    return idx;
}

inline edge_index Mesh::alloc_edge()
{
    auto idx = edge_index(size_all_edges());

    mHalfedgeToFace.push_back(face_index::invalid());
    mHalfedgeToFace.push_back(face_index::invalid());
    mHalfedgeToVertex.push_back(vertex_index::invalid());
    mHalfedgeToVertex.push_back(vertex_index::invalid());
    mHalfedgeToNextHalfedge.push_back(halfedge_index::invalid());
    mHalfedgeToNextHalfedge.push_back(halfedge_index::invalid());
    mHalfedgeToPrevHalfedge.push_back(halfedge_index::invalid());
    mHalfedgeToPrevHalfedge.push_back(halfedge_index::invalid());

    // notify attributes
    auto hCnt = size_all_halfedges();
    auto eCnt = size_all_edges();
    for (auto p = mEdgeAttrs; p; p = p->mNextAttribute)
        p->resize(eCnt, false);
    for (auto p = mHalfedgeAttrs; p; p = p->mNextAttribute)
        p->resize(hCnt, false);

    return idx;
}

inline face_index Mesh::add_face(const vertex_handle *v_handles, int vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0; i < vcnt; ++i)
        mFaceInsertCache[i] = add_or_get_halfedge(v_handles[i].idx, v_handles[(i + 1) % vcnt].idx);
    return add_face(mFaceInsertCache.data(), vcnt);
}

inline face_index Mesh::add_face(const vertex_index *v_indices, int vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0; i < vcnt; ++i)
        mFaceInsertCache[i] = add_or_get_halfedge(v_indices[i], v_indices[(i + 1) % vcnt]);
    return add_face(mFaceInsertCache.data(), vcnt);
}

inline face_index Mesh::add_face(const halfedge_handle *half_loop, int vcnt)
{
    mFaceInsertCache.resize(vcnt);
    for (auto i = 0; i < vcnt; ++i)
        mFaceInsertCache[i] = half_loop[i].idx;
    return add_face(mFaceInsertCache.data(), vcnt);
}

inline face_index Mesh::add_face(const halfedge_index *half_loop, int vcnt)
{
    assert(vcnt >= 3 && "no support for less-than-triangular faces");

    auto fidx = alloc_face();

    // ensure that half-edges are adjacent at each vertex
    for (auto i = 0; i < vcnt; ++i)
    {
        auto h0 = half_loop[i];
        auto h1 = half_loop[(i + 1) % vcnt];

        // half-edge must form a chain
        assert(to_vertex_of(h0) == from_vertex_of(h1) && "half-edges do not form a chain");
        // half-edge must be free, i.e. allow a new polygon
        assert(is_free(h0) && "half-edge already contains a face");

        // make them adjacent
        make_adjacent(h0, h1);

        // link face
        face_of(h0) = fidx;
    }

    // fix boundary states
    for (auto i = 0; i < vcnt; ++i)
    {
        auto h = half_loop[i];
        auto v = to_vertex_of(h);
        auto f = face_of(opposite(h));

        // fix vertex
        fix_boundary_state_of(v);

        // fix face
        if (f.is_valid())
            fix_boundary_state_of(f);
    }

    // set up face data
    halfedge_of(fidx) = half_loop[0];

    // fix new face
    fix_boundary_state_of(fidx);

    return fidx;
}

inline edge_index Mesh::add_or_get_edge(vertex_index v_from, vertex_index v_to)
{
    assert(v_from != v_to);

    // already exists?
    auto he = find_halfedge(v_from, v_to);
    if (he.is_valid())
        return edge_of(he);

    auto &vd_from = vertex(v_from);
    auto &vd_to = vertex(v_to);

    // allocate new
    auto he_size = (int)mHalfedges.size();
    auto h_from_to_idx = halfedge_index(he_size + 0);
    auto h_to_from_idx = halfedge_index(he_size + 1);
    auto eidx = edge_index(he_size >> 1);
    halfedge_info h_from_to;
    halfedge_info h_to_from;

    // setup data (self-connected edge)
    h_from_to.to_vertex = v_to;
    h_to_from.to_vertex = v_from;
    h_from_to.next_halfedge = h_to_from_idx;
    h_to_from.next_halfedge = h_from_to_idx;

    // link from vertex
    if (vd_from.is_isolated())
        vd_from.outgoing_halfedge = h_from_to_idx;
    else
    {
        auto from_in_idx = find_free_incident(v_from);
        assert(from_in_idx.is_valid() && "vertex is already fully connected");
        auto &from_in = halfedge(from_in_idx);
        auto from_out_idx = from_in.next_halfedge;
        auto &from_out = halfedge(from_out_idx);

        from_in.next_halfedge = h_from_to_idx;
        h_from_to.prev_halfedge = from_in_idx;

        h_to_from.next_halfedge = from_out_idx;
        from_out.prev_halfedge = h_to_from_idx;
    }

    // link to vertex
    if (vd_to.is_isolated())
        vd_to.outgoing_halfedge = h_to_from_idx;
    else
    {
        auto to_in_idx = find_free_incident(v_to);
        assert(to_in_idx.is_valid() && "vertex is already fully connected");
        auto &to_in = halfedge(to_in_idx);
        auto to_out_idx = to_in.next_halfedge;
        auto &to_out = halfedge(to_out_idx);

        to_in.next_halfedge = h_to_from_idx;
        h_to_from.prev_halfedge = to_in_idx;

        h_from_to.next_halfedge = to_out_idx;
        to_out.prev_halfedge = h_from_to_idx;
    }

    // finalize
    mHalfedges.push_back(h_from_to);
    mHalfedges.push_back(h_to_from);

    // notify attributes
    auto hCnt = (int)mHalfedges.size();
    auto eCnt = hCnt >> 1;
    for (auto p = mHalfedgeAttrs; p; p = p->mNextAttribute)
        p->resize(hCnt, false);
    for (auto p = mEdgeAttrs; p; p = p->mNextAttribute)
        p->resize(eCnt, false);

    return eidx;
}

inline halfedge_index Mesh::add_or_get_halfedge(vertex_index v_from, vertex_index v_to)
{
    auto e = add_or_get_edge(v_from, v_to);
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    return halfedge(h0).to_vertex == v_to ? h0 : h1;
}

inline edge_index Mesh::add_or_get_edge(halfedge_index h_from, halfedge_index h_to)
{
    assert(h_from != h_to);
    auto &hd_from = halfedge(h_from);
    auto &hd_to = halfedge(h_to);

    auto v_from = hd_from.to_vertex;
    auto v_to = hd_to.to_vertex;

    auto ex_he = find_halfedge(v_from, v_to);
    if (ex_he.is_valid())
    {
        auto &existing_hed = halfedge(ex_he);
        auto &ex_opp_hed = halfedge(opposite(ex_he));
        assert(existing_hed.prev_halfedge == h_from && ex_opp_hed.prev_halfedge == h_to);
        (void)existing_hed;
        (void)ex_opp_hed;
        // TODO: Maybe try rewriting an existing halfedge that does NOT yet have the right connection.
        return edge_of(ex_he);
    }

    assert(hd_from.face.is_invalid() && hd_to.face.is_invalid() && "Cannot insert into a face");

    // allocate new
    auto he_size = (int)mHalfedges.size();
    auto h_from_to_idx = halfedge_index(he_size + 0);
    auto h_to_from_idx = halfedge_index(he_size + 1);
    auto eidx = edge_index(he_size >> 1);
    halfedge_info h_from_to;
    halfedge_info h_to_from;

    // setup data (self-connected edge)
    h_from_to.to_vertex = v_to;
    h_to_from.to_vertex = v_from;

    // Link from side
    auto h_from_next_idx = hd_from.next_halfedge;
    auto &h_from_next = halfedge(h_from_next_idx);

    hd_from.next_halfedge = h_from_to_idx;
    h_from_to.prev_halfedge = h_from;

    h_from_next.prev_halfedge = h_to_from_idx;
    h_to_from.next_halfedge = h_from_next_idx;

    // Link to side
    auto h_to_next_idx = hd_to.next_halfedge;
    auto &h_to_next = halfedge(h_to_next_idx);

    hd_to.next_halfedge = h_to_from_idx;
    h_to_from.prev_halfedge = h_to;

    h_to_next.prev_halfedge = h_from_to_idx;
    h_from_to.next_halfedge = h_to_next_idx;

    // finalize
    mHalfedges.push_back(h_from_to);
    mHalfedges.push_back(h_to_from);

    // notify attributes
    auto hCnt = (int)mHalfedges.size();
    auto eCnt = hCnt >> 1;
    for (auto p = mHalfedgeAttrs; p; p = p->mNextAttribute)
        p->resize(hCnt, false);
    for (auto p = mEdgeAttrs; p; p = p->mNextAttribute)
        p->resize(eCnt, false);

    return eidx;
}

inline halfedge_index Mesh::add_or_get_halfedge(halfedge_index h_from, halfedge_index h_to)
{
    auto e = add_or_get_edge(h_from, h_to);
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    return next_halfedge_of(h_from) == h0 ? h0 : h1;
}

inline void Mesh::make_adjacent(halfedge_index he_in, halfedge_index he_out)
{
    // see http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm ::makeAdjacent

    auto he_b = next_halfedge_of(he_in);
    auto he_d = prev_halfedge_of(he_out);

    // already correct
    if (he_b == he_out)
        return;

    // find free half-edge after `out` but before `in`
    auto he_g = find_free_incident(opposite(he_out), he_in);
    assert(he_g.is_valid()); // unable to make adjacent

    auto he_h = next_halfedge_of(he_g);

    // properly rewire
    connect_prev_next(he_in, he_out);
    connect_prev_next(he_g, he_b);
    connect_prev_next(he_d, he_h);
}

inline void Mesh::remove_face(face_index f_idx)
{
    assert(!is_removed(f_idx));

    auto he_begin = halfedge_of(f_idx);
    auto he = he_begin;
    do
    {
        assert(face_of(h) == f_idx);

        // set half-edge face to invalid
        face_of(he) = face_index::invalid();

        // fix outgoing vertex half-edge
        // (vertex correctly reports is_boundary)
        outgoing_halfedge_of(from_vertex_of(he)) = he;

        // fix opposite face half-edge
        auto ohe = opposite(he);
        auto of = face_of(ohe);
        if (of.is_valid())
            halfedge_of(of) = ohe;

        // advance
        he = next_halfedge_of(he);
    } while (he != he_begin);

    // mark removed
    // (at the end!)
    set_removed(f_idx);

    // bookkeeping
    mRemovedFaces++;
    mCompact = false;
}

inline void Mesh::remove_edge(edge_index e_idx)
{
    auto hi_in = halfedge_of(e_idx, 0);
    auto hi_out = halfedge_of(e_idx, 1);

    auto &h_in = halfedge(hi_in);
    auto &h_out = halfedge(hi_out);

    assert(h_in.is_valid());
    assert(h_out.is_valid());

    auto f0 = h_in.face;
    auto f1 = h_out.face;

    // remove adjacent faces
    if (f0.is_valid())
        remove_face(f0);
    if (f1.is_valid())
        remove_face(f1);

    // rewire vertices
    auto &v_in_to = vertex(h_in.to_vertex);
    auto &v_out_to = vertex(h_out.to_vertex);

    auto hi_out_prev = h_out.prev_halfedge;
    auto hi_out_next = h_out.next_halfedge;

    auto hi_in_prev = h_in.prev_halfedge;
    auto hi_in_next = h_in.next_halfedge;

    // modify vertex if outgoing half-edge is going to be removed
    if (v_in_to.outgoing_halfedge == hi_out)
    {
        if (hi_in_next == hi_out) // v_in_to becomes isolated
            v_in_to.outgoing_halfedge = halfedge_index::invalid();
        else
            v_in_to.outgoing_halfedge = hi_in_next;
    }

    if (v_out_to.outgoing_halfedge == hi_in)
    {
        if (hi_out_next == hi_in) // v_out_to becomes isolated
            v_out_to.outgoing_halfedge = halfedge_index::invalid();
        else
            v_out_to.outgoing_halfedge = hi_out_next;
    }

    // reqire half-edges
    halfedge(hi_out_prev).next_halfedge = hi_in_next;
    halfedge(hi_in_next).prev_halfedge = hi_out_prev;

    halfedge(hi_in_prev).next_halfedge = hi_out_next;
    halfedge(hi_out_next).prev_halfedge = hi_in_prev;

    // remove half-edges
    h_in.set_removed();
    h_out.set_removed();

    // bookkeeping
    mRemovedHalfedges++;
    mRemovedHalfedges++;
    mCompact = false;
}

inline void Mesh::remove_vertex(vertex_index v_idx)
{
    assert(!is_removed(v_idx));

    // remove all outgoing edges
    while (!is_isolated(v_idx))
        remove_edge(edge_of(outgoing_halfedge_of(v_idx)));

    // mark removed
    set_removed(v_idx);

    // bookkeeping
    mRemovedVertices++;
    mCompact = false;
}

inline void Mesh::fix_boundary_state_of(vertex_index v_idx)
{
    assert(!is_isolated(v_idx));

    auto he_begin = outgoing_halfedge_of(v_idx);
    auto he = he_begin;
    do
    {
        // if half-edge is boundary, set it
        if (is_free(he))
        {
            outgoing_halfedge_of(v_idx) = he;
            return;
        }

        // advance
        he = next_halfedge_of(opposite(he));
    } while (he != he_begin);
}

inline void Mesh::fix_boundary_state_of(face_index f_idx)
{
    auto he_begin = halfedge_of(f_idx);
    auto he = he_begin;
    do
    {
        // if half-edge is boundary, set it
        if (is_free(opposite(he)))
        {
            halfedge_of(f_idx) = he;
            return;
        }

        // advance
        he = next_halfedge_of(he);
    } while (he != he_begin);
}

inline void Mesh::fix_boundary_state_of_vertices(face_index f_idx)
{
    auto he_begin = halfedge_of(f_idx);
    auto he = he_begin;
    do
    {
        // fix vertex
        fix_boundary_state_of(to_vertex_of(he));

        // advance
        he = next_halfedge_of(he);
    } while (he != he_begin);
}

inline halfedge_index Mesh::find_free_incident(halfedge_index in_begin, halfedge_index in_end) const
{
    assert(halfedge(in_begin).to_vertex == halfedge(in_end).to_vertex);

    auto he = in_begin;
    do
    {
        assert(to_vertex_of(he) == to_vertex(in_end));

        // free? found one!
        if (is_free(he))
            return he;

        // next half-edge of vertex
        he = opposite(next_halfedge_of(he));
    } while (he != in_end);

    return halfedge_index::invalid();
}

inline halfedge_index Mesh::find_free_incident(vertex_index v) const
{
    auto in_begin = opposite(outgoing_halfedge_of(v));
    return find_free_incident(in_begin, in_begin);
}

inline halfedge_index Mesh::find_halfedge(vertex_index from, vertex_index to) const
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

inline bool Mesh::is_boundary(vertex_index idx) const
{
    auto oh = outgoing_halfedge_of(idx);
    return oh.is_valid() && is_boundary(oh);
}

inline bool Mesh::is_free(halfedge_index idx) const { return face_of(idx).is_invalid(); }
inline bool Mesh::is_boundary(halfedge_index idx) const { return is_free(idx); }

inline bool Mesh::is_isolated(vertex_index idx) const { return outgoing_halfedge_of(idx).is_invalid(); }

inline bool Mesh::is_removed(vertex_index idx) const { return outgoing_halfedge_of(idx).value >= -1; }
inline bool Mesh::is_removed(face_index idx) const { return halfedge_of(idx).is_invalid(); }
inline bool Mesh::is_removed(edge_index idx) const { return to_vertex_of(halfedge_of(idx, 0)).is_invalid(); }
inline bool Mesh::is_removed(halfedge_index idx) const { return to_vertex_of(idx).is_invalid(); }

inline void Mesh::set_removed(vertex_index idx) { outgoing_halfedge_of(idx) = halfedge_index::invalid(); }
inline void Mesh::set_removed(face_index idx) { halfedge_of(idx) = halfedge_index::invalid(); }
inline void Mesh::set_removed(edge_index idx)
{
    to_vertex_of(halfedge_of(idx, 0)) = vertex_index::invalid();
    to_vertex_of(halfedge_of(idx, 1)) = vertex_index::invalid();
}

inline face_index &Mesh::face_of(halfedge_index idx) { return mHalfedgeToFace[(int)idx]; }
inline vertex_index &Mesh::to_vertex_of(halfedge_index idx) { return mHalfedgeToVertex[(int)idx]; }
inline halfedge_index &Mesh::next_halfedge_of(halfedge_index idx) { return mHalfedgeToNextHalfedge[(int)idx]; }
inline halfedge_index &Mesh::prev_halfedge_of(halfedge_index idx) { return mHalfedgeToPrevHalfedge[(int)idx]; }
inline halfedge_index &Mesh::halfedge_of(face_index idx) { return mFaceToHalfedge[(int)idx]; }
inline halfedge_index &Mesh::outgoing_halfedge_of(vertex_index idx) { return mVertexToOutgoingHalfedge[(int)idx]; }

inline face_index Mesh::face_of(halfedge_index idx) const { return mHalfedgeToFace[(int)idx]; }
inline vertex_index Mesh::to_vertex_of(halfedge_index idx) const { return mHalfedgeToVertex[(int)idx]; }
inline halfedge_index Mesh::next_halfedge_of(halfedge_index idx) const { return mHalfedgeToNextHalfedge[(int)idx]; }
inline halfedge_index Mesh::prev_halfedge_of(halfedge_index idx) const { return mHalfedgeToPrevHalfedge[(int)idx]; }
inline halfedge_index Mesh::halfedge_of(face_index idx) const { return mFaceToHalfedge[(int)idx]; }
inline halfedge_index Mesh::outgoing_halfedge_of(vertex_index idx) const { return mVertexToOutgoingHalfedge[(int)idx]; }

inline halfedge_index Mesh::opposite(halfedge_index he) const { return halfedge_index(he.value ^ 1); }

inline vertex_index &Mesh::from_vertex_of(halfedge_index idx) { return to_vertex_of(opposite(idx)); }
inline vertex_index Mesh::from_vertex_of(halfedge_index idx) const { return to_vertex_of(opposite(idx)); }

inline vertex_index Mesh::next_valid_idx_from(vertex_index idx) const
{
    auto s = size_all_vertices();
    auto i = idx;
    while (i.value < s && is_removed(i))
        i.value++;
    return i;
}

inline vertex_index Mesh::prev_valid_idx_from(vertex_index idx) const
{
    auto i = idx;
    while (i.value >= 0 && is_removed(i))
        i.value--;
    return i;
}

inline edge_index Mesh::next_valid_idx_from(edge_index idx) const
{
    auto s = size_all_edges();
    auto i = idx;
    while (i.value < s && is_removed(i))
        i.value++;
    return i;
}

inline edge_index Mesh::prev_valid_idx_from(edge_index idx) const
{
    auto i = idx;
    while (i.value >= 0 && is_removed(i))
        i.value--;
    return i;
}

inline face_index Mesh::next_valid_idx_from(face_index idx) const
{
    auto s = size_all_faces();
    auto i = idx;
    while (i.value < s && is_removed(i))
        i.value++;
    return i;
}

inline face_index Mesh::prev_valid_idx_from(face_index idx) const
{
    auto i = idx;
    while (i.value >= 0 && is_removed(i))
        i.value--;
    return i;
}

inline halfedge_index Mesh::next_valid_idx_from(halfedge_index idx) const
{
    auto s = size_all_halfedges();
    auto i = idx;
    while (i.value < s && is_removed(i))
        i.value++;
    return i;
}

inline halfedge_index Mesh::prev_valid_idx_from(halfedge_index idx) const
{
    auto i = idx;
    while (i.value >= 0 && is_removed(i))
        i.value--;
    return i;
}

inline void Mesh::connect_prev_next(halfedge_index prev, halfedge_index next)
{
    next_halfedge_of(prev) = next;
    prev_halfedge_of(next) = prev;
}

inline vertex_index Mesh::face_split(face_index f)
{
    // TODO: can be made more performant

    auto h_begin = halfedge_of(f);

    // remove face
    remove_face(f);

    // add vertex
    vertex_index vs[3];
    vs[0] = add_vertex();

    // add triangles
    auto h = h_begin;
    do
    {
        vs[1] = from_vertex_of(h);
        vs[2] = to_vertex_of(h);

        add_face(vs, 3);

        // NOTE: add_face inserted a new halfedge
        h = next_halfedge_of(opposite(next_halfedge_of(h)));
    } while (h != h_begin);

    return vs[0];
}

inline vertex_index Mesh::edge_split(edge_index e)
{
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    auto &h0_ref = halfedge(h0);
    auto &h1_ref = halfedge(h1);

    auto v0 = h0_ref.to_vertex;
    auto v1 = h1_ref.to_vertex;
    auto f0 = h0_ref.face;
    auto f1 = h1_ref.face;

    // add vertex
    auto v = add_vertex();

    // add two new edges
    auto e1 = alloc_edge();
    auto e2 = alloc_edge();
    auto e1h0 = halfedge_of(e1, 0);
    auto e1h1 = halfedge_of(e1, 1);
    auto e2h0 = halfedge_of(e2, 0);
    auto e2h1 = halfedge_of(e2, 1);

    // rewire edges
    auto &e1h0_ref = halfedge(e1h0);
    auto &e1h1_ref = halfedge(e1h1);
    auto &e2h0_ref = halfedge(e2h0);
    auto &e2h1_ref = halfedge(e2h1);

    auto h0_prev = h0_ref.prev_halfedge;
    auto h0_next = h0_ref.next_halfedge;
    auto h1_prev = h1_ref.prev_halfedge;
    auto h1_next = h1_ref.next_halfedge;

    e1h0_ref.face = f0;
    e2h0_ref.face = f0;
    e1h1_ref.face = f1;
    e2h1_ref.face = f1;

    e1h0_ref.to_vertex = v0;
    e2h0_ref.to_vertex = v;
    e1h1_ref.to_vertex = v;
    e2h1_ref.to_vertex = v1;

    connect_prev_next(h0_prev, e2h0);
    connect_prev_next(e2h0, e1h0);
    connect_prev_next(e1h0, h0_next);

    connect_prev_next(h1_prev, e1h1);
    connect_prev_next(e1h1, e2h1);
    connect_prev_next(e2h1, h1_next);

    // rewire vertices
    auto &v0_ref = vertex(h0_ref.to_vertex);
    auto &v1_ref = vertex(h1_ref.to_vertex);
    if (v0_ref.outgoing_halfedge == h1)
        v0_ref.outgoing_halfedge = e1h1;
    if (v1_ref.outgoing_halfedge == h0)
        v1_ref.outgoing_halfedge = e2h0;

    // rewire faces
    if (f0.is_valid())
    {
        auto &f0_ref = face(f0);
        if (f0_ref.halfedge == h0)
            f0_ref.halfedge = e1h0;
    }
    if (f1.is_valid())
    {
        auto &f1_ref = face(f1);
        if (f1_ref.halfedge == h1)
            f1_ref.halfedge = e2h1;
    }

    // remove edge
    h0_ref.set_removed();
    h1_ref.set_removed();
    mRemovedHalfedges += 2;
    mCompact = false;

    return v;
}

inline vertex_index Mesh::halfedge_split(halfedge_index h)
{
    // add vertex and edge
    auto v = add_vertex();
    auto e = alloc_edge();

    auto h0 = h;
    auto h1 = opposite(h);
    auto h2 = halfedge_of(e, 0);
    auto h3 = halfedge_of(e, 1);

    auto v0 = halfedge(h0).to_vertex;
    auto v1 = halfedge(h1).to_vertex;

    // rewire edges
    auto &h0_ref = halfedge(h0);
    auto &h1_ref = halfedge(h1);
    auto &h2_ref = halfedge(h2);
    auto &h3_ref = halfedge(h3);

    auto h0_next = h0_ref.next_halfedge;
    auto h1_prev = h1_ref.prev_halfedge;

    auto f0 = h0_ref.face;
    auto f1 = h1_ref.face;

    h2_ref.face = f0;
    h3_ref.face = f1;

    h0_ref.to_vertex = v;
    h1_ref.to_vertex = v1; //< already there
    h2_ref.to_vertex = v0;
    h3_ref.to_vertex = v;

    connect_prev_next(h0, h2);
    connect_prev_next(h2, h0_next);

    connect_prev_next(h1_prev, h3);
    connect_prev_next(h3, h1);

    // rewire vertices
    auto &v0_ref = vertex(v0);
    if (v0_ref.outgoing_halfedge == h1)
        v0_ref.outgoing_halfedge = h3;

    vertex(v).outgoing_halfedge = h1_ref.is_free() ? h1 : h2; // boundary

    // rewire faces
    // -> already ok

    return v;
}

inline face_index Mesh::face_fill(halfedge_index h)
{
    assert(is_boundary(h));

    auto f = alloc_face();
    auto &f_ref = face(f);

    f_ref.halfedge = h;

    auto h_begin = h;
    do
    {
        auto &h_ref = halfedge(h);

        // set face
        h_ref.face = f;

        // fix face boundary
        if (is_boundary(opposite(h)))
            f_ref.halfedge = h;

        // fix adj face boundary
        auto adj_face = halfedge(opposite(h)).face;
        if (adj_face.is_valid())
            fix_boundary_state_of(adj_face);

        // advance
        h = h_ref.next_halfedge;
    } while (h != h_begin);

    // fix vertex boundaries
    fix_boundary_state_of_vertices(f);

    return f;
}

inline void Mesh::halfedge_attach(halfedge_index h, vertex_index v)
{
    assert(vertex(v).is_isolated());

    auto &h_ref = halfedge(h);
    auto h_next = h_ref.next_halfedge;
    auto v_to = h_ref.to_vertex;

    auto f = h_ref.face;

    auto e = alloc_edge();
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);

    auto &h0_ref = halfedge(h0);
    auto &h1_ref = halfedge(h1);

    h0_ref.face = f;
    h0_ref.to_vertex = v;

    h1_ref.face = f;
    h1_ref.to_vertex = v_to;

    vertex(v).outgoing_halfedge = h1;

    connect_prev_next(h, h0);
    connect_prev_next(h0, h1);
    connect_prev_next(h1, h_next);
}

inline void Mesh::vertex_collapse(vertex_index v)
{
    auto &v_ref = vertex(v);

    // isolated vertices are just removed
    if (v_ref.is_isolated())
    {
        remove_vertex(v);
    }
    // boundary vertices are special
    else if (is_boundary(v))
    {
        assert(0 && "not implemented");
    }
    else // interior vertex
    {
        auto h_begin = halfedge(vertex(v).outgoing_halfedge).next_halfedge;

        remove_vertex(v);

        assert(is_boundary(h_begin));

        // TODO: optimize
        std::vector<halfedge_index> hs;
        auto h = h_begin;
        do
        {
            // add half-edge ring
            hs.push_back(h);

            // advance
            h = halfedge(h).next_halfedge;
        } while (h != h_begin);

        // add face
        add_face(hs.data(), (int)hs.size());
    }
}

inline void Mesh::halfedge_collapse(halfedge_index h)
{
    // TODO: collapse half-edge
    // preserve adjacent non-triangles

    assert(0 && "not implemented");
}

inline void Mesh::edge_rotate_next(edge_index e)
{
    assert(!handle_of(e).is_boundary() && "does not work on boundaries");
    assert(handle_of(e).vertexA().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");
    assert(handle_of(e).vertexB().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");

    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    auto &h0_ref = halfedge(h0);
    auto &h1_ref = halfedge(h1);

    auto h0_next = h0_ref.next_halfedge;
    auto h0_prev = h0_ref.prev_halfedge;
    auto h1_next = h1_ref.next_halfedge;
    auto h1_prev = h1_ref.prev_halfedge;

    auto &h0_next_ref = halfedge(h0_next);
    auto &h1_next_ref = halfedge(h1_next);

    auto h0_next_next = h0_next_ref.next_halfedge;
    auto h1_next_next = h1_next_ref.next_halfedge;

    // fix vertices
    auto &v0 = vertex(h0_ref.to_vertex);
    if (v0.outgoing_halfedge == h1)
        v0.outgoing_halfedge = h0_next;
    auto &v1 = vertex(h1_ref.to_vertex);
    if (v1.outgoing_halfedge == h0)
        v1.outgoing_halfedge = h1_next;

    // fix faces
    face(h0_ref.face).halfedge = h0;
    face(h1_ref.face).halfedge = h1;

    // fix half-edges
    h0_ref.to_vertex = h0_next_ref.to_vertex;
    h1_ref.to_vertex = h1_next_ref.to_vertex;
    h0_next_ref.face = h1_ref.face;
    h1_next_ref.face = h0_ref.face;

    // move to next
    connect_prev_next(h1_prev, h0_next);
    connect_prev_next(h0_prev, h1_next);

    connect_prev_next(h0_next, h1);
    connect_prev_next(h1_next, h0);

    connect_prev_next(h0, h0_next_next);
    connect_prev_next(h1, h1_next_next);

    // fix boundary state
    fix_boundary_state_of(h0_ref.face);
    fix_boundary_state_of(h1_ref.face);
}

inline void Mesh::edge_rotate_prev(edge_index e)
{
    assert(!handle_of(e).is_boundary() && "does not work on boundaries");
    assert(handle_of(e).vertexA().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");
    assert(handle_of(e).vertexB().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");

    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    auto &h0_ref = halfedge(h0);
    auto &h1_ref = halfedge(h1);

    auto h0_next = h0_ref.next_halfedge;
    auto h0_prev = h0_ref.prev_halfedge;
    auto h1_next = h1_ref.next_halfedge;
    auto h1_prev = h1_ref.prev_halfedge;

    auto &h0_prev_ref = halfedge(h0_prev);
    auto &h1_prev_ref = halfedge(h1_prev);

    auto h0_prev_prev = h0_prev_ref.prev_halfedge;
    auto &h0_prev_prev_ref = halfedge(h0_prev_prev);
    auto h1_prev_prev = h1_prev_ref.prev_halfedge;
    auto &h1_prev_prev_ref = halfedge(h1_prev_prev);

    // fix vertex
    auto &v0 = vertex(h0_ref.to_vertex);
    if (v0.outgoing_halfedge == h1)
        v0.outgoing_halfedge = h0_next;
    auto &v1 = vertex(h1_ref.to_vertex);
    if (v1.outgoing_halfedge == h0)
        v1.outgoing_halfedge = h1_next;

    // fix faces
    face(h0_ref.face).halfedge = h0;
    face(h1_ref.face).halfedge = h1;

    // fix half-edge
    h1_ref.to_vertex = h0_prev_prev_ref.to_vertex;
    h0_ref.to_vertex = h1_prev_prev_ref.to_vertex;
    h0_prev_ref.face = h1_ref.face;
    h1_prev_ref.face = h0_ref.face;

    // move to next
    connect_prev_next(h0_prev, h1_next);
    connect_prev_next(h1_prev, h0_next);

    connect_prev_next(h1, h0_prev);
    connect_prev_next(h0, h1_prev);

    connect_prev_next(h0_prev_prev, h0);
    connect_prev_next(h1_prev_prev, h1);

    // fix boundary state
    fix_boundary_state_of(h0_ref.face);
    fix_boundary_state_of(h1_ref.face);
}

inline void Mesh::halfedge_rotate_next(halfedge_index h)
{
    assert(handle_of(h).next().next().next() != h && "does not work for triangles");
    assert(!handle_of(h).edge().is_boundary() && "does not work on boundaries");
    assert(handle_of(h).vertex_to().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");

    auto h0 = h;
    auto h1 = opposite(h);
    auto &h0_ref = halfedge(h0);
    auto &h1_ref = halfedge(h1);

    auto h0_next = h0_ref.next_halfedge;
    auto h1_prev = h1_ref.prev_halfedge;
    auto &h0_next_ref = halfedge(h0_next);
    auto h0_next_next = h0_next_ref.next_halfedge;

    // fix vertex
    auto &v = vertex(h0_ref.to_vertex);
    if (v.outgoing_halfedge == h1)
        v.outgoing_halfedge = h0_next;

    // fix faces
    face(h0_ref.face).halfedge = h0;
    face(h1_ref.face).halfedge = h1;

    // fix half-edges
    h0_ref.to_vertex = h0_next_ref.to_vertex;
    h0_next_ref.face = h1_ref.face;

    // move to next
    connect_prev_next(h1_prev, h0_next);
    connect_prev_next(h0_next, h1);
    connect_prev_next(h0, h0_next_next);

    // fix boundary state
    fix_boundary_state_of(h0_ref.face);
    fix_boundary_state_of(h1_ref.face);
}

inline void Mesh::halfedge_rotate_prev(halfedge_index h)
{
    assert(handle_of(h).prev().prev().prev() != h && "does not work for triangles");
    assert(!handle_of(h).edge().is_boundary() && "does not work on boundaries");
    assert(handle_of(h).vertex_from().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");

    auto h0 = h;
    auto h1 = opposite(h);
    auto &h0_ref = halfedge(h0);
    auto &h1_ref = halfedge(h1);

    auto h0_prev = h0_ref.prev_halfedge;
    auto h1_next = h1_ref.next_halfedge;
    auto &h0_prev_ref = halfedge(h0_prev);
    auto h0_prev_prev = h0_prev_ref.prev_halfedge;
    auto &h0_prev_prev_ref = halfedge(h0_prev_prev);

    // fix vertex
    auto &v = vertex(h1_ref.to_vertex);
    if (v.outgoing_halfedge == h0)
        v.outgoing_halfedge = h1_next;

    // fix faces
    face(h0_ref.face).halfedge = h0;
    face(h1_ref.face).halfedge = h1;

    // fix half-edge
    h1_ref.to_vertex = h0_prev_prev_ref.to_vertex;
    h0_prev_ref.face = h1_ref.face;

    // move to next
    connect_prev_next(h0_prev, h1_next);
    connect_prev_next(h1, h0_prev);
    connect_prev_next(h0_prev_prev, h0);

    // fix boundary state
    fix_boundary_state_of(h0_ref.face);
    fix_boundary_state_of(h1_ref.face);
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
    for (auto &h_to : mHalfedgeToVertex)
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
    for (auto &h_f : mHalfedgeToFace)
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
    for (auto &v_out : mVertexToOutgoingHalfedge)
        if (v_out.value >= 0)
            v_out.value = hp[v_out.value];

    for (auto &f_h : mFaceToHalfedge)
        if (f_h.value >= 0)
            f_h.value = hp[f_h.value];

    for (auto &h_next : mHalfedgeToNextHalfedge)
        if (h_next.value >= 0)
            h_next.value = hp[h_next.value];

    for (auto &h_prev : mHalfedgeToPrevHalfedge)
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
        if (!is_removed(vertex_index(i)))
        {
            v_old_to_new[i] = (int)v_new_to_old.size();
            v_new_to_old.push_back(i);
        }

    for (auto i = 0; i < f_cnt; ++i)
        if (!is_removed(face_index(i)))
        {
            f_old_to_new[i] = (int)f_new_to_old.size();
            f_new_to_old.push_back(i);
        }

    for (auto i = 0; i < e_cnt; ++i)
        if (!is_removed(edge_index(i)))
            e_new_to_old.push_back(i);

    for (auto i = 0; i < h_cnt; ++i)
        if (!is_removed(halfedge_index(i)))
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

    mVertexToOutgoingHalfedge.resize(v_new_to_old.size());
    mFaceToHalfedge.resize(f_new_to_old.size());
    mHalfedgeToFace.resize(h_new_to_old.size());
    mHalfedgeToVertex.resize(h_new_to_old.size());
    mHalfedgeToNextHalfedge.resize(h_new_to_old.size());
    mHalfedgeToPrevHalfedge.resize(h_new_to_old.size());

    for (auto &v_out : mVertexToOutgoingHalfedge)
        if (v_out.value >= 0)
            v_out.value = h_old_to_new[v_out.value];

    for (auto &f_h : mFaceToHalfedge)
        if (f_h.value >= 0)
            f_h.value = h_old_to_new[f_h.value];

    for (auto &h_next : mHalfedgeToNextHalfedge)
        if (h_next.value >= 0)
            h_next.value = h_old_to_new[h_next.value];
    for (auto &h_prev : mHalfedgeToPrevHalfedge)
        if (h_prev.value >= 0)
            h_prev.value = h_old_to_new[h_prev.value];
    for (auto &h_f : mHalfedgeToFace)
        if (h_f.value >= 0)
            h_f.value = f_old_to_new[h_f.value];
    for (auto &h_v : mHalfedgeToVertex)
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
    mVertexToOutgoingHalfedge.shrink_to_fit();
    mFaceToHalfedge.shrink_to_fit();
    mHalfedgeToFace.shrink_to_fit();
    mHalfedgeToVertex.shrink_to_fit();
    mHalfedgeToNextHalfedge.shrink_to_fit();
    mHalfedgeToPrevHalfedge.shrink_to_fit();

    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->resize(size_all_vertices(), true);
    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->resize(size_all_faces(), true);
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->resize(size_all_edges(), true);
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->resize(size_all_halfedges(), true);

    mRemovedFaces = 0;
    mRemovedHalfedges = 0;
    mRemovedVertices = 0;
    mCompact = true;
}

inline void Mesh::clear()
{
    for (auto &v_h : mVertexToOutgoingHalfedge)
        v_h = halfedge_index::invalid();
    for (auto &f_h : mFaceToHalfedge)
        f_h = halfedge_index::invalid();
    for (auto &h_v : mHalfedgeToVertex)
        h_v = vertex_index::invalid();

    mCompact = false;
    compactify();
}

inline void Mesh::copy_from(const Mesh &m)
{
    // copy topo
    mVertexToOutgoingHalfedge = m.mVertexToOutgoingHalfedge;
    mFaceToHalfedge = m.mFaceToHalfedge;
    mHalfedgeToFace = m.mHalfedgeToFace;
    mHalfedgeToVertex = m.mHalfedgeToVertex;
    mHalfedgeToNextHalfedge = m.mHalfedgeToNextHalfedge;
    mHalfedgeToPrevHalfedge = m.mHalfedgeToPrevHalfedge;

    // copy helper data
    mRemovedFaces = m.mRemovedFaces;
    mRemovedHalfedges = m.mRemovedHalfedges;
    mRemovedVertices = m.mRemovedVertices;
    mCompact = m.mCompact;

    // resize attributes
    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->resize(size_all_vertices(), true);
    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->resize(size_all_faces(), true);
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->resize(size_all_edges(), true);
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->resize(size_all_halfedges(), true);
}

inline SharedMesh Mesh::copy() const
{
    auto m = create();
    m->copy_from(*this);
    return m;
}

inline void Mesh::reserve_faces(int capacity)
{
    for (auto a = mFaceAttrs; a; a = a->mNextAttribute)
        a->resize(capacity, false);

    mFaceToHalfedge.reserve(capacity);
}

inline void Mesh::reserve_vertices(int capacity)
{
    for (auto a = mVertexAttrs; a; a = a->mNextAttribute)
        a->resize(capacity, false);

    mVertexToOutgoingHalfedge.reserve(capacity);
}

inline void Mesh::reserve_edges(int capacity)
{
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->resize(capacity, false);
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->resize(capacity << 1, false);

    mHalfedgeToFace.reserve(capacity * 2);
    mHalfedgeToVertex.reserve(capacity * 2);
    mHalfedgeToNextHalfedge.reserve(capacity * 2);
    mHalfedgeToPrevHalfedge.reserve(capacity * 2);
}

inline void Mesh::reserve_halfedges(int capacity)
{
    for (auto a = mHalfedgeAttrs; a; a = a->mNextAttribute)
        a->resize(capacity, false);
    for (auto a = mEdgeAttrs; a; a = a->mNextAttribute)
        a->resize(capacity >> 1, false);

    mHalfedgeToFace.reserve(capacity);
    mHalfedgeToVertex.reserve(capacity);
    mHalfedgeToNextHalfedge.reserve(capacity);
    mHalfedgeToPrevHalfedge.reserve(capacity);
}
}
