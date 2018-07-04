#include "off.hh"

#include <fstream>
#include <sstream>

namespace polymesh
{
void write_off(const std::string &filename, const Mesh &mesh, const vertex_attribute<glm::vec3> &position)
{
    std::ofstream file(filename);
    write_off(file, mesh, position);
}
void write_off(std::ostream &out, const Mesh &mesh, const vertex_attribute<glm::vec3> &position)
{
    out << "OFF\n";
    out << mesh.vertices().size() << " " << mesh.faces().size() << " " << mesh.edges().size() << "\n";

    for (auto v : mesh.all_vertices())
    {
        auto pos = v[position];
        out << pos.x << " " << pos.y << " " << pos.z << "\n";
    }

    for (auto f : mesh.faces())
    {
        out << f.vertices().size();
        for (auto v : f.vertices())
            out << " " << v.idx.value;
        out << "\n";
    }
}

bool read_off(const std::string &filename, Mesh &mesh, vertex_attribute<glm::vec3> &position)
{
    std::ifstream file(filename);
    if (!file.good())
        return false;

    return read_off(file, mesh, position);
}

bool read_off(std::istream &input, Mesh &mesh, vertex_attribute<glm::vec3> &position)
{
    std::string str;
    input >> str;
    if (str != "OFF")
        return false;

    // read counts
    int v_cnt, f_cnt, e_cnt;
    input >> v_cnt >> f_cnt >> e_cnt;
    (void)e_cnt; // unused

    // read vertices
    for (auto i = 0; i < v_cnt; ++i)
    {
        auto v = mesh.vertices().add();
        auto &pos = v[position];
        input >> pos.x >> pos.y >> pos.z;
    }

    // read faces
    std::vector<vertex_handle> vs;
    for (auto i = 0; i < f_cnt; ++i)
    {
        int valence;
        input >> valence;
        vs.resize(valence);
        for (auto vi = 0; vi < valence; ++vi)
        {
            int v;
            input >> v;
            vs[vi] = mesh[vertex_index(v)];
        }

        mesh.faces().add(vs);
    }

    return true;
}
}
