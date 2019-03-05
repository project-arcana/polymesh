#include "pm.hh"

#include <fstream>
#include <iostream>
#include <mutex>
#include <typeindex>
#include <unordered_map>

#ifdef POLYMESH_SUPPORT_GLM
#include <glm/matrix.hpp>
#include <glm/ext/quaternion_float.hpp>
#endif

#include "../low_level_api.hh"

namespace polymesh
{
namespace
{
std::unordered_map<std::string, std::unique_ptr<detail::GenericAttributeSerializer>> sSerializers;
const std::string unregistered_type_name = "UNREGISTERED_TYPE";
} // namespace

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


#define REGISTER_TYPE(type) register_type<type>(#type)
static bool registered_default_types = []() {
    REGISTER_TYPE(bool);
    REGISTER_TYPE(float);
    REGISTER_TYPE(double);

    REGISTER_TYPE(int8_t);
    REGISTER_TYPE(int16_t);
    REGISTER_TYPE(int32_t);
    REGISTER_TYPE(int64_t);
    REGISTER_TYPE(uint8_t);
    REGISTER_TYPE(uint16_t);
    REGISTER_TYPE(uint32_t);
    REGISTER_TYPE(uint64_t);

#ifdef POLYMESH_SUPPORT_GLM
    REGISTER_TYPE(glm::vec2);
    REGISTER_TYPE(glm::vec3);
    REGISTER_TYPE(glm::vec4);
    REGISTER_TYPE(glm::bvec2);
    REGISTER_TYPE(glm::bvec3);
    REGISTER_TYPE(glm::bvec4);
    REGISTER_TYPE(glm::dvec2);
    REGISTER_TYPE(glm::dvec3);
    REGISTER_TYPE(glm::dvec4);
    REGISTER_TYPE(glm::ivec2);
    REGISTER_TYPE(glm::ivec3);
    REGISTER_TYPE(glm::ivec4);
    REGISTER_TYPE(glm::uvec2);
    REGISTER_TYPE(glm::uvec3);
    REGISTER_TYPE(glm::uvec4);

    REGISTER_TYPE(glm::mat2x2);
    REGISTER_TYPE(glm::mat2x3);
    REGISTER_TYPE(glm::mat2x4);
    REGISTER_TYPE(glm::mat3x2);
    REGISTER_TYPE(glm::mat3x3);
    REGISTER_TYPE(glm::mat3x4);
    REGISTER_TYPE(glm::mat4x2);
    REGISTER_TYPE(glm::mat4x3);
    REGISTER_TYPE(glm::mat4x4);

    REGISTER_TYPE(glm::dmat2x2);
    REGISTER_TYPE(glm::dmat2x3);
    REGISTER_TYPE(glm::dmat2x4);
    REGISTER_TYPE(glm::dmat3x2);
    REGISTER_TYPE(glm::dmat3x3);
    REGISTER_TYPE(glm::dmat3x4);
    REGISTER_TYPE(glm::dmat4x2);
    REGISTER_TYPE(glm::dmat4x3);
    REGISTER_TYPE(glm::dmat4x4);

    REGISTER_TYPE(glm::quat);
#endif

    register_type<std::string>("std::string", detail::string_serdes{});

    return true;
}();
} // namespace polymesh
