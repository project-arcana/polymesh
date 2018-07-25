#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <string>

#include "../Mesh.hh"
#include "../attribute_collection.hh"
#include "../attributes.hh"

#include "../detail/AttributeSerializer.hh"

namespace polymesh
{
void write_pm(std::string const &filename, Mesh const &mesh, attribute_collection const &attributes);
void write_pm(std::ostream &out, Mesh const &mesh, attribute_collection const &attributes);

bool read_pm(std::string const &filename, Mesh &mesh, attribute_collection &attributes);
bool read_pm(std::istream &input, Mesh &mesh, attribute_collection &attributes);

namespace detail
{
template <typename T>
struct bytewise_serdes
{
    void serialize(std::ostream &out, T const *data, size_t num_items) { out.write(static_cast<char const *>(data), num_items * sizeof(T)); }
    void deserialize(std::istream &in, T *data, size_t num_items) { in.read(static_cast<char *>(data), num_items * sizeof(T)); }
};

void registerAttributeSerializer(std::string const &identifier, std::unique_ptr<detail::GenericAttributeSerializer> ptr);
}

template <typename T, typename SerDes>
void registerType(std::string const &identifier, SerDes &&serializer = detail::bytewise_serdes<T>{})
{
    auto ptr = std::make_unique<detail::AttributeSerializer<T, SerDes>>(serializer);
    detail::registerAttributeSerializer(identifier, std::move(ptr));
}
}
