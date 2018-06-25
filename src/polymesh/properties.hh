#pragma once

#include <cstddef>
#include <vector>

#include "cursors.hh"
#include "property_base.hh"

/** Properties
 *
 * Golden rule:
 *  - the Mesh must always outlive the property!
 *
 * Create properties:
 *   auto myProp = mesh.vertices().make_property(0.0f);
 *
 * Access properties:
 *   vertex_handle v; // or _index
 *   v[myProp] = 7;
 *   myProp[v] = 7;
 *
 * TODO: correct copy and move ctors/assignments
 */

namespace polymesh
{
template <typename PropT>
struct vertex_property : vertex_property_base
{
    // data access
public:
    PropT& operator[](vertex_handle v) { return mData[v.idx.value]; }
    PropT const& operator[](vertex_handle v) const { return mData[v.idx.value]; }
    PropT& operator[](vertex_index v) { return mData[v.value]; }
    PropT const& operator[](vertex_index v) const { return mData[v.value]; }

    PropT* data() { return mData.data(); }
    PropT const* data() const { return mData.data(); }
    size_t size() const;

    // methods
public:
    void clear(PropT const& value);
    void clear();

    // data
private:
    std::vector<PropT> mData;
    PropT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }

    // ctor
private:
    vertex_property(Mesh const* mesh, PropT const& def_value);
    friend class vertex_collection;
};

template <typename PropT>
struct face_property : face_property_base
{
    // data access
public:
    PropT& operator[](face_handle v) { return mData[v.idx.value]; }
    PropT const& operator[](face_handle v) const { return mData[v.idx.value]; }
    PropT& operator[](face_index v) { return mData[v.value]; }
    PropT const& operator[](face_index v) const { return mData[v.value]; }

    PropT* data() { return mData.data(); }
    PropT const* data() const { return mData.data(); }
    size_t size() const;

    // methods
public:
    void clear(PropT const& value);
    void clear();

    // data
private:
    std::vector<PropT> mData;
    PropT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }

    // ctor
private:
    face_property(Mesh const* mesh, PropT const& def_value);
    friend class face_collection;
};

template <typename PropT>
struct edge_property : edge_property_base
{
    // data access
public:
    PropT& operator[](edge_handle v) { return mData[v.idx.value]; }
    PropT const& operator[](edge_handle v) const { return mData[v.idx.value]; }
    PropT& operator[](edge_index v) { return mData[v.value]; }
    PropT const& operator[](edge_index v) const { return mData[v.value]; }

    PropT* data() { return mData.data(); }
    PropT const* data() const { return mData.data(); }
    size_t size() const;

    // methods
public:
    void clear(PropT const& value);
    void clear();

    // data
private:
    std::vector<PropT> mData;
    PropT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }

    // ctor
private:
    edge_property(Mesh const* mesh, PropT const& def_value);
    friend class edge_collection;
};

template <typename PropT>
struct halfedge_property : halfedge_property_base
{
    // data access
public:
    PropT& operator[](halfedge_handle v) { return mData[v.idx.value]; }
    PropT const& operator[](halfedge_handle v) const { return mData[v.idx.value]; }
    PropT& operator[](halfedge_index v) { return mData[v.value]; }
    PropT const& operator[](halfedge_index v) const { return mData[v.value]; }

    PropT* data() { return mData.data(); }
    PropT const* data() const { return mData.data(); }
    size_t size() const;

    // methods
public:
    void clear(PropT const& value);
    void clear();

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }

    // data
private:
    std::vector<PropT> mData;
    PropT mDefaultValue;

    // ctor
private:
    halfedge_property(Mesh const* mesh, PropT const& def_value);
    friend class halfedge_collection;
};

/// ======== IMPLEMENTATION ========


/// ======== CURSOR IMPLEMENTATION ========

template <typename PropT>
PropT& face_index::operator[](face_property<PropT>& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT const& face_index::operator[](face_property<PropT> const& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT& face_handle::operator[](face_property<PropT>& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT const& face_handle::operator[](face_property<PropT> const& prop) const
{
    return prop[*this];
}

template <typename PropT>
PropT& vertex_index::operator[](vertex_property<PropT>& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT const& vertex_index::operator[](vertex_property<PropT> const& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT& vertex_handle::operator[](vertex_property<PropT>& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT const& vertex_handle::operator[](vertex_property<PropT> const& prop) const
{
    return prop[*this];
}

template <typename PropT>
PropT& edge_index::operator[](edge_property<PropT>& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT const& edge_index::operator[](edge_property<PropT> const& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT& edge_handle::operator[](edge_property<PropT>& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT const& edge_handle::operator[](edge_property<PropT> const& prop) const
{
    return prop[*this];
}

template <typename PropT>
PropT& halfedge_index::operator[](halfedge_property<PropT>& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT const& halfedge_index::operator[](halfedge_property<PropT> const& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT& halfedge_handle::operator[](halfedge_property<PropT>& prop) const
{
    return prop[*this];
}
template <typename PropT>
PropT const& halfedge_handle::operator[](halfedge_property<PropT> const& prop) const
{
    return prop[*this];
}
}
