#pragma once

#include <cstring>

#include <polymesh/Mesh.hh>
#include <polymesh/attributes.hh>
#include <polymesh/span.hh>

namespace polymesh
{
/**
 * Raw attributes are attributes that store a fixed (but runtime determined) number of bytes per primitive
 * The number of bytes per primitive is called stride
 *
 * NOTE: they are always zero-initialized
 *
 * TODO: iteration
 * TODO: smart range
 */
template <class tag>
struct raw_primitive_attribute : primitive_attribute_base<tag>
{
    template <class A>
    using attribute = typename primitive<tag>::template attribute<A>;
    using index_t = typename primitive<tag>::index;
    using handle_t = typename primitive<tag>::handle;
    using tag_t = tag;

    // data access
public:
    span<std::byte> operator[](handle_t h)
    {
        POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
        POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value < this->size() && "out of bounds");
        return {this->mData.get() + h.idx.value * mStride, mStride};
    }
    span<std::byte const> operator[](handle_t h) const
    {
        POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
        POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value < this->size() && "out of bounds");
        return {this->mData.get() + h.idx.value * mStride, mStride};
    }
    span<std::byte> operator[](index_t h)
    {
        POLYMESH_ASSERT(h.is_valid());
        return {this->mData.get() + h.value * mStride, mStride};
    }
    span<std::byte const> operator[](index_t h) const
    {
        POLYMESH_ASSERT(h.is_valid());
        return {this->mData.get() + h.value * mStride, mStride};
    }

    span<std::byte> operator()(handle_t h)
    {
        POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
        POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value < this->size() && "out of bounds");
        return {this->mData.get() + h.idx.value * mStride, mStride};
    }
    span<std::byte const> operator()(handle_t h) const
    {
        POLYMESH_ASSERT(this->mMesh == h.mesh && "Handle belongs to a different mesh");
        POLYMESH_ASSERT(0 <= h.idx.value && h.idx.value < this->size() && "out of bounds");
        return {this->mData.get() + h.idx.value * mStride, mStride};
    }
    span<std::byte> operator()(index_t h)
    {
        POLYMESH_ASSERT(h.is_valid());
        return {this->mData.get() + h.value * mStride, mStride};
    }

    span<std::byte const> operator()(index_t h) const
    {
        POLYMESH_ASSERT(h.is_valid());
        return {this->mData.get() + h.value * mStride, mStride};
    }

    std::byte* data() { return mData.get(); }
    std::byte const* data() const { return mData.get(); }

    int size() const { return primitive<tag>::all_size(*this->mMesh); }
    int stride() const { return mStride; }
    int capacity() const { return primitive<tag>::capacity(*this->mMesh); }
    size_t byte_size() const override { return size() * mStride; }
    size_t allocated_byte_size() const override { return capacity() * mStride; }

    /// true iff this attribute is still attached to a mesh
    /// do not use the attribute if not valid
    bool is_valid() const { return this->mMesh != nullptr; }

    // methods
public:
    /// sets all attribute values to zero
    void clear()
    {
        if (byte_size() > 0)
            std::memset(this->mData.get(), 0, byte_size());
    }

    /// Saves ALL data into a vector (includes possibly removed ones)
    std::vector<std::byte> to_raw_vector() const
    {
        auto r = std::vector<std::byte>(byte_size());
        if (r.size() > 0)
            std::memcpy(r.data(), this->mData.get(), byte_size());
        return r;
    }

    /// returns a new attribute where all elements were bitcast to the given type
    /// NOTE: requires trivially copyable T with sizeof(T) == stride()
    template <class T>
    auto to() const -> attribute<T>
    {
        static_assert(std::is_trivially_copyable_v<T>, "only works for trivially copyable types");
        POLYMESH_ASSERT(sizeof(T) == mStride && "stride must match exactly with type size");
        auto a = attribute<T>(this->mesh());
        if (byte_size() > 0)
            std::memcpy(a.data(), this->mData.get(), byte_size());
        return a;
    }

    // public ctor
public:
    raw_primitive_attribute() = default;
    raw_primitive_attribute(Mesh const& mesh, int stride) : primitive_attribute_base<tag>(&mesh), mStride(stride)
    {
        POLYMESH_ASSERT(mStride > 0 && "only positive stride supported");

        // register
        this->register_attr();

        // alloc data (zero-init)
        this->mData.reset(new std::byte[this->capacity() * mStride]());
    }

    // members
