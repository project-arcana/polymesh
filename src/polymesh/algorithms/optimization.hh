#pragma once

#include "../Mesh.hh"

#include "../detail/permutation.hh"
#include "../detail/random.hh"
#include "../detail/union_find.hh"

namespace polymesh
{
/// Optimizes mesh layout for face traversals
// TODO: half-edge iteration should be cache local
void optimize_for_face_traversal(Mesh& m);

/// Optimizes mesh layout for vertex traversals
// TODO: half-edge iteration should be cache local
void optimize_for_vertex_traversal(Mesh& m);

/// Optimizes mesh layout for indexed face rendering
void optimize_for_rendering(Mesh& m);

/// optimizes edge indices for a given face neighborhood
void optimize_edges_for_faces(Mesh& m);
/// optimizes vertex indices for a given face neighborhood
void optimize_vertices_for_faces(Mesh& m);

/// optimizes edge indices for a given vertex neighborhood
void optimize_edges_for_vertices(Mesh& m);
/// optimizes face indices for a given vertex neighborhood
void optimize_faces_for_vertices(Mesh& m);

/// Calculates a cache-coherent face layout in O(n log n) time
/// Can be applied using m.faces().permute(...)
/// Returns remapping [curr_idx] = new_idx
std::vector<int> cache_coherent_face_layout(Mesh const& m);

/// Calculates a cache-coherent vertex layout in O(n log n) time
/// Can be applied using m.vertices().permute(...)
/// Returns remapping [curr_idx] = new_idx
std::vector<int> cache_coherent_vertex_layout(Mesh const& m);

/// ======== IMPLEMENTATION ========

inline void optimize_for_face_traversal(Mesh& m)
{
    m.faces().permute(cache_coherent_face_layout(m));
    optimize_edges_for_faces(m);
    optimize_vertices_for_faces(m);
}

inline void optimize_for_vertex_traversal(Mesh& m)
{
    m.vertices().permute(cache_coherent_vertex_layout(m));
    optimize_edges_for_vertices(m);
    optimize_faces_for_vertices(m);
}

inline void optimize_for_rendering(Mesh& m)
{
    // TODO
    assert(0 && "TODO");
}

inline std::vector<int> cache_coherent_face_layout(Mesh const& m)
{
    // build binary tree
    // - approx min cut
    // - refine approx
    // - split

    std::vector<std::pair<int, int>> edges;
    for (auto e : m.edges())
        edges.emplace_back((int)e.faceA(), (int)e.faceB());

    // TODO

    std::vector<int> id;
    for (auto i = 0u; i < m.faces().size(); ++i)
        id.push_back(i);
    // TODO
    return id;
}

inline std::vector<int> cache_coherent_vertex_layout(Mesh const& m)
{
    assert(0 && "TODO");
    return {};
}

inline void optimize_edges_for_faces(Mesh& m)
{
    uint64_t rng = 1;

    std::vector<std::pair<int, int>> face_edge_indices;
    for (auto e : m.edges())
    {
        auto fA = e.faceA();
        auto fB = e.faceB();
        auto f = fA.is_invalid() ? fB : fB.is_invalid() ? fA : detail::xorshift64star(rng) % 2 ? fA : fB;
        face_edge_indices.emplace_back(f.idx.value, e.idx.value);
    }

    // sort by face idx
    sort(face_edge_indices.begin(), face_edge_indices.end());

    // extract edge indices
    std::vector<int> permutation(face_edge_indices.size());
    for (auto i = 0u; i < face_edge_indices.size(); ++i)
        permutation[i] = face_edge_indices[i].second;

    // apply permutation
    m.edges().permute(permutation);
}

inline void optimize_edges_for_vertices(Mesh& m)
{
    uint64_t rng = 1;

    std::vector<std::pair<int, int>> vertex_edge_indices;
    for (auto e : m.edges())
    {
        auto r = detail::xorshift64star(rng);
        vertex_edge_indices.emplace_back(r % 2 ? e.vertexA().idx.value : e.vertexB().idx.value, e.idx.value);
    }

    // sort by vertex idx
    sort(vertex_edge_indices.begin(), vertex_edge_indices.end());

    // extract edge indices
    std::vector<int> permutation(vertex_edge_indices.size());
    for (auto i = 0u; i < vertex_edge_indices.size(); ++i)
        permutation[i] = vertex_edge_indices[i].second;

    // apply permutation
    m.edges().permute(permutation);
}

inline void optimize_faces_for_vertices(Mesh& m)
{
    uint64_t rng = 1;

    std::vector<std::pair<int, int>> vertex_face_indices;
    for (auto f : m.faces())
    {
        vertex_handle vv;
        auto cnt = 0;
        for (auto v : f.vertices())
        {
            ++cnt;
            if (detail::xorshift64star(rng) % cnt == 0)
                vv = v;
        }
        vertex_face_indices.emplace_back(vv.idx.value, f.idx.value);
    }

    // sort by vertex idx
    sort(vertex_face_indices.begin(), vertex_face_indices.end());

    // extract face indices
    std::vector<int> permutation(vertex_face_indices.size());
    for (auto i = 0u; i < vertex_face_indices.size(); ++i)
        permutation[i] = vertex_face_indices[i].second;

    // apply permutation
    m.faces().permute(permutation);
}

inline void optimize_vertices_for_faces(Mesh& m)
{
    uint64_t rng = 1;

    std::vector<std::pair<int, int>> face_vertex_indices;
    for (auto v : m.vertices())
    {
        face_handle ff;
        auto cnt = 0;
        for (auto f : v.faces())
        {
            if (f.is_invalid())
                continue;

            ++cnt;
            if (detail::xorshift64star(rng) % cnt == 0)
                ff = f;
        }
        face_vertex_indices.emplace_back(ff.idx.value, v.idx.value);
    }

    // sort by face idx
    sort(face_vertex_indices.begin(), face_vertex_indices.end());

    // extract vertex indices
    std::vector<int> permutation(face_vertex_indices.size());
    for (auto i = 0u; i < face_vertex_indices.size(); ++i)
        permutation[i] = face_vertex_indices[i].second;

    // apply permutation
    m.vertices().permute(permutation);
}
}
