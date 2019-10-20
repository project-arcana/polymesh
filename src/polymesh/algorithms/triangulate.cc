#include "triangulate.hh"

#include <vector>

void polymesh::triangulate_naive(polymesh::Mesh& m)
{
    std::vector<vertex_handle> vs;
    for (auto f : m.faces())
    {
        vs.clear();
        f.vertices().into_vector(vs);

        if (vs.size() <= 3)
            continue;

        // remove
        m.faces().remove(f);

        // triangulate
        for (auto i = 2u; i < vs.size(); ++i)
            m.faces().add(vs[0], vs[i - 1], vs[i]);
    }
}
