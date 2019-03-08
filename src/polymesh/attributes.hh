#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "attribute_base.hh"
#include "cursors.hh"
#include "properties.hh"
#include "ranges.hh"
#include "tmp.hh"

/** Attributes
 *
 * Golden rule:
 *  - the Mesh must always outlive the attribute!
 *
 * Create attributes:
 *   auto myAttr = mesh.vertices().make_attribute(0.0f);
 *   auto myAttr = attribute(mesh.vertices(), 0.0f);
 *   auto myAttr = attribute<float>(mesh.vertices());
 *   auto myAttr = vertex_attribute<float>(mesh);
 *
 * Access attributes:
 *   vertex_handle v; // or _index
 *   v[myAttr] = 7;
 *   myAttr[v] = 7;
 *
 * Currently an attribute has 40 bytes + sizeof(AttrT) overhead
 */

namespace polymesh
{
template <class tag, class AttrT>
struct primitive_attribute : primitive_attribute_base<tag>, smart_range<primitive_attribute<tag, AttrT>, AttrT>
{
    template <class A>
    using attribute = typename primitive<tag>::template attribute<A>;
    using index_t = typename primitive<tag>::index;
    using handle_t = typename primitive<tag>::handle;
    using tag_t = tag;

    // data access
public:
    AttrT& operator[](handle_t h);
    AttrT const& operator[](handle_t h) const;
    AttrT& operator[](index_t h)
    {
        assert(h.is_valid());
        return mData[h.value];
    }
    AttrT const& operator[](index_t h) const
    {
        assert(h.is_valid());
        return mData[h.value];
    }

    AttrT& operator()(handle_t h);
    AttrT const& operator()(handle_t h) const;
    AttrT& operator()(index_t h)
    {
        assert(h.is_valid());
        return mData[h.value];
    }

    AttrT const& operator()(index_t h) const
    {
        assert(h.is_valid());
        return mData[h.value];
    }

    AttrT* data() { return mData.get(); }
    AttrT const* data() const { return mData.get(); }

    int size() const;
    int capacity() const;

    attribute_iterator<primitive_attribute&> begin() { return {0, *this}; }
    attribute_iterator<primitive_attribute const&> begin() const { return {0, *this}; }
    attribute_iterator<primitive_attribute&> end() { return {size(), *this}; }
    attribute_iterator<primitive_attribute const&> end() const { return {size(), *this}; }

    AttrT const& get_default_value() const { return mDefaultValue; }
    // cannot be set because this is more expensive than just setting

    /// true iff this attribute is still attached to a mesh
    /// do not use the attribute if not valid
    bool is_valid() const { return this->mMesh != nullptr; }

    // methods
public:
    void clear(AttrT const& value);
    void clear();

    /// returns a new attribute where the given function was applied to each entry
    template <class FuncT>
    auto map(FuncT&& f) const -> attribute<tmp::decayed_result_type_of<FuncT, AttrT>>;
    /// returns a new attribute where all elements were cast to the given type
    template <class T>
    auto to() const -> attribute<T>;
    /// applies to given function to each attribute entry (calls f(e))
    template <class FuncT>
    void apply(FuncT&& f);
    /// sets each attribute to f(primitive)
    template <class FuncT>
    void compute(FuncT&& f);

    template <class FuncT>
    auto view(FuncT&& f) const -> readonly_property<primitive_attribute<tag, AttrT> const&, FuncT>;
#ifndef _MSC_VER // cannot overload this apparently
    template <class FuncT>
    void view(FuncT&& f) && = delete;
#endif

    // template <class ReadT, class WriteT>
    // auto view(ReadT&& r, WriteT&& w) -> readwrite_property<primitive_attribute<tag, AttrT>, ReadT, WriteT>;
    // template <class ReadT, class WriteT>
    // void view(ReadT&& r, WriteT&& w) && = delete;

