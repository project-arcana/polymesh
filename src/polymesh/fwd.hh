#pragma once

namespace polymesh
{
class Mesh;

/// a tag class used to represent the primitive type "vertex"
struct vertex_tag
{
};
/// a tag class used to represent the primitive type "face"
struct face_tag
{
};
/// a tag class used to represent the primitive type "edge"
struct edge_tag
{
};
/// a tag class used to represent the primitive type "halfedge"
struct halfedge_tag
{
};

template <class AttrT>
struct vertex_attribute;
template <class AttrT>
struct face_attribute;
template <class AttrT>
struct edge_attribute;
template <class AttrT>
struct halfedge_attribute;

template <typename tag>
struct all_primitive_iterator;
template <typename tag>
struct valid_primitive_iterator;

struct vertex_index;
struct face_index;
struct edge_index;
struct halfedge_index;

struct vertex_handle;
struct face_handle;
struct edge_handle;
struct halfedge_handle;

struct all_vertex_collection;
struct all_face_collection;
struct all_edge_collection;
struct all_halfedge_collection;

struct valid_vertex_collection;
struct valid_face_collection;
struct valid_edge_collection;
struct valid_halfedge_collection;

struct all_vertex_const_collection;
struct all_face_const_collection;
struct all_edge_const_collection;
struct all_halfedge_const_collection;

struct valid_vertex_const_collection;
struct valid_face_const_collection;
struct valid_edge_const_collection;
struct valid_halfedge_const_collection;

struct face_vertex_ring;
struct face_edge_ring;
struct face_halfedge_ring;
struct face_face_ring;
struct face_all_face_ring;

struct vertex_halfedge_out_ring;
struct vertex_halfedge_in_ring;
struct vertex_face_ring;
struct vertex_all_face_ring;
struct vertex_edge_ring;
struct vertex_vertex_ring;

struct halfedge_ring;

struct attribute_collection;
}

// alias pm
namespace pm = polymesh;
