#include "cache-optimization.hh"

#include <polymesh/detail/permutation.hh>
#include <polymesh/detail/random.hh>
#include <polymesh/detail/union_find.hh>

#include <polymesh/attributes/partitioning.hh>

#include <polymesh/std/hash.hh>

#include <unordered_map>

void polymesh::optimize_for_face_traversal(polymesh::Mesh& m)
{
    m.faces().permute(cache_coherent_face_layout(m));
    optimize_edges_for_faces(m);
    optimize_vertices_for_faces(m);
}

void polymesh::optimize_for_vertex_traversal(polymesh::Mesh& m)
{
    m.vertices().permute(cache_coherent_vertex_layout(m));
    optimize_edges_for_vertices(m);
    optimize_faces_for_vertices(m);
}

void polymesh::optimize_for_rendering(polymesh::Mesh& m)
{
    // TODO
    POLYMESH_ASSERT(0 && "TODO");
}

std::vector<int> polymesh::cache_coherent_face_layout(const polymesh::Mesh& m)
{
    if (m.faces().empty())
        return {};
    POLYMESH_ASSERT(m.faces().size() == m.all_faces().size() && "non-compact currently not supported");

    auto clusters = make_partitioning(m.faces());

    std::vector<std::pair<float, std::pair<face_index, face_index>>> edges;
    for (auto e : m.edges())
        if (!e.is_boundary())
            edges.push_back({-1, {e.faceA(), e.faceB()}});

    struct node
    {
        face_index rep;
        std::vector<node*> children;

        bool is_leaf() const { return children.empty(); }

        void assign_idx(int& next_idx, std::vector<int>& indices) const
        {
            if (is_leaf())
            {
                POLYMESH_ASSERT((int)rep < (int)indices.size());
                indices[(int)rep] = next_idx++;
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

    std::unordered_map<face_index, node*> cluster_centers;
    for (auto f : m.faces())
        cluster_centers[f] = new node{f};

    int64_t fcnt = m.all_faces().size();
    std::unordered_map<int64_t, float> cluster_neighbors;

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

            auto s0 = clusters[f0].size();
            auto s1 = clusters[f1].size();

            if (s0 + s1 <= cluster_limit)
                clusters.merge(f0, f1);
        }

        // collect new neighbors
        cluster_neighbors.clear();
        for (auto e : edges)
        {
            auto w = e.first;
            auto f0 = clusters[e.second.first].root();
            auto f1 = clusters[e.second.second].root();

            if (f0 == f1)
                continue;

            if (f0 > f1)
                std::swap(f0, f1);

            cluster_neighbors[(int)f0 * fcnt + (int)f1] += w;
        }

        // create new edges
        edges.clear();
        for (auto const& kvp : cluster_neighbors)
        {
            auto f0 = face_index(int(kvp.first / fcnt));
            auto f1 = face_index(int(kvp.first % fcnt));
            edges.push_back({kvp.second, {f0, f1}});
        }
        sort(edges.begin(), edges.end());

        // new cluster centers
        std::unordered_map<face_index, node*> new_centers;
        // .. create nodes
        for (auto const& kvp : cluster_centers)
            if (clusters[kvp.first].is_root())
                new_centers[kvp.first] = new node{kvp.first};
        // .. add children
        for (auto const& kvp : cluster_centers)
            new_centers[clusters[kvp.first].root()]->children.push_back(kvp.second);
        // .. replace old
        cluster_centers = new_centers;
    }

    // distribute indices
    std::vector<int> new_indices(m.all_faces().size());
    int next_idx = 0;
    for (auto const& kvp : cluster_centers)
        kvp.second->assign_idx(next_idx, new_indices);
    POLYMESH_ASSERT(next_idx == m.faces().size());

    // cleanup
    for (auto const& kvp : cluster_centers)
        delete kvp.second;

    return new_indices;
}

std::vector<int> polymesh::cache_coherent_vertex_layout(const polymesh::Mesh& m)
{
    if (m.vertices().empty())
        return {};
    POLYMESH_ASSERT(m.vertices().size() == m.all_vertices().size() && "non-compact currently not supported");

    auto clusters = make_partitioning(m.vertices());

    std::vector<std::pair<float, std::pair<vertex_index, vertex_index>>> edges;
    for (auto e : m.edges())
        edges.push_back({-1, {e.vertexA(), e.vertexB()}});

    struct node
    {
        vertex_index rep;
        std::vector<node*> children;

        bool is_leaf() const { return children.empty(); }

        void assign_idx(int& next_idx, std::vector<int>& indices) const
        {
            if (is_leaf())
            {
                POLYMESH_ASSERT((int)rep < (int)indices.size());
                indices[(int)rep] = next_idx++;
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

    std::unordered_map<vertex_index, node*> cluster_centers;
    for (auto f : m.vertices())
        cluster_centers[f] = new node{f};

    int64_t vcnt = m.all_vertices().size();
    std::unordered_map<int64_t, float> cluster_neighbors;

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

            auto s0 = clusters[f0].size();
            auto s1 = clusters[f1].size();

            if (s0 + s1 <= cluster_limit)
                clusters.merge(f0, f1);
        }

        // collect new neighbors
        cluster_neighbors.clear();
        for (auto e : edges)
        {
            auto w = e.first;
            auto f0 = clusters[e.second.first].root();
            auto f1 = clusters[e.second.second].root();

            if (f0 == f1)
                continue;

            if (f0 > f1)
                std::swap(f0, f1);

            cluster_neighbors[(int)f0 * vcnt + (int)f1] += w;
        }

        // create new edges
        edges.clear();
        for (auto const& kvp : cluster_neighbors)
        {
            auto f0 = vertex_index(int(kvp.first / vcnt));
            auto f1 = vertex_index(int(kvp.first % vcnt));
            edges.push_back({kvp.second, {f0, f1}});
        }
        sort(edges.begin(), edges.end());

        // new cluster centers
        std::unordered_map<vertex_index, node*> new_centers;
        // .. create nodes
        for (auto const& kvp : cluster_centers)
            if (clusters[kvp.first].is_root())
                new_centers[kvp.first] = new node{kvp.first};
        // .. add children
        for (auto const& kvp : cluster_centers)
            new_centers[clusters[kvp.first].root()]->children.push_back(kvp.second);
        // .. replace old
        cluster_centers = new_centers;
    }

    // distribute indices
    std::vector<int> new_indices(m.all_vertices().size());
    int next_idx = 0;
    for (auto const& kvp : cluster_centers)
        kvp.second->assign_idx(next_idx, new_indices);
    POLYMESH_ASSERT(next_idx == m.vertices().size());

    // cleanup
    for (auto const& kvp : cluster_centers)
        delete kvp.second;

    return new_indices;
}

void polymesh::optimize_edges_for_faces(polymesh::Mesh& m)
{
    std::vector<std::pair<int, int>> face_edge_indices;
    for (auto e : m.edges())
    {
        auto fA = e.faceA();
        auto fB = e.faceB();
        auto f = fA.is_invalid() ? (int)fB : fB.is_invalid() ? (int)fA : std::min((int)fA, (int)fB);
        face_edge_indices.emplace_back(f, (int)e);
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

void polymesh::optimize_edges_for_vertices(polymesh::Mesh& m)
{
    std::vector<std::pair<int, int>> vertex_edge_indices;
    for (auto e : m.edges())
        vertex_edge_indices.emplace_back(std::min(e.vertexA().idx.value, e.vertexB().idx.value), e.idx.value);

    // sort by vertex idx
    sort(vertex_edge_indices.begin(), vertex_edge_indices.end());

    // extract edge indices
    std::vector<int> permutation(vertex_edge_indices.size());
    for (auto i = 0u; i < vertex_edge_indices.size(); ++i)
        permutation[vertex_edge_indices[i].second] = i;

    // apply permutation
    m.edges().permute(permutation);
}

void polymesh::optimize_faces_for_vertices(polymesh::Mesh& m)
{
    std::vector<std::pair<int, int>> vertex_face_indices;
    for (auto f : m.faces())
    {
        vertex_handle vv;
        auto cnt = 0;
        for (auto v : f.vertices())
        {
            ++cnt;
            if (vv.is_invalid() || (int)v < (int)vv)
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

void polymesh::optimize_vertices_for_faces(polymesh::Mesh& m)
{
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
            if (ff.is_invalid() || (int)f < (int)ff)
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
