#include "formats.hh"

#include <algorithm>

#include "formats/obj.hh"
#include "formats/off.hh"
#include "formats/pm.hh"
#include "formats/stl.hh"

void polymesh::load(const std::string &filename, polymesh::Mesh &m, vertex_attribute<glm::vec3> &pos)
{
    if (filename.find('.') == std::string::npos)
    {
        std::cerr << "could not find extension of " << filename << std::endl;
        return;
    }

    auto ext = filename.substr(filename.rfind('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "obj")
    {
        read_obj(filename, m, pos);
    }
    else if (ext == "off")
    {
        read_off(filename, m, pos);
    }
    else if (ext == "stl")
    {
        read_stl(filename, m, pos);
    }
    else
    {
        std::cerr << "unknown extension: " << ext << " (of " << filename << ")" << std::endl;
    }
}
