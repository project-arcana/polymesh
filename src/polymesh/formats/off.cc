#include "off.hh"

#include <fstream>
#include <iostream>
#include <sstream>

namespace polymesh
{
template <class ScalarT>
void write_off(const std::string& filename, vertex_attribute<std::array<ScalarT, 3>> const& position)
{
    std::ofstream file(filename);
    write_off(file, position);
}

template <class ScalarT>
void write_off(std::ostream& out, vertex_attribute<std::array<ScalarT, 3>> const& position)
{
    auto const& mesh = position.mesh();

    out << "OFF\n";
    out << mesh.vertices().size() << " " << mesh.faces().size() << " " << mesh.edges().size() << "\n";

    for (auto v : mesh.all_vertices())
    {
        auto pos = v[position];
        out << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
    }

    for (auto f : mesh.faces())
    {
        out << f.vertices().size();
        for (auto v : f.vertices())
            out << " " << v.idx.value;
        out << "\n";
    }
}

template <class ScalarT>
bool read_off(const std::string& filename, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position)
{
    std::ifstream file(filename);
    if (!file.good())
        return false;

    return read_off(file, mesh, position);
}

template <class ScalarT>
bool read_off(std::istream& input, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position)
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
        auto& pos = v[position];
        input >> pos[0] >> pos[1] >> pos[2];
    }

    // read faces
    auto non_manifold = 0;
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

        // ignore face colors
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (!mesh.faces().can_add(vs))
        {
            ++non_manifold;
            continue;
        }

        mesh.faces().add(vs);
    }

    if (non_manifold > 0)
        std::cerr << "skipped " << non_manifold << " face(s) because mesh would become non-manifold" << std::endl;

    return non_manifold == 0;
}

template void write_off<float>(std::string const& filename, vertex_attribute<std::array<float, 3>> const& position);
template void write_off<float>(std::ostream& out, vertex_attribute<std::array<float, 3>> const& position);
template bool read_off<float>(std::string const& filename, Mesh& mesh, vertex_attribute<std::array<float, 3>>& position);
template bool read_off<float>(std::istream& input, Mesh& mesh, vertex_attribute<std::array<float, 3>>& position);

template void write_off<double>(std::string const& filename, vertex_attribute<std::array<double, 3>> const& position);
template void write_off<double>(std::ostream& out, vertex_attribute<std::array<double, 3>> const& position);
template bool read_off<double>(std::string const& filename, Mesh& mesh, vertex_attribute<std::array<double, 3>>& position);
template bool read_off<double>(std::istream& input, Mesh& mesh, vertex_attribute<std::array<double, 3>>& position);
} // namespace polymesh
