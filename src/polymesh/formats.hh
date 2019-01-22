#pragma once

#include <array>
#include <cstring>

#include "Mesh.hh"

namespace polymesh
{
/// loads a mesh from a file
template <class Vec3>
bool load(std::string const& filename, Mesh& m, vertex_attribute<Vec3>& pos);
/// saves a mesh to a file
template <class Vec3>
void save(std::string const& filename, Mesh& m, vertex_attribute<Vec3> const& pos);

// ------- IMPLEMENTATION -------

namespace detail
{
template <class ScalarT>
bool load(std::string const& filename, Mesh& m, vertex_attribute<std::array<ScalarT, 3>>& pos);
template <class ScalarT>
void save(std::string const& filename, Mesh& m, vertex_attribute<std::array<ScalarT, 3>> const& pos);
} // namespace detail

template <class Vec3>
bool load(std::string const& filename, Mesh& m, vertex_attribute<Vec3>& pos)
{
    static_assert(sizeof(Vec3) == sizeof(float) * 3 || sizeof(Vec3) == sizeof(double) * 3, "position type must be 3 floats or 3 doubles");

    bool ok;
    if (sizeof(Vec3) == sizeof(float) * 3)
    {
        auto tmp_pos = m.vertices().make_attribute<std::array<float, 3>>();
        ok = detail::load<float>(filename, m, tmp_pos);
        std::memcpy(pos.data(), tmp_pos.data(), sizeof(Vec3) * m.vertices().size());
    }
    else
    {
        auto tmp_pos = m.vertices().make_attribute<std::array<double, 3>>();
        ok = detail::load<double>(filename, m, tmp_pos);
        std::memcpy(pos.data(), tmp_pos.data(), sizeof(Vec3) * m.vertices().size());
    }

    return ok;
}

template <class Vec3>
void save(std::string const& filename, Mesh& m, vertex_attribute<Vec3>& pos)
{
    static_assert(sizeof(Vec3) == sizeof(float) * 3 || sizeof(Vec3) == sizeof(double) * 3, "position type must be 3 floats or 3 doubles");

    if (sizeof(Vec3) == sizeof(float) * 3)
    {
        auto tmp_pos = m.vertices().make_attribute<std::array<float, 3>>();
        std::memcpy(tmp_pos.data(), pos.data(), sizeof(Vec3) * m.vertices().size());
        detail::save<float>(filename, m, tmp_pos);
    }
    else
    {
        auto tmp_pos = m.vertices().make_attribute<std::array<double, 3>>();
        std::memcpy(tmp_pos.data(), pos.data(), sizeof(Vec3) * m.vertices().size());
        detail::save<double>(filename, m, tmp_pos);
    }
}
} // namespace polymesh
