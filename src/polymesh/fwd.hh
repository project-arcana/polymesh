#pragma once

namespace polymesh
{
class Mesh;

struct vertex_tag;
struct face_tag;
struct edge_tag;
struct halfedge_tag;

template <class AttrT>
struct vertex_attribute;
template <class AttrT>
struct face_attribute;
template <class AttrT>
struct edge_attribute;
template <class AttrT>
struct halfedge_attribute;

struct vertex_index;
struct face_index;
struct edge_index;
struct halfedge_index;

struct vertex_handle;
struct face_handle;
struct edge_handle;
struct halfedge_handle;

struct vertex_collection;
struct face_collection;
struct edge_collection;
struct halfedge_collection;

struct const_vertex_collection;
struct const_face_collection;
struct const_edge_collection;
struct const_halfedge_collection;

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
