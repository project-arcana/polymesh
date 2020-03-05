#pragma once

#include <queue>

#include <polymesh/Mesh.hh>
#include <polymesh/properties.hh>

namespace polymesh
{
/**
 * PriorityF: (edge_handle) -> optional<T>
 *   (nullopt means edge should not be split, higher priorities are split first)
 * SplitF: (vertex_handle v, halfedge_handle h, vertex_handle v_from, vertex_handle v_to, auto&& emit_edge) -> void
 *   (h is a new halfedge pointing to the new vertex v, but can be used to access the old attribute data as well)
 *   (emit_edge is edge_index -> void and must be called for any newly user-created edge.
 *    the two guaranteed new edges from the split are already added)
 *
 * NOTE: if mesh was compact before it is compact afterwards
 */
template <class PriorityF, class SplitF>
void split_edges(Mesh& m, PriorityF&& priorityF, SplitF&& splitF)
{
    using PrioT = std::decay_t<decltype(priorityF(pm::edge_handle{}).value())>;

    struct entry
    {
        PrioT priority;
        pm::edge_index edge;

        bool operator<(entry const& rhs) const { return priority < rhs.priority; }
    };

    std::priority_queue<entry> queue;

    auto const emit_edge = [&](edge_index new_e) {
        if (auto p = priorityF(m[new_e]); p.has_value())
            queue.push({p.value(), new_e});
    };

    // fill initial queue
    for (auto e : m.edges())
        emit_edge(e);

    // perform splits
    while (!queue.empty())
    {
        edge_handle e = m[queue.top().edge];
        queue.pop();
        POLYMESH_ASSERT(e.is_valid() && !e.is_removed());

        auto h = e.halfedgeA();
        auto v_from = h.vertex_from();
        auto v_to = h.vertex_to();
        auto new_v = m.halfedges().split(h);
        auto e0 = h.edge();
        auto e1 = h.next().edge();

        // split callback
        splitF(new_v, h, v_from, v_to, emit_edge);
        emit_edge(e0);
        emit_edge(e1);
    }
}

/**
 * same as split_edges but splitF has no emit_edge and triangle splits are performed
 * the topology must not be altered in splitF
 *
 * Example:
 *
 *   pm::split_edges_trimesh(m,
 *                           [&](pm::edge_handle e) -> tg::optional<float> {
 *                               auto const l = pm::edge_length(e, pos);
 *                               if (l < target_el)
 *                                   return {};
 *                               return l;
 *                           },
 *                           [&](pm::vertex_handle v, pm::halfedge_handle, pm::vertex_handle v_from, pm::vertex_handle v_to) {
 *                               pos[v] = mix(pos[v_to], pos[v_from], 0.5f);
 *                               // TODO: maybe propagate other attributes
 *                           });
 */
template <class PriorityF, class SplitF>
void split_edges_trimesh(Mesh& m, PriorityF&& priorityF, SplitF&& splitF)
{
    POLYMESH_ASSERT(is_triangle_mesh(m));

    split_edges(m, priorityF, [&](vertex_handle v_new, halfedge_handle h, vertex_handle v_from, vertex_handle v_to, auto&& emit_edge) {
        auto ll = low_level_api(m);
        edge_index new_edge_a;
        edge_index new_edge_b;

        if (v_new.is_boundary())
        {
            auto const f_new = m[ll.alloc_face()];
            auto const e_new = m[ll.alloc_edge()];
            auto const h0_new = e_new.halfedgeA();
            auto const h1_new = e_new.halfedgeB();

            auto const h_old = h.is_boundary() ? h.next().opposite() : h;
            auto const f_old = h_old.face();
            auto const v_opp = h_old.prev().vertex_from();
            auto const h_f_new = h_old.next();

            POLYMESH_ASSERT(f_old.is_valid());

            // fix topo
            ll.to_vertex_of(h0_new) = v_opp;
            ll.to_vertex_of(h1_new) = v_new;

            ll.halfedge_of(f_old) = h_old;
            ll.halfedge_of(f_new) = h_f_new;

            ll.face_of(h0_new) = f_old;
            ll.face_of(h1_new) = f_new;
            ll.face_of(h_f_new) = f_new;
            ll.face_of(h_f_new.next()) = f_new;

            ll.connect_prev_next(h_old, h0_new);
            ll.connect_prev_next(h1_new, h_f_new);
            ll.connect_prev_next(h0_new, h_old.prev());
            ll.connect_prev_next(h_f_new.next(), h1_new);

            new_edge_a = e_new;
        }
        else
        {
            auto const f_0_new = m[ll.alloc_face()];
            auto const f_1_new = m[ll.alloc_face()];
            auto const e_0_new = m[ll.alloc_edge()];
            auto const e_1_new = m[ll.alloc_edge()];
            auto const h00 = e_0_new.halfedgeA();
            auto const h01 = e_0_new.halfedgeB();
            auto const h10 = e_1_new.halfedgeA();
            auto const h11 = e_1_new.halfedgeB();

            auto const f_0_old = h.face();
            auto const f_1_old = h.opposite().face();

            auto const v0 = h.prev().vertex_from();
            auto const v1 = h.opposite().next().vertex_to();

            auto const h_prev = h.prev();
            auto const h_next = h.next();
            auto const h_next_opp = h_next.opposite();
            auto const h_opp_next = h.opposite().next();
            auto const h_next_next = h_next.next();
            auto const h_opp_next_next = h_opp_next.next();

            // fix topo
            ll.to_vertex_of(h00) = v0;
            ll.to_vertex_of(h01) = v_new;
            ll.to_vertex_of(h10) = v_new;
            ll.to_vertex_of(h11) = v1;

            ll.halfedge_of(f_0_old) = h_prev;
            ll.halfedge_of(f_0_new) = h_next_next;
            ll.halfedge_of(f_1_old) = h_opp_next;
            ll.halfedge_of(f_1_new) = h_opp_next_next;

            ll.face_of(h00) = f_0_old;
            ll.face_of(h01) = f_0_new;
            ll.face_of(h10) = f_1_old;
            ll.face_of(h11) = f_1_new;

            ll.face_of(h_next) = f_0_new;
            ll.face_of(h_next_next) = f_0_new;
            ll.face_of(h_next_opp) = f_1_new;
            ll.face_of(h_opp_next_next) = f_1_new;

            ll.connect_prev_next(h, h00);
            ll.connect_prev_next(h00, h_prev);

            ll.connect_prev_next(h_next_next, h01);
            ll.connect_prev_next(h01, h_next);

            ll.connect_prev_next(h_opp_next, h10);
            ll.connect_prev_next(h10, h.opposite());

            ll.connect_prev_next(h_next_opp, h11);
            ll.connect_prev_next(h11, h_opp_next_next);

            new_edge_a = e_0_new;
            new_edge_b = e_1_new;
        }

        // user callback
        splitF(v_new, h, v_from, v_to);

        // emit edges
        if (new_edge_a.is_valid())
            emit_edge(new_edge_a);
        if (new_edge_b.is_valid())
            emit_edge(new_edge_b);
    });
}
}
