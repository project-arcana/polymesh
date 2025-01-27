#pragma once

#include <array>

#include <clean-core/string_view.hh>

#include "Mesh.hh"

namespace polymesh
{
/// loads a mesh from a file
template <class Pos3>
bool load(cc::string_view filename, Mesh& m, vertex_attribute<Pos3>& pos);
/// saves a mesh to a file
template <class Pos3>
void save(cc::string_view filename, vertex_attribute<Pos3> const& pos);

template <class Pos3>
struct load_result
{
    unique_ptr<Mesh> mesh;
    vertex_attribute<Pos3> pos;
    explicit operator bool() const { return mesh != nullptr; }
};

template <class Pos3>
load_result<Pos3> load(cc::string_view filename)
{
    auto m = Mesh::create();
    auto pos = vertex_attribute<Pos3>(*m);
    load(filename, *m, pos);
    return {std::move(m), std::move(pos)};
}

// ------- IMPLEMENTATION -------

namespace detail
{
template <class ScalarT>
bool load(cc::string_view filename, Mesh& m, vertex_attribute<std::array<ScalarT, 3>>& pos);
template <class ScalarT>
void save(cc::string_view filename, vertex_attribute<std::array<ScalarT, 3>> const& pos);
} // namespace detail

template <class Pos3>
bool load(cc::string_view filename, Mesh& m, vertex_attribute<Pos3>& pos)
{
    static_assert(sizeof(Pos3) == sizeof(float) * 3 || sizeof(Pos3) == sizeof(double) * 3, "position type must be 3 floats or 3 doubles");
    POLYMESH_ASSERT(&m == &pos.mesh() && "wrong mesh");

    bool ok;
    if (sizeof(Pos3) == sizeof(float) * 3)
    {
        auto tmp_pos = m.vertices().make_attribute<std::array<float, 3>>();
        ok = detail::load<float>(filename, m, tmp_pos);
        std::memcpy(pos.data(), tmp_pos.data(), sizeof(Pos3) * m.all_vertices().size());
    }
    else
    {
        auto tmp_pos = m.vertices().make_attribute<std::array<double, 3>>();
        ok = detail::load<double>(filename, m, tmp_pos);
        std::memcpy(pos.data(), tmp_pos.data(), sizeof(Pos3) * m.all_vertices().size());
    }

    return ok;
}

template <class Pos3>
void save(cc::string_view filename, vertex_attribute<Pos3> const& pos)
{
    static_assert(sizeof(Pos3) == sizeof(float) * 3 || sizeof(Pos3) == sizeof(double) * 3, "position type must be 3 floats or 3 doubles");

    auto const& m = pos.mesh();
    if (sizeof(Pos3) == sizeof(float) * 3)
    {
        auto tmp_pos = m.vertices().template make_attribute<std::array<float, 3>>();
        std::memcpy(tmp_pos.data(), pos.data(), sizeof(Pos3) * m.all_vertices().size());
        detail::save<float>(filename, tmp_pos);
    }
    else
    {
        auto tmp_pos = m.vertices().template make_attribute<std::array<double, 3>>();
        std::memcpy(tmp_pos.data(), pos.data(), sizeof(Pos3) * m.all_vertices().size());
        detail::save<double>(filename, tmp_pos);
    }
}
} // namespace polymesh
