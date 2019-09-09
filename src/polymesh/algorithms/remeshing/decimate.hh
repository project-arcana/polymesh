#pragma once

#include <iostream>

#include <utility>
#include <vector>

#include <polymesh/Mesh.hh>
#include <polymesh/detail/random.hh>
#include <polymesh/fields.hh>

namespace polymesh
{
struct decimation_settings
{
    // quality settings
    float max_error = 0.5f;
    float max_normal_dev = 0.05f; // 0 means none, 1 means 90°

    // algo settings
    float pre_decimate_factor = 0.0f; // decimates in initial run if err < f * max_err
    float early_accept_factor = 0.0f; // accepts immediately if err < f * max_err
    int max_tries = 10;
    int choices_per_it = 8;
};

/*
 * Error-function-based decimation
 *
 * Initial per-vertex errors must be provided in vertex_errors
 *
 * ErrorF must have the following operations:
 *      ErrorF + ErrorF       (combines two error functions)
 *      ErrorF(Pos3)          (evaluates the error quadric)
 *      closest_point(ErrorF) (computes the error-minimizing position)
 *
 * Note: tg::quadric3 implements this interface
 */
template <class Pos3, class ErrorF>
void decimate(polymesh::Mesh& m, //
              polymesh::vertex_attribute<Pos3>& pos,
              polymesh::vertex_attribute<ErrorF> const& vertex_errors,
              decimation_settings const& settings)
{
    using vec_t = typename field3<Pos3>::vec_t;

    uint64_t rng = 0xDEADBEEF;

    auto gen = 0;
    auto errors = vertex_errors; // copy
    auto vreach = m.vertices().make_attribute_with_default(-1);

    using error_value_t = decltype(std::declval<ErrorF>()(std::declval<Pos3>()));

    // candidates
    struct candidate_info
    {
        Pos3 q_min;        // new position
        error_value_t err; // error
    };
    std::vector<halfedge_index> candidates;

    // true if candidate cannot become valid anymore
    auto const is_candidate_permanently_invalid = [](halfedge_handle h) -> bool {
        return h.is_removed() ||         //
               h.edge().is_boundary() || //
               h.vertex_from().is_boundary();
    };

    // helper
    auto const dot = [](vec_t const& a, vec_t const& b) { return field3<Pos3>::dot(a, b); };
    auto const cross = [](vec_t const& a, vec_t const& b) { return field3<Pos3>::cross(a, b); };

    // computes candidate info on demand
    // returns true iff candidate is usable
    auto const check_candidate_info = [&](halfedge_index hi, float max_err, candidate_info& info) -> bool {
        auto h = hi.of(m);
        assert(!is_candidate_permanently_invalid(h));

        auto const v_to = h.vertex_to();
        auto const v_from = h.vertex_from();

        // compute best new position and error
        Pos3 q;
        {
            auto const E = errors[v_from] + errors[v_to];
            q = v_to.is_boundary() ? pos[v_to] : closest_point(E);
            auto const err = E(q);

            if (err > max_err)
                return false;

            // enter new info
            info.q_min = q;
            info.err = err;
        }

        // cannot collapse certain concave configurations
        // check for flips and zeros
        {
            ++gen;

            auto p_to = pos[v_to];
            auto p_from = pos[v_from];

            auto v_ok_0 = h.next().vertex_to();
            auto v_ok_1 = h.opposite().prev().vertex_from();

            if (v_ok_0 == v_ok_1)
                return false; // valence-2

            for (auto hh : v_to.outgoing_halfedges())
            {
                auto v0 = hh.vertex_to();
                auto v1 = hh.next().vertex_to();

                if (v0 == v_from || v1 == v_from)
                    continue; // these faces will be removed during collapse

                auto p0 = pos[v0];
                auto p1 = pos[v1];

                auto n_before = cross(p0 - p_to, p1 - p_to);
                auto n_after = cross(p0 - q, p1 - q);
                auto dot_before_after = dot(n_before, n_after);

                if (dot_before_after <= 0)
                    return false; // no flips

                if (dot_before_after * dot_before_after < dot(n_before, n_before) * dot(n_after, n_after) * (1 - settings.max_normal_dev))
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

                if (dot_before_after * dot_before_after < dot(n_before, n_before) * dot(n_after, n_after) * (1 - settings.max_normal_dev))
                    return false; // too much normal deviation
            }
        }

        // finally: error below threshold, topology OK.
        return true;
    };

    // actually performs a collapse
    auto collapse_halfedge = [&m, &errors, &pos](halfedge_index hi, Pos3 q_min) {
        auto h = hi.of(m);
        auto v = h.vertex_to();

        // update quadric and position
        errors[v] = errors[v] + errors[h.vertex_from()];
        pos[v] = q_min;
        //std::cout << q_min.x << " " << q_min.y << " " << q_min.z << std::endl;

        // perform collapse
        POLYMESH_ASSERT(!h.edge().is_boundary());
        POLYMESH_ASSERT(!h.vertex_from().is_boundary());
        m.halfedges().collapse(h);
    };

    // initial decimation
    {
        auto max_pre_err = settings.max_error * settings.pre_decimate_factor;
        candidates.reserve(m.halfedges().size());

        for (auto h : m.halfedges())
        {
            if (is_candidate_permanently_invalid(h))
                continue; // skip boundaries

            // try to immediately collapse
            candidate_info cinfo;
            if (check_candidate_info(h, max_pre_err, cinfo))
            {
                collapse_halfedge(h, cinfo.q_min);
                continue;
            }

            // otherwise: add to candidates
            candidates.push_back(h);
        }
    }

    // iterative decimation
    int tries_left = settings.max_tries;
    auto early_accept_error = settings.early_accept_factor * settings.max_error;
    while (tries_left-- > 0)
    {
        // find best edge to decimate
        Pos3 min_q;
        halfedge_index min_h;
        auto min_err = std::numeric_limits<float>::max();
        int min_i = -1;
        {
            for (auto i = 0; i < settings.choices_per_it; ++i)
            {
                if (candidates.empty())
                    break; // cannot decimate further

                auto ci = (int)(detail::xorshift64star(rng) % candidates.size());
                auto h = candidates[ci];

                // check if candidate must be removed
                if (is_candidate_permanently_invalid(h.of(m)))
                {
                    std::swap(candidates[ci], candidates.back());
                    candidates.pop_back();
                    --i;
                    continue;
                }

                // check if collapse is valid
                candidate_info info;
                if (!check_candidate_info(h, settings.max_error, info))
                    continue; // candidate invalid

                // check if collapse is better
                if (info.err < min_err)
                {
                    min_err = info.err;
                    min_h = h;
                    min_i = ci;
                    min_q = info.q_min;

                    if (min_err < early_accept_error)
                        break; // early accept
                }
            }
        }

        if (min_i == -1)
            continue; // no candidate found

        std::cout << "collapse " << min_i << ", err " << min_err << std::endl;

        // remove candidate
        std::swap(candidates[min_i], candidates.back());
        candidates.pop_back();

        // collapse halfedge
        collapse_halfedge(min_h, min_q);

        // reset counter
        tries_left = settings.max_tries;
    }
}
}
