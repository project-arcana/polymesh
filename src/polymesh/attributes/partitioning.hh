#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class tag>
struct partitioning;

template <class mesh_ptr, class tag, class iterator>
partitioning<tag> make_partitioning(smart_collection<mesh_ptr, tag, iterator> const& c);

/**
 * Datatype for disjoint subsets (union-find datastructure)
 *
 * Usage:
 *      auto p = make_partitioning(m.vertices());
 *      p.merge(v0, v1);
 *      p[v0].size();
 *      p.clear();
 */
template <class tag>
struct partitioning
{
    using index_t = typename primitive<tag>::index;
    template <class AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;

    // methods
public:
    /// merges two partitions
    /// returns true iff i and j were in different partitions before
    bool merge(index_t i, index_t j);

    /// returns the size of the partition of i
    int size_of(index_t i);

    /// returns the root element of the partition of i
    index_t root_of(index_t i);

    /// resets all partitions to singletons
    void clear();

    /// returns the number of partitions
    int size() const;

    // partition
public:
    struct partition
    {
        // methods
    public:
        /// returns the size of this partition
        int size() { return p.size_of(i); }
        /// returns the root (representative) element of this partition
        index_t root() { return p.root_of(i); }
        /// returns true iff this index is the representative
        bool is_root() { return i == root(); }
        /// merges this partition with another one
        bool merge_with(index_t j) { return p.merge(i, j); }

        /// returns true iff j belongs to the same partition
        bool operator==(index_t j) { return root() == p.root_of(j); }
        /// returns true iff j belongs to a different partition
        bool operator!=(index_t j) { return root() != p.root_of(j); }

    private:
        partitioning& p;
        index_t i;

    public:
        partition(partitioning& p, index_t i) : p(p), i(i) {}
    };

    partition operator[](index_t i) { return {*this, i}; }

    // ctor
public:
    partitioning(Mesh const& m) : parents(m), sizes(m) { clear(); }

private:
    attribute<index_t> parents;
    attribute<int> sizes;
    int partitions;
};

// ======== IMPLEMENTATION ========

template <class mesh_ptr, class tag, class iterator>
partitioning<tag> make_partitioning(smart_collection<mesh_ptr, tag, iterator> const& c)
{
    return {c.mesh()};
}

template <class tag>
bool partitioning<tag>::merge(index_t i, index_t j)
{
    auto ri = root_of(i);
    auto rj = root_of(j);

    if (ri == rj)
        return false;

    // ensure |I| >= |J|
    if (size_of(ri) < size_of(rj))
        std::swap(ri, rj);

    parents[rj] = ri;
    sizes[ri] += sizes[rj];
    --partitions;

    return true;
}

template <class tag>
int partitioning<tag>::size_of(index_t i)
{
    return sizes[root_of(i)];
}

template <class tag>
int partitioning<tag>::size() const
{
    return partitions;
}

template <class tag>
typename partitioning<tag>::index_t partitioning<tag>::root_of(index_t i)
{
    auto p = parents[i];
    if (p != i)
    {
        p = root_of(p);
        parents[i] = p;
    }
    return p;
}

template <class tag>
void partitioning<tag>::clear()
{
    auto const& m = parents.mesh();
    auto s = parents.size();
    auto ll = low_level_api(m);
    for (auto i = 0; i < s; ++i)
    {
        auto idx = index_t(i);
        parents[idx] = idx;
        sizes[idx] = ll.is_removed(idx) ? 0 : 1;
    }

    partitions = primitive<tag>::valid_size(m);
}
}
