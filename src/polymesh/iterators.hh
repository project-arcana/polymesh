#pragma once

#include "assert.hh"
#include "cursors.hh"
#include "low_level_api.hh"

// For iterator interfaces, see http://anderberg.me/2016/07/04/c-custom-iterators/
// Note: some iterator methods are implemented at the end of Mesh.hh to ensure inlining

namespace polymesh
{
struct end_iterator // sentinel
{
};

// ================= ITERATOR =================

/// derive from smart_iterator
/// implement:
///   ElementT operator*() const // return current element
///   void advance()             // move to next entry (is never called if !valid())
///   boo is_valid() const       // return true if entries left
template <class this_t>
struct smart_iterator
{
    void operator++() { static_cast<this_t*>(this)->advance(); }
    bool operator==(end_iterator) const { return !static_cast<this_t const*>(this)->is_valid(); }
    bool operator!=(end_iterator) const { return static_cast<this_t const*>(this)->is_valid(); }
};

template <typename tag>
struct valid_primitive_iterator : smart_iterator<valid_primitive_iterator<tag>>
{
    using handle_t = typename primitive<tag>::handle;
    using index_t = typename primitive<tag>::index;

    static constexpr bool is_valid_only_iterator = true;

    valid_primitive_iterator(Mesh const& mesh, index_t begin, index_t end) : mesh(&mesh), current(begin), end(end)
    {
        current = low_level_api(mesh).next_valid_idx_from(begin);
    }

    handle_t operator*() const { return {mesh, current}; }
    void advance()
    {
        ++current.value;
        current = low_level_api(mesh).next_valid_idx_from(current);
    }
    bool is_valid() const { return current != end; }

    static int primitive_size(Mesh const& m) { return primitive<tag>::valid_size(m); }

private:
    Mesh const* mesh;
    index_t current;
    index_t end;
};

template <typename tag>
struct all_primitive_iterator : smart_iterator<all_primitive_iterator<tag>>
{
    using handle_t = typename primitive<tag>::handle;
    using index_t = typename primitive<tag>::index;

    static constexpr bool is_valid_only_iterator = false;

    all_primitive_iterator(Mesh const& mesh, index_t begin, index_t end) : mesh(&mesh), current(begin), end(end) {}

    handle_t operator*() const { return {mesh, current}; }
    void advance() { ++current.value; }
    bool is_valid() const { return current != end; }

    static int primitive_size(Mesh const& m) { return primitive<tag>::all_size(m); }

private:
    Mesh const* mesh;
    index_t current;
    index_t end;
};


// ================= ATTRIBUTES =================

template <class AttributeT>
struct attribute_iterator : smart_iterator<attribute_iterator<AttributeT>>
{
    int idx;
    int end;
    AttributeT& attr;

    attribute_iterator(int idx, int end, AttributeT& attr) : idx(idx), end(end), attr(attr) {}

    decltype(auto) operator*() const { return attr.data()[idx]; }
    void advance() { ++idx; }
    bool is_valid() const { return idx != end; }
};


// ================= FILTER + MAP =================

template <class IteratorT, class PredT>
struct filtering_iterator final : smart_iterator<filtering_iterator<IteratorT, PredT>>
{
    filtering_iterator(IteratorT it, PredT p) : it(std::forward<IteratorT>(it)), pred(std::forward<PredT>(p))
    {
        if (it.is_valid())
            advance(); // skip initially false elements
    }

    IteratorT it;
    PredT pred;

    decltype(auto) operator*() const { return *it; }
    void advance()
    {
        do
        {
            it.advance();
        } while (it.is_valid() && !pred(*it));
    }
    bool is_valid() const { return it.is_valid(); }
};

template <class IteratorT, class MapT>
struct mapped_iterator : smart_iterator<mapped_iterator<IteratorT, MapT>>
{
    IteratorT it;
    MapT map;

    decltype(auto) operator*() const { return map(*it); }
    void advance() { it.advance(); }
    bool is_valid() const { return it.is_valid(); }
};


// ================= CIRCULATOR =================

/// Generic half-edge circulator (via CRTP)
/// implement operator* given the current `handle`
/// implement void advance() to change `handle`
/// inherit ctor's from primitive_circulator
template <typename this_t>
struct primitive_circulator : smart_iterator<this_t>
{
    halfedge_handle handle;
    halfedge_index end;
    bool at_begin;

