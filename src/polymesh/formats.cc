#include "formats.hh"

#include <algorithm>
#include <fstream>

#include "formats/obj.hh"
#include "formats/off.hh"
#include "formats/pm.hh"
#include "formats/stl.hh"

bool polymesh::load(const std::string &filename, polymesh::Mesh &m, vertex_attribute<glm::vec3> &pos)
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
        std::cerr << "unknown extension: " << ext << " (of " << filename << ")" << std::endl;
        return false;
    }
}
