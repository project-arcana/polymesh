#pragma once

#include <limits>
#include <queue>
#include <utility>
#include <vector>

#include <polymesh/Mesh.hh>
#include <polymesh/detail/random.hh>
#include <polymesh/fields.hh>

namespace polymesh
{
/**
 * Default configuration for incremental decimation
 *
 * ErrorF must have the following operations:
 *      ErrorF + ErrorF       (combines two error functions)
 *      ErrorF(Pos3)          (evaluates the error quadric)
 *      closest_point(ErrorF) (computes the error-minimizing position)
 *
 * Note: tg::quadric3 implements this interface
 *
 * Custom configs must provide:
 *   max_normal_dev
 *   should_stop(pm::Mesh const& m, error_value_t curr_error) -> bool
 *   eval(Pos3 const& p, ErrorF const& e) -> error_value_t
 *   merge(ErrorF const& a, ErrorF const& b) -> ErrorF
 *   is_collapse_allowed(pm::halfedge_handle h) -> bool
 *   collapsed_pos(pm::halfedge_handle h, ErrorF const& e) -> Pos3
 */
template <class Pos3, class ErrorF>
struct decimate_config
{
    using scalar_t = scalar_of<Pos3>;
    using error_value_t = std::decay_t<decltype(std::declval<ErrorF>()(std::declval<Pos3>()))>;

    /// decimation will stop once error > max_error
    error_value_t max_error = std::numeric_limits<error_value_t>::max();

    /// decimation will stop once m.vertices().size < target_vertex_count
    int target_vertex_count = 0;

    /// edge collapses where normal would change more than a certain angle are ignored
    /// (0 means 0°, 1 means 90°)
    scalar_t max_normal_dev = scalar_t(1);

    /// returns true if decimation should be stopped (can be static or member function)
    bool should_stop(pm::Mesh const& m, error_value_t curr_error) const
    {
        return m.vertices().size() <= target_vertex_count || curr_error > max_error;
    }

    /// evaluates the error function (can be static or member function)
    static error_value_t eval(Pos3 const& p, ErrorF const& e) { return e(p); }

    /// merges two error functions (can be static or member function)
    static ErrorF merge(ErrorF const& a, ErrorF const& b) { return a + b; }

    /// returns whether
    static bool is_collapse_allowed(pm::halfedge_handle /* h */) { return true; }

    /// returns the position after collapsing the halfedge (can be static or member function)
    static Pos3 collapsed_pos(pm::halfedge_handle /* h */, ErrorF const& e) { return closest_point(e); }

    /// creates a config that will decimate down to the target count
    static decimate_config down_to(int target_vertex_count)
    {
        auto cfg = decimate_config{};
        cfg.target_vertex_count = target_vertex_count;
        return cfg;
    }

