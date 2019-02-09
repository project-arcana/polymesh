#pragma once

#include "fwd.hh"

namespace polymesh
{
template <typename tag>
struct primitive;

template <>
struct primitive<vertex_tag>
{
    static constexpr auto name = "vertex";

    using index = vertex_index;
    using handle = vertex_handle;

    using all_iterator = all_primitive_iterator<vertex_tag>;
    using all_collection = all_vertex_collection;
    using all_const_collection = all_vertex_const_collection;

    using valid_iterator = valid_primitive_iterator<vertex_tag>;
    using valid_collection = valid_vertex_collection;
    using valid_const_collection = valid_vertex_const_collection;

    template <class AttrT>
    using attribute = vertex_attribute<AttrT>;

    static int all_size(Mesh const& m);
    static int valid_size(Mesh const& m);
    static int capacity(Mesh const& m);
    static void reserve(Mesh& m, int capacity);
    static all_collection all_collection_of(Mesh& m);
    static all_const_collection all_collection_of(Mesh const& m);
    static valid_collection valid_collection_of(Mesh& m);
    static valid_const_collection valid_collection_of(Mesh const& m);
};

template <>
struct primitive<face_tag>
{
    static constexpr auto name = "face";

    using index = face_index;
    using handle = face_handle;

    using all_iterator = all_primitive_iterator<face_tag>;
    using all_collection = all_face_collection;
    using all_const_collection = all_face_const_collection;

    using valid_iterator = valid_primitive_iterator<face_tag>;
    using valid_collection = valid_face_collection;
    using valid_const_collection = valid_face_const_collection;

    template <class AttrT>
    using attribute = face_attribute<AttrT>;

    static int all_size(Mesh const& m);
    static int valid_size(Mesh const& m);
    static int capacity(Mesh const& m);
    static void reserve(Mesh& m, int capacity);
    static all_collection all_collection_of(Mesh& m);
    static all_const_collection all_collection_of(Mesh const& m);
    static valid_collection valid_collection_of(Mesh& m);
    static valid_const_collection valid_collection_of(Mesh const& m);
};

template <>
struct primitive<edge_tag>
{
    static constexpr auto name = "edge";

    using index = edge_index;
    using handle = edge_handle;

    using all_iterator = all_primitive_iterator<edge_tag>;
    using all_collection = all_edge_collection;
    using all_const_collection = all_edge_const_collection;

    using valid_iterator = valid_primitive_iterator<edge_tag>;
    using valid_collection = valid_edge_collection;
    using valid_const_collection = valid_edge_const_collection;

    template <class AttrT>
    using attribute = edge_attribute<AttrT>;

    static int all_size(Mesh const& m);
    static int valid_size(Mesh const& m);
    static int capacity(Mesh const& m);
    static void reserve(Mesh& m, int capacity);
    static all_collection all_collection_of(Mesh& m);
    static all_const_collection all_collection_of(Mesh const& m);
    static valid_collection valid_collection_of(Mesh& m);
    static valid_const_collection valid_collection_of(Mesh const& m);
};

template <>
struct primitive<halfedge_tag>
{
    static constexpr auto name = "half-edge";

    using index = halfedge_index;
    using handle = halfedge_handle;

    using all_iterator = all_primitive_iterator<halfedge_tag>;
    using all_collection = all_halfedge_collection;
    using all_const_collection = all_halfedge_const_collection;

    using valid_iterator = valid_primitive_iterator<halfedge_tag>;
    using valid_collection = valid_halfedge_collection;
    using valid_const_collection = valid_halfedge_const_collection;

    template <class AttrT>
    using attribute = halfedge_attribute<AttrT>;

    static int all_size(Mesh const& m);
    static int valid_size(Mesh const& m);
    static int capacity(Mesh const& m);
    static void reserve(Mesh& m, int capacity);
    static all_collection all_collection_of(Mesh& m);
    static all_const_collection all_collection_of(Mesh const& m);
    static valid_collection valid_collection_of(Mesh& m);
    static valid_const_collection valid_collection_of(Mesh const& m);
};
}
