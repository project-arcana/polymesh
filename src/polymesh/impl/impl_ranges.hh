#pragma once

#include "../Mesh.hh"

namespace polymesh
{

inline int primitive<vertex_tag>::all_size(Mesh const& m)
{
    return m.size_vertices();
}
inline int primitive<vertex_tag>::valid_size(Mesh const& m)
{
    return m.size_valid_vertices();
}
inline int primitive<face_tag>::all_size(Mesh const& m)
{
    return m.size_faces();
}
inline int primitive<face_tag>::valid_size(Mesh const& m)
{
    return m.size_valid_faces();
}
inline int primitive<edge_tag>::all_size(Mesh const& m)
{
    return m.size_edges();
}
inline int primitive<edge_tag>::valid_size(Mesh const& m)
{
    return m.size_valid_edges();
}
inline int primitive<halfedge_tag>::all_size(Mesh const& m)
{
    return m.size_halfedges();
}
inline int primitive<halfedge_tag>::valid_size(Mesh const& m)
{
    return m.size_valid_halfedges();
}

inline valid_vertex_collection Mesh::vertices()
{
    valid_vertex_collection c;
    c.mesh = this;
    return c;
}
inline valid_vertex_const_collection Mesh::vertices() const
{
    valid_vertex_const_collection c;
    c.mesh = this;
    return c;
}
inline all_vertex_collection Mesh::all_vertices()
{
    all_vertex_collection c;
    c.mesh = this;
    return c;
}
inline all_vertex_const_collection Mesh::all_vertices() const
{
    all_vertex_const_collection c;
    c.mesh = this;
    return c;
}
inline valid_face_collection Mesh::faces()
{
    valid_face_collection c;
    c.mesh = this;
    return c;
}
inline valid_face_const_collection Mesh::faces() const
{
    valid_face_const_collection c;
    c.mesh = this;
    return c;
}
inline all_face_collection Mesh::all_faces()
{
    all_face_collection c;
    c.mesh = this;
    return c;
}
inline all_face_const_collection Mesh::all_faces() const
{
    all_face_const_collection c;
    c.mesh = this;
    return c;
}
inline valid_edge_collection Mesh::edges()
{
    valid_edge_collection c;
    c.mesh = this;
    return c;
}
inline valid_edge_const_collection Mesh::edges() const
{
    valid_edge_const_collection c;
    c.mesh = this;
    return c;
}
inline all_edge_collection Mesh::all_edges()
{
    all_edge_collection c;
    c.mesh = this;
    return c;
}
inline all_edge_const_collection Mesh::all_edges() const
{
    all_edge_const_collection c;
    c.mesh = this;
    return c;
}
inline valid_halfedge_collection Mesh::halfedges()
{
    valid_halfedge_collection c;
    c.mesh = this;
    return c;
}
inline valid_halfedge_const_collection Mesh::halfedges() const
{
    valid_halfedge_const_collection c;
    c.mesh = this;
    return c;
}
inline all_halfedge_collection Mesh::all_halfedges()
{
    all_halfedge_collection c;
    c.mesh = this;
    return c;
}
inline all_halfedge_const_collection Mesh::all_halfedges() const
{
    all_halfedge_const_collection c;
    c.mesh = this;
    return c;
}


inline all_vertex_collection primitive<vertex_tag>::all_collection_of(Mesh &m)
{
    return m.all_vertices();
}

inline all_vertex_const_collection primitive<vertex_tag>::all_collection_of(const Mesh &m)
{
    return m.all_vertices();
}

inline all_face_collection primitive<face_tag>::all_collection_of(Mesh &m)
{
    return m.all_faces();
}

inline all_face_const_collection primitive<face_tag>::all_collection_of(const Mesh &m)
{
    return m.all_faces();
}

inline all_edge_collection primitive<edge_tag>::all_collection_of(Mesh &m)
{
    return m.all_edges();
}

inline all_edge_const_collection primitive<edge_tag>::all_collection_of(const Mesh &m)
{
    return m.all_edges();
}

inline all_halfedge_collection primitive<halfedge_tag>::all_collection_of(Mesh &m)
{
    return m.all_halfedges();
}

inline all_halfedge_const_collection primitive<halfedge_tag>::all_collection_of(const Mesh &m)
{
    return m.all_halfedges();
}

// - Vertices -

inline int vertex_collection::size() const
{
    return mesh->size_vertices();
}

inline void vertex_collection::reserve(int capacity) const
{
    mesh->reserve_vertices(capacity);
}

inline vertex_handle vertex_collection::add() const
{
    return mesh->handle_of(mesh->add_vertex());
}

inline all_vertex_iterator vertex_collection::begin() const
{
    return mesh->vertices_begin();
}

inline all_vertex_iterator vertex_collection::end() const
{
    return mesh->vertices_end();
}

inline int vertex_const_collection::size() const
{
    return mesh->size_vertices();
}

inline all_vertex_iterator vertex_const_collection::begin() const
{
    return mesh->vertices_begin();
}

inline all_vertex_iterator vertex_const_collection::end() const
{
    return mesh->vertices_end();
}

inline int valid_vertex_collection::size() const
{
    return mesh->size_valid_vertices();
}

inline valid_vertex_iterator valid_vertex_collection::begin() const
{
    return mesh->valid_vertices_begin();
}

inline valid_vertex_iterator valid_vertex_collection::end() const
{
    return mesh->valid_vertices_end();
}

// - Faces -

inline int face_collection::size() const
{
    return mesh->size_faces();
}

inline void face_collection::reserve(int capacity) const
{
    mesh->reserve_faces(capacity);
}

inline face_handle face_collection::add(const vertex_handle *v_handles, int vcnt) const
{
    return mesh->handle_of(mesh->add_face(v_handles, vcnt));
}

inline face_handle face_collection::add(const halfedge_handle *half_loop, int vcnt) const
{
    return mesh->handle_of(mesh->add_face(half_loop, vcnt));
}

inline face_handle face_collection::add(std::vector<vertex_handle> const &v_handles) const
{
    return add(v_handles.data(), v_handles.size());
}

inline face_handle face_collection::add(std::vector<halfedge_handle> const &half_loop) const
{
    return add(half_loop.data(), half_loop.size());
}

inline face_handle face_collection::add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    halfedge_index hs[3] = {
        mesh->add_or_get_halfedge(v0.idx, v1.idx), //
        mesh->add_or_get_halfedge(v1.idx, v2.idx), //
        mesh->add_or_get_halfedge(v2.idx, v0.idx), //
    };
    return mesh->handle_of(mesh->add_face(hs, 3));
}

