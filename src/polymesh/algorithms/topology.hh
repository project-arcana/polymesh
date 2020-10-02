#pragma once

#include <vector>

#include <polymesh/Mesh.hh>

namespace polymesh
{
/// Given a face handle, returns the topologically farthest (but finite) face
/// (i.e. the last face that would be visited in a BFS)
face_handle farthest_face(face_handle f);

// ======== IMPLEMENTATION ========

inline face_handle farthest_face(face_handle f)
{
    std::vector<face_index> q_curr;
    std::vector<face_index> q_next;

    auto const& m = *f.mesh;
    auto visited = m.faces().make_attribute(false);

    q_curr.push_back(f.idx);

    face_handle last_f = f;

    while (!q_curr.empty())
    {
        for (auto f : q_curr)
            // visit neighbors
            for (auto ff : m[f].adjacent_faces())
                if (ff.is_valid())
                {
                    if (visited[ff])
                        continue;
                    visited[ff] = true;
                    last_f = ff;

                    q_next.push_back(ff);
                }

        std::swap(q_curr, q_next);
    }

    return last_f;
}
}
