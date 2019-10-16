#pragma once

// TODO: remove me (50ms)
#include <cmath>

#include <cstddef>
#include <utility>

namespace polymesh
{
namespace detail
{
template <class Pos3>
struct vec_type
{
    static auto test(Pos3 const& a, Pos3 const& b) -> decltype(a - b) { return a - b; }
    static Pos3 test(Pos3 const& a, ...) { return a; }

    using type = decltype(test(Pos3(), Pos3()));
};
} // namespace detail

/// Type trait for 3D vector types
template <class Pos3>
struct field3
{
    using pos_t = Pos3;
    using vec_t = typename detail::vec_type<Pos3>::type;
    using scalar_t = std::decay_t<decltype(std::declval<pos_t>()[0])>;

    static pos_t make_pos(scalar_t x, scalar_t y, scalar_t z)
    {
        pos_t p;
        p[0] = x;
        p[1] = y;
        p[2] = z;
        return p;
    }
    static vec_t make_vec(scalar_t x, scalar_t y, scalar_t z)
    {
        vec_t p;
        p[0] = x;
        p[1] = y;
        p[2] = z;
        return p;
    }
    static pos_t zero_pos() { return make_pos(0, 0, 0); }
    static vec_t zero_vec() { return make_vec(0, 0, 0); }

    static scalar_t dot(vec_t const& a, vec_t const& b)
    {
        return a[0] * b[0] + //
               a[1] * b[1] + //
               a[2] * b[2];
    }

    static vec_t cross(vec_t const& a, vec_t const& b)
    {
        return vec_t(a[1] * b[2] - a[2] * b[1], //
                     a[2] * b[0] - a[0] * b[2], //
                     a[0] * b[1] - a[1] * b[0]);
    }

    static scalar_t length(vec_t const& a) { return std::sqrt(dot(a, a)); }

    static scalar_t scalar(scalar_t v) { return v; }
};

template <class Pos3>
using scalar_of = typename field3<Pos3>::scalar_t;
} // namespace polymesh
