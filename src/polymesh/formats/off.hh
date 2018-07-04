#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <string>

#include "../Mesh.hh"

namespace polymesh
{
void write_off(std::string const& filename, Mesh const& mesh, vertex_attribute<glm::vec3> const& position);
void write_off(std::ostream& out, Mesh const& mesh, vertex_attribute<glm::vec3> const& position);
bool read_off(std::string const& filename, Mesh& mesh, vertex_attribute<glm::vec3>& position);
bool read_off(std::istream& input, Mesh& mesh, vertex_attribute<glm::vec3>& position);
}
