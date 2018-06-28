#pragma once

#include <cstddef>
#include <vector>

#include "attribute_base.hh"
#include "cursors.hh"

/** Attributes
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

    AttrT* data() { return mData; }
    AttrT const* data() const { return mData; }
    size_t size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    // data
private:
    attribute_data<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }
    void apply_remapping(std::vector<int> const& map) override;

    // ctor
private:
    vertex_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct vertex_collection;

    // move & copy
public:
    vertex_attribute(vertex_attribute const&);
    vertex_attribute(vertex_attribute&&);
    vertex_attribute& operator=(vertex_attribute const&);
    vertex_attribute& operator=(vertex_attribute&&);
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
    attribute_data<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }
    void apply_remapping(std::vector<int> const& map) override;

    // ctor
private:
    face_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct face_collection;

    // move & copy
public:
    face_attribute(face_attribute const&);
    face_attribute(face_attribute&&);
    face_attribute& operator=(face_attribute const&);
    face_attribute& operator=(face_attribute&&);
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
    attribute_data<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(size_t newSize) override { mData.resize(newSize, mDefaultValue); }
    void apply_remapping(std::vector<int> const& map) override;

    // ctor
private:
    edge_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct edge_collection;

    // move & copy
public:
    edge_attribute(edge_attribute const&);
    edge_attribute(edge_attribute&&);
    edge_attribute& operator=(edge_attribute const&);
    edge_attribute& operator=(edge_attribute&&);
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
    attribute_data<AttrT> mData;
    AttrT mDefaultValue;

    // ctor
private:
    halfedge_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct halfedge_collection;

    // move & copy
public:
    halfedge_attribute(halfedge_attribute const&);
    halfedge_attribute(halfedge_attribute&&);
    halfedge_attribute& operator=(halfedge_attribute const&);
    halfedge_attribute& operator=(halfedge_attribute&&);
};

/// ======== IMPLEMENTATION ========

template <typename AttrT>
void vertex_attribute<AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template <typename AttrT>
void face_attribute<AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template <typename AttrT>
void edge_attribute<AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template <typename AttrT>
void halfedge_attribute<AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}

template <typename AttrT>
vertex_attribute<AttrT>::vertex_attribute(vertex_attribute const& rhs) : vertex_attribute_base(rhs.mMesh) // copy
{
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <typename AttrT>
vertex_attribute<AttrT>::vertex_attribute(vertex_attribute&& rhs) : vertex_attribute_base(rhs.mMesh) // move
{
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <typename AttrT>
vertex_attribute<AttrT>& vertex_attribute<AttrT>::operator=(vertex_attribute const& rhs) // copy
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <typename AttrT>
vertex_attribute<AttrT>& vertex_attribute<AttrT>::operator=(vertex_attribute&& rhs) // move
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <typename AttrT>
face_attribute<AttrT>::face_attribute(face_attribute const& rhs) : face_attribute_base(rhs.mMesh) // copy
{
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <typename AttrT>
face_attribute<AttrT>::face_attribute(face_attribute&& rhs) : face_attribute_base(rhs.mMesh) // move
{
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <typename AttrT>
face_attribute<AttrT>& face_attribute<AttrT>::operator=(face_attribute const& rhs) // copy
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <typename AttrT>
face_attribute<AttrT>& face_attribute<AttrT>::operator=(face_attribute&& rhs) // move
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <typename AttrT>
edge_attribute<AttrT>::edge_attribute(edge_attribute const& rhs) : edge_attribute_base(rhs.mMesh) // copy
{
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <typename AttrT>
edge_attribute<AttrT>::edge_attribute(edge_attribute&& rhs) : edge_attribute_base(rhs.mMesh) // move
{
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <typename AttrT>
edge_attribute<AttrT>& edge_attribute<AttrT>::operator=(edge_attribute const& rhs) // copy
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <typename AttrT>
edge_attribute<AttrT>& edge_attribute<AttrT>::operator=(edge_attribute&& rhs) // move
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <typename AttrT>
halfedge_attribute<AttrT>::halfedge_attribute(halfedge_attribute const& rhs)
  : halfedge_attribute_base(rhs.mMesh) // copy
{
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <typename AttrT>
halfedge_attribute<AttrT>::halfedge_attribute(halfedge_attribute&& rhs) : halfedge_attribute_base(rhs.mMesh) // move
{
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <typename AttrT>
halfedge_attribute<AttrT>& halfedge_attribute<AttrT>::operator=(halfedge_attribute const& rhs) // copy
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <typename AttrT>
halfedge_attribute<AttrT>& halfedge_attribute<AttrT>::operator=(halfedge_attribute&& rhs) // move
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
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
