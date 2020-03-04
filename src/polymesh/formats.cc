#include <iostream>

#include "formats.hh"

#include <algorithm>
#include <fstream>

#include "formats/obj.hh"
#include "formats/off.hh"
#include "formats/stl.hh"

template <class ScalarT>
bool polymesh::detail::load(const std::string& filename, polymesh::Mesh& m, vertex_attribute<std::array<ScalarT, 3>>& pos)
{
    if (!std::ifstream(filename).good())
    {
        std::cerr << "File does not exist or is not readable: " << filename << std::endl;
        return false;
    }

    if (filename.find('.') == std::string::npos)
    {
        std::cerr << "could not find extension of " << filename << std::endl;
        return false;
    }

    auto ext = filename.substr(filename.rfind('.') + 1);
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
        std::cerr << "unknown/unsupported extension: " << ext << " (of " << filename << ")" << std::endl;
        return false;
    }
}

template <class ScalarT>
void polymesh::detail::save(std::string const& filename, vertex_attribute<std::array<ScalarT, 3>> const& pos)
{
    auto ext = filename.substr(filename.rfind('.') + 1);
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
        std::cerr << "unknown/unsupported extension: " << ext << " (of " << filename << ")" << std::endl;
    }
}

template bool polymesh::detail::load<float>(std::string const& filename, Mesh& m, vertex_attribute<std::array<float, 3>>& pos);
template bool polymesh::detail::load<double>(std::string const& filename, Mesh& m, vertex_attribute<std::array<double, 3>>& pos);

template void polymesh::detail::save<float>(std::string const& filename, vertex_attribute<std::array<float, 3>> const& pos);
template void polymesh::detail::save<double>(std::string const& filename, vertex_attribute<std::array<double, 3>> const& pos);