inline face_handle face_collection::add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    halfedge_index hs[4] = {
        mesh->add_or_get_halfedge(v0.idx, v1.idx), //
        mesh->add_or_get_halfedge(v1.idx, v2.idx), //
        mesh->add_or_get_halfedge(v2.idx, v3.idx), //
        mesh->add_or_get_halfedge(v3.idx, v0.idx), //
    };
    return mesh->handle_of(mesh->add_face(hs, 4));
}

inline face_handle face_collection::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const
{
    halfedge_index hs[3] = {h0.idx, h1.idx, h2.idx};
    return mesh->handle_of(mesh->add_face(hs, 3));
}

inline face_handle face_collection::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const
{
    halfedge_index hs[4] = {h0.idx, h1.idx, h2.idx, h3.idx};
    return mesh->handle_of(mesh->add_face(hs, 4));
}

template <size_t N>
inline face_handle face_collection::add(const vertex_handle (&v_handles)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0; i < N; ++i)
        hs[i] = mesh->find_halfedge(v_handles[i].idx, v_handles[(i + 1) % N].idx);
    return mesh->handle_of(mesh->add_face(hs, N));
}

template <size_t N>
inline face_handle face_collection::add(const halfedge_handle (&half_loop)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0; i < N; ++i)
        hs[i] = half_loop[i].idx;
    return mesh->handle_of(mesh->add_face(hs, N));
}

