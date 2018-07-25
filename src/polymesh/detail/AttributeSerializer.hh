#pragma once

#include <iostream>
#include <typeindex>

#include "../attributes.hh"

namespace polymesh
{
namespace detail
{
class GenericAttributeSerializer
{
public:
    virtual ~GenericAttributeSerializer() {}

    /// can this attribute be (de)serialized by this serializer?
    virtual bool isCompatibleTo(primitive_attribute_base<vertex_tag> const &attr) = 0;
    virtual bool isCompatibleTo(primitive_attribute_base<halfedge_tag> const &attr) = 0;
    virtual bool isCompatibleTo(primitive_attribute_base<edge_tag> const &attr) = 0;
    virtual bool isCompatibleTo(primitive_attribute_base<face_tag> const &attr) = 0;

    virtual void serialize(std::ostream &out, primitive_attribute_base<vertex_tag> const &attr) = 0;
    virtual void serialize(std::ostream &out, primitive_attribute_base<halfedge_tag> const &attr) = 0;
    virtual void serialize(std::ostream &out, primitive_attribute_base<edge_tag> const &attr) = 0;
    virtual void serialize(std::ostream &out, primitive_attribute_base<face_tag> const &attr) = 0;

    virtual void deserialize(std::istream &in, primitive_attribute_base<vertex_tag> &attr) = 0;
    virtual void deserialize(std::istream &in, primitive_attribute_base<halfedge_tag> &attr) = 0;
    virtual void deserialize(std::istream &in, primitive_attribute_base<edge_tag> &attr) = 0;
    virtual void deserialize(std::istream &in, primitive_attribute_base<face_tag> &attr) = 0;

    virtual std::type_index vertexAttributeType() = 0;
};

template <typename T, typename SerDes>
class AttributeSerializer : GenericAttributeSerializer
{
public:
    AttributeSerializer(SerDes const& serdes) : mSerdes(serdes) {}

    bool isCompatibleTo(primitive_attribute_base<vertex_tag> const &attr) override
    {
        return dynamic_cast<primitive_attribute<vertex_tag, T> const *>(&attr);
    }
    bool isCompatibleTo(primitive_attribute_base<halfedge_tag> const &attr) override
    {
        return dynamic_cast<primitive_attribute<halfedge_tag, T> const *>(&attr);
    }
    bool isCompatibleTo(primitive_attribute_base<edge_tag> const &attr) override
    {
        return dynamic_cast<primitive_attribute<edge_tag, T> const *>(&attr);
    }
    bool isCompatibleTo(primitive_attribute_base<face_tag> const &attr) override
    {
        return dynamic_cast<primitive_attribute<face_tag, T> const *>(&attr);
    }

    void serialize(std::ostream &out, primitive_attribute_base<vertex_tag> const &attr) override
    {
        auto const &specific = dynamic_cast<primitive_attribute<vertex_tag, T> const &>(attr);
        mSerdes.serialize(out, specific.data(), specific.size());
    }

    void deserialize(std::istream &in, primitive_attribute_base<vertex_tag> &attr) override
    {
        auto &specific = dynamic_cast<primitive_attribute<vertex_tag, T> &>(attr);
        mSerdes.deserialize(in, specific.data(), specific.size());
    }

    std::type_index vertexAttributeType() override {
        return typeid (primitive_attribute<vertex_tag, T>);
    }

private:
    SerDes mSerdes;
};

}
}
