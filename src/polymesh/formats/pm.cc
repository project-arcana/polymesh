#include "pm.hh"
#include <fstream>
#include <iostream>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include "../low_level_api.hh"

namespace polymesh
{
static std::unordered_map<std::string, std::unique_ptr<detail::GenericAttributeSerializer>> sSerializers;

void detail::register_attribute_serializer(const std::string &identifier, std::unique_ptr<detail::GenericAttributeSerializer> ptr)
{
    sSerializers[identifier] = std::move(ptr);
}

template <class Tag>
std::pair<std::string, detail::GenericAttributeSerializer *> find_serializer_for(primitive_attribute_base<Tag> const &attr)
{
    for (auto const &pair : sSerializers)
    {
        if (pair.second->is_compatible_to(attr))
            return {pair.first, pair.second.get()};
    }
    return {{}, nullptr};
}

struct pm_header
{
    char pm[4] = {'P', 'M', 0, 0};
    int32_t num_vertices;
    int32_t num_halfedges;
    int32_t num_faces;

    int32_t num_vertex_attributes;
    int32_t num_halfedge_attributes;
    int32_t num_edge_attributes;
    int32_t num_face_attributes;

    bool valid() const { return pm[0] == 'P' && pm[1] == 'M' && pm[2] == 0 && pm[3] == 0; }
};

template <class tag>
static std::istream &read_index(std::istream &in, primitive_index<tag> &idx)
{
    int32_t val;
    in.read(reinterpret_cast<char *>(&val), sizeof(int32_t));
    idx.value = val;
    return in;
}

template <class tag>
static std::ostream &write_index(std::ostream &out, primitive_index<tag> const &idx)
{
    const int32_t val = idx.value;
    return out.write(reinterpret_cast<char const *>(&val), sizeof(int32_t));
}

static std::ostream &write_string(std::ostream &out, std::string const &text) { return out.write(text.c_str(), text.size() + 1); }
static std::istream &read_string(std::istream &in, std::string &text) { return std::getline(in, text, '\0'); }

static const std::string unregistered_type_name = "UNREGISTERED_TYPE";

template <class tag>
static std::ostream &storeAttributes(std::ostream &out, std::map<std::string, std::unique_ptr<primitive_attribute_base<tag>>> const &attrs)
{
    for (auto const &attr : attrs)
    {
        write_string(out, attr.first); // Attribute Name

        auto const &ser = find_serializer_for(*attr.second);
        if (ser.second)
        {
            write_string(out, ser.first);             // Attribute Type
            ser.second->serialize(out, *attr.second); // Attribute Data
        }
        else
        {
            write_string(out, unregistered_type_name);
            std::cout << "polymesh::write_pm: " << attr.first << " has unregistered type and is not going to be written." << std::endl;
        }
    }
    return out;
}

template <class tag>
static bool restoreAttributes(std::istream &in, Mesh const &mesh, attribute_collection &attrs, uint32_t count)
{
    using tag_ptr = tag *;

    for (uint32_t i = 0; i < count; i++)
    {
        std::string attrName, attrType;
        read_string(in, attrName);
        read_string(in, attrType);

        if (attrType == unregistered_type_name)
            continue;

        auto it = sSerializers.find(attrType);
        if (it != sSerializers.end())
        {
            it->second->deserialize(in, mesh, attrs, attrName, tag_ptr{});
        }
        else
        {
            std::cout << "polymesh::read_pm: " << attrName << " has unregistered type " << attrType << ", unable to restore remaining attributes." << std::endl;
            return false;
        }
    }

    return true;
}

void write_pm(std::ostream &out, const Mesh &mesh, const attribute_collection &attributes)
{
    auto ll = low_level_api(mesh);

    if (!mesh.is_compact())
        std::cout << "polymesh::write_pm: saving a non-compact mesh." << std::endl;

    pm_header header;
    header.num_vertices = mesh.all_vertices().size();
    header.num_halfedges = mesh.all_halfedges().size();
    header.num_faces = mesh.all_faces().size();
    header.num_vertex_attributes = attributes.vertex_attributes().size();
    header.num_halfedge_attributes = attributes.halfedge_attributes().size();
    header.num_edge_attributes = attributes.edge_attributes().size();
    header.num_face_attributes = attributes.face_attributes().size();
    out.write(reinterpret_cast<char *>(&header), sizeof(header));

    // Store mesh topology
    for (int i = 0; i < header.num_faces; ++i)
        write_index(out, ll.halfedge_of(face_index(i)));
    for (int i = 0; i < header.num_vertices; i++)
        write_index(out, ll.outgoing_halfedge_of(vertex_index(i)));

    for (int i = 0; i < header.num_halfedges; ++i)
        write_index(out, ll.to_vertex_of(halfedge_index(i)));
    for (int i = 0; i < header.num_halfedges; ++i)
        write_index(out, ll.face_of(halfedge_index(i)));
    for (int i = 0; i < header.num_halfedges; ++i)
        write_index(out, ll.next_halfedge_of(halfedge_index(i)));
    for (int i = 0; i < header.num_halfedges; ++i)
        write_index(out, ll.prev_halfedge_of(halfedge_index(i)));

    // Store attributes
    storeAttributes(out, attributes.vertex_attributes());
    storeAttributes(out, attributes.halfedge_attributes());
    storeAttributes(out, attributes.edge_attributes());
    storeAttributes(out, attributes.face_attributes());
}

bool read_pm(std::istream &input, Mesh &mesh, attribute_collection &attributes)
{
    pm_header header;
    input.read(reinterpret_cast<char *>(&header), sizeof(header));
    assert(header.valid() && "PM-File contains the wrong magic number!");

    mesh.clear();
    auto ll = low_level_api(mesh);
    ll.alloc_primitives(header.num_vertices, header.num_faces, header.num_halfedges);

    for (int i = 0; i < header.num_faces; ++i)
        read_index(input, ll.halfedge_of(face_index(i)));
    for (int i = 0; i < header.num_vertices; i++)
        read_index(input, ll.outgoing_halfedge_of(vertex_index(i)));

    for (int i = 0; i < header.num_halfedges; ++i)
        read_index(input, ll.to_vertex_of(halfedge_index(i)));
    for (int i = 0; i < header.num_halfedges; ++i)
        read_index(input, ll.face_of(halfedge_index(i)));
    for (int i = 0; i < header.num_halfedges; ++i)
        read_index(input, ll.next_halfedge_of(halfedge_index(i)));
    for (int i = 0; i < header.num_halfedges; ++i)
        read_index(input, ll.prev_halfedge_of(halfedge_index(i)));

    // Restore attributes
    return restoreAttributes<vertex_tag>(input, mesh, attributes, header.num_vertex_attributes)
           && restoreAttributes<halfedge_tag>(input, mesh, attributes, header.num_halfedge_attributes)
           && restoreAttributes<edge_tag>(input, mesh, attributes, header.num_edge_attributes)
           && restoreAttributes<face_tag>(input, mesh, attributes, header.num_face_attributes) //
           && !input.fail();
}

void write_pm(const std::string &filename, const Mesh &mesh, const attribute_collection &attributes)
{
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    write_pm(out, mesh, attributes);
}

bool read_pm(const std::string &filename, Mesh &mesh, attribute_collection &attributes)
{
    std::ifstream in(filename, std::ios::binary);
    return read_pm(in, mesh, attributes);
}
}
