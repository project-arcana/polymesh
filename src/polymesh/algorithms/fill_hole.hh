#pragma once

#include <vector>

#include <polymesh/Mesh.hh>
#include <polymesh/fields.hh>

namespace polymesh
{
/// Fills a hole given by the boundary halfdedge "boundary_start" using dynamic programming to compute the area minimizing triangulation
template <class Pos3>
void fill_hole(Mesh& m, vertex_attribute<Pos3> const& position, halfedge_handle boundary_start)
{
    POLYMESH_ASSERT(boundary_start.is_boundary());

    // This is a dynamic programming approach that fills a two-dimensional table of weights.
    // The boundary is indexed by numbers ranging from 0 to n.
    // The entry weights[x,y] (accessed via weights[index_of(x,y)]) gives the minimal weight to triangulate the boudary from vertex x to vertex y.
    // Since only entries that span at least three vertices can span any triangles, only these need to be stored.
    // The other entries are implicitly zero.
    // This means, x ranges from 0 to n-1 and y ranges from 1 to n.
    // In this implementation, the weight is equal to the triangle area.
    // To extract the correct triangulation after the table of weights has been computed, a chosen triangle is also associated with each entry in the table.

    std::vector<pm::vertex_index> boundary;
    { // fill boundary
        auto current = boundary_start;
        do
        {
            boundary.push_back(current.vertex_to());
            current = current.next();
        } while (current != boundary_start);
    }

    auto const n = int(boundary.size()) - 1;
    // only entries in the lower left half of the table can have non-zero entries.
    auto const table_size = (n * (n - 1)) / 2;

    auto weights = std::vector<float>();
    auto chosen_triangle = std::vector<int>();
    weights.resize(table_size);
    chosen_triangle.resize(table_size);

    auto const index_of = [&](int x, int y) -> int {
        POLYMESH_ASSERT(0 <= x && x <= n - 1);
        POLYMESH_ASSERT(1 <= y && y <= n);
        POLYMESH_ASSERT(x < y);
        return x + ((y - 1) * (y - 2)) / 2;
    };

    auto const weight_at = [&](int x, int y) -> float {
        if (x + 1 == y) // the diagonal is 0, no need to store it
            return 0.0f;
        return weights[index_of(x, y)];
    };

    auto const triangle_at = [&](int x, int y) -> int {
        if (x + 2 == y) // only one unique triangle can be chosen here
            return x + 1;
        return chosen_triangle[index_of(x, y)];
    };

    auto const triangle_area = [&](int x, int y, int z) {
        auto const p0 = position[boundary[x]];
        auto const p1 = position[boundary[y]];
        auto const p2 = position[boundary[z]];
        return field3<Pos3>::length(field3<Pos3>::cross(p0 - p1, p0 - p2)) * field3<Pos3>::scalar(0.5f);
    };

    // initialize with triangle sizes
    for (auto i = 0; i < n - 2; ++i)
        weights[index_of(i, i + 2)] = triangle_area(i, i + 1, i + 2);

    // fill the table using dynamic programming
    for (auto d = 3; d <= n; ++d)
    {
        for (auto i = 0; i < n - d + 1; ++i)
        {
            auto const x = i;
            auto const y = d + i;

            // find the optimal triangulation for the boundary from vertex x to vertex y.

            auto min_weight = weight_at(x, x + 1) + triangle_area(x, x + 1, y) + weight_at(x + 1, y);
            int t = x + 1;
            for (auto k = 2; x + k < y; ++k)
            {
                auto const w = weight_at(x, x + k) + triangle_area(x, x + k, y) + weight_at(x + k, y);
                if (w < min_weight)
                {
                    min_weight = w;
                    t = x + k;
                }
            }

            weights[index_of(x, y)] = min_weight;
            chosen_triangle[index_of(x, y)] = t;
        }
    }

    // backtrack the chosen triangles
    std::vector<std::pair<int, int>> stack;
    stack.push_back({0, n});
    while (!stack.empty())
    {
        auto const [a, c] = stack.back();
        stack.pop_back();
        auto const b = triangle_at(a, c);
        m.faces().add(boundary[a].of(m), boundary[b].of(m), boundary[c].of(m));
        if (a + 1 < b)
            stack.push_back({a, b});
        if (b + 1 < c)
            stack.push_back({b, c});
    }
}
}
