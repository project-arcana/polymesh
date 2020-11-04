#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class tag, class AttrT>
AttrT& primitive_attribute<tag, AttrT>::operator[](handle_t h)
{
    POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value < this->size() && "out of bounds");
    return mData[h.idx.value];
}
template <class tag, class AttrT>
AttrT const& primitive_attribute<tag, AttrT>::operator[](handle_t h) const
{
    POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value < this->size() && "out of bounds");
    return mData[h.idx.value];
}
template <class tag, class AttrT>
AttrT& primitive_attribute<tag, AttrT>::operator()(handle_t h)
{
    POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value < this->size() && "out of bounds");
    return mData[h.idx.value];
}
template <class tag, class AttrT>
AttrT const& primitive_attribute<tag, AttrT>::operator()(handle_t h) const
{
    POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value < this->size() && "out of bounds");
    return mData[h.idx.value];
}

template <class AttrT>
AttrT& edge_attribute<AttrT>::operator[](halfedge_handle h)
{
    POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value / 2 < this->size() && "out of bounds");
    return this->mData[h.idx.value >> 1];
}
template <class AttrT>
AttrT const& edge_attribute<AttrT>::operator[](halfedge_handle h) const
{
    POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value / 2 < this->size() && "out of bounds");
    return this->mData[h.idx.value >> 1];
}
template <class AttrT>
AttrT& edge_attribute<AttrT>::operator()(halfedge_handle h)
{
    POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value / 2 < this->size() && "out of bounds");
    return this->mData[h.idx.value >> 1];
}
template <class AttrT>
AttrT const& edge_attribute<AttrT>::operator()(halfedge_handle h) const
{
    POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
    POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value / 2 < this->size() && "out of bounds");
    return this->mData[h.idx.value >> 1];
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::copy_from(span<AttrT const> data)
{
    std::copy_n(data.data(), std::min(int(data.size()), this->size()), this->data());
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::copy_from(const AttrT* data, int cnt)
{
    std::copy_n(data, std::min(cnt, this->size()), this->data());
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::copy_from(attribute<AttrT> const& data)
{
    std::copy_n(data.data(), std::min(data.size(), this->size()), this->data());
}

template <class tag, class AttrT>
auto primitive_attribute<tag, AttrT>::copy_to(Mesh const& m) const -> attribute<AttrT>
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
    std::copy_n(this->data(), s, r.data());
    return r;
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::apply_remapping(const std::vector<int>& map)
{
    for (auto i = 0u; i < map.size(); ++i)
        this->mData[i] = this->mData[map[i]];
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::apply_transpositions(std::vector<std::pair<int, int>> const& ts)
{
    using std::swap;
    for (auto t : ts)
        swap(this->mData[t.first], this->mData[t.second]);
}

// ==== User ctor: delegates to internal standard ctor
template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::primitive_attribute(Mesh const& mesh, const AttrT& def_value) : primitive_attribute(&mesh, def_value)
{
}

// ==== Internal standard ctor: registers and allocates default data
template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::primitive_attribute(const Mesh* mesh, const AttrT& def_value)
  : primitive_attribute_base<tag>(mesh), mDefaultValue(def_value)
{
    // register
    this->register_attr();

    // alloc data
    this->mData.reset(new AttrT[this->capacity()]);

    // fill everything with default
    std::fill_n(this->mData.get(), this->capacity(), this->mDefaultValue);
}

// ==== Copy ctor: register, copy rhs data, fill default data
template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::primitive_attribute(primitive_attribute const& rhs) noexcept : primitive_attribute_base<tag>(rhs.mMesh) // copy
{
    // get default value
    this->mDefaultValue = rhs.mDefaultValue;

    // register attr
    this->register_attr();

    // alloc data
    this->mData.reset(new AttrT[this->capacity()]);

    // copy ALL data (valid and defaulted)
    std::copy_n(rhs.mData.get(), this->capacity(), this->mData.get());
}

// ==== Move ctor: take rhs data, deregister rhs
template <class tag, class AttrT>
primitive_attribute<tag, AttrT>::primitive_attribute(primitive_attribute&& rhs) noexcept : primitive_attribute_base<tag>(rhs.mMesh) // move
{
    // take default value and data from rhs
    this->mDefaultValue = std::move(rhs.mDefaultValue);
    this->mData = std::move(rhs.mData);

    // deregister rhs
    rhs.deregister_attr();

    // register lhs
    this->register_attr();
}

// ==== Copy assignment: register onto new mesh, copy data and default value from rhs
template <class tag, class AttrT>
primitive_attribute<tag, AttrT>& primitive_attribute<tag, AttrT>::operator=(primitive_attribute const& rhs) noexcept // copy
{
    if (this == &rhs) // prevent self-copy
        return *this;

    // save old capacity for no-realloc path
    auto old_capacity = is_valid() ? this->capacity() : 0;

    // deregister from old mesh
    this->deregister_attr();

    // register into new mesh
    this->mMesh = rhs.mMesh;
    this->register_attr();

    // realloc if new capacity
    auto new_capacity = this->capacity();
    if (old_capacity != new_capacity)
    {
        if (new_capacity == 0)
            this->mData.reset();
        else
            this->mData.reset(new AttrT[new_capacity]());
    }

    // copy ALL data (valid and defaulted)
    this->mDefaultValue = rhs.mDefaultValue;
    std::copy_n(rhs.mData.get(), this->capacity(), this->mData.get());

    return *this;
}

// ==== Move assignment: register onto new mesh, take rhs data, invalidate rhs
template <class tag, class AttrT>
primitive_attribute<tag, AttrT>& primitive_attribute<tag, AttrT>::operator=(primitive_attribute&& rhs) noexcept // move
{
    if (this == &rhs) // prevent self-move
        return *this;

    // deregister from old mesh
    this->deregister_attr();

    // register into new mesh
    this->mMesh = rhs.mMesh;
    this->register_attr();

    // take data of rhs
    this->mDefaultValue = std::move(rhs.mDefaultValue);
    this->mData = std::move(rhs.mData);

    // deregister rhs
    rhs.deregister_attr();

    return *this;
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::resize_from(int old_size)
{
    // mesh is already resized, thus capacity() and size() return new values
    // old_size is size before resize

    auto new_capacity = this->capacity();
    auto shared_size = std::min(this->size(), old_size);
    POLYMESH_ASSERT(shared_size <= new_capacity && "size cannot exceed capacity");

    // alloc new data
    auto new_data = new_capacity > 0 ? new AttrT[new_capacity]() : nullptr;

    // copy shared region to new data
    std::copy_n(this->mData.get(), shared_size, new_data);

    // fill rest with default value
    std::fill(new_data + shared_size, new_data + new_capacity, mDefaultValue);

    // replace old data
    this->mData.reset(new_data);
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::clear_with_default()
{
    std::fill_n(this->data(), this->size(), mDefaultValue);
}

inline void Mesh::register_attr(primitive_attribute_base<vertex_tag>* attr) const
{
    // insert in front
    auto nextAttrs = mVertexAttrs;
    mVertexAttrs = attr;
    attr->mNextAttribute = nextAttrs;
    if (nextAttrs)
        nextAttrs->mPrevAttribute = attr;
}

inline void Mesh::deregister_attr(primitive_attribute_base<vertex_tag>* attr) const
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

inline void Mesh::register_attr(primitive_attribute_base<face_tag>* attr) const
{
    // insert in front
    auto nextAttrs = mFaceAttrs;
    mFaceAttrs = attr;
    attr->mNextAttribute = nextAttrs;
    if (nextAttrs)
        nextAttrs->mPrevAttribute = attr;
}

inline void Mesh::deregister_attr(primitive_attribute_base<face_tag>* attr) const
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

inline void Mesh::register_attr(primitive_attribute_base<edge_tag>* attr) const
{
    // insert in front
    auto nextAttrs = mEdgeAttrs;
    mEdgeAttrs = attr;
    attr->mNextAttribute = nextAttrs;
    if (nextAttrs)
        nextAttrs->mPrevAttribute = attr;
}

inline void Mesh::deregister_attr(primitive_attribute_base<edge_tag>* attr) const
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

inline void Mesh::register_attr(primitive_attribute_base<halfedge_tag>* attr) const
{
    // insert in front
    auto nextAttrs = mHalfedgeAttrs;
    mHalfedgeAttrs = attr;
    attr->mNextAttribute = nextAttrs;
    if (nextAttrs)
        nextAttrs->mPrevAttribute = attr;
}

inline void Mesh::deregister_attr(primitive_attribute_base<halfedge_tag>* attr) const
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
int primitive_attribute<tag, AttrT>::size() const
{
    return primitive<tag>::all_size(*this->mMesh);
}

template <class tag, class AttrT>
int primitive_attribute<tag, AttrT>::capacity() const
{
    return primitive<tag>::capacity(*this->mMesh);
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::clear(AttrT const& value)
{
    std::fill_n(this->data(), size(), value);
}

template <class tag, class AttrT>
void primitive_attribute<tag, AttrT>::clear()
{
    this->clear(this->mDefaultValue);
}

template <class tag, class AttrT>
template <class FuncT>
auto primitive_attribute<tag, AttrT>::map(FuncT&& f) const -> attribute<tmp::decayed_result_type_of<FuncT, AttrT>>
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
void primitive_attribute<tag, AttrT>::apply(FuncT&& f)
{
    auto s = size();
    auto d = data();
    for (auto i = 0; i < s; ++i)
        f(d[i]);
}

template <class tag, class AttrT>
template <class FuncT>
void primitive_attribute<tag, AttrT>::compute(FuncT&& f)
{
    auto d = data();
    for (auto h : primitive<tag>::valid_collection_of(*this->mMesh))
        d[(int)h] = f(h);
}

template <class tag, class AttrT>
template <class FuncT>
auto primitive_attribute<tag, AttrT>::view(FuncT&& f) const& -> attribute_view<primitive_attribute<tag, AttrT> const&, FuncT>
{
    return attribute_view<primitive_attribute<tag, AttrT> const&, FuncT>(*this, std::forward<FuncT>(f));
}

template <class tag, class AttrT>
template <class FuncT>
auto primitive_attribute<tag, AttrT>::view(FuncT&& f) & -> attribute_view<primitive_attribute<tag, AttrT>&, FuncT>
{
    return attribute_view<primitive_attribute<tag, AttrT>&, FuncT>(*this, std::forward<FuncT>(f));
}

} // namespace polymesh
