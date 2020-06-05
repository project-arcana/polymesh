#include "ico_sphere.hh"

#include <array>
#include <cmath>
#include <map>
#include <vector>

namespace
{
polymesh::detail::pos3f slerp(polymesh::detail::pos3f const& x, polymesh::detail::pos3f const& y, float a)
{
    float cos_alpha = x.x * y.x + x.y * y.y + x.z * y.z;
    float alpha = std::acos(cos_alpha);
    float sin_alpha = std::sin(alpha);
    float t1 = std::sin((1.0f - a) * alpha) / sin_alpha;
    float t2 = std::sin(a * alpha) / sin_alpha;
    return {x.x * t1 + y.x * t2, x.y * t1 + y.y * t2, x.z * t1 + y.z * t2};
}
}

polymesh::unique_array<polymesh::detail::pos3f> polymesh::detail::add_ico_sphere_impl(polymesh::Mesh& m, int subdiv)
{
    auto const start_vertex_index = m.all_vertices().size();
    auto const total_positions = 12 + 30 * subdiv + 20 * subdiv * (subdiv - 1) / 2;
    auto position = unique_array<pos3f>(total_positions);

    int next_vertex_index = 0;
    auto const add_vertex = [&]() {
        m.vertices().add();
        return next_vertex_index++;
    };

    // base icosahedron
    position[add_vertex()] = {0.000000f, -1.000000f, 0.000000f};
    position[add_vertex()] = {0.723600f, -0.447215f, 0.525720f};
    position[add_vertex()] = {-0.276385f, -0.447215f, 0.850640f};
    position[add_vertex()] = {-0.894425f, -0.447215f, 0.000000f};
    position[add_vertex()] = {-0.276385f, -0.447215f, -0.850640f};
    position[add_vertex()] = {0.723600f, -0.447215f, -0.525720f};
    position[add_vertex()] = {0.276385f, 0.447215f, 0.850640f};
    position[add_vertex()] = {-0.723600f, 0.447215f, 0.525720f};
    position[add_vertex()] = {-0.723600f, 0.447215f, -0.525720f};
    position[add_vertex()] = {0.276385f, 0.447215f, -0.850640f};
    position[add_vertex()] = {0.894425f, 0.447215f, 0.000000f};
    position[add_vertex()] = {0.000000f, 1.000000f, 0.000000f};

    auto const indices = std::array{1, 2,  3, 2, 1, 6, 1, 3, 4, 1, 4, 5,  1, 5,  6,  2, 6,  11, 3, 2, 7,  4, 3, 8,  5,  4, 9,  6,  5,  10,
                                    2, 11, 7, 3, 7, 8, 4, 8, 9, 5, 9, 10, 6, 10, 11, 7, 11, 12, 8, 7, 12, 9, 8, 12, 10, 9, 12, 11, 10, 12};

    std::map<std::pair<int, int>, std::vector<int>> edge_verts;

    auto const segments = subdiv + 1;

    // edge vertices
    for (auto i = 0; i < int(indices.size()); i += 3)
    {
        auto v0 = indices[i + 0] - 1;
        auto v1 = indices[i + 1] - 1;
        auto v2 = indices[i + 2] - 1;

        auto process = [&](int v0, int v1) {
            auto& verts = edge_verts[{v0, v1}];
            for (auto i = 0; i <= segments; ++i)
            {
                if (i == 0)
                    verts.push_back(v1);
                else if (i == segments)
                    verts.push_back(v0);
                else
                {
                    auto const v = add_vertex();
                    position[v] = slerp(position[v1], position[v0], float(i) / float(segments));
                    verts.push_back(v);
                }
            }
        };

        if (v0 < v1)
            process(v0, v1);
        if (v1 < v2)
            process(v1, v2);
        if (v2 < v0)
            process(v2, v0);
    }


    std::vector<int> verts;
    verts.resize((segments + 1) * (segments + 1));

    // face vertices
    for (auto i = 0; i < int(indices.size()); i += 3)
    {
        auto const v0 = indices[i + 0] - 1;
        auto const v1 = indices[i + 1] - 1;
        auto const v2 = indices[i + 2] - 1;
        auto const p0 = position[v0];
        auto const p1 = position[v1];
        auto const p2 = position[v2];

        auto const& ev01 = v0 < v1 ? edge_verts[{v0, v1}] : edge_verts[{v1, v0}];
        auto const& ev02 = v0 < v2 ? edge_verts[{v0, v2}] : edge_verts[{v2, v0}];
        auto const& ev12 = v1 < v2 ? edge_verts[{v1, v2}] : edge_verts[{v2, v1}];

        for (auto ia = 0; ia <= segments; ++ia)
            for (auto ib = 0; ib <= segments - ia; ++ib)
            {
                int v;
                if (ia == 0)
                {
                    if (v1 < v2)
                        v = ev12[ib];
                    else
                        v = ev12[segments - ib];
                }
                else if (ib == 0)
                {
                    if (v0 < v2)
                        v = ev02[ia];
                    else
                        v = ev02[segments - ia];
                }
                else if (ia + ib == segments)
                {
                    if (v0 < v1)
                        v = ev01[ia];
                    else
                        v = ev01[ib];
                }
                else
                {
                    v = add_vertex();

                    auto const a = float(ia) / float(segments);
                    auto const b = float(ib) / float(segments);
                    auto const c = 1.0f - a - b;
                    POLYMESH_ASSERT(a >= 0 && b >= 0 && c >= 0);

                    auto const p = slerp(p1, p0, (a / (a + b)));
                    position[v] = slerp(p, p2, (c / (a + b + c)));
                }

                verts[ib * (segments + 1) + ia] = v;
            }

        // faces
        for (auto ia = 0; ia < segments; ++ia)
            for (auto ib = 0; ib < segments - ia; ++ib)
            {
                auto v00 = vertex_index(verts[(ib + 0) * (segments + 1) + (ia + 0)] + start_vertex_index).of(m);
                auto v10 = vertex_index(verts[(ib + 1) * (segments + 1) + (ia + 0)] + start_vertex_index).of(m);
                auto v01 = vertex_index(verts[(ib + 0) * (segments + 1) + (ia + 1)] + start_vertex_index).of(m);
                auto v11 = vertex_index(verts[(ib + 1) * (segments + 1) + (ia + 1)] + start_vertex_index).of(m);

                POLYMESH_ASSERT(v00.is_valid());
                POLYMESH_ASSERT(v10.is_valid());
                POLYMESH_ASSERT(v01.is_valid());

                m.faces().add(v10, v00, v01);

                if (ia + ib + 2 <= segments)
                    m.faces().add(v11, v10, v01);
            }
    }

    return position;
}
