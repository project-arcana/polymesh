#pragma once

#include "../Mesh.hh"
#include "../fields.hh"

namespace polymesh
{
/// calculates the barycentric coordinates of a given point p within a face f
/// NOTE: asserts that f is triangular
/// NOTE: also works for other points in the same plane as f
template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
std::array<Scalar, 3> barycoords(face_handle f, vertex_attribute<Pos3> const& positions, Pos3 p);

/// ======== IMPLEMENTATION ========

template <class Pos3, class Scalar>
std::array<Scalar, 3> barycoords(face_handle f, vertex_attribute<Pos3> const& positions, Pos3 p)
{
    POLYMESH_ASSERT(f.vertices().size() == 3 && "only supports triangles");

    auto ps = f.vertices().to_array<3>(positions);

    auto e10 = ps[1] - ps[0];
    auto e21 = ps[2] - ps[1];

    auto n = field3<Pos3>::cross(e10, e21);

    auto signed_area = [&](Pos3 const& v0, Pos3 const& v1, Pos3 const& v2) {
        auto d1 = v1 - v0;
        auto d2 = v2 - v0;

        auto a = field3<Pos3>::cross(d1, d2);

        return field3<Pos3>::dot(a, n);
    };

    auto a = signed_area(ps[0], ps[1], ps[2]);
    auto a0 = signed_area(p, ps[1], ps[2]);
    auto a1 = signed_area(p, ps[2], ps[0]);
    auto a2 = signed_area(p, ps[0], ps[1]);

    auto inv_a = Scalar(1) / a;
    return {a0 * inv_a, a1 * inv_a, a2 * inv_a};
}
}
