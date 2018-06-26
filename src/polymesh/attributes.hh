#pragma once

#include <cstddef>
#include <vector>

#include "cursors.hh"
#include "attribute_base.hh"

/** Attrerties
 *
 * Golden rule:
 *  - the Mesh must always outlive the attribute!
 *
 * Create attributes:
 *   auto myAttr = mesh.vertices().make_attribute(0.0f);
 *
 * Access attributes:
 *   vertex_handle v; // or _index
 *   v[myAttr] = 7;
 *   myAttr[v] = 7;
 *
 * TODO: correct copy and move ctors/assignments
 */

namespace polymesh
{
template <typename AttrT>
struct vertex_attribute : vertex_attribute_base
{
    // data access
public:
    AttrT& operator[](vertex_handle v) { return mData[v.idx.value]; }
    AttrT const& operator[](vertex_handle v) const { return mData[v.idx.value]; }
    AttrT& operator[](vertex_index v) { return mData[v.value]; }
    AttrT const& operator[](vertex_index v) const { return mData[v.value]; }

    AttrT* data() { return mData.data(); }
    AttrT const* data() const { return mData.data(); }
    size_t size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    // data
private:
    std::vector<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }
    void apply_remapping(std::vector<int> const& map) override;

    // ctor
private:
    vertex_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct vertex_collection;
};

template <typename AttrT>
struct face_attribute : face_attribute_base
{
    // data access
public:
    AttrT& operator[](face_handle v) { return mData[v.idx.value]; }
    AttrT const& operator[](face_handle v) const { return mData[v.idx.value]; }
    AttrT& operator[](face_index v) { return mData[v.value]; }
    AttrT const& operator[](face_index v) const { return mData[v.value]; }

    AttrT* data() { return mData.data(); }
    AttrT const* data() const { return mData.data(); }
    size_t size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    // data
private:
    std::vector<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }
    void apply_remapping(std::vector<int> const& map) override;

    // ctor
private:
    face_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct face_collection;
};

template <typename AttrT>
struct edge_attribute : edge_attribute_base
{
    // data access
public:
    AttrT& operator[](edge_handle v) { return mData[v.idx.value]; }
    AttrT const& operator[](edge_handle v) const { return mData[v.idx.value]; }
    AttrT& operator[](edge_index v) { return mData[v.value]; }
    AttrT const& operator[](edge_index v) const { return mData[v.value]; }

    AttrT* data() { return mData.data(); }
    AttrT const* data() const { return mData.data(); }
    size_t size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    // data
private:
    std::vector<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }
    void apply_remapping(std::vector<int> const& map) override;

    // ctor
private:
    edge_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct edge_collection;
};

template <typename AttrT>
struct halfedge_attribute : halfedge_attribute_base
{
    // data access
public:
    AttrT& operator[](halfedge_handle v) { return mData[v.idx.value]; }
    AttrT const& operator[](halfedge_handle v) const { return mData[v.idx.value]; }
    AttrT& operator[](halfedge_index v) { return mData[v.value]; }
    AttrT const& operator[](halfedge_index v) const { return mData[v.value]; }

    AttrT* data() { return mData.data(); }
    AttrT const* data() const { return mData.data(); }
    size_t size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }
    void apply_remapping(std::vector<int> const& map) override;

    // data
private:
    std::vector<AttrT> mData;
    AttrT mDefaultValue;

    // ctor
private:
    halfedge_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct halfedge_collection;
};

/// ======== IMPLEMENTATION ========

template<typename AttrT>
void vertex_attribute<AttrT>::apply_remapping(const std::vector<int> &map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template<typename AttrT>
void face_attribute<AttrT>::apply_remapping(const std::vector<int> &map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template<typename AttrT>
void edge_attribute<AttrT>::apply_remapping(const std::vector<int> &map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template<typename AttrT>
void halfedge_attribute<AttrT>::apply_remapping(const std::vector<int> &map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}

/// ======== CURSOR IMPLEMENTATION ========

template <typename AttrT>
AttrT& face_index::operator[](face_attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT const& face_index::operator[](face_attribute<AttrT> const& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT& face_handle::operator[](face_attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT const& face_handle::operator[](face_attribute<AttrT> const& attr) const
{
    return attr[*this];
}

template <typename AttrT>
AttrT& vertex_index::operator[](vertex_attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT const& vertex_index::operator[](vertex_attribute<AttrT> const& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT& vertex_handle::operator[](vertex_attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT const& vertex_handle::operator[](vertex_attribute<AttrT> const& attr) const
{
    return attr[*this];
}

template <typename AttrT>
AttrT& edge_index::operator[](edge_attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT const& edge_index::operator[](edge_attribute<AttrT> const& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT& edge_handle::operator[](edge_attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT const& edge_handle::operator[](edge_attribute<AttrT> const& attr) const
{
    return attr[*this];
}

template <typename AttrT>
AttrT& halfedge_index::operator[](halfedge_attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT const& halfedge_index::operator[](halfedge_attribute<AttrT> const& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT& halfedge_handle::operator[](halfedge_attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <typename AttrT>
AttrT const& halfedge_handle::operator[](halfedge_attribute<AttrT> const& attr) const
{
    return attr[*this];
}

}
