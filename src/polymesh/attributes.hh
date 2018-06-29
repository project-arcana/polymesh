#pragma once

#include <cstddef>
#include <vector>

#include "attribute_base.hh"
#include "cursors.hh"
#include "tmp.hh"

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
template <class Primitive, class AttrT>
struct primitive_attribute
{
};


// ===========================================
// OLD CODE:


template <class AttrT>
struct vertex_attribute : vertex_attribute_base
{
    // data access
public:
    AttrT& operator[](vertex_handle v) { return mData[v.idx.value]; }
    AttrT const& operator[](vertex_handle v) const { return mData[v.idx.value]; }
    AttrT& operator[](vertex_index v) { return mData[v.value]; }
    AttrT const& operator[](vertex_index v) const { return mData[v.value]; }

    AttrT* data() { return mData.data; }
    AttrT const* data() const { return mData.data; }
    int size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    /// returns a new attribute where the given function was applied to each entry
    template <class FuncT>
    auto map(FuncT f) const -> vertex_attribute<tmp::result_type_of<FuncT, AttrT>>;
    /// applies to given function to each attribute entry
    template <class FuncT>
    void apply(FuncT f);

    // data
private:
    attribute_data<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(int newSize) override { mData.resize(newSize, mDefaultValue); }
    void apply_remapping(std::vector<int> const& map) override;

    // ctor
private:
    vertex_attribute(Mesh const* mesh, AttrT const& def_value);
    friend struct vertex_collection;
    friend struct const_vertex_collection;

    // move & copy
public:
    vertex_attribute(vertex_attribute const&);
    vertex_attribute(vertex_attribute&&);
    vertex_attribute& operator=(vertex_attribute const&);
    vertex_attribute& operator=(vertex_attribute&&);
};

template <class AttrT>
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
    int size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    // data
private:
    attribute_data<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(int newSize) override { mData.resize(newSize, mDefaultValue); }
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

template <class AttrT>
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
    int size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    // data
private:
    attribute_data<AttrT> mData;
    AttrT mDefaultValue;

    void on_resize(int newSize) override { mData.resize(newSize, mDefaultValue); }
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

template <class AttrT>
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
    int size() const;

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    void on_resize(int newSize) override { mData.resize(newSize, mDefaultValue); }
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

template <class AttrT>
void vertex_attribute<AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template <class AttrT>
void face_attribute<AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template <class AttrT>
void edge_attribute<AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}
template <class AttrT>
void halfedge_attribute<AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        mData[i] = mData[map[i]];
}

template <class AttrT>
vertex_attribute<AttrT>::vertex_attribute(vertex_attribute const& rhs) : vertex_attribute_base(rhs.mMesh) // copy
{
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <class AttrT>
vertex_attribute<AttrT>::vertex_attribute(vertex_attribute&& rhs) : vertex_attribute_base(rhs.mMesh) // move
{
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <class AttrT>
vertex_attribute<AttrT>& vertex_attribute<AttrT>::operator=(vertex_attribute const& rhs) // copy
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <class AttrT>
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

template <class AttrT>
face_attribute<AttrT>::face_attribute(face_attribute const& rhs) : face_attribute_base(rhs.mMesh) // copy
{
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <class AttrT>
face_attribute<AttrT>::face_attribute(face_attribute&& rhs) : face_attribute_base(rhs.mMesh) // move
{
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <class AttrT>
face_attribute<AttrT>& face_attribute<AttrT>::operator=(face_attribute const& rhs) // copy
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <class AttrT>
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

template <class AttrT>
edge_attribute<AttrT>::edge_attribute(edge_attribute const& rhs) : edge_attribute_base(rhs.mMesh) // copy
{
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <class AttrT>
edge_attribute<AttrT>::edge_attribute(edge_attribute&& rhs) : edge_attribute_base(rhs.mMesh) // move
{
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <class AttrT>
edge_attribute<AttrT>& edge_attribute<AttrT>::operator=(edge_attribute const& rhs) // copy
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <class AttrT>
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

template <class AttrT>
halfedge_attribute<AttrT>::halfedge_attribute(halfedge_attribute const& rhs)
  : halfedge_attribute_base(rhs.mMesh) // copy
{
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <class AttrT>
halfedge_attribute<AttrT>::halfedge_attribute(halfedge_attribute&& rhs) : halfedge_attribute_base(rhs.mMesh) // move
{
    mDefaultValue = std::move(rhs.mDefaultValue);
    mData = std::move(rhs.mData);
    mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    register_attr();
}

template <class AttrT>
halfedge_attribute<AttrT>& halfedge_attribute<AttrT>::operator=(halfedge_attribute const& rhs) // copy
{
    deregister_attr();

    mMesh = rhs.mMesh;
    mDefaultValue = rhs.mDefaultValue;
    mData = rhs.mData;
    mDataSize = rhs.mDataSize;

    register_attr();
}

template <class AttrT>
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

template <class tag>
template <class AttrT>
AttrT& primitive_index<tag>::operator[](primitive_index::attribute<AttrT>& attr) const
{
    return attr[*this];
}
template <class tag>
template <class AttrT>
AttrT const& primitive_index<tag>::operator[](primitive_index::attribute<AttrT> const& attr) const
{
    return attr[*this];
}
template <class tag>
template <class AttrT>
AttrT& primitive_index<tag>::operator[](primitive_index::attribute<AttrT>* attr) const
{
    return (*attr)[*this];
}
template <class tag>
template <class AttrT>
AttrT const& primitive_index<tag>::operator[](primitive_index::attribute<AttrT> const* attr) const
{
    return (*attr)[*this];
}

template <class tag>
template <class AttrT>
AttrT& primitive_handle<tag>::operator[](primitive_handle::attribute<AttrT>& attr) const
{
    return attr[idx];
}
template <class tag>
template <class AttrT>
AttrT const& primitive_handle<tag>::operator[](primitive_handle::attribute<AttrT> const& attr) const
{
    return attr[idx];
}
template <class tag>
template <class AttrT>
AttrT& primitive_handle<tag>::operator[](primitive_handle::attribute<AttrT>* attr) const
{
    return (*attr)[idx];
}
template <class tag>
template <class AttrT>
AttrT const& primitive_handle<tag>::operator[](primitive_handle::attribute<AttrT> const* attr) const
{
    return (*attr)[idx];
}

}
