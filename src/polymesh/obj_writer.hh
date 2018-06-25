#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <string>

#include "Mesh.hh"

namespace polymesh
{
struct obj_writer
{
    obj_writer(std::string const& filename);
    obj_writer(std::ostream& out);
    ~obj_writer();

    void write_object_name(std::string object_name);
    void write_mesh(Mesh const& mesh,
                    vertex_property<glm::vec3> const& position,
                    vertex_property<glm::vec2> const* tex_coord = nullptr,
                    vertex_property<glm::vec3> const* normal = nullptr);

private:
    std::ostream* tmp_out = nullptr;
    std::ostream* out = nullptr;

    int vertex_idx = 1;
    int texture_idx = 1;
    int normal_idx = 1;
};
}