    /// creates a config that will decimate up to the given error
    static decimate_config up_to_error(error_value_t max_error)
    {
        auto cfg = decimate_config{};
        cfg.max_error = max_error;
        return cfg;
    }
};

/**
 * Error-function-based incremental decimation
 *
 * Initial per-vertex errors must be provided in vertex_errors
 *
 * NOTE: currently does not touch the boundary
 *
 * Example:
 *
 *     pm::Mesh m;
 *     auto pos = pm::vertex_attribute<tg::pos3>(...);
 *     auto errors = pm::vertex_attribute<tg::quadric3>(...);
 *
 *     pm::decimate_down_to(m, pos, errors, 1000);
 */
template <class Pos3, class ErrorF, class ConfigT = decimate_config<Pos3, ErrorF>>
void decimate(pm::Mesh& m, //
              pm::vertex_attribute<Pos3>& pos,
              pm::vertex_attribute<ErrorF>& errors,
              ConfigT const& config);

/// calls decimate with a default configuration that decimates until a target vertex count is reached
template <class Pos3, class ErrorF>
void decimate_down_to(pm::Mesh& m, //
                      pm::vertex_attribute<Pos3>& pos,
                      pm::vertex_attribute<ErrorF>& errors,
                      int target_vertex_cnt)
{
    return decimate(m, pos, errors, decimate_config<Pos3, ErrorF>::down_to(target_vertex_cnt));
}

/// calls decimate with a default configuration that decimates until a target error value is reached
template <class Pos3, class ErrorF, class ErrorValueT>
void decimate_up_to_error(pm::Mesh& m, //
                          pm::vertex_attribute<Pos3>& pos,
                          pm::vertex_attribute<ErrorF>& errors,
                          ErrorValueT max_error)
{
    return decimate(m, pos, errors, decimate_config<Pos3, ErrorF>::up_to_error(max_error));
}


// ======================== IMPLEMENTATION ========================

template <class Pos3, class ErrorF, class ConfigT>
void decimate(pm::Mesh& m, //
              pm::vertex_attribute<Pos3>& pos,
              pm::vertex_attribute<ErrorF>& errors,
              ConfigT const& config)
{
    using error_value_t = std::decay_t<decltype(std::declval<ErrorF>()(std::declval<Pos3>()))>;

    struct entry
    {
        error_value_t error;
        pm::halfedge_index halfedge;
        int gen;
        Pos3 pos;

        bool operator<(entry const& rhs) const { return error > rhs.error; }
    };

    std::priority_queue<entry> queue;

    auto gen = 0;
    auto edge_gen = m.halfedges().make_attribute(0);
    auto vreach = m.vertices().make_attribute(-1);

    auto const enqueue = [&](pm::halfedge_handle h) {
        if (!config.is_collapse_allowed(h))
            return;

        auto const v_to = h.vertex_to();
        auto const v_from = h.vertex_from();

        if (v_from.is_boundary())
            return; // cannot enqueue if boundary

        auto const Q = config.merge(errors[v_to], errors[v_from]);
        auto const p = v_to.is_boundary() ? pos[v_to] : config.collapsed_pos(h, Q);
        queue.push({config.eval(p, Q), h, edge_gen[h], p});
    };

    auto const can_be_collapsed = [&](pm::halfedge_handle h, Pos3 q) -> bool {
        auto const v_to = h.vertex_to();
        auto const v_from = h.vertex_from();

        ++gen;

        auto const p_to = pos[v_to];
        auto const p_from = pos[v_from];

        // cannot collapse to valence 2 vertex
        auto const v_ok_0 = h.next().vertex_to();
        auto const v_ok_1 = h.opposite().prev().vertex_from();

        if (v_ok_0 == v_ok_1)
            return false; // valence-2

        // check flipped normals and certain topological constraints
        for (auto hh : v_to.outgoing_halfedges())
        {
            auto const v0 = hh.vertex_to();
            auto const v1 = hh.next().vertex_to();

            if (v0 == v_from || v1 == v_from)
                continue; // these faces will be removed during collapse

            auto const p0 = pos[v0];
            auto const p1 = pos[v1];

            auto const n_before = cross(p0 - p_to, p1 - p_to);
            auto const n_after = cross(p0 - q, p1 - q);
            auto const dot_before_after = dot(n_before, n_after);

            if (dot_before_after <= 0)
                return false; // no flips

            if (dot_before_after * dot_before_after < dot(n_before, n_before) * dot(n_after, n_after) * (1 - config.max_normal_dev))
                return false; // too much normal deviation

            vreach[v0] = gen;
        }

        for (auto hh : v_from.outgoing_halfedges())
        {
            auto v0 = hh.vertex_to();
            auto v1 = hh.next().vertex_to();

            if (vreach[v0] == gen && v0 != v_ok_0 && v0 != v_ok_1)
                return false; // more connections than expected

            if (v0 == v_to || v1 == v_to)
                continue; // these faces will be removed during collapse

            auto p0 = pos[v0];
            auto p1 = pos[v1];

            auto n_before = cross(p0 - p_from, p1 - p_from);
            auto n_after = cross(p0 - q, p1 - q);
            auto dot_before_after = dot(n_before, n_after);

            if (dot_before_after <= 0)
                return false; // no flips

            if (dot_before_after * dot_before_after < dot(n_before, n_before) * dot(n_after, n_after) * (1 - config.max_normal_dev))
                return false; // too much normal deviation
        }

        // finally: error below threshold, topology OK.
        return true;
    };

    // initial edges
    for (auto h : m.halfedges())
        enqueue(h);

    // decimate
    while (!queue.empty())
    {
        // get best element
        auto entry = queue.top();
        queue.pop();
        pm::halfedge_handle h = m[entry.halfedge];

        // exit condition
        if (config.should_stop(m, entry.error))
            break;

        // deleted halfedge
        if (h.is_removed())
            continue;

        auto const v_to = h.vertex_to();

        // invalid entry
        if (edge_gen[h] != entry.gen)
            continue;

        // check if collapse valid
        if (!can_be_collapsed(h, entry.pos))
            continue;

        // perform collapse
        POLYMESH_ASSERT(!h.edge().is_boundary());
        POLYMESH_ASSERT(!h.vertex_from().is_boundary());
        errors[v_to] = config.merge(errors[v_to], errors[h.vertex_from()]);
        pos[v_to] = entry.pos;
        m.halfedges().collapse(h);

        // update edge gens and enqueue new entries
        for (auto const ee : v_to.edges())
        {
            edge_gen[ee.halfedgeA()]++;
            edge_gen[ee.halfedgeB()]++;
            enqueue(ee.halfedgeA());
            enqueue(ee.halfedgeB());
        }
    }
}
}
