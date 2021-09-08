#pragma once

#include <polymesh/Mesh.hh>
#include "../fields.hh"

namespace polymesh
{
template <class ScalarT>
struct normalize_result
{
    ScalarT scale = 1;
    ScalarT center_x = 0;
    ScalarT center_y = 0;
    ScalarT center_z = 0;
};

/// Applies a translation and a uniform rescaling such that the mesh is centerd at (0,0,0) and within the [-1 .. 1] cube
/// Returns scale and center so that applying scale * p + center on a normalized point p yields the original point
template <class Pos3>
auto normalize(vertex_attribute<Pos3>& pos)
{
    using ScalarT = std::decay_t<decltype(pos.first()[0])>;

    if (pos.mesh().vertices().size() == 0)
        return normalize_result<ScalarT>{};

    auto mm = pos.minmax();
    auto mi = mm.min;
    auto ma = mm.max;

    auto cx = (mi[0] + ma[0]) * ScalarT(0.5);
    auto cy = (mi[1] + ma[1]) * ScalarT(0.5);
    auto cz = (mi[2] + ma[2]) * ScalarT(0.5);

    auto sx = ma[0] - mi[0];
    auto sy = ma[1] - mi[1];
    auto sz = ma[2] - mi[2];
    auto s = std::max(sx, std::max(sy, sz)) * ScalarT(0.5);
    s = std::max(s, std::numeric_limits<ScalarT>::min());
    auto s_inv = 1 / s;
    for (auto& p : pos)
    {
        p[0] = (p[0] - cx) * s_inv;
        p[1] = (p[1] - cy) * s_inv;
        p[2] = (p[2] - cz) * s_inv;
    }
    return normalize_result<ScalarT>{s, cx, cy, cz};
}
} // namespace polymesh
