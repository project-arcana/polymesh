#pragma once

#include "fwd.hh"

namespace polymesh
{
template<typename tag>
struct primitive;

template<>
struct primitive<vertex_tag>
{
    using index = vertex_index;
    using handle = vertex_handle;

    template<class AttrT>
    using attribute = vertex_attribute<AttrT>;
};

template<>
struct primitive<face_tag>
{
    using index = face_index;
    using handle = face_handle;

    template<class AttrT>
    using attribute = face_attribute<AttrT>;
};

template<>
struct primitive<edge_tag>
{
    using index = edge_index;
    using handle = edge_handle;

    template<class AttrT>
    using attribute = edge_attribute<AttrT>;
};

template<>
struct primitive<halfedge_tag>
{
    using index = halfedge_index;
    using handle = halfedge_handle;

    template<class AttrT>
    using attribute = halfedge_attribute<AttrT>;
};
}