inline face_iterator face_collection::begin() const
{
    return mesh->faces_begin();
}

inline face_iterator face_collection::end() const
{
    return mesh->faces_end();
}

inline int face_const_collection::size() const
{
    return mesh->size_faces();
}

inline face_iterator face_const_collection::begin() const
{
    return mesh->faces_begin();
}

inline face_iterator face_const_collection::end() const
{
    return mesh->faces_end();
}

inline int valid_face_collection::size() const
{
    return mesh->size_valid_faces();
}

inline valid_face_iterator valid_face_collection::begin() const
{
    return mesh->valid_faces_begin();
}

inline valid_face_iterator valid_face_collection::end() const
{
    return mesh->valid_faces_end();
}

// - Edges -

inline int edge_collection::size() const
{
    return mesh->size_edges();
}

inline void edge_collection::reserve(int capacity) const
{
    mesh->reserve_edges(capacity);
}

inline edge_handle edge_collection::add_or_get(vertex_handle v_from, vertex_handle v_to)
{
    return mesh->handle_of(mesh->add_or_get_edge(v_from.idx, v_to.idx));
}

inline edge_iterator edge_collection::begin() const
{
    return mesh->edges_begin();
}

inline edge_iterator edge_collection::end() const
{
    return mesh->edges_end();
}

inline int edge_const_collection::size() const
{
    return mesh->size_edges();
}

inline edge_iterator edge_const_collection::begin() const
{
    return mesh->edges_begin();
}

inline edge_iterator edge_const_collection::end() const
{
    return mesh->edges_end();
}

inline int valid_edge_collection::size() const
{
    return mesh->size_valid_edges();
}

inline valid_edge_iterator valid_edge_collection::begin() const
{
    return mesh->valid_edges_begin();
}

inline valid_edge_iterator valid_edge_collection::end() const
{
    return mesh->valid_edges_end();
}

// - Halfedges -

inline int halfedge_collection::size() const
{
    return mesh->size_halfedges();
}

inline void halfedge_collection::reserve(int capacity) const
{
    mesh->reserve_halfedges(capacity);
}

inline halfedge_handle halfedge_collection::add_or_get(vertex_handle v_from, vertex_handle v_to)
{
    return mesh->handle_of(mesh->add_or_get_halfedge(v_from.idx, v_to.idx));
}

inline halfedge_iterator halfedge_collection::begin() const
{
    return mesh->halfedges_begin();
}

inline halfedge_iterator halfedge_collection::end() const
{
    return mesh->halfedges_end();
}

inline int halfedge_const_collection::size() const
{
    return mesh->size_halfedges();
}

inline halfedge_iterator halfedge_const_collection::begin() const
{
    return mesh->halfedges_begin();
}

inline halfedge_iterator halfedge_const_collection::end() const
{
    return mesh->halfedges_end();
}

inline int valid_halfedge_collection::size() const
{
    return mesh->size_valid_halfedges();
}

inline valid_halfedge_iterator valid_halfedge_collection::begin() const
{
    return mesh->valid_halfedges_begin();
}

inline valid_halfedge_iterator valid_halfedge_collection::end() const
{
    return mesh->valid_halfedges_end();
}

inline void vertex_collection::remove(vertex_handle v) const
{
    mesh->remove_vertex(v.idx);
}

inline void face_collection::remove(face_handle f) const
{
    mesh->remove_face(f.idx);
}

inline void edge_collection::remove(edge_handle e) const
{
    mesh->remove_edge(e.idx);
}

inline void halfedge_collection::remove_edge(halfedge_handle h) const
{
    mesh->remove_edge(mesh->edge_of(h.idx));
}

}
