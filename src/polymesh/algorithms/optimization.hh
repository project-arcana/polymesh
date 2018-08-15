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
    if (m.faces().empty())
        return {};
    assert(m.faces().size() == m.all_faces().size() && "non-compact currently not supported");

    polymesh::detail::disjoint_set clusters(m.all_faces().size());

    std::vector<std::pair<float, std::pair<int, int>>> edges;
    for (auto e : m.edges())
        edges.push_back({-1, {(int)e.faceA(), (int)e.faceB()}});

    struct node
    {
        int rep;
        std::vector<node*> children;

        bool is_leaf() const { return children.empty(); }

        void assign_idx(int& next_idx, std::vector<int>& indices) const
        {
            if (is_leaf())
            {
                indices[rep] = next_idx++;
            }
            else
            {
                for (auto n : children)
                    n->assign_idx(next_idx, indices);
            }
        }

        ~node()
        {
            for (auto n : children)
                delete n;
        }
    };

    std::map<int, node*> cluster_centers;
    for (auto f : m.faces())
        cluster_centers[(int)f] = new node{(int)f};

    std::map<std::pair<int, int>, float> cluster_neighbors;

    // bottom-up clustering
    auto cluster_limit = 1;
    while (!edges.empty())
    {
        cluster_limit *= 2;

        // merge edges where appropriate
        for (auto e : edges)
        {
            auto f0 = e.second.first;
            auto f1 = e.second.second;

            auto s0 = clusters.size_of(f0);
            auto s1 = clusters.size_of(f1);

            if (s0 + s1 <= cluster_limit)
                clusters.do_union(f0, f1);
        }

        // collect new neighbors
        cluster_neighbors.clear();
        for (auto e : edges)
        {
            auto w = e.first;
            auto f0 = clusters.find(e.second.first);
            auto f1 = clusters.find(e.second.second);

            if (f0 == f1)
                continue;

            if (f0 > f1)
                std::swap(f0, f1);

            cluster_neighbors[{f0, f1}] += w;
        }

        // create new edges
        edges.clear();
        for (auto const& kvp : cluster_neighbors)
            edges.push_back({kvp.second, kvp.first});
        sort(edges.begin(), edges.end());

        // new cluster centers
        std::map<int, node*> new_centers;
        // .. create nodes
        for (auto const& kvp : cluster_centers)
            if (clusters.is_representative(kvp.first))
                new_centers[kvp.first] = new node{kvp.first};
        // .. add children
        for (auto const& kvp : cluster_centers)
            new_centers[clusters.find(kvp.first)]->children.push_back(kvp.second);
        // .. replace old
        cluster_centers = new_centers;
    }

    // distribute indices
    std::vector<int> new_indices(m.all_faces().size());
    int next_idx = 0;
    for (auto const& kvp : cluster_centers)
        kvp.second->assign_idx(next_idx, new_indices);
    assert(next_idx == m.faces().size());

    // cleanup
    for (auto const& kvp : cluster_centers)
        delete kvp.second;

    return new_indices;
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
        face_edge_indices.emplace_back((int)f, (int)e);
    }

    // sort by face idx
    sort(face_edge_indices.begin(), face_edge_indices.end());

    // extract edge indices
    std::vector<int> permutation(face_edge_indices.size());
    for (auto i = 0u; i < face_edge_indices.size(); ++i)
        permutation[face_edge_indices[i].second] = i;

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
        permutation[vertex_edge_indices[i].second] = i;

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
        permutation[vertex_face_indices[i].second] = i;

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
        permutation[face_vertex_indices[i].second] = i;

    // apply permutation
    m.vertices().permute(permutation);
}
}
