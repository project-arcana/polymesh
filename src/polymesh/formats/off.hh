#pragma once

#include <array>
#include <iosfwd>

#include <clean-core/string_view.hh>

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class ScalarT>
void write_off(cc::string_view filename, vertex_attribute<std::array<ScalarT, 3>> const& position);
template <class ScalarT>
void write_off(std::ostream& out, vertex_attribute<std::array<ScalarT, 3>> const& position);
template <class ScalarT>
bool read_off(cc::string_view filename, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position);
template <class ScalarT>
bool read_off(std::istream& input, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position);
} // namespace polymesh
