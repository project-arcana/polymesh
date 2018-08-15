#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <string>

#include "../Mesh.hh"

namespace polymesh
{
void write_stl_binary(std::string const& filename, Mesh const& mesh, vertex_attribute<glm::vec3> const& position, face_attribute<glm::vec3> const* normals = nullptr);
void write_stl_binary(std::ostream& out, Mesh const& mesh, vertex_attribute<glm::vec3> const& position, face_attribute<glm::vec3> const* normals = nullptr);
bool read_stl(std::string const& filename, Mesh& mesh, vertex_attribute<glm::vec3>& position, face_attribute<glm::vec3>* normals = nullptr);
bool read_stl(std::istream& input, Mesh& mesh, vertex_attribute<glm::vec3>& position, face_attribute<glm::vec3>* normals = nullptr);
bool read_stl_binary(std::istream& input, Mesh& mesh, vertex_attribute<glm::vec3>& position, face_attribute<glm::vec3>* normals = nullptr);
bool read_stl_ascii(std::istream& input, Mesh& mesh, vertex_attribute<glm::vec3>& position, face_attribute<glm::vec3>* normals = nullptr);
bool is_ascii_stl(std::istream& input);
}
