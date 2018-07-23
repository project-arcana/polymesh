#pragma once

#include <cassert>

#include "cursors.hh"

// For iterator interfaces, see http://anderberg.me/2016/07/04/c-custom-iterators/
// Note: some iterator methods are implemented at the end of Mesh.hh to ensure inlining

namespace polymesh
{
// ================= ITERATOR =================

template <typename tag>
struct valid_primitive_iterator
{
    using handle_t = typename primitive<tag>::handle;

    static const bool is_all_iterator = false;
    static const bool is_valid_iterator = true;

    valid_primitive_iterator() = default;
    valid_primitive_iterator(handle_t handle) : handle(handle) { this->handle.idx = handle.mesh->next_valid_idx_from(handle.idx); }

    handle_t operator*() const { return handle; }
    valid_primitive_iterator& operator++()
    {
        ++handle.idx.value;
        handle.idx = handle.mesh->next_valid_idx_from(handle.idx);
        return *this;
    }
    valid_primitive_iterator operator++(int) const
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(valid_primitive_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(valid_primitive_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

    static int primitive_size(Mesh const& m) { return primitive<tag>::valid_size(m); }

private:
    handle_t handle;
};

template <typename tag>
struct all_primitive_iterator
{
    using handle_t = typename primitive<tag>::handle;

    static const bool is_all_iterator = true;
    static const bool is_valid_iterator = false;

    all_primitive_iterator() = default;
    all_primitive_iterator(handle_t handle) : handle(handle) {}

    handle_t operator*() const { return handle; }
    all_primitive_iterator& operator++()
    {
        ++handle.idx.value;
        return *this;
    }
    all_primitive_iterator operator++(int) const
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(all_primitive_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx == rhs.handle.idx;
    }
    bool operator!=(all_primitive_iterator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return handle.idx != rhs.handle.idx;
    }

    static int primitive_size(Mesh const& m) { return primitive<tag>::all_size(m); }

private:
    handle_t handle;
};

// ================= CIRCULATOR =================

/// Generic half-edge circulator (via CRTP)
/// implement operator* given the current `handle`
/// implement void advance() to change `handle`
/// inherit ctor's from primitive_circulator
template<typename this_t>
struct primitive_circulator
{
    primitive_circulator() = default;
    primitive_circulator(halfedge_handle handle, bool not_at_begin) : handle(handle), not_at_begin(not_at_begin) {}

    primitive_circulator& operator++()
    {
        static_cast<this_t*>(this)->advance();
        not_at_begin = true;
        return *this;
    }
    primitive_circulator operator++(int) const
    {
        auto i = *this;
        return ++i;
    }
    bool operator==(primitive_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin == rhs.not_at_begin && handle.idx == rhs.handle.idx;
    }
    bool operator!=(primitive_circulator const& rhs) const
    {
        assert(handle.mesh == rhs.handle.mesh && "comparing iterators from different meshes");
        return not_at_begin != rhs.not_at_begin || handle.idx != rhs.handle.idx;
    }

protected:
    halfedge_handle handle;
    bool not_at_begin;
};

struct face_vertex_circulator : primitive_circulator<face_vertex_circulator>
{
    using primitive_circulator<face_vertex_circulator>::primitive_circulator;
    vertex_handle operator*() const { return handle.vertex_to(); }
    void advance() { handle = handle.next(); }
};
struct face_halfedge_circulator : primitive_circulator<face_halfedge_circulator>
{
    using primitive_circulator<face_halfedge_circulator>::primitive_circulator;
    halfedge_handle operator*() const { return handle; }
    void advance() { handle = handle.next(); }
};
struct face_edge_circulator : primitive_circulator<face_edge_circulator>
{
    using primitive_circulator<face_edge_circulator>::primitive_circulator;
    edge_handle operator*() const { return handle.edge(); }
    void advance() { handle = handle.next(); }
};
struct face_face_circulator : primitive_circulator<face_vertex_circulator>
{
    using primitive_circulator<face_vertex_circulator>::primitive_circulator;
    face_handle operator*() const { return handle.opposite_face(); }
    void advance() { handle = handle.next(); }
};

struct vertex_halfedge_out_circulator : primitive_circulator<vertex_halfedge_out_circulator>
{
    using primitive_circulator<vertex_halfedge_out_circulator>::primitive_circulator;
    halfedge_handle operator*() const { return handle; }
    void advance() { handle = handle.opposite().next(); }
};
struct vertex_halfedge_in_circulator : primitive_circulator<vertex_halfedge_in_circulator>
{
    using primitive_circulator<vertex_halfedge_in_circulator>::primitive_circulator;
    halfedge_handle operator*() const { return handle.opposite(); }
    void advance() { handle = handle.opposite().next(); }
};
struct vertex_face_circulator : primitive_circulator<vertex_face_circulator>
{
    using primitive_circulator<vertex_face_circulator>::primitive_circulator;
    face_handle operator*() const { return handle.face(); }
    void advance() { handle = handle.opposite().next(); }
};
struct vertex_vertex_circulator : primitive_circulator<vertex_vertex_circulator>
{
    using primitive_circulator<vertex_vertex_circulator>::primitive_circulator;
    vertex_handle operator*() const { return handle.vertex_to(); }
    void advance() { handle = handle.opposite().next(); }
};
struct vertex_edge_circulator : primitive_circulator<vertex_edge_circulator>
{
    using primitive_circulator<vertex_edge_circulator>::primitive_circulator;
    edge_handle operator*() const { return handle.edge(); }
    void advance() { handle = handle.opposite().next(); }
};

struct halfedge_ring_circulator : primitive_circulator<halfedge_ring_circulator>
{
    using primitive_circulator<halfedge_ring_circulator>::primitive_circulator;
    halfedge_handle operator*() const { return handle; }
    void advance() { handle = handle.next(); }
};
}
