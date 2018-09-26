#include "Mesh.hh"

#include <cassert>
#include <map>
#include <set>

#include "debug.hh"

using namespace polymesh;

#define polyassert(v) \
    do                \
    {                 \
        assert(v);    \
        (void)(v);    \
    } while (0)

void Mesh::assert_consistency() const
{
    // check sizes
    assert(mHalfedgeToNextHalfedge.size() % 2 == 0); ///< even number of halfedges

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
            polyassert(h.is_valid());
            polyassert(!h.is_removed());
            ++valid_vertex_cnt;
        }
        for (auto h : faces())
        {
            polyassert(h.is_valid());
            polyassert(!h.is_removed());
            ++valid_face_cnt;
        }
        for (auto h : edges())
        {
            polyassert(h.is_valid());
            polyassert(!h.is_removed());
            ++valid_edge_cnt;
        }
        for (auto h : halfedges())
        {
            polyassert(h.is_valid());
            polyassert(!h.is_removed());
            ++valid_halfedge_cnt;
        }

        polyassert(vertex_cnt == all_vertices().size());
        polyassert(face_cnt == all_faces().size());
        polyassert(edge_cnt == all_edges().size());
        polyassert(halfedge_cnt == all_halfedges().size());

        polyassert(valid_vertex_cnt == vertices().size());
        polyassert(valid_face_cnt == faces().size());
        polyassert(valid_edge_cnt == edges().size());
        polyassert(valid_halfedge_cnt == halfedges().size());

        polyassert(vertex_cnt == valid_vertex_cnt + invalid_vertex_cnt);
        polyassert(face_cnt == valid_face_cnt + invalid_face_cnt);
        polyassert(edge_cnt == valid_edge_cnt + invalid_edge_cnt);
        polyassert(halfedge_cnt == valid_halfedge_cnt + invalid_halfedge_cnt);

        polyassert(mRemovedFaces == invalid_face_cnt);
        polyassert(mRemovedVertices == invalid_vertex_cnt);
        polyassert(mRemovedHalfedges == invalid_halfedge_cnt);
        polyassert(invalid_edge_cnt * 2 == invalid_halfedge_cnt);
        polyassert(valid_edge_cnt * 2 == valid_halfedge_cnt);
        polyassert(edge_cnt * 2 == halfedge_cnt);
    }

	// check validity
    for (auto f : faces())
    {
        polyassert(f.any_halfedge().is_valid());
    }
    for (auto f : halfedges())
    {
        polyassert(f.vertex_to().is_valid());
    }

    // check only non-removed can be accessed topologically
    for (auto f : faces())
    {
        polyassert(!f.any_halfedge().is_removed());
        polyassert(!f.any_vertex().is_removed());

        for (auto v : f.vertices())
            polyassert(!v.is_removed());

        for (auto h : f.halfedges())
            polyassert(!h.is_removed());

        for (auto f : f.adjacent_faces())
            polyassert(!f.is_removed());

        for (auto f : f.edges())
            polyassert(!f.is_removed());
    }
    for (auto v : vertices())
    {
        polyassert(!v.any_face().is_removed());
        polyassert(!v.any_edge().is_removed());
        polyassert(!v.any_incoming_halfedge().is_removed());
        polyassert(!v.any_outgoing_halfedge().is_removed());

        for (auto v : v.adjacent_vertices())
            polyassert(!v.is_removed());

        for (auto h : v.incoming_halfedges())
            polyassert(!h.is_removed());

        for (auto h : v.outgoing_halfedges())
            polyassert(!h.is_removed());

        for (auto f : v.faces())
            polyassert(!f.is_removed());

        for (auto f : v.edges())
            polyassert(!f.is_removed());
    }
    for (auto e : edges())
    {
        polyassert(!e.faceA().is_removed());
        polyassert(!e.faceB().is_removed());

        polyassert(!e.vertexA().is_removed());
        polyassert(!e.vertexB().is_removed());

        polyassert(!e.halfedgeA().is_removed());
        polyassert(!e.halfedgeB().is_removed());
    }
    for (auto h : halfedges())
    {
        polyassert(!h.prev().is_removed());
        polyassert(!h.next().is_removed());
        polyassert(!h.edge().is_removed());
        polyassert(!h.vertex_from().is_removed());
        polyassert(!h.vertex_to().is_removed());
        polyassert(!h.face().is_removed());
        polyassert(!h.opposite().is_removed());
        polyassert(!h.opposite_face().is_removed());
    }

    // check half-edge consistencies
    for (auto h : halfedges())
    {
        polyassert(h.next().is_valid());
        polyassert(h.prev().is_valid());
        polyassert(h.opposite().is_valid());
        polyassert(h.vertex_to().is_valid());
        polyassert(h.vertex_from().is_valid());
        // face can be invalid

        polyassert(h.next().prev() == h);
        polyassert(h.prev().next() == h);
        polyassert(h.opposite().opposite() == h);

        if (!h.is_boundary())
            polyassert(h.face().halfedges().contains(h));
        polyassert(h.vertex_to().incoming_halfedges().contains(h));
        polyassert(h.vertex_from().outgoing_halfedges().contains(h));

        polyassert(h.edge().halfedgeA() == h || h.edge().halfedgeB() == h);

        polyassert(h.next().vertex_from() == h.vertex_to());
        polyassert(h.prev().vertex_to() == h.vertex_from());

        auto ref_face = h.face();
        if (ref_face.is_valid())
            for (auto h : h.ring())
                polyassert(h.face() == ref_face);
    }

    // check vertex consistencies
    for (auto v : vertices())
    {
        if (!v.is_isolated())
        {
            polyassert(v.any_incoming_halfedge().is_valid());
            polyassert(v.any_outgoing_halfedge().is_valid());
            // polyassert(v.any_valid_face().is_valid()); -> wiremeshes are non-isolated but have no faces
            polyassert(v.any_edge().is_valid());

            polyassert(v.any_incoming_halfedge().vertex_to() == v);
            polyassert(v.any_outgoing_halfedge().vertex_from() == v);

            for (auto f : v.faces())
                if (f.is_valid())
                    polyassert(f.vertices().contains(v));

            for (auto h : v.outgoing_halfedges())
                polyassert(h.vertex_from() == v);

            for (auto h : v.incoming_halfedges())
                polyassert(h.vertex_to() == v);

            for (auto vv : v.adjacent_vertices())
                polyassert(vv.adjacent_vertices().contains(v));

            for (auto e : v.edges())
                polyassert(e.vertexA() == v || e.vertexB() == v);
        }
        else
        {
            polyassert(v.any_face().is_invalid());
            polyassert(v.any_valid_face().is_invalid());
            polyassert(v.any_incoming_halfedge().is_invalid());
            polyassert(v.any_outgoing_halfedge().is_invalid());
            polyassert(v.any_edge().is_invalid());

            polyassert(v.faces().size() == 0);
            polyassert(v.edges().size() == 0);
            polyassert(v.adjacent_vertices().size() == 0);
            polyassert(v.outgoing_halfedges().size() == 0);
            polyassert(v.incoming_halfedges().size() == 0);
        }
    }

    // check face consistencies
    for (auto f : faces())
    {
        polyassert(f.any_halfedge().is_valid());
        polyassert(f.any_vertex().is_valid());

        polyassert(f.any_halfedge().face() == f);
        polyassert(f.any_vertex().faces().contains(f));

        for (auto h : f.halfedges())
            polyassert(h.face() == f);

        for (auto v : f.vertices())
            polyassert(v.faces().contains(f));

        for (auto ff : f.adjacent_faces())
            if (ff.is_valid())
                polyassert(ff.adjacent_faces().contains(f));

        for (auto e : f.edges())
            polyassert(e.faceA() == f || e.faceB() == f);
    }

    // check edge consistencies
    for (auto e : edges())
    {
        polyassert(e.vertexA().is_valid());
        polyassert(e.vertexB().is_valid());
        polyassert(e.halfedgeA().is_valid());
        polyassert(e.halfedgeB().is_valid());
        // faces can be invalid

        polyassert(e.faceA().is_invalid() || e.faceA().edges().contains(e));
        polyassert(e.faceB().is_invalid() || e.faceB().edges().contains(e));

        polyassert(e.vertexA().edges().contains(e));
        polyassert(e.vertexB().edges().contains(e));

        polyassert(e.halfedgeA().edge() == e);
        polyassert(e.halfedgeB().edge() == e);
    }

    // check boundaries
    for (auto h : halfedges())
        if (h.is_boundary())
        {
            polyassert(h.face().is_invalid());
            polyassert(h.edge().is_boundary());

            if (h.opposite().is_boundary())
            {
                polyassert(h.edge().is_isolated());
                polyassert(h.opposite_face().is_invalid());
            }
            else
            {
                polyassert(h.opposite_face().is_boundary());
            }

            polyassert(h.vertex_to().is_boundary());
            polyassert(h.vertex_from().is_boundary());
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

        polyassert(v_e_sum == 2 * size_valid_edges());
        // WRONG: polyassert(f_h_sum == size_valid_halfedges());

        // TODO: more?
    }

    // compactness
    if (is_compact())
    {
        for (auto v : all_vertices())
        {
            polyassert(v.is_valid());
            polyassert(!v.is_removed());
        }

        for (auto f : all_faces())
        {
            polyassert(f.is_valid());
            polyassert(!f.is_removed());
        }

        for (auto e : all_edges())
        {
            polyassert(e.is_valid());
            polyassert(!e.is_removed());
        }

        for (auto h : all_halfedges())
        {
            polyassert(h.is_valid());
            polyassert(!h.is_removed());
        }
    }

    // check half-edge uniqueness
    std::map<int, std::set<int>> hes;
    for (auto h : halfedges())
    {
        auto v0 = h.vertex_from().idx.value;
        auto v1 = h.vertex_to().idx.value;

        if (!hes[v0].insert(v1).second)
            polyassert(false && "duplicated half-edge");
    }
}
