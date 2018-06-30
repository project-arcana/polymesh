#pragma once

#include "../Mesh.hh"

namespace polymesh
{
inline bool vertex_handle::is_removed() const { return idx.is_valid() && !mesh->vertex(idx).is_valid(); }
inline bool face_handle::is_removed() const { return idx.is_valid() && !mesh->face(idx).is_valid(); }
inline bool edge_handle::is_removed() const { return idx.is_valid() && !mesh->halfedge(idx, 0).is_valid(); }
inline bool halfedge_handle::is_removed() const { return idx.is_valid() && !mesh->halfedge(idx).is_valid(); }

inline bool vertex_handle::is_isolated() const { return mesh->vertex(idx).is_isolated(); }

inline bool vertex_handle::is_boundary() const
{
    auto const &v = mesh->vertex(idx);
    if (v.is_isolated())
        return true;
    return mesh->halfedge(v.outgoing_halfedge).is_free();
}

inline bool face_handle::is_boundary() const { return mesh->halfedge(mesh->opposite(mesh->face(idx).halfedge)).is_free(); }

inline bool edge_handle::is_isolated() const { return mesh->halfedge(idx, 0).is_free() && mesh->halfedge(idx, 1).is_free(); }

inline bool edge_handle::is_boundary() const { return mesh->halfedge(idx, 0).is_free() || mesh->halfedge(idx, 1).is_free(); }

inline bool halfedge_handle::is_boundary() const { return mesh->halfedge(idx).is_free(); }

inline vertex_handle halfedge_handle::vertex_to() const { return mesh->handle_of(mesh->halfedge(idx).to_vertex); }

inline vertex_handle halfedge_handle::vertex_from() const { return mesh->handle_of(mesh->halfedge(mesh->opposite(idx)).to_vertex); }

inline edge_handle halfedge_handle::edge() const { return mesh->handle_of(mesh->edge_of(idx)); }

inline face_handle halfedge_handle::face() const { return mesh->handle_of(mesh->halfedge(idx).face); }

inline halfedge_handle halfedge_handle::next() const { return mesh->handle_of(mesh->halfedge(idx).next_halfedge); }

inline halfedge_handle halfedge_handle::prev() const { return mesh->handle_of(mesh->halfedge(idx).prev_halfedge); }

inline halfedge_handle halfedge_handle::opposite() const { return mesh->handle_of(mesh->opposite(idx)); }

inline face_handle halfedge_handle::opposite_face() const { return mesh->handle_of(mesh->halfedge(mesh->opposite(idx)).face); }

inline halfedge_handle edge_handle::halfedgeA() const { return mesh->handle_of(mesh->halfedge_of(idx, 0)); }

inline halfedge_handle edge_handle::halfedgeB() const { return mesh->handle_of(mesh->halfedge_of(idx, 1)); }

inline vertex_handle edge_handle::vertexA() const { return mesh->handle_of(mesh->halfedge(mesh->halfedge_of(idx, 0)).to_vertex); }

inline vertex_handle edge_handle::vertexB() const { return mesh->handle_of(mesh->halfedge(mesh->halfedge_of(idx, 1)).to_vertex); }

inline face_handle edge_handle::faceA() const { return mesh->handle_of(mesh->halfedge(mesh->halfedge_of(idx, 0)).face); }

inline face_handle edge_handle::faceB() const { return mesh->handle_of(mesh->halfedge(mesh->halfedge_of(idx, 1)).face); }

inline face_handle vertex_handle::any_face() const
{
    auto h = mesh->vertex(idx).outgoing_halfedge;
    return mesh->handle_of(h.is_valid() ? mesh->halfedge(h).face : face_index::invalid());
}

inline face_handle vertex_handle::any_valid_face() const
{
    for (auto f : faces())
        if (f.is_valid())
            return f;
    return mesh->handle_of(face_index::invalid());
}

inline halfedge_handle vertex_handle::any_outgoing_halfedge() const { return mesh->handle_of(mesh->vertex(idx).outgoing_halfedge); }

inline halfedge_handle vertex_handle::any_incoming_halfedge() const
{
    auto h = mesh->vertex(idx).outgoing_halfedge;
    return mesh->handle_of(h.is_valid() ? mesh->opposite(h) : halfedge_index::invalid());
}

inline edge_handle vertex_handle::any_edge() const
{
    auto h = mesh->vertex(idx).outgoing_halfedge;
    return mesh->handle_of(h.is_valid() ? mesh->edge_of(h) : edge_index::invalid());
}

inline vertex_handle face_handle::any_vertex() const { return mesh->handle_of(mesh->halfedge(mesh->face(idx).halfedge).to_vertex); }

inline halfedge_handle face_handle::any_halfedge() const { return mesh->handle_of(mesh->face(idx).halfedge); }

inline face_vertex_ring face_handle::vertices() const { return {*this}; }

inline face_edge_ring face_handle::edges() const { return {*this}; }

inline face_halfedge_ring face_handle::halfedges() const { return {*this}; }

inline face_face_ring face_handle::adjacent_faces() const { return {*this}; }

inline vertex_halfedge_in_ring vertex_handle::incoming_halfedges() const { return {*this}; }

inline vertex_halfedge_out_ring vertex_handle::outgoing_halfedges() const { return {*this}; }

inline vertex_edge_ring vertex_handle::edges() const { return {*this}; }

inline vertex_face_ring vertex_handle::faces() const { return {*this}; }

inline vertex_vertex_ring vertex_handle::adjacent_vertices() const { return {*this}; }
}
