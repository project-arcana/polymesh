#pragma once

#include "../Mesh.hh"

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

/// ======== IMPLEMENTATION ========

inline vertex_attribute<int> vertex_components(Mesh const& m, int* comps)
{
    auto comp = m.vertices().make_attribute_with_default(-1);

    auto c_cnt = 0;
    for (auto seed : m.vertices())
        if (comp[seed] == -1)
        {
            std::queue<vertex_handle> q;
            q.push(seed);
            seed[comp] = c_cnt;

            while (!q.empty())
            {
                auto v = q.front();
                q.pop();

                for (auto vv : v.adjacent_vertices())
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
            std::queue<face_handle> q;
            q.push(seed);
            seed[comp] = c_cnt;

            while (!q.empty())
            {
                auto f = q.front();
                q.pop();

                for (auto ff : f.adjacent_faces())
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
}
