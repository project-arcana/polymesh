#pragma once

#include "cursors.hh"

/// Bindings for property-based functions
/// Example usage:
/// auto areas = m.faces().map(fbind(face_area<glm::vec3>, pos));

namespace polymesh
{
template <class FuncT, class... Args>
auto fbind(FuncT&& f, Args... args) -> decltype(f(std::declval<face_handle>(), std::forward<Args>(args)...))
{
    return [&](face_handle h) { return f(h, std::forward<Args>(args)...); };
}

template <class FuncT, class... Args>
auto vbind(FuncT&& f, Args... args) -> decltype(f(std::declval<vertex_handle>(), std::forward<Args>(args)...))
{
    return [&](vertex_handle h) { return f(h, std::forward<Args>(args)...); };
}

template <class FuncT, class... Args>
auto ebind(FuncT&& f, Args... args) -> decltype(f(std::declval<edge_handle>(), std::forward<Args>(args)...))
{
    return [&](edge_handle h) { return f(h, std::forward<Args>(args)...); };
}

template <class FuncT, class... Args>
auto hbind(FuncT&& f, Args... args) -> decltype(f(std::declval<halfedge_handle>(), std::forward<Args>(args)...))
{
    return [&](halfedge_handle h) { return f(h, std::forward<Args>(args)...); };
}
}
