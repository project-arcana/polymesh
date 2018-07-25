#include "pm.hh"
#include <iostream>
#include <fstream>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include "../low_level_api.hh"

namespace polymesh
{
static std::unordered_map<std::string, std::unique_ptr<detail::GenericAttributeSerializer>> sSerializersByName;
static std::unordered_map<std::type_index, detail::GenericAttributeSerializer *> sSerializersByTypeIndex;
static std::mutex sSerializersMutex;

void registerAttributeSerializer(const std::string &identifier, std::unique_ptr<detail::GenericAttributeSerializer> ptr)
{
    std::lock_guard<std::mutex> _(sSerializersMutex);
    sSerializersByTypeIndex[ptr->vertexAttributeType()] = ptr.get();
    sSerializersByName[identifier] = std::move(ptr);
}

struct PMHeader
{
    char pm[4] = {'P', 'M', 0, 0};
    uint32_t num_vertices;
    uint32_t num_halfedges;
    uint32_t num_faces;

    uint32_t num_vertex_attributes;
    uint32_t num_halfedge_attributes;
    uint32_t num_edge_attributes;
    uint32_t num_face_attributes;

    bool valid() const { return pm[0] == 'P' && pm[1] == 'M' && pm[2] == 0 && pm[3] == 0; }
};

template <class Tag>
static std::istream &read_index(std::istream &in, primitive_index<Tag> &idx)
{
    int32_t val;
    in.read(reinterpret_cast<char*>(&val), sizeof(int32_t));
    idx.value = val;
    return in;
}

template <class Tag>
static std::ostream& write_index(std::ostream& out, primitive_index<Tag> const& idx) {
    const int32_t val = idx.value;
    return out.write(reinterpret_cast<char const*>(&val), sizeof(int32_t));
}

void write_pm(std::ostream &out, const Mesh &mesh, const attribute_collection &attributes)
{
    auto ll = low_level_api(mesh);

    if (!mesh.is_compact())
        std::cout << "polymesh::write_pm: saving a non-compact mesh." << std::endl;

    PMHeader header;
    header.num_vertices = mesh.all_vertices().size();
    header.num_halfedges = mesh.all_halfedges().size();
    header.num_faces = mesh.all_faces().size();
    out.write(reinterpret_cast<char *>(&header), sizeof(header));

    for (int i = 0; i < header.num_halfedges; ++i)
        write_index(out, ll.face_of(halfedge_index(i)));

    for (int i = 0; i < header.num_halfedges; ++i)
        write_index(out, ll.to_vertex_of(halfedge_index(i)));

    for (int i = 0; i < header.num_halfedges; ++i)
        write_index(out, ll.next_halfedge_of(halfedge_index(i)));

    for (int i = 0; i < header.num_halfedges; ++i)
        write_index(out, ll.prev_halfedge_of(halfedge_index(i)));

    for (int i = 0; i < header.num_faces; ++i)
        write_index(out, ll.halfedge_of(face_index(i)));

    for (int i = 0; i < header.num_vertices; i++)
        write_index(out, ll.outgoing_halfedge_of(vertex_index(i)));
}

bool read_pm(std::istream &input, Mesh &mesh, attribute_collection &attributes)
{
    PMHeader header;
    input.read(reinterpret_cast<char *>(&header), sizeof(header));
    assert(header.valid() && "PM-File contains the wrong magic number!");

    mesh.clear();
    auto ll = low_level_api(mesh);
    ll.alloc_primitives(header.num_vertices, header.num_faces, header.num_halfedges);

    for (int i = 0; i < header.num_halfedges; ++i)
        read_index(input, ll.face_of(halfedge_index(i)));

    for (int i = 0; i < header.num_halfedges; ++i)
        read_index(input, ll.to_vertex_of(halfedge_index(i)));

    for (int i = 0; i < header.num_halfedges; ++i)
        read_index(input, ll.next_halfedge_of(halfedge_index(i)));

    for (int i = 0; i < header.num_halfedges; ++i)
        read_index(input, ll.prev_halfedge_of(halfedge_index(i)));

    for (int i = 0; i < header.num_faces; ++i)
        read_index(input, ll.halfedge_of(face_index(i)));

    for (int i = 0; i < header.num_vertices; i++)
        read_index(input, ll.outgoing_halfedge_of(vertex_index(i)));

    return !input.fail();
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
