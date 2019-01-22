#pragma once

#include <array>
#include <iostream>
#include <string>

#include "../Mesh.hh"

namespace polymesh
{
template <class ScalarT>
void write_off(std::string const& filename, Mesh const& mesh, vertex_attribute<std::array<ScalarT, 3>> const& position);
template <class ScalarT>
void write_off(std::ostream& out, Mesh const& mesh, vertex_attribute<std::array<ScalarT, 3>> const& position);
template <class ScalarT>
bool read_off(std::string const& filename, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position);
template <class ScalarT>
bool read_off(std::istream& input, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position);
} // namespace polymesh
