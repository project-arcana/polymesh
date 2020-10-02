#pragma once

#include <queue>

#include <polymesh/Mesh.hh>
#include "primitive_set.hh"

/// CAUTION: these iterators do NOT work like normal iterators where you can make copies!
///          they are designed for direct consumption by range-based for ONLY!
///
/// TODO: move queue into begin iterator, use sentinel for end

namespace polymesh
{
namespace detail
{
template <class tag, class range_t>
struct bfs_iterator;

template <class tag, class queue_t = std::queue<typename primitive<tag>::index>, class set_t = primitive_set<typename primitive<tag>::index>>
struct bfs_range
{
    using index_t = typename primitive<tag>::index;
    using handle_t = typename primitive<tag>::handle;

    bfs_range(handle_t h) : mesh(h.mesh)
    {
        queue.push(h);
        seen.insert(h);
    }

    bfs_iterator<tag, bfs_range> begin() { return {*this, queue.front().of(mesh)}; }
    bfs_iterator<tag, bfs_range> end() { return {*this, handle_t()}; }

    handle_t advance(handle_t curr)
    {
        POLYMESH_ASSERT(curr == queue.front() && "this iterator type can only be used in a single range-based for!");
        queue.pop();

        expand(curr);

        return queue.empty() ? handle_t() : queue.front().of(mesh);
    }

    template <class handle = handle_t, class Enabled = typename std::enable_if<std::is_same<handle, vertex_handle>::value>::type>
    void expand(vertex_handle v)
    {
        for (auto vv : v.adjacent_vertices())
            if (seen.insert(vv))
                queue.push(vv);
    }
    template <class handle = handle_t, class Enabled = typename std::enable_if<std::is_same<handle, face_handle>::value>::type>
    void expand(face_handle f)
    {
        for (auto ff : f.adjacent_faces())
            if (ff.is_valid() && seen.insert(ff))
                queue.push(ff);
    }
    template <class handle = handle_t, class Enabled = typename std::enable_if<std::is_same<handle, edge_handle>::value>::type>
    void expand(edge_handle e)
    {
        for (auto ee : e.vertexA().edges())
            if (seen.insert(ee))
                queue.push(ee);

        for (auto ee : e.vertexB().edges())
            if (seen.insert(ee))
                queue.push(ee);
    }
    template <class handle = handle_t, class Enabled = typename std::enable_if<std::is_same<handle, halfedge_handle>::value>::type>
    void expand(halfedge_handle h)
    {
        for (auto hh : h.vertex_to().outgoing_halfedges())
            if (seen.insert(hh))
                queue.push(hh);
    }

private:
    Mesh const* mesh;
    queue_t queue;
    set_t seen;
};

template <class tag, class range_t>
struct bfs_iterator
{
    using index_t = typename primitive<tag>::index;
    using handle_t = typename primitive<tag>::handle;

    range_t* range;
    handle_t handle;

    bfs_iterator() = default;
    bfs_iterator(range_t& range, handle_t handle) : range(&range), handle(handle) {}

    handle_t operator*() const { return handle; }
    bfs_iterator& operator++()
    {
        handle = range->advance(handle);
        return *this;
    }
    bfs_iterator operator++(int)
    {
        auto i = *this;
        operator++();
        return i;
    }
    bool operator==(bfs_iterator const& rhs) const { return handle.idx == rhs.handle.idx; }
    bool operator!=(bfs_iterator const& rhs) const { return handle.idx != rhs.handle.idx; }
};
}
}
