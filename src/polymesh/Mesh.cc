#include "Mesh.hh"

#include <cassert>
#include <map>
#include <set>

#include "debug.hh"

using namespace polymesh;

void Mesh::assert_consistency() const
{
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
            assert(!h.is_removed());
            ++valid_vertex_cnt;
        }
        for (auto h : valid_faces())
        {
            assert(h.is_valid());
            assert(!h.is_removed());
            ++valid_face_cnt;
        }
        for (auto h : valid_edges())
        {
            assert(h.is_valid());
            assert(!h.is_removed());
            ++valid_edge_cnt;
        }
        for (auto h : valid_halfedges())
        {
            assert(h.is_valid());
            assert(!h.is_removed());
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

    // check only non-removed can be accessed topologically
    for (auto f : valid_faces())
    {
        assert(!f.any_halfedge().is_removed());
        assert(!f.any_vertex().is_removed());

        for (auto v : f.vertices())
            assert(!v.is_removed());

        for (auto h : f.halfedges())
            assert(!h.is_removed());

        for (auto f : f.adjacent_faces())
            assert(!f.is_removed());

        for (auto f : f.edges())
            assert(!f.is_removed());
    }
    for (auto v : valid_vertices())
    {
        assert(!v.any_face().is_removed());
        assert(!v.any_edge().is_removed());
        assert(!v.any_incoming_halfedge().is_removed());
        assert(!v.any_outgoing_halfedge().is_removed());

        for (auto v : v.adjacent_vertices())
            assert(!v.is_removed());

        for (auto h : v.incoming_halfedges())
            assert(!h.is_removed());

        for (auto h : v.outgoing_halfedges())
            assert(!h.is_removed());

        for (auto f : v.faces())
            assert(!f.is_removed());

        for (auto f : v.edges())
            assert(!f.is_removed());
    }
    for (auto e : valid_edges())
    {
        assert(!e.faceA().is_removed());
        assert(!e.faceB().is_removed());

        assert(!e.vertexA().is_removed());
        assert(!e.vertexB().is_removed());

        assert(!e.halfedgeA().is_removed());
        assert(!e.halfedgeB().is_removed());
    }
    for (auto h : valid_halfedges())
    {
        assert(!h.prev().is_removed());
        assert(!h.next().is_removed());
        assert(!h.edge().is_removed());
        assert(!h.vertex_from().is_removed());
        assert(!h.vertex_to().is_removed());
        assert(!h.face().is_removed());
        assert(!h.opposite().is_removed());
        assert(!h.opposite_face().is_removed());
    }

    // check half-edge consistencies
    for (auto h : valid_halfedges())
    {
        assert(h.next().is_valid());
        assert(h.prev().is_valid());
        assert(h.opposite().is_valid());
        assert(h.vertex_to().is_valid());
        assert(h.vertex_from().is_valid());
        // face can be invalid

        assert(h.next().prev() == h);
        assert(h.prev().next() == h);
        assert(h.opposite().opposite() == h);

        if (!h.is_boundary())
            assert(h.face().halfedges().contains(h));
        assert(h.vertex_to().incoming_halfedges().contains(h));
        assert(h.vertex_from().outgoing_halfedges().contains(h));

        assert(h.edge().halfedgeA() == h || h.edge().halfedgeB() == h);
    }

    // check vertex consistencies
    for (auto v : valid_vertices())
    {
        if (!v.is_isolated())
        {
            assert(v.any_incoming_halfedge().is_valid());
            assert(v.any_outgoing_halfedge().is_valid());
            // assert(v.any_valid_face().is_valid()); -> wiremeshes are non-isolated but have no faces
            assert(v.any_edge().is_valid());

            assert(v.any_incoming_halfedge().vertex_to() == v);
            assert(v.any_outgoing_halfedge().vertex_from() == v);

            for (auto f : v.faces())
                if (f.is_valid())
                    assert(f.vertices().contains(v));

            for (auto h : v.outgoing_halfedges())
                assert(h.vertex_from() == v);

            for (auto h : v.incoming_halfedges())
                assert(h.vertex_to() == v);

            for (auto vv : v.adjacent_vertices())
                assert(vv.adjacent_vertices().contains(v));

            for (auto e : v.edges())
                assert(e.vertexA() == v || e.vertexB() == v);
        }
        else
        {
            assert(v.any_face().is_invalid());
            assert(v.any_valid_face().is_invalid());
            assert(v.any_incoming_halfedge().is_invalid());
            assert(v.any_outgoing_halfedge().is_invalid());
            assert(v.any_edge().is_invalid());

            assert(v.faces().size() == 0);
            assert(v.edges().size() == 0);
            assert(v.adjacent_vertices().size() == 0);
            assert(v.outgoing_halfedges().size() == 0);
            assert(v.incoming_halfedges().size() == 0);
        }
    }

    // check face consistencies
    for (auto f : valid_faces())
    {
        assert(f.any_halfedge().is_valid());
        assert(f.any_vertex().is_valid());

        assert(f.any_halfedge().face() == f);
        assert(f.any_vertex().faces().contains(f));

        for (auto h : f.halfedges())
            assert(h.face() == f);

        for (auto v : f.vertices())
            assert(v.faces().contains(f));

        for (auto ff : f.adjacent_faces())
            if (ff.is_valid())
                assert(ff.adjacent_faces().contains(f));

        for (auto e : f.edges())
            assert(e.faceA() == f || e.faceB() == f);
    }

    // check edge consistencies
    for (auto e : valid_edges())
    {
        assert(e.vertexA().is_valid());
        assert(e.vertexB().is_valid());
        assert(e.halfedgeA().is_valid());
        assert(e.halfedgeB().is_valid());
        // faces can be invalid

        assert(e.faceA().is_invalid() || e.faceA().edges().contains(e));
        assert(e.faceB().is_invalid() || e.faceB().edges().contains(e));

        assert(e.vertexA().edges().contains(e));
        assert(e.vertexB().edges().contains(e));

        assert(e.halfedgeA().edge() == e);
        assert(e.halfedgeB().edge() == e);
    }

    // check boundaries
    for (auto h : valid_halfedges())
        if (h.is_boundary())
        {
            assert(h.face().is_invalid());
            assert(h.edge().is_boundary());

            if (h.opposite().is_boundary())
            {
                assert(h.edge().is_isolated());
                assert(h.opposite_face().is_invalid());
            }
            else
            {
                assert(h.opposite_face().is_boundary());
            }

            assert(h.vertex_to().is_boundary());
            assert(h.vertex_from().is_boundary());
        }

    // check derived counts
    {
        auto v_e_sum = 0;
        auto f_h_sum = 0;

        for (auto v : valid_vertices())
        {
            v_e_sum += v.edges().size();
        }
        for (auto f : valid_faces())
        {
            f_h_sum += f.halfedges().size();
        }

        assert(v_e_sum == 2 * size_valid_edges());
        // WRONG: assert(f_h_sum == size_valid_halfedges());

        // TODO: more?
    }

    // compactness
    if (is_compact())
    {
        for (auto v : vertices())
        {
            assert(v.is_valid());
            assert(!v.is_removed());
        }

        for (auto f : faces())
        {
            assert(f.is_valid());
            assert(!f.is_removed());
        }

        for (auto e : edges())
        {
            assert(e.is_valid());
            assert(!e.is_removed());
        }

        for (auto h : halfedges())
        {
            assert(h.is_valid());
            assert(!h.is_removed());
        }
    }

    // check half-edge uniqueness
    std::map<int, std::set<int>> hes;
    for (auto h : valid_halfedges())
    {
        auto v0 = h.vertex_from().idx.value;
        auto v1 = h.vertex_to().idx.value;

        if (!hes[v0].insert(v1).second)
            assert(false && "duplicated half-edge");
    }
}
