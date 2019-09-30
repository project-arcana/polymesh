#pragma once

#include <map>
#include <string>

#include <polymesh/assert.hh>
#include <polymesh/attributes.hh>
#include <polymesh/detail/unique_ptr.hh>

namespace polymesh
{
/// Collection of named attributes (requires RTTI)
/// Attributes are owned by the collection
///
/// Usage:
///
///   attribute_collection ac;
///
///   // insert attributes
///   // NOTE: copies the attribute!
///   // TODO: support std::move
///   ac["aPosition"] = m.vertices().make_attribute<glm::vec3>();
///
///   // access attributes (must exist)
///   auto aPos = ac["aPosition"].vertex<glm::vec3>();
struct attribute_collection
{
    struct accessor;
    struct const_accessor;

    /// returns access to a named attribute
    accessor operator[](std::string const& name) { return {name, *this}; }
    const_accessor operator[](std::string const& name) const { return {name, *this}; }

    /// access to all attributes
    /// (should usually not be used)
    std::map<std::string, unique_ptr<primitive_attribute_base<vertex_tag>>> const& vertex_attributes() const { return mVertexAttrs; }
    std::map<std::string, unique_ptr<primitive_attribute_base<face_tag>>> const& face_attributes() const { return mFaceAttrs; }
    std::map<std::string, unique_ptr<primitive_attribute_base<edge_tag>>> const& edge_attributes() const { return mEdgeAttrs; }
    std::map<std::string, unique_ptr<primitive_attribute_base<halfedge_tag>>> const& halfedge_attributes() const { return mHalfedgeAttrs; }

public:
    struct accessor
    {
        std::string name;
        attribute_collection& ref;

        template <class AttrT>
        accessor& operator=(vertex_attribute<AttrT> const& a)
        {
            ref.mVertexAttrs[name].reset(new vertex_attribute<AttrT>(a));
            return *this;
        }
        template <class AttrT>
        accessor& operator=(face_attribute<AttrT> const& a)
        {
            ref.mFaceAttrs[name].reset(new face_attribute<AttrT>(a));
            return *this;
        }
        template <class AttrT>
        accessor& operator=(edge_attribute<AttrT> const& a)
        {
            ref.mEdgeAttrs[name].reset(new edge_attribute<AttrT>(a));
            return *this;
        }
        template <class AttrT>
        accessor& operator=(halfedge_attribute<AttrT> const& a)
        {
            ref.mHalfedgeAttrs[name].reset(new halfedge_attribute<AttrT>(a));
            return *this;
        }

        template <class AttrT>
        vertex_attribute<AttrT>& vertex()
        {
            auto pa = ref.mVertexAttrs.at(name).get();
            POLYMESH_ASSERT(pa && "non-existent attribute");
            return *dynamic_cast<vertex_attribute<AttrT>*>(pa);
        }
        template <class AttrT>
        face_attribute<AttrT>& face()
        {
            auto pa = ref.mFaceAttrs.at(name).get();
            POLYMESH_ASSERT(pa && "non-existent attribute");
            return *dynamic_cast<face_attribute<AttrT>*>(pa);
        }
        template <class AttrT>
        edge_attribute<AttrT>& edge()
        {
            auto pa = ref.mEdgeAttrs.at(name).get();
            POLYMESH_ASSERT(pa && "non-existent attribute");
            return *dynamic_cast<edge_attribute<AttrT>*>(pa);
        }
        template <class AttrT>
        halfedge_attribute<AttrT>& halfedge()
        {
            auto pa = ref.mHalfedgeAttrs.at(name).get();
            POLYMESH_ASSERT(pa && "non-existent attribute");
            return *dynamic_cast<halfedge_attribute<AttrT>*>(pa);
        }
    };
    struct const_accessor
    {
        std::string name;
        attribute_collection const& ref;

        template <class AttrT>
        vertex_attribute<AttrT>& vertex()
        {
            auto pa = ref.mVertexAttrs.at(name).get();
            POLYMESH_ASSERT(pa && "non-existent attribute");
            return *dynamic_cast<vertex_attribute<AttrT>*>(pa);
        }
        template <class AttrT>
        face_attribute<AttrT>& face()
        {
            auto pa = ref.mFaceAttrs.at(name).get();
            POLYMESH_ASSERT(pa && "non-existent attribute");
            return *dynamic_cast<face_attribute<AttrT>*>(pa);
        }
        template <class AttrT>
        edge_attribute<AttrT>& edge()
        {
            auto pa = ref.mEdgeAttrs.at(name).get();
            POLYMESH_ASSERT(pa && "non-existent attribute");
            return *dynamic_cast<edge_attribute<AttrT>*>(pa);
        }
        template <class AttrT>
        halfedge_attribute<AttrT>& halfedge()
        {
            auto pa = ref.mHalfedgeAttrs.at(name).get();
            POLYMESH_ASSERT(pa && "non-existent attribute");
            return *dynamic_cast<halfedge_attribute<AttrT>*>(pa);
        }
    };

private:
    std::map<std::string, unique_ptr<primitive_attribute_base<vertex_tag>>> mVertexAttrs;
    std::map<std::string, unique_ptr<primitive_attribute_base<face_tag>>> mFaceAttrs;
    std::map<std::string, unique_ptr<primitive_attribute_base<edge_tag>>> mEdgeAttrs;
    std::map<std::string, unique_ptr<primitive_attribute_base<halfedge_tag>>> mHalfedgeAttrs;

    friend accessor;
    friend const_accessor;
};
}
