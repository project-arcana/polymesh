#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class tag>
template <class FuncT, class ResultT>
ResultT primitive_index<tag>::operator[](FuncT&& f) const
{
    return f(*static_cast<typename primitive<tag>::index const*>(this));
}

template <class tag>
template <class FuncT, class ResultT>
ResultT primitive_handle<tag>::operator[](FuncT&& f) const
{
    return f(*static_cast<typename primitive<tag>::handle const*>(this));
}

template <class tag>
template <class AttrT>
AttrT& primitive_index<tag>::operator[](attribute<AttrT>* attr) const
{
    static AttrT def_attr;
    return attr ? (*attr)[*static_cast<typename primitive<tag>::index const*>(this)] : def_attr;
}

template <class tag>
template <class AttrT>
AttrT const& primitive_index<tag>::operator[](attribute<AttrT> const* attr) const
{
    static AttrT const def_attr;
    return attr ? (*attr)[*static_cast<typename primitive<tag>::index const*>(this)] : def_attr;
}

template <class tag>
template <class AttrT>
AttrT& primitive_handle<tag>::operator[](attribute<AttrT>* attr) const
{
    static AttrT def_attr;
    return attr ? (*attr)[*static_cast<typename primitive<tag>::handle const*>(this)] : def_attr;
}

template <class tag>
template <class AttrT>
AttrT const& primitive_handle<tag>::operator[](attribute<AttrT> const* attr) const
{
    static AttrT const def_attr{};
    return attr ? (*attr)[*static_cast<typename primitive<tag>::handle const*>(this)] : def_attr;
}

inline bool vertex_handle::is_removed() const { return idx.is_valid() && low_level_api(mesh).is_removed(idx); }
inline bool face_handle::is_removed() const { return idx.is_valid() && low_level_api(mesh).is_removed(idx); }
inline bool edge_handle::is_removed() const { return idx.is_valid() && low_level_api(mesh).is_removed(idx); }
inline bool halfedge_handle::is_removed() const { return idx.is_valid() && low_level_api(mesh).is_removed(idx); }

inline bool vertex_handle::is_isolated() const { return low_level_api(mesh).is_isolated(idx); }
inline bool edge_handle::is_isolated() const { return low_level_api(mesh).is_isolated(idx); }

inline bool vertex_handle::is_boundary() const { return low_level_api(mesh).is_boundary(idx); }
inline bool face_handle::is_boundary() const { return low_level_api(mesh).is_boundary(idx); }
inline bool edge_handle::is_boundary() const { return low_level_api(mesh).is_boundary(idx); }
inline bool halfedge_handle::is_boundary() const { return low_level_api(mesh).is_boundary(idx); }

inline vertex_handle halfedge_handle::vertex_to() const { return mesh->handle_of(low_level_api(mesh).to_vertex_of(idx)); }
inline vertex_handle halfedge_handle::vertex_from() const { return mesh->handle_of(low_level_api(mesh).from_vertex_of(idx)); }
inline halfedge_handle halfedge_handle::next() const { return mesh->handle_of(low_level_api(mesh).next_halfedge_of(idx)); }
inline halfedge_handle halfedge_handle::prev() const { return mesh->handle_of(low_level_api(mesh).prev_halfedge_of(idx)); }
inline halfedge_handle halfedge_handle::opposite() const { return mesh->handle_of(low_level_api(mesh).opposite(idx)); }
inline edge_handle halfedge_handle::edge() const { return mesh->handle_of(low_level_api(mesh).edge_of(idx)); }
inline face_handle halfedge_handle::face() const { return mesh->handle_of(low_level_api(mesh).face_of(idx)); }
inline face_handle halfedge_handle::opposite_face() const { return mesh->handle_of(low_level_api(mesh).opposite_face_of(idx)); }

inline halfedge_handle edge_handle::halfedgeA() const { return mesh->handle_of(low_level_api(mesh).halfedge_of(idx, 0)); }
inline halfedge_handle edge_handle::halfedgeB() const { return mesh->handle_of(low_level_api(mesh).halfedge_of(idx, 1)); }
inline vertex_handle edge_handle::vertexA() const { return mesh->handle_of(low_level_api(mesh).to_vertex_of(idx, 0)); }
inline vertex_handle edge_handle::vertexB() const { return mesh->handle_of(low_level_api(mesh).to_vertex_of(idx, 1)); }
inline face_handle edge_handle::faceA() const { return mesh->handle_of(low_level_api(mesh).face_of(idx, 0)); }
inline face_handle edge_handle::faceB() const { return mesh->handle_of(low_level_api(mesh).face_of(idx, 1)); }

inline face_handle vertex_handle::any_face() const
{
    auto h = low_level_api(mesh).outgoing_halfedge_of(idx);
    return mesh->handle_of(h.is_valid() ? low_level_api(mesh).face_of(h) : face_index::invalid);
}

inline face_handle vertex_handle::any_valid_face() const
{
    for (auto f : faces())
        if (f.is_valid())
            return f;
    return mesh->handle_of(face_index::invalid);
}

inline halfedge_handle vertex_handle::any_outgoing_halfedge() const { return mesh->handle_of(low_level_api(mesh).outgoing_halfedge_of(idx)); }

inline halfedge_handle vertex_handle::any_incoming_halfedge() const
{
    auto h = low_level_api(mesh).outgoing_halfedge_of(idx);
    return mesh->handle_of(h.is_valid() ? low_level_api(mesh).opposite(h) : halfedge_index::invalid);
}

inline edge_handle vertex_handle::any_edge() const
{
    auto h = low_level_api(mesh).outgoing_halfedge_of(idx);
    return mesh->handle_of(h.is_valid() ? low_level_api(mesh).edge_of(h) : edge_index::invalid);
}

inline vertex_handle face_handle::any_vertex() const
{
    return mesh->handle_of(low_level_api(mesh).to_vertex_of(low_level_api(mesh).halfedge_of(idx)));
}

inline halfedge_handle face_handle::any_halfedge() const { return mesh->handle_of(low_level_api(mesh).halfedge_of(idx)); }

inline face_vertex_ring face_handle::vertices() const { return {*this}; }

inline face_edge_ring face_handle::edges() const { return {*this}; }

inline face_halfedge_ring face_handle::halfedges() const { return {*this}; }

inline face_face_ring face_handle::adjacent_faces() const { return {*this}; }

inline face_all_face_ring face_handle::all_adjacent_faces() const { return {*this}; }

inline vertex_halfedge_in_ring vertex_handle::incoming_halfedges() const { return {*this}; }

inline vertex_halfedge_out_ring vertex_handle::outgoing_halfedges() const { return {*this}; }

inline vertex_edge_ring vertex_handle::edges() const { return {*this}; }

inline vertex_face_ring vertex_handle::faces() const { return {*this}; }

inline vertex_all_face_ring vertex_handle::all_faces() const { return {*this}; }

inline vertex_vertex_ring vertex_handle::adjacent_vertices() const { return {*this}; }

inline halfedge_ring halfedge_handle::ring() const { return {*this}; }
} // namespace polymesh
