#pragma once

#include "../Mesh.hh"

namespace polymesh
{
inline vertex_index low_level_api_mutable::add_vertex() const { return alloc_vertex(); }

inline vertex_index low_level_api_mutable::alloc_vertex() const { return m.alloc_vertex(); }
inline face_index low_level_api_mutable::alloc_face() const { return m.alloc_face(); }
inline edge_index low_level_api_mutable::alloc_edge() const { return m.alloc_edge(); }
inline void low_level_api_mutable::alloc_primitives(int vertices, int faces, int halfedges) const { m.alloc_primitives(vertices, faces, halfedges); }

inline void low_level_api_mutable::reserve_vertices(int capacity) const { m.reserve_vertices(capacity); }
inline void low_level_api_mutable::reserve_edges(int capacity) const { m.reserve_edges(capacity); }
inline void low_level_api_mutable::reserve_halfedges(int capacity) const { m.reserve_halfedges(capacity); }
inline void low_level_api_mutable::reserve_faces(int capacity) const { m.reserve_faces(capacity); }

inline void low_level_api_mutable::permute_faces(const std::vector<int> &p) const { m.permute_faces(p); }
inline void low_level_api_mutable::permute_edges(const std::vector<int> &p) const { m.permute_edges(p); }
inline void low_level_api_mutable::permute_vertices(const std::vector<int> &p) const { m.permute_vertices(p); }

namespace detail
{
// NOTE: this is only ever used in the following three functions and there it is immediately consumed
inline halfedge_index *face_insert_cache(int cnt)
{
    static thread_local std::vector<halfedge_index> mFaceInsertCache;
    mFaceInsertCache.resize(cnt);
    return mFaceInsertCache.data();
}
} // namespace detail

inline face_index low_level_api_mutable::add_face(const vertex_handle *v_handles, int vcnt, face_index res_idx) const
{
    auto cache = detail::face_insert_cache(vcnt);
    for (auto i = 0; i < vcnt; ++i)
        cache[i] = add_or_get_halfedge(v_handles[(i + vcnt - 1) % vcnt].idx, v_handles[i].idx);
    return add_face(cache, vcnt, res_idx);
}

inline face_index low_level_api_mutable::add_face(const vertex_index *v_indices, int vcnt, face_index res_idx) const
{
    auto cache = detail::face_insert_cache(vcnt);
    for (auto i = 0; i < vcnt; ++i)
        cache[i] = add_or_get_halfedge(v_indices[(i + vcnt - 1) % vcnt], v_indices[i]);
    return add_face(cache, vcnt, res_idx);
}

inline face_index low_level_api_mutable::add_face(const halfedge_handle *half_loop, int vcnt, face_index res_idx) const
{
    auto cache = detail::face_insert_cache(vcnt);
    for (auto i = 0; i < vcnt; ++i)
        cache[i] = half_loop[i].idx;
    return add_face(cache, vcnt, res_idx);
}

inline face_index low_level_api_mutable::add_face(const halfedge_index *half_loop, int vcnt, face_index res_idx) const
{
    assert(vcnt >= 3 && "no support for less-than-triangular faces");
    assert((res_idx.is_invalid() || is_removed(res_idx)) && "resurrected index must be previously removed!");

    auto fidx = res_idx.is_valid() ? res_idx : alloc_face();

    // on resurrect: fix counts
    if (res_idx.is_valid())
    {
        m.mRemovedFaces--;
        // no mCompact change!
    }

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
        auto f = opposite_face_of(h);

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

inline edge_index low_level_api_mutable::add_or_get_edge(vertex_index v_from, vertex_index v_to) const
{
    assert(v_from != v_to);

    // already exists?
    auto he = find_halfedge(v_from, v_to);
    if (he.is_valid())
        return edge_of(he);

    // allocate new
    auto e = alloc_edge();
    auto h_from_to = halfedge_of(e, 0);
    auto h_to_from = halfedge_of(e, 1);

    // setup data (self-connected edge)
    to_vertex_of(h_from_to) = v_to;
    to_vertex_of(h_to_from) = v_from;
    connect_prev_next(h_from_to, h_to_from);
    connect_prev_next(h_to_from, h_from_to);

    // link from vertex
    if (is_isolated(v_from))
        outgoing_halfedge_of(v_from) = h_from_to;
    else
    {
        auto from_in = find_free_incident(v_from);
        assert(from_in.is_valid() && "vertex is already fully connected");

        auto from_out = next_halfedge_of(from_in);

        connect_prev_next(from_in, h_from_to);
        connect_prev_next(h_to_from, from_out);
    }

    // link to vertex
    if (is_isolated(v_to))
        outgoing_halfedge_of(v_to) = h_to_from;
    else
    {
        auto to_in = find_free_incident(v_to);
        assert(to_in.is_valid() && "vertex is already fully connected");

        auto to_out = next_halfedge_of(to_in);

        connect_prev_next(to_in, h_to_from);
        connect_prev_next(h_from_to, to_out);
    }

    return e;
}

inline halfedge_index low_level_api_mutable::add_or_get_halfedge(vertex_index v_from, vertex_index v_to) const
{
    auto e = add_or_get_edge(v_from, v_to);
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    return to_vertex_of(h0) == v_to ? h0 : h1;
}

inline edge_index low_level_api_mutable::add_or_get_edge(halfedge_index h_from, halfedge_index h_to) const
{
    assert(h_from != h_to);

    auto v_from = to_vertex_of(h_from);
    auto v_to = to_vertex_of(h_to);

    auto ex_he = find_halfedge(v_from, v_to);
    if (ex_he.is_valid())
    {
        assert(prev_halfedge_of(ex_he) == h_from && prev_halfedge_of(opposite(ex_he)) == h_to);

        // TODO: Maybe try rewriting an existing halfedge that does NOT yet have the right connection.
        return edge_of(ex_he);
    }

    assert(is_free(h_from) && is_free(h_to) && "Cannot insert into a face");

    // allocate new
    auto e = alloc_edge();
    auto h_from_to = halfedge_of(e, 0);
    auto h_to_from = halfedge_of(e, 1);

    // setup data (self-connected edge)
    to_vertex_of(h_from_to) = v_to;
    to_vertex_of(h_to_from) = v_from;

    // Link from side
    auto h_from_next = next_halfedge_of(h_from);

    connect_prev_next(h_from, h_from_to);
    connect_prev_next(h_to_from, h_from_next);

    // Link to side
    auto h_to_next = next_halfedge_of(h_to);

    connect_prev_next(h_to, h_to_from);
    connect_prev_next(h_from_to, h_to_next);

    return e;
}

inline halfedge_index low_level_api_mutable::add_or_get_halfedge(halfedge_index h_from, halfedge_index h_to) const
{
    auto e = add_or_get_edge(h_from, h_to);
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);
    return next_halfedge_of(h_from) == h0 ? h0 : h1;
}

inline void low_level_api_mutable::make_adjacent(halfedge_index he_in, halfedge_index he_out) const
{
    // see http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm ::makeAdjacent

    auto he_b = next_halfedge_of(he_in);
    auto he_d = prev_halfedge_of(he_out);

    // already correct
    if (he_b == he_out)
        return;

    // find free half-edge after `out` but before `in`
    auto he_g = find_free_incident(opposite(he_out), he_in);
    assert(he_g.is_valid() && "unable to make halfedges adjacent. maybe mesh is not manifold?"); // unable to make adjacent

    auto he_h = next_halfedge_of(he_g);

    // properly rewire
    connect_prev_next(he_in, he_out);
    connect_prev_next(he_g, he_b);
    connect_prev_next(he_d, he_h);
}

inline void low_level_api_mutable::remove_face(face_index f_idx) const
{
    assert(!is_removed(f_idx));

    auto he_begin = halfedge_of(f_idx);
    auto he = he_begin;
    do
    {
        assert(face_of(he) == f_idx);

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
}

inline void low_level_api_mutable::remove_edge(edge_index e_idx) const
{
    auto h_in = halfedge_of(e_idx, 0);
    auto h_out = halfedge_of(e_idx, 1);

    assert(!is_removed(h_in));
    assert(!is_removed(h_out));

    auto f0 = face_of(h_in);
    auto f1 = face_of(h_out);

    // remove adjacent faces
    if (f0.is_valid())
        remove_face(f0);
    if (f1.is_valid())
        remove_face(f1);

    // rewire vertices
    auto v_in_to = to_vertex_of(h_in);
    auto v_out_to = to_vertex_of(h_out);

    auto hi_out_prev = prev_halfedge_of(h_out);
    auto hi_out_next = next_halfedge_of(h_out);

    auto hi_in_prev = prev_halfedge_of(h_in);
    auto hi_in_next = next_halfedge_of(h_in);

    // modify vertex if outgoing half-edge is going to be removed
    auto &v_in_to_out = outgoing_halfedge_of(v_in_to);
    if (v_in_to_out == h_out)
    {
        if (hi_in_next == h_out) // v_in_to becomes isolated
            v_in_to_out = halfedge_index::invalid();
        else
            v_in_to_out = hi_in_next;
    }

    auto &v_out_to_out = outgoing_halfedge_of(v_out_to);
    if (v_out_to_out == h_in)
    {
        if (hi_out_next == h_in) // v_out_to becomes isolated
            v_out_to_out = halfedge_index::invalid();
        else
            v_out_to_out = hi_out_next;
    }

    // reqire half-edges
    connect_prev_next(hi_out_prev, hi_in_next);
    connect_prev_next(hi_in_prev, hi_out_next);

    // remove half-edges
    set_removed(e_idx);
}

inline void low_level_api_mutable::remove_vertex(vertex_index v_idx) const
{
    assert(!is_removed(v_idx));

    // remove all outgoing edges
    while (!is_isolated(v_idx))
        remove_edge(edge_of(outgoing_halfedge_of(v_idx)));

    // mark removed
    set_removed(v_idx);
}

inline void low_level_api_mutable::clear_removed_edge_vector() const
{
    assert(m.edges().empty() && "only works for no-edge meshes");

    // this was using vector::clear before, which does not change the capacity either
    m.mHalfedgesSize = 0;

    m.mRemovedHalfedges = 0;
    // no mCompact change!
}

inline void low_level_api_mutable::fix_boundary_state_of(vertex_index v_idx) const
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

inline void low_level_api_mutable::fix_boundary_state_of(face_index f_idx) const
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

inline void low_level_api_mutable::fix_boundary_state_of_vertices(face_index f_idx) const
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

inline void low_level_api_mutable::set_removed(vertex_index idx) const
{
    assert(!is_removed(idx) && "cannot remove an already removed entry");
    outgoing_halfedge_of(idx).value = -2;

    // bookkeeping
    m.mRemovedVertices++;
    m.mCompact = false;
}

inline void low_level_api_mutable::set_removed(face_index idx) const
{
    assert(!is_removed(idx) && "cannot remove an already removed entry");
    halfedge_of(idx) = halfedge_index::invalid();

    // bookkeeping
    m.mRemovedFaces++;
    m.mCompact = false;
}

inline void low_level_api_mutable::set_removed(edge_index idx) const
{
    assert(!is_removed(idx) && "cannot remove an already removed entry");
    to_vertex_of(halfedge_of(idx, 0)) = vertex_index::invalid();
    to_vertex_of(halfedge_of(idx, 1)) = vertex_index::invalid();

    // bookkeeping
    m.mRemovedHalfedges++;
    m.mRemovedHalfedges++;
    m.mCompact = false;
}

inline void low_level_api_mutable::set_removed_counts(int r_vertices, int r_faces, int r_edges)
{
    m.mRemovedVertices = r_vertices;
    m.mRemovedFaces = r_faces;
    m.mRemovedHalfedges = r_edges * 2;
    m.mCompact = r_vertices == 0 && r_faces == 0 && r_edges == 0;
}

inline void low_level_api_mutable::connect_prev_next(halfedge_index prev, halfedge_index next) const
{
    next_halfedge_of(prev) = next;
    prev_halfedge_of(next) = prev;
}

inline vertex_index low_level_api_mutable::face_split(face_index f) const
{
    auto v = add_vertex();
    face_split(f, v);
    return v;
}

inline void low_level_api_mutable::face_split(face_index f, vertex_index v) const
{
    assert(is_isolated(v));
    // TODO: can be made more performant

    auto h_begin = halfedge_of(f);

    // remove face
    remove_face(f);

    // add vertex
    vertex_index vs[3];
    vs[0] = v;

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
}

inline vertex_index low_level_api_mutable::edge_split(edge_index e) const
{
    auto v = add_vertex();
    edge_split(e, v);
    return v;
}

inline void low_level_api_mutable::edge_split(edge_index e, vertex_index v) const
{
    assert(is_isolated(v));

    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);

    auto v0 = to_vertex_of(h0);
    auto v1 = to_vertex_of(h1);
    auto f0 = face_of(h0);
    auto f1 = face_of(h1);

    // add two new edges
    auto e1 = alloc_edge();
    auto e2 = alloc_edge();
    auto e1h0 = halfedge_of(e1, 0);
    auto e1h1 = halfedge_of(e1, 1);
    auto e2h0 = halfedge_of(e2, 0);
    auto e2h1 = halfedge_of(e2, 1);

    // rewire edges
    auto h0_prev = prev_halfedge_of(h0);
    auto h0_next = next_halfedge_of(h0);
    auto h1_prev = prev_halfedge_of(h1);
    auto h1_next = next_halfedge_of(h1);

    face_of(e1h0) = f0;
    face_of(e2h0) = f0;
    face_of(e1h1) = f1;
    face_of(e2h1) = f1;

    to_vertex_of(e1h0) = v0;
    to_vertex_of(e2h0) = v;
    to_vertex_of(e1h1) = v;
    to_vertex_of(e2h1) = v1;

    connect_prev_next(e2h0, e1h0);
    connect_prev_next(e1h1, e2h1);

    connect_prev_next(h0_prev, e2h0);
    connect_prev_next(e1h0, h0_next);
    connect_prev_next(h1_prev, e1h1);
    connect_prev_next(e2h1, h1_next);

    // self-connected?
    if (h0_prev == h1)
    {
        connect_prev_next(e2h1, e2h0);
    }
    if (h1_next == h0)
    {
        connect_prev_next(e1h0, e1h1);
    }

    // rewire vertices
    auto &v0_out = outgoing_halfedge_of(v0);
    auto &v1_out = outgoing_halfedge_of(v1);
    if (v0_out == h1)
        v0_out = e1h1;
    if (v1_out == h0)
        v1_out = e2h0;

    outgoing_halfedge_of(v) = is_boundary(e1h0) ? e1h0 : e2h1;

    // rewire faces
    if (f0.is_valid())
    {
        auto &f0_h = halfedge_of(f0);
        if (f0_h == h0)
            f0_h = e1h0;
    }
    if (f1.is_valid())
    {
        auto &f1_h = halfedge_of(f1);
        if (f1_h == h1)
            f1_h = e2h1;
    }

    // remove edge
    set_removed(e);
}

inline vertex_index low_level_api_mutable::halfedge_split(halfedge_index h) const
{
    auto v = add_vertex();
    halfedge_split(h, v);
    return v;
}

inline void low_level_api_mutable::halfedge_split(halfedge_index h, vertex_index v) const
{
    // add edge
    auto e = alloc_edge();

    auto h0 = h;
    auto h1 = opposite(h);
    auto h2 = halfedge_of(e, 0);
    auto h3 = halfedge_of(e, 1);

    auto v0 = to_vertex_of(h0);
    auto v1 = to_vertex_of(h1);

    // rewire edges
    auto h0_next = next_halfedge_of(h0);
    auto h1_prev = prev_halfedge_of(h1);


    auto f0 = face_of(h0);
    auto f1 = face_of(h1);

    face_of(h2) = f0;
    face_of(h3) = f1;

    to_vertex_of(h0) = v;
    to_vertex_of(h1) = v1; //< already there
    to_vertex_of(h2) = v0;
    to_vertex_of(h3) = v;

    connect_prev_next(h0, h2);
    connect_prev_next(h3, h1);

    if (h0_next == h1) // self connected?
    {
        connect_prev_next(h2, h3);
    }
    else
    {
        connect_prev_next(h2, h0_next);
        connect_prev_next(h1_prev, h3);
    }


    // rewire vertices
    auto &v0_out = outgoing_halfedge_of(v0);
    if (v0_out == h1)
        v0_out = h3;

    outgoing_halfedge_of(v) = is_boundary(h1) ? h1 : h2; // boundary

    // rewire faces
    // -> already ok
}

inline face_index low_level_api_mutable::face_fill(halfedge_index h) const
{
    assert(is_boundary(h));

    auto f = alloc_face();

    halfedge_of(f) = h;

    auto h_begin = h;
    do
    {
        // set face
        face_of(h) = f;

        // fix face boundary
        if (is_boundary(opposite(h)))
            halfedge_of(f) = h;

        // fix adj face boundary
        auto adj_face = opposite_face_of(h);
        if (adj_face.is_valid())
            fix_boundary_state_of(adj_face);

        // advance
        h = next_halfedge_of(h);
    } while (h != h_begin);

    // fix vertex boundaries
    fix_boundary_state_of_vertices(f);

    return f;
}

inline void low_level_api_mutable::halfedge_attach(halfedge_index h, vertex_index v) const
{
    assert(is_isolated(v));

    auto h_next = next_halfedge_of(h);
    auto v_to = to_vertex_of(h);

    auto f = face_of(h);

    auto e = alloc_edge();
    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);

