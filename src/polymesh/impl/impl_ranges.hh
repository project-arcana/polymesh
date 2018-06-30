#pragma once

#include "../Mesh.hh"

namespace polymesh
{
template <class mesh_ptr, class tag, class iterator>
int smart_collection<mesh_ptr, tag, iterator>::size() const
{
    return iterator::primitive_size(*mesh);
}

template <class mesh_ptr, class tag, class iterator>
void smart_collection<mesh_ptr, tag, iterator>::reserve(int capacity) const
{
    return primitive<tag>::reserve(*mesh, capacity);
}

template <class mesh_ptr, class tag, class iterator>
template <class PropT>
typename primitive<tag>::template attribute<PropT> smart_collection<mesh_ptr, tag, iterator>::make_attribute(PropT const &def_value)
{
    return typename primitive<tag>::template attribute<PropT>(mesh, def_value);
}

template <class mesh_ptr, class tag, class iterator>
iterator smart_collection<mesh_ptr, tag, iterator>::begin() const
{
    return {{this->mesh, primitive<tag>::index(0)}};
}

template <class mesh_ptr, class tag, class iterator>
iterator smart_collection<mesh_ptr, tag, iterator>::end() const
{
    return {{this->mesh, primitive<tag>::index(primitive<tag>::all_size(*this->mesh))}};
}

template <class iterator>
void vertex_collection<iterator>::remove(vertex_handle v) const
{
    this->mesh->remove_vertex(v.idx);
}

template <class iterator>
void face_collection<iterator>::remove(face_handle f) const
{
    this->mesh->remove_face(f.idx);
}

template <class iterator>
void edge_collection<iterator>::remove(edge_handle e) const
{
    this->mesh->remove_edge(e.idx);
}

template <class iterator>
void halfedge_collection<iterator>::remove_edge(halfedge_handle h) const
{
    this->mesh->remove_edge(this->mesh->edge_of(h.idx));
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

template <class iterator>
vertex_handle vertex_collection<iterator>::add() const
{
    return this->mesh->handle_of(this->mesh->add_vertex());
}

template <class iterator>
face_handle face_collection<iterator>::add(const vertex_handle *v_handles, int vcnt) const
{
    return this->mesh->handle_of(this->mesh->add_face(v_handles, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(const halfedge_handle *half_loop, int vcnt) const
{
    return this->mesh->handle_of(this->mesh->add_face(half_loop, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<vertex_handle> const &v_handles) const
{
    return add(v_handles.data(), v_handles.size());
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<halfedge_handle> const &half_loop) const
{
    return add(half_loop.data(), (int)half_loop.size());
}

template <class iterator>
face_handle face_collection<iterator>::add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    halfedge_index hs[3] = {
        this->mesh->add_or_get_halfedge(v0.idx, v1.idx), //
        this->mesh->add_or_get_halfedge(v1.idx, v2.idx), //
        this->mesh->add_or_get_halfedge(v2.idx, v0.idx), //
    };
    return this->mesh->handle_of(this->mesh->add_face(hs, 3));
}

template <class iterator>
face_handle face_collection<iterator>::add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    halfedge_index hs[4] = {
        this->mesh->add_or_get_halfedge(v0.idx, v1.idx), //
        this->mesh->add_or_get_halfedge(v1.idx, v2.idx), //
        this->mesh->add_or_get_halfedge(v2.idx, v3.idx), //
        this->mesh->add_or_get_halfedge(v3.idx, v0.idx), //
    };
    return this->mesh->handle_of(this->mesh->add_face(hs, 4));
}

template <class iterator>
face_handle face_collection<iterator>::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const
{
    halfedge_index hs[3] = {h0.idx, h1.idx, h2.idx};
    return this->mesh->handle_of(this->mesh->add_face(hs, 3));
}

template <class iterator>
face_handle face_collection<iterator>::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const
{
    halfedge_index hs[4] = {h0.idx, h1.idx, h2.idx, h3.idx};
    return this->mesh->handle_of(this->mesh->add_face(hs, 4));
}

template <class iterator>
template <size_t N>
face_handle face_collection<iterator>::add(const vertex_handle (&v_handles)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0; i < N; ++i)
        hs[i] = this->mesh->find_halfedge(v_handles[i].idx, v_handles[(i + 1) % N].idx);
    return this->mesh->handle_of(this->mesh->add_face(hs, N));
}

template <class iterator>
template <size_t N>
face_handle face_collection<iterator>::add(const halfedge_handle (&half_loop)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0; i < N; ++i)
        hs[i] = half_loop[i].idx;
    return this->mesh->handle_of(this->mesh->add_face(hs, N));
}

template <class iterator>
edge_handle edge_collection<iterator>::add_or_get(vertex_handle v_from, vertex_handle v_to)
{
    return this->mesh->handle_of(this->mesh->add_or_get_edge(v_from.idx, v_to.idx));
}

template <class iterator>
halfedge_handle halfedge_collection<iterator>::add_or_get(vertex_handle v_from, vertex_handle v_to)
{
    return this->mesh->handle_of(this->mesh->add_or_get_halfedge(v_from.idx, v_to.idx));
}
}
