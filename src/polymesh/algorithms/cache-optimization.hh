#pragma once

#include <vector>

#include <polymesh/Mesh.hh>

namespace polymesh
{
/// Optimizes mesh layout for face traversals
// TODO: half-edge iteration should be cache local
void optimize_for_face_traversal(Mesh& m);

/// Optimizes mesh layout for vertex traversals
// TODO: half-edge iteration should be cache local
void optimize_for_vertex_traversal(Mesh& m);

/// Optimizes mesh layout for indexed face rendering
// TODO: not implemented
void optimize_for_rendering(Mesh& m);

/// optimizes edge indices for a given face neighborhood
void optimize_edges_for_faces(Mesh& m);
/// optimizes vertex indices for a given face neighborhood
void optimize_vertices_for_faces(Mesh& m);

/// optimizes edge indices for a given vertex neighborhood
void optimize_edges_for_vertices(Mesh& m);
/// optimizes face indices for a given vertex neighborhood
void optimize_faces_for_vertices(Mesh& m);

/// Calculates a cache-coherent face layout in O(n log n) time
/// Can be applied using m.faces().permute(...)
/// Returns remapping [curr_idx] = new_idx
std::vector<int> cache_coherent_face_layout(Mesh const& m);

/// Calculates a cache-coherent vertex layout in O(n log n) time
/// Can be applied using m.vertices().permute(...)
/// Returns remapping [curr_idx] = new_idx
std::vector<int> cache_coherent_vertex_layout(Mesh const& m);
}
