#include "Mesh.hh"

#include <map>
#include <set>

#include "assert.hh"
#include "debug.hh"

using namespace polymesh;

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
                if (f.is_valid())
                    POLYMESH_ASSERT(f.vertices().contains(v));

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
            if (ff.is_valid())
                POLYMESH_ASSERT(ff.adjacent_faces().contains(f));

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
