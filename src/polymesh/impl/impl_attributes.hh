#pragma once

#include "../Mesh.hh"

namespace polymesh
{
template <class tag, class AttrT>
AttrT &primitive_attribute<tag, AttrT>::operator[](handle_t h)
{
    assert(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    return mData[h.idx.value];
}
template <class tag, class AttrT>
AttrT const &primitive_attribute<tag, AttrT>::operator[](handle_t h) const
{
    assert(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    return mData[h.idx.value];
}
template <class tag, class AttrT>
AttrT &primitive_attribute<tag, AttrT>::operator()(handle_t h)
{
    assert(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    return mData[h.idx.value];
}
template <class tag, class AttrT>
AttrT const &primitive_attribute<tag, AttrT>::operator()(handle_t h) const
{
    assert(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    return mData[h.idx.value];
}

template <class AttrT>
AttrT &edge_attribute<AttrT>::operator[](halfedge_handle h)
{
    assert(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    return this->mData[h.idx.value >> 1];
}
template <class AttrT>
AttrT const &edge_attribute<AttrT>::operator[](halfedge_handle h) const
{
    assert(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    return this->mData[h.idx.value >> 1];
}
template <class AttrT>
AttrT &edge_attribute<AttrT>::operator()(halfedge_handle h)
{
    assert(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    return this->mData[h.idx.value >> 1];
}
template <class AttrT>
AttrT const &edge_attribute<AttrT>::operator()(halfedge_handle h) const
{
    assert(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    return this->mData[h.idx.value >> 1];
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::copy_from(const std::vector<AttrT> &data)
{
    auto s = std::min((int)data.size(), this->size());
    for (auto i = 0; i < s; ++i)
        this->mData[i] = data[i];
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::copy_from(const AttrT *data, int cnt)
{
    auto s = std::min(cnt, this->size());
    for (auto i = 0; i < s; ++i)
        this->mData[i] = data[i];
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::copy_from(attribute<AttrT> const &data)
{
    auto s = std::min(data.size(), this->size());
    for (auto i = 0; i < s; ++i)
        this->mData[i] = data.mData[i];
}

template <class tag, class AttrT>
auto primitive_attribute<tag, AttrT>::copy_to(Mesh const &m) const -> attribute<AttrT>
{
    attribute<AttrT> new_attr(m, this->mDefaultValue);
    new_attr.copy_from(this->data(), this->size());
    return new_attr;
}

template <class tag, class AttrT>
std::vector<AttrT> primitive_attribute<tag, AttrT>::to_vector() const
{
    auto s = this->size();
    std::vector<AttrT> r(s);
    for (auto i = 0; i < s; ++i)
        r[i] = this->mData[i];
    return r;
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

    auto s = std::min(rhs.size(), this->size());
    for (auto i = 0; i < s; ++i)
        this->mData[i] = rhs.mData[i];

    this->register_attr();
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::primitive_attribute(primitive_attribute &&rhs) noexcept : primitive_attribute_base<tag>(rhs.mMesh) // move
{
    this->mDefaultValue = std::move(rhs.mDefaultValue);
    this->mData = rhs.mData;
    rhs.mData = nullptr;

    rhs.deregister_attr();
    this->register_attr();
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT> &primitive_attribute<tag, AttrT>::operator=(primitive_attribute const &rhs) noexcept // copy
{
    this->deregister_attr();

    if (this->capacity() < rhs.capacity())
        this->resize(rhs.capacity());

    this->mMesh = rhs.mMesh;
    this->mDefaultValue = rhs.mDefaultValue;
    this->copy_from(rhs);

    this->register_attr();

    return *this;
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT> &primitive_attribute<tag, AttrT>::operator=(primitive_attribute &&rhs) noexcept // move
{
    this->deregister_attr();

    this->mMesh = rhs.mMesh;
    this->mDefaultValue = std::move(rhs.mDefaultValue);
    this->mData = rhs.mData;
    rhs.mData = nullptr;

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
    attr->resizeFrom(0);
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
    attr->resizeFrom(0);
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
    attr->resizeFrom(0);
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
    attr->resizeFrom(0);
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
primitive_attribute<tag, AttrT>::primitive_attribute(Mesh const &mesh, const AttrT &def_value) : primitive_attribute(&mesh, def_value)
{
}

template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::~primitive_attribute()
{
    delete[] mData;
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
int primitive_attribute<tag, AttrT>::capacity() const
{
    return primitive<tag>::capacity(*this->mMesh);
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::clear(AttrT const &value)
{
    std::fill_n(mData, size(), value);
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
template <class T>
auto primitive_attribute<tag, AttrT>::to() const -> attribute<T>
{
    auto attr = primitive<tag>::all_collection_of(*this->mMesh).template make_attribute<T>();
    auto s = size();
    auto d_in = data();
    auto d_out = attr.data();
    for (auto i = 0; i < s; ++i)
        d_out[i] = static_cast<T>(d_in[i]);
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

template <class tag, class AttrT>
template <class FuncT>
void primitive_attribute<tag, AttrT>::compute(FuncT &&f)
{
    auto d = data();
    for (auto h : primitive<tag>::valid_collection_of(*this->mMesh))
        d[(int)h] = f(h);
}

template <class tag, class AttrT>
template <class FuncT>
auto primitive_attribute<tag, AttrT>::view(FuncT &&f) const -> readonly_property<primitive_attribute<tag, AttrT> const &, FuncT>
{
    return readonly_property<primitive_attribute<tag, AttrT> const &, FuncT>(*this, f);
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::on_resize_from(int oldSize)
{
    auto newSize = capacity();
    auto sharedSize = std::min(size(), oldSize);

    auto *newData = newSize > 0 ? new AttrT[newSize] : nullptr;
    std::copy_n(mData, sharedSize, newData);
    delete[] mData;
    mData = newData;

    if (newSize > sharedSize)
    {
        std::fill(mData + sharedSize, mData + newSize, mDefaultValue);
    }
}

} // namespace polymesh