    face_of(h0) = f;
    to_vertex_of(h0) = v;

    face_of(h1) = f;
    to_vertex_of(h1) = v_to;

    outgoing_halfedge_of(v) = h1;

    connect_prev_next(h, h0);
    connect_prev_next(h0, h1);
    connect_prev_next(h1, h_next);
}

inline void low_level_api_mutable::halfedge_merge(halfedge_index h) const
{
    auto v_center = from_vertex_of(h);

    assert(m.handle_of(v_center).adjacent_vertices().size() == 2 && "vertex_from must have valence 2");

    //   |                                   |
    //   |      h_prev               h       |
    // v_from --------> v_center ---------> v_to
    //   |                                   |
    //   |                                   |

    auto h_prev = prev_halfedge_of(h);
    auto h_prev_opp = opposite(h_prev);
    auto h_opp = opposite(h);
    auto h_prev_prev = prev_halfedge_of(h_prev);
    auto h_prev_opp_next = next_halfedge_of(h_prev_opp);

    auto f_a = face_of(h);
    auto f_b = face_of(h_opp);

    // auto v_to = to_vertex_of(h);
    auto v_from = from_vertex_of(h_prev);

    // Set from vertex
    to_vertex_of(h_opp) = v_from;
    // Equivalent to
    // from_vertex_of(h) = v_from;

    connect_prev_next(h_prev_prev, h);
    connect_prev_next(h_opp, h_prev_opp_next);

    // fix vertex
    if (outgoing_halfedge_of(v_from) == h_prev)
        outgoing_halfedge_of(v_from) = h;

    // fix adjacent faces
    if (f_a.is_valid() && halfedge_of(f_a) == h_prev)
        halfedge_of(f_a) = h;

    if (f_b.is_valid() && halfedge_of(f_b) == h_prev_opp)
        halfedge_of(f_b) = h_opp;

    // remove
    set_removed(edge_of(h_prev));
    set_removed(v_center);
}

