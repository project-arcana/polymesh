#pragma once

#include "fwd.hh"

namespace polymesh
{
template<typename tag>
struct primitive;

template<>
struct primitive<vertex_tag>
{
    static constexpr auto name = "vertex";

    using index = vertex_index;
    using handle = vertex_handle;
    using collection = vertex_collection;
    using const_collection = const_vertex_collection;
    using all_iterator = all_primitive_iterator<vertex_tag>;
    using valid_iterator = valid_primitive_iterator<vertex_tag>;

    template<class AttrT>
    using attribute = vertex_attribute<AttrT>;

    static collection collection_of(Mesh& m);
    static const_collection collection_of(Mesh const& m);
};

template<>
struct primitive<face_tag>
{
    static constexpr auto name = "face";

    using index = face_index;
    using handle = face_handle;
    using collection = face_collection;
    using const_collection = const_face_collection;
    using all_iterator = all_primitive_iterator<face_tag>;
    using valid_iterator = valid_primitive_iterator<face_tag>;

    template<class AttrT>
    using attribute = face_attribute<AttrT>;

    static collection collection_of(Mesh& m);
    static const_collection collection_of(Mesh const& m);
};

template<>
struct primitive<edge_tag>
{
    static constexpr auto name = "edge";

    using index = edge_index;
    using handle = edge_handle;
    using collection = edge_collection;
    using const_collection = const_edge_collection;
    using all_iterator = all_primitive_iterator<edge_tag>;
    using valid_iterator = valid_primitive_iterator<edge_tag>;

    template<class AttrT>
    using attribute = edge_attribute<AttrT>;

    static collection collection_of(Mesh& m);
    static const_collection collection_of(Mesh const& m);
};

template<>
struct primitive<halfedge_tag>
{
    static constexpr auto name = "half-edge";

    using index = halfedge_index;
    using handle = halfedge_handle;
    using collection = halfedge_collection;
    using const_collection = const_halfedge_collection;
    using all_iterator = all_primitive_iterator<halfedge_tag>;
    using valid_iterator = valid_primitive_iterator<halfedge_tag>;

    template<class AttrT>
    using attribute = halfedge_attribute<AttrT>;

    static collection collection_of(Mesh& m);
    static const_collection collection_of(Mesh const& m);
};

}
