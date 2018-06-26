#include "Mesh.hh"

#include <cassert>

using namespace polymesh;

void Mesh::compactify()
{
    if (is_compact())
        return;

    /// TODO

    mCompact = true;
}

void Mesh::assert_consistency() const
{
    /// TODO
    /// check compact!

    // check sizes
    assert(mHalfedges.size() % 2 == 0); ///< even number of halfedges

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

        for (auto h : vertices())
        {
            ++vertex_cnt;
            if (h.is_removed())
                ++invalid_vertex_cnt;
        }

        for (auto h : faces())
        {
            ++face_cnt;
            if (h.is_removed())
                ++invalid_face_cnt;
        }

        for (auto h : edges())
        {
            ++edge_cnt;
            if (h.is_removed())
                ++invalid_edge_cnt;
        }

        for (auto h : halfedges())
        {
            ++halfedge_cnt;
            if (h.is_removed())
                ++invalid_halfedge_cnt;
        }

        for (auto h : valid_vertices())
        {
            assert(h.is_valid());
            ++valid_vertex_cnt;
        }
        for (auto h : valid_faces())
        {
            assert(h.is_valid());
            ++valid_face_cnt;
        }
        for (auto h : valid_edges())
        {
            assert(h.is_valid());
            ++valid_edge_cnt;
        }
        for (auto h : valid_halfedges())
        {
            assert(h.is_valid());
            ++valid_halfedge_cnt;
        }

        assert(vertex_cnt == vertices().size());
        assert(face_cnt == faces().size());
        assert(edge_cnt == edges().size());
        assert(halfedge_cnt == halfedges().size());

        assert(valid_vertex_cnt == valid_vertices().size());
        assert(valid_face_cnt == valid_faces().size());
        assert(valid_edge_cnt == valid_edges().size());
        assert(valid_halfedge_cnt == valid_halfedges().size());

        assert(vertex_cnt == valid_vertex_cnt + invalid_vertex_cnt);
        assert(face_cnt == valid_face_cnt + invalid_face_cnt);
        assert(edge_cnt == valid_edge_cnt + invalid_edge_cnt);
        assert(halfedge_cnt == valid_halfedge_cnt + invalid_halfedge_cnt);

        assert(mRemovedFaces == invalid_face_cnt);
        assert(mRemovedVertices == invalid_vertex_cnt);
        assert(mRemovedHalfedges == invalid_halfedge_cnt);
        assert(invalid_edge_cnt * 2 == invalid_halfedge_cnt);
        assert(valid_edge_cnt * 2 == valid_halfedge_cnt);
        assert(edge_cnt * 2 == halfedge_cnt);
    }

    // check prev-next heh

    // check topology consistencies

    // check iterators

    // check only non-removed can be accessed topologically
    for (auto f : valid_faces())
    {
        for (auto v : f.vertices())
            assert(!v.is_removed());
    }
}
