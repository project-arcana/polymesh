#pragma once

namespace polymesh
{
class Mesh;

struct vertex_tag;
struct face_tag;
struct edge_tag;
struct halfedge_tag;

template <class PropT>
struct vertex_attribute;
template <class PropT>
struct face_attribute;
template <class PropT>
struct edge_attribute;
template <class PropT>
struct halfedge_attribute;

struct vertex_index;
struct face_index;
struct edge_index;
struct halfedge_index;

struct vertex_handle;
struct face_handle;
struct edge_handle;
struct halfedge_handle;

struct face_vertex_ring;
struct face_edge_ring;
struct face_halfedge_ring;
struct face_face_ring;

struct vertex_halfedge_out_ring;
struct vertex_halfedge_in_ring;
struct vertex_face_ring;
struct vertex_edge_ring;
struct vertex_vertex_ring;
}
