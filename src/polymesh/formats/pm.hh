#pragma once

#include <iosfwd>
#include <string>

#include "../Mesh.hh"
#include "../attribute_collection.hh"
#include "../attributes.hh"

#include "../detail/AttributeSerializer.hh"

namespace polymesh
{
void write_pm(std::string const& filename, Mesh const& mesh, attribute_collection const& attributes);
void write_pm(std::ostream& out, Mesh const& mesh, attribute_collection const& attributes);

bool read_pm(std::string const& filename, Mesh& mesh, attribute_collection& attributes);
bool read_pm(std::istream& input, Mesh& mesh, attribute_collection& attributes);

namespace detail
{
void ostream_write(std::ostream& out, char const* data, size_t size);
void ostream_read(std::istream& in, char* data, size_t size);

template <typename T>
struct bytewise_serdes
{
    void serialize(std::ostream& out, T const* data, size_t num_items) const
    {
        ostream_write(out, reinterpret_cast<char const*>(data), num_items * sizeof(T));
    }
    void deserialize(std::istream& in, T* data, size_t num_items) const { ostream_read(in, reinterpret_cast<char*>(data), num_items * sizeof(T)); }
};

struct string_serdes
{
    void serialize(std::ostream& out, std::string const* strings, size_t num_items) const
    {
        for (size_t i = 0; i < num_items; i++)
            ostream_write(out, strings[i].c_str(), strings[i].size() + 1);
    }
    void deserialize(std::istream& in, std::string* strings, size_t num_items) const
    {
        for (size_t i = 0; i < num_items; i++)
            std::getline(in, strings[i], '\0');
    }
};

void register_attribute_serializer(std::string const& identifier, unique_ptr<detail::GenericAttributeSerializer> ptr);
}

template <typename T, typename serdes = detail::bytewise_serdes<T>>
void register_type(std::string const& identifier, serdes&& serializer = detail::bytewise_serdes<T>{})
{
    unique_ptr<detail::GenericAttributeSerializer> ptr;
    ptr.reset(new detail::AttributeSerializer<T, serdes>(serializer));
    detail::register_attribute_serializer(identifier, std::move(ptr));
}
}
