#pragma once

#include <iosfwd>

#include <clean-core/string_view.hh>

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class ScalarT>
void write_obj(cc::string_view filename, vertex_attribute<std::array<ScalarT, 3>> const& position);
template <class ScalarT>
bool read_obj(cc::string_view filename, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position);

template <class ScalarT>
struct obj_writer
{
    obj_writer(cc::string_view filename);
    obj_writer(std::ostream& out);
    ~obj_writer();

    void write_object_name(std::string object_name);
    void write_mesh(vertex_attribute<std::array<ScalarT, 4>> const& position,
                    halfedge_attribute<std::array<ScalarT, 3>> const* tex_coord = nullptr,
                    halfedge_attribute<std::array<ScalarT, 3>> const* normal = nullptr);
    void write_mesh(vertex_attribute<std::array<ScalarT, 3>> const& position,
                    vertex_attribute<std::array<ScalarT, 2>> const* tex_coord = nullptr,
                    vertex_attribute<std::array<ScalarT, 3>> const* normal = nullptr);

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
template <class ScalarT>
struct obj_reader
{
    obj_reader(cc::string_view filename, Mesh& mesh);
    obj_reader(std::istream& in, Mesh& mesh);

    // get properties of the obj
public:
    vertex_attribute<std::array<ScalarT, 4>> const& get_positions() const { return positions; }
    halfedge_attribute<std::array<ScalarT, 3>> const& get_tex_coords() const { return tex_coords; }
    halfedge_attribute<std::array<ScalarT, 3>> const& get_normals() const { return normals; }

    /// Number of faces that could not be added
    int error_faces() const { return n_error_faces; }

private:
    void parse(std::istream& in, Mesh& mesh);

    vertex_attribute<std::array<ScalarT, 4>> positions;
    halfedge_attribute<std::array<ScalarT, 3>> tex_coords;
    halfedge_attribute<std::array<ScalarT, 3>> normals;

    int n_error_faces = 0;
};
} // namespace polymesh
