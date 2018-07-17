#pragma once

#include <vector>

#include "../Mesh.hh"

namespace polymesh {

/// Given a face handle, returns the topologically farthest (but finite) face
/// (i.e. the last face that would be visited in a BFS)
face_handle farthest_face(face_handle f);

/// ======== IMPLEMENTATION ========

/*inline face_handle farthest_face(face_handle f)
{
    std::vector<face_index> q_curr;
    std::vector<face_index> q_next;

    q_curr.push_back(f.idx);

    face_handle last_f = f;

    while (!q_curr.empty())
    {
        for (auto f : q_curr)
        {
            // TODO
        }

        std::swap(q_curr, q_next);
    }

    return last_f;
}*/

}
