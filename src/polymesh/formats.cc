#include <iostream>

#include "formats.hh"

#include <algorithm>
#include <fstream>

#include "formats/obj.hh"
#include "formats/off.hh"
#include "formats/stl.hh"

template <class ScalarT>
bool polymesh::detail::load(cc::string_view filename, polymesh::Mesh& m, vertex_attribute<std::array<ScalarT, 3>>& pos)
{
    auto filename_str = std::string(filename.begin(), filename.end());
    if (!std::ifstream(filename_str).good())
    {
        std::cerr << "File does not exist or is not readable: " << filename_str << std::endl;
        return false;
    }

    if (!filename.contains('.'))
    {
        std::cerr << "could not find extension of " << filename_str << std::endl;
        return false;
    }

    auto ext = filename_str.substr(filename_str.rfind('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "obj")
    {
        return read_obj(filename, m, pos);
    }
    else if (ext == "off")
    {
        return read_off(filename, m, pos);
    }
    else if (ext == "stl")
    {
        return read_stl(filename, m, pos);
    }
    else
    {
        std::cerr << "unknown/unsupported extension: " << ext << " (of " << filename_str << ")" << std::endl;
        return false;
    }
}

template <class ScalarT>
void polymesh::detail::save(cc::string_view filename, vertex_attribute<std::array<ScalarT, 3>> const& pos)
{
    auto filename_str = std::string(filename.begin(), filename.end());
    auto ext = filename_str.substr(filename_str.rfind('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "obj")
    {
        return write_obj(filename, pos);
    }
    else if (ext == "off")
    {
        return write_off(filename, pos);
    }
    else if (ext == "stl")
    {
        return write_stl_binary(filename, pos);
    }
    else
    {
        std::cerr << "unknown/unsupported extension: " << ext << " (of " << filename_str << ")" << std::endl;
    }
}

template bool polymesh::detail::load<float>(cc::string_view filename, Mesh& m, vertex_attribute<std::array<float, 3>>& pos);
template bool polymesh::detail::load<double>(cc::string_view filename, Mesh& m, vertex_attribute<std::array<double, 3>>& pos);

template void polymesh::detail::save<float>(cc::string_view filename, vertex_attribute<std::array<float, 3>> const& pos);
template void polymesh::detail::save<double>(cc::string_view filename, vertex_attribute<std::array<double, 3>> const& pos);
