#pragma once

#include <unordered_map>

#include <polymesh/Mesh.hh>

#include "operations.hh"

namespace polymesh
{
/// Merges vertices that report the same key
///
/// Example usage:
///     Mesh m;
///     vertex_attribute<glm::vec3> pos;
///     load_stl(file, m, pos);
///     deduplicate(m, pos);
///
/// Note:
///     preserves (first) vertex and face attributes ONLY!
///     edge/halfedge attributes are UNDEFINED (will probably contain uncorrelated old data)
///
/// CAUTION: currently only works on faces and will remove isolated vertices/edges
///
/// returns number of removed vertices (-1 if deduplication failed (e.g. due to non-manifoldness))
///
/// TODO: use a function_ref and implement this in a .cc
template <class KeyF>
int deduplicate(Mesh& m, KeyF&& kf);

// ======== IMPLEMENTATION ========

template <class KeyF>
int deduplicate(Mesh& m, KeyF&& kf)
{
    using KeyT = typename std::decay<decltype(kf(m.vertices().first()))>::type;

    std::unordered_map<KeyT, vertex_index> remap;
    auto new_idx = m.vertices().make_attribute<vertex_index>();

    // calculate remapped vertices
    for (auto v : m.vertices())
    {
        auto const& k = kf(v);
        if (!remap.count(k))
            remap[k] = v;

        new_idx[v] = remap[k];
    }

    // calc face remapping
    std::vector<vertex_index> poly_verts;
    struct poly
    {
        face_index f;
        int start;
        int count;
    };
    std::vector<poly> polys;
    polys.reserve(m.faces().size());
    for (auto f : m.faces())
    {
        auto s = (int)poly_verts.size();
        for (auto v : f.vertices())
            poly_verts.push_back(new_idx[v]);
        auto e = (int)poly_verts.size();
        polys.push_back({f, s, e - s});
    }

    auto ll = low_level_api(m);

    // remove everything except vertices
    remove_edges_and_faces(m);

    // clear edge vector (new edges are allocated from idx 0)
    ll.clear_removed_edge_vector();

    // add remapped faces
    auto manifold = true;
    for (auto const& p : polys)
    {
        if (ll.can_add_face(poly_verts.data() + p.start, p.count))
            ll.add_face(poly_verts.data() + p.start, p.count, p.f);
        else
            manifold = false;
    }

    // remove duplicated vertices
    int removed = 0;
    for (auto v : m.vertices())
        if (new_idx[v] != v)
        {
            m.vertices().remove(v);
            ++removed;
        }

    return manifold ? removed : -1;
}
}
