#pragma once

#include "../Mesh.hh"
#include "../fields.hh"

namespace polymesh
{
/// calculates the barycentric coordinates of a given point p within a face f
/// NOTE: asserts that f is triangular
/// NOTE: also works for other points in the same plane as f
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
std::array<Scalar, 3> barycoords(face_handle f, vertex_attribute<Vec3> const& positions, Vec3 p);

/// ======== IMPLEMENTATION ========

template <class Vec3, class Scalar>
std::array<Scalar, 3> barycoords(face_handle f, vertex_attribute<Vec3> const& positions, Vec3 p)
{
    assert(f.vertices().size() == 3 && "only supports triangles");

    auto ps = f.vertices().to_array<3>(positions);

    auto e10 = ps[1] - ps[0];
    auto e21 = ps[2] - ps[1];
    auto e02 = ps[0] - ps[2];

    auto n = field_3d<Vec3>::cross(e10, e21);

    auto signed_area = [&](Vec3 const& v0, Vec3 const& v1, Vec3 const& v2) {
        auto d1 = v1 - v0;
        auto d2 = v2 - v0;

        auto a = field_3d<Vec3>::cross(d1, d2);

        return field_3d<Vec3>::dot(a, n);
    };

    auto a = signed_area(ps[0], ps[1], ps[2]);
    auto a0 = signed_area(p, ps[1], ps[2]);
    auto a1 = signed_area(p, ps[2], ps[0]);
    auto a2 = signed_area(p, ps[0], ps[1]);

    return {a0 / a, a1 / a, a2 / a};
}
}
