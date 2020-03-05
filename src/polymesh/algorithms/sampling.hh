#pragma once

#include <iostream>

#include <random>
#include <vector>

#include <polymesh/Mesh.hh>
#include <polymesh/assert.hh>

#include <polymesh/properties.hh>

namespace polymesh
{
template <class Pos3>
void add_uniform_samples(std::vector<Pos3>& output, vertex_attribute<Pos3> const& pos, int count, size_t seed = 0xDEADBEEF)
{
    Mesh const& m = pos.mesh();
    POLYMESH_ASSERT(is_triangle_mesh(m) && "only supported for trimeshes");

    using T = typename field3<Pos3>::scalar_t;

    std::default_random_engine rng;
    rng.seed(seed);
    auto bary_dis = std::uniform_real_distribution<float>(0.0f, 1.0f);

    // alloc count
    auto const bi = output.size();
    output.resize(output.size() + count);

    // compute prefix sum of areas
    std::vector<double> next_area;
    double area_sum = 0;
    for (auto const f : m.faces())
    {
        auto const a = triangle_area(f, pos);
        area_sum += a;
        next_area.emplace_back(area_sum);
    }

    // generate samples
    auto fi = 0;
    for (auto i = 0; i < count; ++i)
    {
        auto const area = (i + 0.5) / count * area_sum;
        while (fi + 1 < int(next_area.size()) && next_area[fi] < area)
            ++fi;

        auto const f = m.faces()[fi];
        auto const [p0, p1, p2] = f.vertices().to_array<3>(pos);

        auto a = bary_dis(rng);
        auto b = bary_dis(rng);
        if (a + b > 1)
        {
            a = 1 - a;
            b = 1 - b;
        }
        auto const c = 1 - a - b;

        auto const p = (p0 * a + p1 * b + p2 * c) / T(1);
        output[bi + i] = p;
    }
}

template <class Pos3>
std::vector<Pos3> uniform_samples(vertex_attribute<Pos3> const& pos, int count, size_t seed = 0xDEADBEEF)
{
    std::vector<Pos3> v;
    add_uniform_samples(v, pos, count, seed);
    return v;
}
}
