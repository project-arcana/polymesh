#include "triangulate.hh"

#include <polymesh/properties.hh>

#include <vector>

void polymesh::triangulate_naive(polymesh::Mesh& m)
{
    auto is_inner_valence2 = [](vertex_handle v) { return !v.is_boundary() && valence(v) == 2; };

    std::vector<vertex_handle> vs;
    for (auto f : m.faces())
    {
        vs.clear();
        f.vertices().into_vector(vs);

        if (vs.size() <= 3)
            continue;

        // find non-valence 2
        auto si = 0;
        for (auto i = 0u; i < vs.size(); ++i)
        {
            if (!is_inner_valence2(vs[i]))
            {
                si = i;
                break;
            }
        }
        POLYMESH_ASSERT(!is_inner_valence2(vs[si]) && "could not find start vertex (second vertex must not be inner valence 2)");
        si--; // make sure v1 is never valence 2
        if (si < 0)
            si += int(vs.size());

        // remove
        // NOTE: AFTER finding start idx
        m.faces().remove(f);

        // triangulate
        for (auto i = 2u; i < vs.size(); ++i)
            m.faces().add(vs[si], vs[(si + i - 1) % vs.size()], vs[(si + i) % vs.size()]);
    }
}
