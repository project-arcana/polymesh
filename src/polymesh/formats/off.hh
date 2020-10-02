#pragma once

#include <array>
#include <iosfwd>
#include <string>

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class ScalarT>
void write_off(std::string const& filename, vertex_attribute<std::array<ScalarT, 3>> const& position);
template <class ScalarT>
void write_off(std::ostream& out, vertex_attribute<std::array<ScalarT, 3>> const& position);
template <class ScalarT>
bool read_off(std::string const& filename, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position);
template <class ScalarT>
bool read_off(std::istream& input, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position);
} // namespace polymesh
