#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <string>

#include "../Mesh.hh"

namespace polymesh
{
void write_obj(std::string const& filename,
               Mesh const& mesh,
               vertex_attribute<glm::vec3> const& position,
               vertex_attribute<glm::vec2> const* tex_coord = nullptr,
               vertex_attribute<glm::vec3> const* normal = nullptr);

struct obj_writer
{
    obj_writer(std::string const& filename);
    obj_writer(std::ostream& out);
    ~obj_writer();

    void write_object_name(std::string object_name);
    void write_mesh(Mesh const& mesh,
                    vertex_attribute<glm::vec3> const& position,
                    vertex_attribute<glm::vec2> const* tex_coord = nullptr,
                    vertex_attribute<glm::vec3> const* normal = nullptr);

    // TODO: tex coords and normals as half-edge attributes

private:
    std::ostream* tmp_out = nullptr;
    std::ostream* out = nullptr;

    int vertex_idx = 1;
    int texture_idx = 1;
    int normal_idx = 1;
};

// clears the given mesh before adding data
// obj must be manifold
// no negative indices
struct obj_reader
{
    obj_reader(std::string const& filename, Mesh& mesh);
    obj_reader(std::istream& in, Mesh& mesh);

    // get properties of the obj
    // NOTE: these always return fresh copies of the attribute!
public:
    vertex_attribute<glm::vec4> positions_vec4() const;
    vertex_attribute<glm::vec3> positions_vec3() const;
    halfedge_attribute<glm::vec3> tex_coords_vec3() const;
    halfedge_attribute<glm::vec3> normals_vec3() const;

private:
    void parse(std::istream& in, Mesh& mesh);

    vertex_attribute<glm::vec4> positions;
    halfedge_attribute<glm::vec3> tex_coords;
    halfedge_attribute<glm::vec3> normals;
};
}
