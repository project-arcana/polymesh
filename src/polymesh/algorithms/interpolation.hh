#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
/**
 * Interpolation functions interpolate an attribute using neighbors and given weights
 *
 * Weighting functions are of signature (idx, handle) -> WeightT
 *
 * Weights are automatically normalized
 *
 * The kind of neighborhood is defined by the handle and the attribute
 *
 * NOTE:
 *  - interpolating halfedge attributes for a vertex using OUTGOING halfedges
 *
 * Supported combinations:
 *  vertex -> {vertex, face, edge, outgoing-halfedge}
 *  face -> {vertex, face, edge, halfedge}
 *  edge -> {vertex, face}
 *
 * Usage:
 *  auto centroid = interpolate(face, pos, 1, 1, 1)
 *
 * TODO: switch to span-based API
 */

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, std::vector<W> const& ws);

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, W const& w0, W const& w1);

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, W const& w0, W const& w1, W const& w2);

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, W const& w0, W const& w1, W const& w2, W const& w3);

template <class T, class W, class handle_t, template <class> class attr_t, size_t N>
T interpolate(handle_t h, attr_t<T> const& attr, const W (&ws)[N]);

template <class T, class W, class handle_t, template <class> class attr_t, size_t N>
T interpolate(handle_t h, attr_t<T> const& attr, std::array<W, N> const& ws);

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, W const* ws, int wcnt);

template <class T, class WeightFuncT, class handle_t, template <class> class attr_t, class Enabled = decltype(std::declval<WeightFuncT>()(0, typename primitive<typename attr_t<T>::tag_t>::handle{}))>
T interpolate(handle_t h, attr_t<T> const& attr, WeightFuncT&& wh);

// ======== IMPLEMENTATION ========

namespace detail
{
template <class T, class W, class handle_t, class tag>
T interpolate_attr(handle_t h, typename primitive<tag>::template attribute<T> const& attr, W const* ws, int wcnt);
}

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, std::vector<W> const& ws)
{
    return interpolate(h, attr, ws.data(), (int)ws.size());
}

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, W const& w0, W const& w1)
{
    W ws[] = {w0, w1};
    return interpolate(h, attr, ws, 2);
}

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, W const& w0, W const& w1, W const& w2)
{
    W ws[] = {w0, w1, w2};
    return interpolate(h, attr, ws, 3);
}

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, W const& w0, W const& w1, W const& w2, W const& w3)
{
    W ws[] = {w0, w1, w2, w3};
    return interpolate(h, attr, ws, 4);
}

template <class T, class W, class handle_t, template <class> class attr_t, size_t N>
T interpolate(handle_t h, attr_t<T> const& attr, const W (&ws)[N])
{
    return interpolate(h, attr, ws, (int)N);
}

template <class T, class W, class handle_t, template <class> class attr_t, size_t N>
T interpolate(handle_t h, attr_t<T> const& attr, std::array<W, N> const& ws)
{
    return interpolate(h, attr, ws.data(), (int)N);
}

template <class T, class W, class handle_t, template <class> class attr_t>
T interpolate(handle_t h, attr_t<T> const& attr, W const* ws, int wcnt)
{
    return detail::interpolate_attr<T, W, handle_t, typename attr_t<T>::tag_t>(h, attr, ws, wcnt);
}

// Actual implementations:
namespace detail
{
template <class T, class WeightFuncT>
T interpolate_attr(face_handle f, face_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, face_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : f.adjacent_faces())
        if (h.is_valid())
        {
            auto w = wf(idx, h);
            res += attr[h] * w;
            weight_sum += w;

            ++idx;
        }

    return idx == 0 ? res : res / weight_sum;
}
template <class T, class WeightFuncT>
T interpolate_attr(face_handle f, edge_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, edge_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : f.edges())
    {
        auto w = wf(idx, h);
        res += attr[h] * w;
        weight_sum += w;

        ++idx;
    }

    return res / weight_sum;
}
template <class T, class WeightFuncT>
T interpolate_attr(face_handle f, halfedge_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, halfedge_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : f.halfedges())
    {
        auto w = wf(idx, h);
        res += attr[h] * w;
        weight_sum += w;

        ++idx;
    }

    return res / weight_sum;
}
template <class T, class WeightFuncT>
T interpolate_attr(face_handle f, vertex_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, vertex_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : f.vertices())
    {
        auto w = wf(idx, h);
        res += attr[h] * w;
        weight_sum += w;

        ++idx;
    }

    return res / weight_sum;
}

template <class T, class WeightFuncT>
T interpolate_attr(vertex_handle v, face_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, face_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : v.faces())
        if (h.is_valid())
        {
            auto w = wf(idx, h);
            res += attr[h] * w;
            weight_sum += w;

            ++idx;
        }

    return idx == 0 ? res : res / weight_sum;
}
template <class T, class WeightFuncT>
T interpolate_attr(vertex_handle v, vertex_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, vertex_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : v.adjacent_vertices())
        if (h.is_valid())
        {
            auto w = wf(idx, h);
            res += attr[h] * w;
            weight_sum += w;

            ++idx;
        }

    return idx == 0 ? res : res / weight_sum;
}
template <class T, class WeightFuncT>
T interpolate_attr(vertex_handle v, edge_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, edge_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : v.edges())
    {
        auto w = wf(idx, h);
        res += attr[h] * w;
        weight_sum += w;

        ++idx;
    }

    return idx == 0 ? res : res / weight_sum;
}
template <class T, class WeightFuncT>
T interpolate_attr(vertex_handle v, halfedge_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, halfedge_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : v.outgoing_halfedges())
    {
        auto w = wf(idx, h);
        res += attr[h] * w;
        weight_sum += w;

        ++idx;
    }

    return idx == 0 ? res : res / weight_sum;
}

template <class T, class WeightFuncT>
T interpolate_attr(edge_handle e, face_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, face_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : {e.faceA(), e.faceB()})
    {
        auto w = wf(idx, h);
        res += attr[h] * w;
        weight_sum += w;

        ++idx;
    }

    return idx == 0 ? res : res / weight_sum;
}
template <class T, class WeightFuncT>
T interpolate_attr(edge_handle e, vertex_attribute<T> const& attr, WeightFuncT&& wf)
{
    using W = typename std::decay<decltype(wf(0, vertex_handle{}))>::type;

    auto idx = 0;
    T res = T{};
    W weight_sum = W(0);

    for (auto h : {e.vertexA(), e.vertexB()})
    {
        auto w = wf(idx, h);
        res += attr[h] * w;
        weight_sum += w;

        ++idx;
    }

    return idx == 0 ? res : res / weight_sum;
}

template <class T, class W, class handle_t, class tag>
T interpolate_attr(handle_t h, typename primitive<tag>::template attribute<T> const& attr, W const* ws, int wcnt)
{
    return interpolate_attr(h, attr, [&](int idx, typename primitive<tag>::handle) {
        if (idx >= wcnt)
            return W{};
        return ws[idx];
    });
}
}

template <class T, class WeightFuncT, class handle_t, template <class> class attr_t, class Enabled>
T interpolate(handle_t h, attr_t<T> const& attr, WeightFuncT&& wh)
{
    return detail::interpolate_attr(h, attr, wh);
}
}
