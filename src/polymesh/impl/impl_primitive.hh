#pragma once

#include "../Mesh.hh"

namespace polymesh
{
// primitive::all_size
inline int primitive<vertex_tag>::all_size(Mesh const &m) { return m.size_vertices(); }
inline int primitive<face_tag>::all_size(Mesh const &m) { return m.size_faces(); }
inline int primitive<edge_tag>::all_size(Mesh const &m) { return m.size_edges(); }
inline int primitive<halfedge_tag>::all_size(Mesh const &m) { return m.size_halfedges(); }

// primitive::valid_size
inline int primitive<vertex_tag>::valid_size(Mesh const &m) { return m.size_valid_vertices(); }
inline int primitive<face_tag>::valid_size(Mesh const &m) { return m.size_valid_faces(); }
inline int primitive<edge_tag>::valid_size(Mesh const &m) { return m.size_valid_edges(); }
inline int primitive<halfedge_tag>::valid_size(Mesh const &m) { return m.size_valid_halfedges(); }

// primitive::reserve
inline void primitive<vertex_tag>::reserve(Mesh &m, int capacity) { m.reserve_vertices(capacity); }
inline void primitive<face_tag>::reserve(Mesh &m, int capacity) { m.reserve_faces(capacity); }
inline void primitive<edge_tag>::reserve(Mesh &m, int capacity) { m.reserve_edges(capacity); }
inline void primitive<halfedge_tag>::reserve(Mesh &m, int capacity) { m.reserve_halfedges(capacity); }

// primitive::all_collection_of
inline all_vertex_collection primitive<vertex_tag>::all_collection_of(Mesh &m) { return m.all_vertices(); }
inline all_vertex_const_collection primitive<vertex_tag>::all_collection_of(const Mesh &m) { return m.all_vertices(); }
inline all_face_collection primitive<face_tag>::all_collection_of(Mesh &m) { return m.all_faces(); }
inline all_face_const_collection primitive<face_tag>::all_collection_of(const Mesh &m) { return m.all_faces(); }
inline all_edge_collection primitive<edge_tag>::all_collection_of(Mesh &m) { return m.all_edges(); }
inline all_edge_const_collection primitive<edge_tag>::all_collection_of(const Mesh &m) { return m.all_edges(); }
inline all_halfedge_collection primitive<halfedge_tag>::all_collection_of(Mesh &m) { return m.all_halfedges(); }
inline all_halfedge_const_collection primitive<halfedge_tag>::all_collection_of(const Mesh &m) { return m.all_halfedges(); }
}