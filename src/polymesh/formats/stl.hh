#pragma once

#include <array>
#include <iosfwd>
#include <string>

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class ScalarT>
void write_stl_binary(std::string const& filename,
                      vertex_attribute<std::array<ScalarT, 3>> const& position,
                      face_attribute<std::array<ScalarT, 3>> const* normals = nullptr);
template <class ScalarT>
void write_stl_binary(std::ostream& out, vertex_attribute<std::array<ScalarT, 3>> const& position, face_attribute<std::array<ScalarT, 3>> const* normals = nullptr);
template <class ScalarT>
bool read_stl(std::string const& filename, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position, face_attribute<std::array<ScalarT, 3>>* normals = nullptr);
template <class ScalarT>
bool read_stl(std::istream& input, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position, face_attribute<std::array<ScalarT, 3>>* normals = nullptr);
template <class ScalarT>
bool read_stl_binary(std::istream& input, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position, face_attribute<std::array<ScalarT, 3>>* normals = nullptr);
template <class ScalarT>
bool read_stl_ascii(std::istream& input, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position, face_attribute<std::array<ScalarT, 3>>* normals = nullptr);

bool is_ascii_stl(std::istream& input);
} // namespace polymesh