    primitive_circulator(halfedge_handle h, bool at_begin = true) : handle(h), end(h), at_begin(at_begin) {}

    bool is_valid() const { return at_begin || handle != end; }
};

struct face_vertex_circulator : primitive_circulator<face_vertex_circulator>
{
    using primitive_circulator<face_vertex_circulator>::primitive_circulator;
    vertex_handle operator*() const { return handle.vertex_to(); }
    void advance()
    {
        handle = handle.next();
        at_begin = false;
    }
};
struct face_halfedge_circulator : primitive_circulator<face_halfedge_circulator>
{
    using primitive_circulator<face_halfedge_circulator>::primitive_circulator;
    halfedge_handle operator*() const { return handle; }
    void advance()
    {
        handle = handle.next();
        at_begin = false;
    }
};
struct face_edge_circulator : primitive_circulator<face_edge_circulator>
{
    using primitive_circulator<face_edge_circulator>::primitive_circulator;
    edge_handle operator*() const { return handle.edge(); }
    void advance()
    {
        handle = handle.next();
        at_begin = false;
    }
};
struct face_face_circulator : primitive_circulator<face_face_circulator>
{
    face_face_circulator(halfedge_handle h) : primitive_circulator(h)
    {
        if (!h.opposite_face().is_valid())
            advance(); // make sure first handle is valid
    }
    face_handle operator*() const { return handle.opposite_face(); }
    void advance()
    {
        do // skip invalid faces
        {
            handle = handle.next();
        } while (handle != end && handle.opposite_face().is_invalid());
        at_begin = false;
    }
};
struct face_all_face_circulator : primitive_circulator<face_all_face_circulator>
{
    face_all_face_circulator(halfedge_handle h) : primitive_circulator(h) {}
    face_handle operator*() const { return handle.opposite_face(); }
    void advance()
    {
        handle = handle.next();
        at_begin = false;
    }
};

struct vertex_halfedge_out_circulator : primitive_circulator<vertex_halfedge_out_circulator>
{
    using primitive_circulator<vertex_halfedge_out_circulator>::primitive_circulator;
    halfedge_handle operator*() const { return handle; }
    void advance()
    {
        handle = handle.prev().opposite();
        at_begin = false;
    }
};
struct vertex_halfedge_in_circulator : primitive_circulator<vertex_halfedge_in_circulator>
{
    using primitive_circulator<vertex_halfedge_in_circulator>::primitive_circulator;
    halfedge_handle operator*() const { return handle.opposite(); }
    void advance()
    {
        handle = handle.prev().opposite();
        at_begin = false;
    }
};
struct vertex_face_circulator : primitive_circulator<vertex_face_circulator>
{
    vertex_face_circulator(halfedge_handle h, bool at_begin) : primitive_circulator(h, at_begin)
    {
        if (h.is_valid() && !h.face().is_valid())
            advance(); // make sure first handle is valid
    }
    face_handle operator*() const { return handle.face(); }
    void advance()
    {
        do // skip invalid faces
        {
            handle = handle.prev().opposite();
        } while (handle != end && handle.face().is_invalid());
        at_begin = false;
    }
};
struct vertex_all_face_circulator : primitive_circulator<vertex_all_face_circulator>
{
    vertex_all_face_circulator(halfedge_handle h, bool at_begin) : primitive_circulator(h, at_begin) {}
    face_handle operator*() const { return handle.face(); }
    void advance()
    {
        handle = handle.prev().opposite();
        at_begin = false;
    }
};
struct vertex_vertex_circulator : primitive_circulator<vertex_vertex_circulator>
{
    using primitive_circulator<vertex_vertex_circulator>::primitive_circulator;
    vertex_handle operator*() const { return handle.vertex_to(); }
    void advance()
    {
        handle = handle.prev().opposite();
        at_begin = false;
    }
};
struct vertex_edge_circulator : primitive_circulator<vertex_edge_circulator>
{
    using primitive_circulator<vertex_edge_circulator>::primitive_circulator;
    edge_handle operator*() const { return handle.edge(); }
    void advance()
    {
        handle = handle.prev().opposite();
        at_begin = false;
    }
};

struct halfedge_ring_circulator : primitive_circulator<halfedge_ring_circulator>
{
    using primitive_circulator<halfedge_ring_circulator>::primitive_circulator;
    halfedge_handle operator*() const { return handle; }
    void advance()
    {
        handle = handle.next();
        at_begin = false;
    }
};
}
