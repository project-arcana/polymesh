#pragma once

#include <iosfwd>
#include <typeindex>

#include <polymesh/Mesh.hh>
#include <polymesh/attributes.hh>
#include <polymesh/ext/attribute_collection.hh>

namespace polymesh
{
namespace detail
{
class GenericAttributeSerializer
{
public:
    virtual ~GenericAttributeSerializer() {}

    /// can this attribute be (de)serialized by this serializer?
    virtual bool is_compatible_to(primitive_attribute_base<vertex_tag> const& attr) = 0;
    virtual bool is_compatible_to(primitive_attribute_base<halfedge_tag> const& attr) = 0;
    virtual bool is_compatible_to(primitive_attribute_base<edge_tag> const& attr) = 0;
    virtual bool is_compatible_to(primitive_attribute_base<face_tag> const& attr) = 0;

    virtual void serialize(std::ostream& out, primitive_attribute_base<vertex_tag> const& attr) = 0;
    virtual void serialize(std::ostream& out, primitive_attribute_base<halfedge_tag> const& attr) = 0;
    virtual void serialize(std::ostream& out, primitive_attribute_base<edge_tag> const& attr) = 0;
    virtual void serialize(std::ostream& out, primitive_attribute_base<face_tag> const& attr) = 0;

    // pointer to tag as dummy parameter to allow deserialize to be used in templates over the tag type
    virtual void deserialize(std::istream& in, Mesh const& mesh, attribute_collection& attrs, std::string const& name, vertex_tag*) = 0;
    virtual void deserialize(std::istream& in, Mesh const& mesh, attribute_collection& attrs, std::string const& name, halfedge_tag*) = 0;
    virtual void deserialize(std::istream& in, Mesh const& mesh, attribute_collection& attrs, std::string const& name, edge_tag*) = 0;
    virtual void deserialize(std::istream& in, Mesh const& mesh, attribute_collection& attrs, std::string const& name, face_tag*) = 0;
};

template <typename T, typename serdes>
class AttributeSerializer final : public GenericAttributeSerializer
{
public:
    AttributeSerializer(serdes const& serializer) : mSerdes(serializer) {}

    bool is_compatible_to(primitive_attribute_base<vertex_tag> const& attr) override
    {
        return dynamic_cast<primitive_attribute<vertex_tag, T> const*>(&attr);
    }
    bool is_compatible_to(primitive_attribute_base<halfedge_tag> const& attr) override
    {
        return dynamic_cast<primitive_attribute<halfedge_tag, T> const*>(&attr);
    }
    bool is_compatible_to(primitive_attribute_base<edge_tag> const& attr) override
    {
        return dynamic_cast<primitive_attribute<edge_tag, T> const*>(&attr);
    }
    bool is_compatible_to(primitive_attribute_base<face_tag> const& attr) override
    {
        return dynamic_cast<primitive_attribute<face_tag, T> const*>(&attr);
    }

    void serialize(std::ostream& out, primitive_attribute_base<vertex_tag> const& attr) override
    {
        auto const& specific = dynamic_cast<primitive_attribute<vertex_tag, T> const&>(attr);
        mSerdes.serialize(out, specific.data(), specific.size());
    }

    void serialize(std::ostream& out, primitive_attribute_base<halfedge_tag> const& attr) override
    {
        auto const& specific = dynamic_cast<primitive_attribute<halfedge_tag, T> const&>(attr);
        mSerdes.serialize(out, specific.data(), specific.size());
    }
    void serialize(std::ostream& out, primitive_attribute_base<edge_tag> const& attr) override
    {
        auto const& specific = dynamic_cast<primitive_attribute<edge_tag, T> const&>(attr);
        mSerdes.serialize(out, specific.data(), specific.size());
    }
    void serialize(std::ostream& out, primitive_attribute_base<face_tag> const& attr) override
    {
        auto const& specific = dynamic_cast<primitive_attribute<face_tag, T> const&>(attr);
        mSerdes.serialize(out, specific.data(), specific.size());
    }

    void deserialize(std::istream& in, Mesh const& mesh, attribute_collection& attrs, std::string const& name, vertex_tag*) override
    {
        auto attr = mesh.vertices().make_attribute<T>();
        mSerdes.deserialize(in, attr.data(), attr.size());
        attrs[name] = attr;
    }

    void deserialize(std::istream& in, Mesh const& mesh, attribute_collection& attrs, std::string const& name, halfedge_tag*) override
    {
        auto attr = mesh.halfedges().make_attribute<T>();
        mSerdes.deserialize(in, attr.data(), attr.size());
        attrs[name] = attr;
    }

    void deserialize(std::istream& in, Mesh const& mesh, attribute_collection& attrs, std::string const& name, edge_tag*) override
    {
        auto attr = mesh.edges().make_attribute<T>();
        mSerdes.deserialize(in, attr.data(), attr.size());
        attrs[name] = attr;
    }

    void deserialize(std::istream& in, Mesh const& mesh, attribute_collection& attrs, std::string const& name, face_tag*) override
    {
        auto attr = mesh.faces().make_attribute<T>();
        mSerdes.deserialize(in, attr.data(), attr.size());
        attrs[name] = attr;
    }

private:
    serdes mSerdes;
};
}
}
