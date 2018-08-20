#pragma once

#include <glm/vec3.hpp>

#include "Mesh.hh"

namespace polymesh
{
/// loads a mesh from a file
void load(std::string const& filename, Mesh& m, vertex_attribute<glm::vec3>& pos);
}
