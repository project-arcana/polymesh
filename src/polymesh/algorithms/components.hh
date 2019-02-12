#pragma once

#include "../Mesh.hh"
#include "../detail/primitive_set.hh"
#include "../detail/topology_iterator.hh"

#include <queue>

namespace polymesh
{
/// Calculates the number of connected components based on vertex connectivity
/// (reports wiremeshes as connected)
/// Returns a vertex attribute for 0-based per-vertex component
/// Optionally returns the total number of components in `comps`
vertex_attribute<int> vertex_components(Mesh const& m, int* comps = nullptr);

/// Calculates the number of connected components based on face connectivity
/// (only counts face-edge-face as connected)
/// Returns a face attribute for 0-based per-face component
/// Optionally returns the total number of components in `comps`
face_attribute<int> face_components(Mesh const& m, int* comps = nullptr);

/// returns a range that iterates over all connected vertices in BFS order
detail::bfs_range<vertex_tag> vertex_component(vertex_handle v);

/// returns a range that iterates over all connected faces in BFS order
detail::bfs_range<face_tag> face_component(face_handle f);
detail::bfs_range<face_tag> face_component(vertex_handle v);

/// ======== IMPLEMENTATION ========

inline vertex_attribute<int> vertex_components(Mesh const& m, int* comps)
{
    auto comp = m.vertices().make_attribute_with_default(-1);

    auto c_cnt = 0;
    for (auto seed : m.vertices())
        if (comp[seed] == -1)
        {
            std::queue<vertex_index> q;
            q.push(seed);
            seed[comp] = c_cnt;

            while (!q.empty())
            {
                auto v = q.front();
                q.pop();

                for (auto vv : m[v].adjacent_vertices())
                    if (vv[comp] != c_cnt)
                    {
                        vv[comp] = c_cnt;
                        q.push(vv);
                    }
            }

            ++c_cnt;
        }

    if (comps)
        *comps = c_cnt;

    return comp;
}

inline face_attribute<int> face_components(Mesh const& m, int* comps)
{
    auto comp = m.faces().make_attribute_with_default(-1);

    auto c_cnt = 0;
    for (auto seed : m.faces())
        if (comp[seed] == -1)
        {
            std::queue<face_index> q;
            q.push(seed);
            seed[comp] = c_cnt;

            while (!q.empty())
            {
                auto f = q.front();
                q.pop();

                for (auto ff : m[f].adjacent_faces())
                    if (ff.is_valid())
                        if (ff[comp] != c_cnt)
                        {
                            ff[comp] = c_cnt;
                            q.push(ff);
                        }
            }

            ++c_cnt;
        }

    if (comps)
        *comps = c_cnt;

    return comp;
}

inline detail::bfs_range<vertex_tag> vertex_component(vertex_handle v) { return {v}; }

inline detail::bfs_range<face_tag> face_component(face_handle f) { return {f}; }
inline detail::bfs_range<face_tag> face_component(vertex_handle v)
{
    for (auto f : v.faces())
        if (f.is_valid())
            return {f};
    return {face_handle()};
}
} // namespace polymesh
