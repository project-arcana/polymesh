#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
// primitive::capacity
inline int primitive<vertex_tag>::capacity(Mesh const& m) { return low_level_api(m).capacity_vertices(); }
inline int primitive<face_tag>::capacity(Mesh const& m) { return low_level_api(m).capacity_faces(); }
inline int primitive<edge_tag>::capacity(Mesh const& m) { return low_level_api(m).capacity_halfedges() >> 1; }
inline int primitive<halfedge_tag>::capacity(Mesh const& m) { return low_level_api(m).capacity_halfedges(); }

// primitive::all_size
inline int primitive<vertex_tag>::all_size(Mesh const& m) { return low_level_api(m).size_all_vertices(); }
inline int primitive<face_tag>::all_size(Mesh const& m) { return low_level_api(m).size_all_faces(); }
inline int primitive<edge_tag>::all_size(Mesh const& m) { return low_level_api(m).size_all_edges(); }
inline int primitive<halfedge_tag>::all_size(Mesh const& m) { return low_level_api(m).size_all_halfedges(); }

// primitive::valid_size
inline int primitive<vertex_tag>::valid_size(Mesh const& m) { return low_level_api(m).size_valid_vertices(); }
inline int primitive<face_tag>::valid_size(Mesh const& m) { return low_level_api(m).size_valid_faces(); }
inline int primitive<edge_tag>::valid_size(Mesh const& m) { return low_level_api(m).size_valid_edges(); }
inline int primitive<halfedge_tag>::valid_size(Mesh const& m) { return low_level_api(m).size_valid_halfedges(); }

// primitive::reserve
inline void primitive<vertex_tag>::reserve(Mesh& m, int capacity) { low_level_api(m).reserve_vertices(capacity); }
inline void primitive<face_tag>::reserve(Mesh& m, int capacity) { low_level_api(m).reserve_faces(capacity); }
inline void primitive<edge_tag>::reserve(Mesh& m, int capacity) { low_level_api(m).reserve_edges(capacity); }
inline void primitive<halfedge_tag>::reserve(Mesh& m, int capacity) { low_level_api(m).reserve_halfedges(capacity); }

// primitive::all_collection_of
inline all_vertex_collection primitive<vertex_tag>::all_collection_of(Mesh& m) { return m.all_vertices(); }
inline all_vertex_const_collection primitive<vertex_tag>::all_collection_of(const Mesh& m) { return m.all_vertices(); }
inline all_face_collection primitive<face_tag>::all_collection_of(Mesh& m) { return m.all_faces(); }
inline all_face_const_collection primitive<face_tag>::all_collection_of(const Mesh& m) { return m.all_faces(); }
inline all_edge_collection primitive<edge_tag>::all_collection_of(Mesh& m) { return m.all_edges(); }
inline all_edge_const_collection primitive<edge_tag>::all_collection_of(const Mesh& m) { return m.all_edges(); }
inline all_halfedge_collection primitive<halfedge_tag>::all_collection_of(Mesh& m) { return m.all_halfedges(); }
inline all_halfedge_const_collection primitive<halfedge_tag>::all_collection_of(const Mesh& m) { return m.all_halfedges(); }

// primitive::valid_collection_of
inline valid_vertex_collection primitive<vertex_tag>::valid_collection_of(Mesh& m) { return m.vertices(); }
inline valid_vertex_const_collection primitive<vertex_tag>::valid_collection_of(const Mesh& m) { return m.vertices(); }
inline valid_face_collection primitive<face_tag>::valid_collection_of(Mesh& m) { return m.faces(); }
inline valid_face_const_collection primitive<face_tag>::valid_collection_of(const Mesh& m) { return m.faces(); }
inline valid_edge_collection primitive<edge_tag>::valid_collection_of(Mesh& m) { return m.edges(); }
inline valid_edge_const_collection primitive<edge_tag>::valid_collection_of(const Mesh& m) { return m.edges(); }
inline valid_halfedge_collection primitive<halfedge_tag>::valid_collection_of(Mesh& m) { return m.halfedges(); }
inline valid_halfedge_const_collection primitive<halfedge_tag>::valid_collection_of(const Mesh& m) { return m.halfedges(); }
}
