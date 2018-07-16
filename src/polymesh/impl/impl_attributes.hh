#pragma once

#include "../Mesh.hh"

namespace polymesh
{
template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::copy_from(const std::vector<AttrT> &data)
{
    auto s = std::min((int)data.size(), this->mDataSize);
    for (auto i = 0; i < s; ++i)
        this->mData[i] = data[i];
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::copy_from(const AttrT *data, int cnt)
{
    auto s = std::min(cnt, this->mDataSize);
    for (auto i = 0; i < s; ++i)
        this->mData[i] = data[i];
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::apply_remapping(const std::vector<int> &map)
{
    for (auto i = 0u; i < map.size(); ++i)
        this->mData[i] = this->mData[map[i]];
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::apply_transpositions(std::vector<std::pair<int, int>> const &ts)
{
    using std::swap;
    for (auto t : ts)
        swap(this->mData[t.first], this->mData[t.second]);
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::primitive_attribute(primitive_attribute const &rhs) noexcept : primitive_attribute_base<tag>(rhs.mMesh) // copy
{
    this->mDefaultValue = rhs.mDefaultValue;
    this->mData = rhs.mData;
    this->mDataSize = rhs.mDataSize;

    this->register_attr();
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::primitive_attribute(primitive_attribute &&rhs) noexcept : primitive_attribute_base<tag>(rhs.mMesh) // move
{
    this->mDefaultValue = std::move(rhs.mDefaultValue);
    this->mData = std::move(rhs.mData);
    this->mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    this->register_attr();
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT> &primitive_attribute<tag, AttrT>::operator=(primitive_attribute const &rhs) noexcept // copy
{
    this->deregister_attr();

    this->mMesh = rhs.mMesh;
    this->mDefaultValue = rhs.mDefaultValue;
    this->mData = rhs.mData;
    this->mDataSize = rhs.mDataSize;

    this->register_attr();

    return *this;
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT> &primitive_attribute<tag, AttrT>::operator=(primitive_attribute &&rhs) noexcept // move
{
    this->deregister_attr();

    this->mMesh = rhs.mMesh;
    this->mDefaultValue = std::move(rhs.mDefaultValue);
    this->mData = std::move(rhs.mData);
    this->mDataSize = rhs.mDataSize;

    rhs.deregister_attr();
    this->register_attr();

    return *this;
}

inline void Mesh::register_attr(primitive_attribute_base<vertex_tag> *attr) const
{
    // insert in front
    auto nextAttrs = mVertexAttrs;
    mVertexAttrs = attr;
    attr->mNextAttribute = nextAttrs;
    if (nextAttrs)
        nextAttrs->mPrevAttribute = attr;

    // resize attr
    attr->resize(all_vertices().size(), false);
}

inline void Mesh::deregister_attr(primitive_attribute_base<vertex_tag> *attr) const
{
    if (attr->mPrevAttribute)
        attr->mPrevAttribute->mNextAttribute = attr->mNextAttribute;

    if (attr->mNextAttribute)
        attr->mNextAttribute->mPrevAttribute = attr->mPrevAttribute;

    if (mVertexAttrs == attr)
        mVertexAttrs = attr->mNextAttribute;

    attr->mNextAttribute = nullptr;
    attr->mPrevAttribute = nullptr;
}

inline void Mesh::register_attr(primitive_attribute_base<face_tag> *attr) const
{
    // insert in front
    auto nextAttrs = mFaceAttrs;
    mFaceAttrs = attr;
    attr->mNextAttribute = nextAttrs;
    if (nextAttrs)
        nextAttrs->mPrevAttribute = attr;

    // resize attr
    attr->resize(all_faces().size(), false);
}

inline void Mesh::deregister_attr(primitive_attribute_base<face_tag> *attr) const
{
    if (attr->mPrevAttribute)
        attr->mPrevAttribute->mNextAttribute = attr->mNextAttribute;

    if (attr->mNextAttribute)
        attr->mNextAttribute->mPrevAttribute = attr->mPrevAttribute;

    if (mFaceAttrs == attr)
        mFaceAttrs = attr->mNextAttribute;

    attr->mNextAttribute = nullptr;
    attr->mPrevAttribute = nullptr;
}

inline void Mesh::register_attr(primitive_attribute_base<edge_tag> *attr) const
{
    // insert in front
    auto nextAttrs = mEdgeAttrs;
    mEdgeAttrs = attr;
    attr->mNextAttribute = nextAttrs;
    if (nextAttrs)
        nextAttrs->mPrevAttribute = attr;

    // resize attr
    attr->resize(all_edges().size(), false);
}

inline void Mesh::deregister_attr(primitive_attribute_base<edge_tag> *attr) const
{
    if (attr->mPrevAttribute)
        attr->mPrevAttribute->mNextAttribute = attr->mNextAttribute;

    if (attr->mNextAttribute)
        attr->mNextAttribute->mPrevAttribute = attr->mPrevAttribute;

    if (mEdgeAttrs == attr)
        mEdgeAttrs = attr->mNextAttribute;

    attr->mNextAttribute = nullptr;
    attr->mPrevAttribute = nullptr;
}

inline void Mesh::register_attr(primitive_attribute_base<halfedge_tag> *attr) const
{
    // insert in front
    auto nextAttrs = mHalfedgeAttrs;
    mHalfedgeAttrs = attr;
    attr->mNextAttribute = nextAttrs;
    if (nextAttrs)
        nextAttrs->mPrevAttribute = attr;

    // resize attr
    attr->resize(all_halfedges().size(), false);
}

inline void Mesh::deregister_attr(primitive_attribute_base<halfedge_tag> *attr) const
{
    if (attr->mPrevAttribute)
        attr->mPrevAttribute->mNextAttribute = attr->mNextAttribute;

    if (attr->mNextAttribute)
        attr->mNextAttribute->mPrevAttribute = attr->mPrevAttribute;

    if (mHalfedgeAttrs == attr)
        mHalfedgeAttrs = attr->mNextAttribute;

    attr->mNextAttribute = nullptr;
    attr->mPrevAttribute = nullptr;
}

template <class tag>
void primitive_attribute_base<tag>::register_attr()
{
    mMesh->register_attr(this);
}

template <class tag>
void primitive_attribute_base<tag>::deregister_attr()
{
    if (mMesh)
    {
        mMesh->deregister_attr(this);
        mMesh = nullptr;
    }
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::primitive_attribute(const Mesh *mesh, const AttrT &def_value)
  : primitive_attribute_base<tag>(mesh), mDefaultValue(def_value)
{
    this->register_attr();
}

template <class tag, class AttrT>
int primitive_attribute<tag, AttrT>::size() const
{
    return primitive<tag>::all_collection_of(*this->mMesh).size();
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::clear(AttrT const &value)
{
    this->mData.clear();
    this->mData.resize(size(), value);
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::clear()
{
    this->clear(this->mDefaultValue);
}

template <class tag, class AttrT>
template <class FuncT>
auto primitive_attribute<tag, AttrT>::map(FuncT &&f) const -> attribute<tmp::decayed_result_type_of<FuncT, AttrT>>
{
    auto attr = primitive<tag>::all_collection_of(*this->mMesh).template make_attribute<tmp::decayed_result_type_of<FuncT, AttrT>>();
    auto s = size();
    auto d_in = data();
    auto d_out = attr.data();
    for (auto i = 0; i < s; ++i)
        d_out[i] = f(d_in[i]);
    return attr; // copy elison
}

template <class tag, class AttrT>
template <class FuncT>
void primitive_attribute<tag, AttrT>::apply(FuncT &&f)
{
    auto s = size();
    auto d = data();
    for (auto i = 0; i < s; ++i)
        f(d[i]);
}
}