protected:
    unique_array<std::byte> mData;
    size_t mStride = 0; ///< number of bytes per element

protected:
    void resize_from(int old_size) override
    {
        // mesh is already resized, thus capacity() and size() return new values
        // old_size is size before resize

        auto new_capacity = this->capacity();
        auto shared_size = std::min(this->size(), old_size);
        POLYMESH_ASSERT(shared_size <= new_capacity && "size cannot exceed capacity");

        // alloc new data
        auto new_data = new_capacity > 0 ? new std::byte[new_capacity * mStride]() : nullptr;

        // copy shared region to new data
        if (shared_size > 0)
            std::memcpy(new_data, this->mData.get(), shared_size * mStride);

        // replace old data
        this->mData.reset(new_data);
    }
    void clear_with_default() override { std::memset(this->mData.get(), 0, byte_size()); }

    void apply_remapping(std::vector<int> const& map) override
    {
        // TODO: could be made faster by special casing a few stride sizes
        for (auto i = 0u; i < map.size(); ++i)
            std::memcpy(&this->mData[i], &this->mData[map[i]], mStride);
    }
    void apply_transpositions(std::vector<std::pair<int, int>> const& ts) override
    {
        for (auto t : ts)
            for (auto i = 0u; i < mStride; ++i)
                swap(this->mData[t.first * mStride + i], this->mData[t.second * mStride + i]);
    }

    template <class MeshT>
    friend struct low_level_attribute_api;

    // move & copy
public:
    raw_primitive_attribute(raw_primitive_attribute const& rhs) noexcept // copy
      : primitive_attribute_base<tag>(rhs.mMesh), mStride(rhs.mStride)
    {
        // register attr
        this->register_attr();

        // alloc data
        this->mData.reset(new std::byte[this->capacity() * mStride]());

        // copy valid data
        std::memcpy(this->mData.get(), rhs.mData.get(), rhs.byte_size());
    }
    raw_primitive_attribute(raw_primitive_attribute&& rhs) noexcept // move
      : primitive_attribute_base<tag>(rhs.mMesh), mStride(rhs.mStride)
    {
        // take data from rhs
        this->mData = std::move(rhs.mData);

        // deregister rhs
        rhs.deregister_attr();

        // register lhs
        this->register_attr();
    }
    raw_primitive_attribute& operator=(raw_primitive_attribute const& rhs) noexcept // copy assign
    {
        if (this == &rhs) // prevent self-copy
            return *this;

        // save old capacity for no-realloc path
        auto old_capacity_bytes = is_valid() ? this->capacity() * mStride : 0;

        // deregister from old mesh
        this->deregister_attr();

        // register into new mesh
        this->mMesh = rhs.mMesh;
        this->mStride = rhs.mStride;
        this->register_attr();

        // realloc if new capacity
        auto new_capacity_bytes = this->capacity() * mStride;
        if (old_capacity_bytes != new_capacity_bytes)
        {
            if (new_capacity_bytes == 0)
                this->mData.reset();
            else
                this->mData.reset(new std::byte[new_capacity_bytes]());
        }

        // copy valid AND defaulted data
        std::memcpy(this->mData.get(), rhs.mData.get(), new_capacity_bytes);

        return *this;
    }
    raw_primitive_attribute& operator=(raw_primitive_attribute&& rhs) noexcept // move assign
    {
        if (this == &rhs) // prevent self-move
            return *this;

        // deregister from old mesh
        this->deregister_attr();

        // register into new mesh
        this->mMesh = rhs.mMesh;
        this->register_attr();

        // take data of rhs
        this->mStride = rhs.mStride;
        this->mData = std::move(rhs.mData);

        // deregister rhs
        rhs.deregister_attr();

        return *this;
    }
};

struct raw_vertex_attribute final : raw_primitive_attribute<vertex_tag>
{
    using raw_primitive_attribute<vertex_tag>::raw_primitive_attribute;
};
struct raw_face_attribute final : raw_primitive_attribute<face_tag>
{
    using raw_primitive_attribute<face_tag>::raw_primitive_attribute;
};
struct raw_edge_attribute final : raw_primitive_attribute<edge_tag>
{
    using raw_primitive_attribute<edge_tag>::raw_primitive_attribute;
};
struct raw_halfedge_attribute final : raw_primitive_attribute<halfedge_tag>
{
    using raw_primitive_attribute<halfedge_tag>::raw_primitive_attribute;
};
}
