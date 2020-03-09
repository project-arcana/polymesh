#include "components.hh"

#include <queue>

using namespace polymesh;

vertex_attribute<int> polymesh::vertex_components(const Mesh& m, int* comps)
{
    auto comp = m.vertices().make_attribute(-1);

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

face_attribute<int> polymesh::face_components(const Mesh& m, int* comps)
{
    auto comp = m.faces().make_attribute(-1);

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