inline void low_level_api_mutable::vertex_collapse(vertex_index v) const
{
    // isolated vertices are just removed
    if (is_isolated(v))
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
        auto h_begin = next_halfedge_of(outgoing_halfedge_of(v));

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
            h = next_halfedge_of(h);
        } while (h != h_begin);

        // add face
        add_face(hs.data(), (int)hs.size());
    }
}

inline void low_level_api_mutable::halfedge_collapse(halfedge_index h) const
{
    auto h0 = h;
    auto h1 = opposite(h);

    assert(!is_boundary(h0) && "boundaries are not supported yet");
    assert(!is_boundary(h1) && "boundaries are not supported yet");

    auto v_to = to_vertex_of(h);
    auto v_from = from_vertex_of(h);

    auto f0 = face_of(h0);
    auto f1 = face_of(h1);

    auto h0_prev = prev_halfedge_of(h0);
    auto h0_next = next_halfedge_of(h0);
    auto h1_prev = prev_halfedge_of(h1);
    auto h1_next = next_halfedge_of(h1);

    // TODO: preserve adjacent non-triangles
    assert(prev_halfedge_of(h0_prev) == h0_next && "non-triangle adjacent faces not supported yet");
    assert(prev_halfedge_of(h1_prev) == h1_next && "non-triangle adjacent faces not supported yet");

    // fix faces
    auto h0_prev_opp = opposite(h0_prev);
    auto h1_next_opp = opposite(h1_next);
    auto fA = face_of(h0_prev_opp);
    auto fB = face_of(h1_next_opp);
    face_of(h0_next) = fA;
    face_of(h1_prev) = fB;
    if (halfedge_of(fA) == h0_prev_opp)
        halfedge_of(fA) = h0_next;
    if (halfedge_of(fB) == h1_next_opp)
        halfedge_of(fB) = h1_prev;

    // fix vertices
    auto hv = h1;
    do
    {
        // point to collapsed vertex
        assert(to_vertex_of(hv) == v_from);
        to_vertex_of(hv) = v_to;

        // advance
        hv = opposite(next_halfedge_of(hv));
    } while (hv != h1);

    if (outgoing_halfedge_of(v_to) == h1)
        outgoing_halfedge_of(v_to) = h0_next;

    auto vA = to_vertex_of(h0_next);
    auto vB = to_vertex_of(h1_next);
    if (outgoing_halfedge_of(vA) == h0_prev)
        outgoing_halfedge_of(vA) = next_halfedge_of(h0_prev_opp);
    if (outgoing_halfedge_of(vB) == h1_next_opp)
        outgoing_halfedge_of(vB) = h1_prev;

    // fix next-prev
    connect_prev_next(h1_prev, next_halfedge_of(h1_next_opp));
    connect_prev_next(prev_halfedge_of(h1_next_opp), h1_prev);

    connect_prev_next(h0_next, next_halfedge_of(h0_prev_opp));
    connect_prev_next(prev_halfedge_of(h0_prev_opp), h0_next);

    // mark for deletion
    set_removed(v_from);
    set_removed(f0);
    set_removed(f1);
    set_removed(edge_of(h));
    set_removed(edge_of(h0_prev));
    set_removed(edge_of(h1_next));
}