    /// copies as much data as possible from the given vector
    void copy_from(std::vector<AttrT> const& data);
    /// copies as much data as possible from the given array
    void copy_from(AttrT const* data, int cnt);
    /// copies as much data as possible from the given attribute
    void copy_from(attribute<AttrT> const& data);

    /// copies all attribute data to another mesh
    /// asserts that sizes are correct
    attribute<AttrT> copy_to(Mesh const& m) const;

    /// Saves ALL data into a vector (includes possibly removed ones)
    std::vector<AttrT> to_vector() const;
    // TODO: specialized implementation of to_vector(FuncT&&)
    using smart_range<primitive_attribute<tag, AttrT>, AttrT>::to_vector;

    // public ctor
public:
    primitive_attribute(Mesh const& mesh, AttrT const& def_value = AttrT());

    // members
protected:
    std::unique_ptr<AttrT[]> mData;
    AttrT mDefaultValue;

protected:
    void resize_from(int old_size) override;
    void clear_with_default() override;
    size_t byte_size() const override { return size() * sizeof(AttrT); }
    size_t allocated_byte_size() const override { return capacity() * sizeof(AttrT); }

    void apply_remapping(std::vector<int> const& map) override;
    void apply_transpositions(std::vector<std::pair<int, int>> const& ts) override;

    template <class MeshT>
    friend struct low_level_attribute_api;

    // ctor
protected:
    primitive_attribute(Mesh const* mesh, AttrT const& def_value);

    // move & copy
public:
    primitive_attribute(primitive_attribute const&) noexcept;
    primitive_attribute(primitive_attribute&&) noexcept;
    primitive_attribute& operator=(primitive_attribute const&) noexcept;
    primitive_attribute& operator=(primitive_attribute&&) noexcept;
};

template <class AttrT>
struct vertex_attribute final : primitive_attribute<vertex_tag, AttrT>
{
    using primitive_attribute<vertex_tag, AttrT>::primitive_attribute;

    template <class mesh_ptr, class tag, class iterator>
    friend struct smart_collection;
};
template <class AttrT>
struct face_attribute final : primitive_attribute<face_tag, AttrT>
{
    using primitive_attribute<face_tag, AttrT>::primitive_attribute;

    template <class mesh_ptr, class tag, class iterator>
    friend struct smart_collection;
};
template <class AttrT>
struct edge_attribute final : primitive_attribute<edge_tag, AttrT>
{
    using primitive_attribute<edge_tag, AttrT>::primitive_attribute;

    // shortcuts for half-edge handles

    AttrT& operator[](halfedge_handle h);
    AttrT const& operator[](halfedge_handle h) const;
    AttrT& operator[](halfedge_index h)
    {
        assert(h.is_valid());
        return this->mData[h.value >> 1];
    }
    AttrT const& operator[](halfedge_index h) const
    {
        assert(h.is_valid());
        return this->mData[h.value >> 1];
    }

    AttrT& operator()(halfedge_handle h);
    AttrT const& operator()(halfedge_handle h) const;
    AttrT& operator()(halfedge_index h)
    {
        assert(h.is_valid());
        return this->mData[h.value >> 1];
    }
    AttrT const& operator()(halfedge_index h) const
    {
        assert(h.is_valid());
        return this->mData[h.value >> 1];
    }

    using primitive_attribute<edge_tag, AttrT>::operator[];
    using primitive_attribute<edge_tag, AttrT>::operator();

    template <class mesh_ptr, class tag, class iterator>
    friend struct smart_collection;
};
template <class AttrT>
struct halfedge_attribute final : primitive_attribute<halfedge_tag, AttrT>
{
    using primitive_attribute<halfedge_tag, AttrT>::primitive_attribute;

    template <class mesh_ptr, class tag, class iterator>
    friend struct smart_collection;
};

template <class AttrT, class Collection>
auto attribute(Collection const& c, AttrT const& defaultValue = {}) -> decltype(c.make_attribute_with_default(defaultValue))
{
    return c.make_attribute_with_default(defaultValue);
}

} // namespace polymesh
