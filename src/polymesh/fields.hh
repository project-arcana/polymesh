#pragma once

#include <cmath>
#include <cstddef>
#include <utility>

namespace polymesh
{
/// Type trait for 3D vector types
template <class Vec3>
struct field_3d
{
    using Point = Vec3;
    using Scalar = typename std::decay<decltype(std::declval<Vec3>()[0])>::type;

    constexpr static Point make(Scalar x, Scalar y, Scalar z) { return Point(x, y, z); }
    constexpr static Point zero() { return make(0, 0, 0); }

    constexpr static Scalar dot(Vec3 const& a, Vec3 const& b)
    {
        return a[0] * b[0] + //
               a[1] * b[1] + //
               a[2] * b[2];
    }

    constexpr static Point cross(Vec3 const& a, Vec3 const& b)
    {
        return Point(a[1] * b[2] - a[2] * b[1], //
                     a[2] * b[0] - a[0] * b[2], //
                     a[0] * b[1] - a[1] * b[0]);
    }

    constexpr static Scalar length(Vec3 const& a) { return std::sqrt(dot(a, a)); }

    template <class T>
    constexpr static Scalar scalar(T const& t)
    {
        return static_cast<Scalar>(t);
    }
};
}