#pragma once

#include <polymesh/Mesh.hh>
#include "../detail/primitive_set.hh"
#include "../detail/topology_iterator.hh"

namespace polymesh
{
/// Calculates the number of connected components based on vertex connectivity
/// (reports wiremeshes as connected)
/// Returns a vertex attribute for 0-based per-vertex component
/// Optionally returns the total number of components in `comps`
vertex_attribute<int> vertex_components(Mesh const& m, int* comps = nullptr);

/// Calculates the number of connected components based on face connectivity
/// (only counts face-edge-face as connected)
/// Returns a face attribute for 0-based per-face component
/// Optionally returns the total number of components in `comps`
face_attribute<int> face_components(Mesh const& m, int* comps = nullptr);

/// returns a range that iterates over all connected vertices in BFS order
detail::bfs_range<vertex_tag> vertex_component(vertex_handle v);

/// returns a range that iterates over all connected faces in BFS order
detail::bfs_range<face_tag> face_component(face_handle f);

/// returns a range that iterates over all connected faces in BFS order
detail::bfs_range<face_tag> face_component(vertex_handle v);

// ======== IMPLEMENTATION ========

inline detail::bfs_range<vertex_tag> vertex_component(vertex_handle v) { return {v}; }
inline detail::bfs_range<face_tag> face_component(face_handle f) { return {f}; }
inline detail::bfs_range<face_tag> face_component(vertex_handle v)
{
    for (auto f : v.faces())
        return {f};
    return {face_handle()};
}
}