inline void low_level_api_mutable::edge_rotate_next(edge_index e) const
{
    assert(!is_boundary(e) && "does not work on boundaries");
    assert(m.handle_of(e).vertexA().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");
    assert(m.handle_of(e).vertexB().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");

    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);

    auto h0_next = next_halfedge_of(h0);
    auto h0_prev = prev_halfedge_of(h0);
    auto h1_next = next_halfedge_of(h1);
    auto h1_prev = prev_halfedge_of(h1);

    auto h0_next_next = next_halfedge_of(h0_next);
    auto h1_next_next = next_halfedge_of(h1_next);

    // fix vertices
    auto &v0_out = outgoing_halfedge_of(to_vertex_of(h0));
    if (v0_out == h1)
        v0_out = h0_next;
    auto &v1_out = outgoing_halfedge_of(to_vertex_of(h1));
    if (v1_out == h0)
        v1_out = h1_next;

    // fix faces
    halfedge_of(face_of(h0)) = h0;
    halfedge_of(face_of(h1)) = h1;

    // fix half-edges
    to_vertex_of(h0) = to_vertex_of(h0_next);
    to_vertex_of(h1) = to_vertex_of(h1_next);
    face_of(h0_next) = face_of(h1);
    face_of(h1_next) = face_of(h0);

    // move to next
    connect_prev_next(h1_prev, h0_next);
    connect_prev_next(h0_prev, h1_next);

    connect_prev_next(h0_next, h1);
    connect_prev_next(h1_next, h0);

    connect_prev_next(h0, h0_next_next);
    connect_prev_next(h1, h1_next_next);

    // fix boundary state
    fix_boundary_state_of(face_of(h0));
    fix_boundary_state_of(face_of(h1));
}

inline void low_level_api_mutable::edge_rotate_prev(edge_index e) const
{
    assert(!is_boundary(e) && "does not work on boundaries");
    assert(m.handle_of(e).vertexA().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");
    assert(m.handle_of(e).vertexB().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");

    auto h0 = halfedge_of(e, 0);
    auto h1 = halfedge_of(e, 1);

    auto h0_next = next_halfedge_of(h0);
    auto h0_prev = prev_halfedge_of(h0);
    auto h1_next = next_halfedge_of(h1);
    auto h1_prev = prev_halfedge_of(h1);

    auto h0_prev_prev = prev_halfedge_of(h0_prev);
    auto h1_prev_prev = prev_halfedge_of(h1_prev);

    // fix vertex
    auto &v0_out = outgoing_halfedge_of(to_vertex_of(h0));
    if (v0_out == h1)
        v0_out = h0_next;
    auto &v1_out = outgoing_halfedge_of(to_vertex_of(h1));
    if (v1_out == h0)
        v1_out = h1_next;

    // fix faces
    halfedge_of(face_of(h0)) = h0;
    halfedge_of(face_of(h1)) = h1;

    // fix half-edge
    to_vertex_of(h1) = to_vertex_of(h0_prev_prev);
    to_vertex_of(h0) = to_vertex_of(h1_prev_prev);
    face_of(h0_prev) = face_of(h1);
    face_of(h1_prev) = face_of(h0);

    // move to next
    connect_prev_next(h0_prev, h1_next);
    connect_prev_next(h1_prev, h0_next);

    connect_prev_next(h1, h0_prev);
    connect_prev_next(h0, h1_prev);

    connect_prev_next(h0_prev_prev, h0);
    connect_prev_next(h1_prev_prev, h1);

    // fix boundary state
    fix_boundary_state_of(face_of(h0));
    fix_boundary_state_of(face_of(h1));
}

inline void low_level_api_mutable::halfedge_rotate_next(halfedge_index h) const
{
    assert(m.handle_of(h).next().next().next() != h && "does not work for triangles");
    assert(!m.handle_of(h).edge().is_boundary() && "does not work on boundaries");
    assert(m.handle_of(h).vertex_to().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");

    auto h0 = h;
    auto h1 = opposite(h);

    auto h0_next = next_halfedge_of(h0);
    auto h1_prev = prev_halfedge_of(h1);
    auto h0_next_next = next_halfedge_of(h0_next);

    // fix vertex
    auto &v_out = outgoing_halfedge_of(to_vertex_of(h0));
    if (v_out == h1)
        v_out = h0_next;

    // fix faces
    halfedge_of(face_of(h0)) = h0;
    halfedge_of(face_of(h1)) = h1;

    // fix half-edges
    to_vertex_of(h0) = to_vertex_of(h0_next);
    face_of(h0_next) = face_of(h1);

    // move to next
    connect_prev_next(h1_prev, h0_next);
    connect_prev_next(h0_next, h1);
    connect_prev_next(h0, h0_next_next);

    // fix boundary state
    fix_boundary_state_of(face_of(h0));
    fix_boundary_state_of(face_of(h1));
}

inline void low_level_api_mutable::halfedge_rotate_prev(halfedge_index h) const
{
    assert(m.handle_of(h).prev().prev().prev() != h && "does not work for triangles");
    assert(!m.handle_of(h).edge().is_boundary() && "does not work on boundaries");
    assert(m.handle_of(h).vertex_from().adjacent_vertices().size() > 2 && "does not work on valence <= 2 vertices");

    auto h0 = h;
    auto h1 = opposite(h);

    auto h0_prev = prev_halfedge_of(h0);
    auto h1_next = next_halfedge_of(h1);
    auto h0_prev_prev = prev_halfedge_of(h0_prev);

    // fix vertex
    auto &v_out = outgoing_halfedge_of(to_vertex_of(h1));
    if (v_out == h0)
        v_out = h1_next;

    // fix faces
    halfedge_of(face_of(h0)) = h0;
    halfedge_of(face_of(h1)) = h1;

    // fix half-edge
    to_vertex_of(h1) = to_vertex_of(h0_prev_prev);
    face_of(h0_prev) = face_of(h1);

    // move to next
    connect_prev_next(h0_prev, h1_next);
    connect_prev_next(h1, h0_prev);
    connect_prev_next(h0_prev_prev, h0);

    // fix boundary state
    fix_boundary_state_of(face_of(h0));
    fix_boundary_state_of(face_of(h1));
}
} // namespace polymesh
