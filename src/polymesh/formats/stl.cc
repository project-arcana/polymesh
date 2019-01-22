#include "stl.hh"

#include <cstddef>

#include <fstream>
#include <sstream>

/*
    UINT8[80] – Header
    UINT32 – Number of triangles

    foreach triangle
        REAL32[3] – Normal vector
        REAL32[3] – Vertex 1
        REAL32[3] – Vertex 2
        REAL32[3] – Vertex 3
        UINT16 – Attribute byte count
    end
 */

namespace polymesh
{
template <class ScalarT>
void write_stl_binary(const std::string &filename,
                      const Mesh &mesh,
                      const vertex_attribute<std::array<ScalarT, 3>> &position,
                      face_attribute<std::array<ScalarT, 3>> const *normals)
{
    std::ofstream file(filename, std::ios_base::binary);
    write_stl_binary(file, mesh, position, normals);
}

template <class ScalarT>
void write_stl_binary(std::ostream &out, const Mesh &mesh, const vertex_attribute<std::array<ScalarT, 3>> &position, face_attribute<std::array<ScalarT, 3>> const *normals)
{
    char header[80] = {};
    uint32_t n_triangles = mesh.faces().size();

    out.write(header, sizeof(header));
    out.write((char const *)&n_triangles, sizeof(n_triangles));

    for (auto f : mesh.faces())
    {
        auto n = f[normals];
        out.write((char const *)&n, sizeof(n));

        auto cnt = 0;
        for (auto v : f.vertices())
        {
            if (cnt >= 3)
            {
                std::cerr << "STL only supports triangles" << std::endl;
                break;
            }

            auto p = position[v];
            out.write((char const *)&p, sizeof(p));

            ++cnt;
        }

        uint16_t attr_cnt = 0;
        out.write((char const *)&attr_cnt, sizeof(attr_cnt));
    }
}

template <class ScalarT>
bool read_stl(const std::string &filename, Mesh &mesh, vertex_attribute<std::array<ScalarT, 3>> &position, face_attribute<std::array<ScalarT, 3>> *normals)
{
    std::ifstream file(filename);
    if (!file.good())
        return false;

    return read_stl(file, mesh, position, normals);
}

template <class ScalarT>
bool read_stl(std::istream &input, Mesh &mesh, vertex_attribute<std::array<ScalarT, 3>> &position, face_attribute<std::array<ScalarT, 3>> *normals)
{
    return is_ascii_stl(input) ? read_stl_ascii(input, mesh, position, normals) : read_stl_binary(input, mesh, position, normals);
}

template <class ScalarT>
bool read_stl_binary(std::istream &input, Mesh &mesh, vertex_attribute<std::array<ScalarT, 3>> &position, face_attribute<std::array<ScalarT, 3>> *normals)
{
    mesh.clear();

    size_t savp = input.tellg();
    input.seekg(0, std::ios_base::end);
    size_t fs_real = input.tellg();
    input.seekg(savp, std::ios_base::beg);

    char header[80];
    input.read(header, sizeof(header));

    // if (header[0] == 's' && header[1] == 'o' && header[2] == 'l' && header[3] == 'i' && header[4] == 'd')
    // {
    //     std::cerr << "ASCII STL is not supported" << std::endl;
    //     return false;
    // }

    uint32_t n_triangles;
    input.read((char *)&n_triangles, sizeof(n_triangles));

    size_t fs_expect = 80 + sizeof(n_triangles) + n_triangles * (sizeof(std::array<ScalarT, 3>) * 4 + sizeof(uint16_t));
    if (fs_expect != fs_real)
    {
        std::cerr << "Expected file size mismatch: " << fs_expect << " vs " << fs_real << " bytes (file corrupt or wrong format?)" << std::endl;
        return false;
    }

    mesh.faces().reserve(n_triangles);
    mesh.vertices().reserve(n_triangles * 3);
    mesh.halfedges().reserve(n_triangles * 3);

    for (auto i = 0u; i < n_triangles; ++i)
    {
        if (!input.good())
        {
            std::cerr << "Premature end of file";
            return false;
        }

        auto v0 = mesh.vertices().add();
        auto v1 = mesh.vertices().add();
        auto v2 = mesh.vertices().add();
        auto f = mesh.faces().add(v0, v1, v2);

        input.read((char *)&f[normals], sizeof(std::array<ScalarT, 3>));
        input.read((char *)&position[v0], sizeof(std::array<ScalarT, 3>));
        input.read((char *)&position[v1], sizeof(std::array<ScalarT, 3>));
        input.read((char *)&position[v2], sizeof(std::array<ScalarT, 3>));
        uint16_t attr_cnt;
        input.read((char *)&attr_cnt, sizeof(attr_cnt));
    }

    return true;
}

template <class ScalarT>
static float read_real_with_nan(std::istream &input)
{
    std::string s;
    input >> s;
    if (s == "NaN" || s == "nan" || s == "NAN")
        return std::numeric_limits<ScalarT>::quiet_NaN();

    std::istringstream ss(s);
    ScalarT f;
    ss >> f;
    return f;
}

template <class ScalarT>
bool read_stl_ascii(std::istream &input, Mesh &mesh, vertex_attribute<std::array<ScalarT, 3>> &position, face_attribute<std::array<ScalarT, 3>> *normals)
{
    mesh.clear();

    std::string s;
    input >> s;

    if (s != "solid")
    {
        std::cerr << "File does not seem to be ASCII stl" << std::endl;
        return false;
    }

    // name
    do
    {
        input >> s;
    } while (input.good() && s != "endsolid" && s != "facet" && s != "faced");

    while (input.good() && s != "endsolid")
    {
        assert(s == "facet" || s == "faced");

        vertex_handle v[3];
        v[0] = mesh.vertices().add();
        v[1] = mesh.vertices().add();
        v[2] = mesh.vertices().add();
        auto f = mesh.faces().add(v);

        input >> s;
        assert(s == "normal");
        std::array<ScalarT, 3> n;
        n[0] = read_real_with_nan<ScalarT>(input);
        n[1] = read_real_with_nan<ScalarT>(input);
        n[2] = read_real_with_nan<ScalarT>(input);
        f[normals] = n;

        input >> s;
        assert(s == "outer");
        input >> s;
        assert(s == "loop");

        for (auto i = 0; i < 3; ++i)
        {
            input >> s;
            assert(s == "vertex");
            std::array<ScalarT, 3> p;
            p[0] = read_real_with_nan<ScalarT>(input);
            p[1] = read_real_with_nan<ScalarT>(input);
            p[2] = read_real_with_nan<ScalarT>(input);
            position[v[i]] = p;
        }

        input >> s;
        assert(s == "endloop");

        input >> s;
        assert(s == "endfacet");

        input >> s; // for next iteration
    }

    return true;
}

bool is_ascii_stl(std::istream &input)
{
    auto savp = input.tellg();

    char solid[5];
    input.read(solid, sizeof(solid));

    if (solid[0] != 's' || solid[1] != 'o' || solid[2] != 'l' || solid[3] != 'i' || solid[4] != 'd')
    {
        input.seekg(savp, std::ios_base::beg);
        return false;
    }

    std::string s;
    input >> s;

    if (s == "facet" || s == "faced" || s == "endsolid")
    {
        input.seekg(savp, std::ios_base::beg);
        return true;
    }

    for (auto i = 0; i < 20; ++i)
    {
        input >> s;

        if (s == "facet" || s == "faced" || s == "endsolid")
        {
            input.seekg(savp, std::ios_base::beg);
            return true;
        }

        if (!input)
        {
            input.seekg(savp, std::ios_base::beg);
            return false;
        }
    }

    input.seekg(savp, std::ios_base::beg);
    return false;
}

template void write_stl_binary<float>(std::string const &filename,
                                      Mesh const &mesh,
                                      vertex_attribute<std::array<float, 3>> const &position,
                                      face_attribute<std::array<float, 3>> const *normals);
template void write_stl_binary<float>(std::ostream &out,
                                      Mesh const &mesh,
                                      vertex_attribute<std::array<float, 3>> const &position,
                                      face_attribute<std::array<float, 3>> const *normals);
template bool read_stl<float>(std::string const &filename, Mesh &mesh, vertex_attribute<std::array<float, 3>> &position, face_attribute<std::array<float, 3>> *normals);
template bool read_stl<float>(std::istream &input, Mesh &mesh, vertex_attribute<std::array<float, 3>> &position, face_attribute<std::array<float, 3>> *normals);
template bool read_stl_binary<float>(std::istream &input, Mesh &mesh, vertex_attribute<std::array<float, 3>> &position, face_attribute<std::array<float, 3>> *normals);
template bool read_stl_ascii<float>(std::istream &input, Mesh &mesh, vertex_attribute<std::array<float, 3>> &position, face_attribute<std::array<float, 3>> *normals);

template void write_stl_binary<double>(std::string const &filename,
                                       Mesh const &mesh,
                                       vertex_attribute<std::array<double, 3>> const &position,
                                       face_attribute<std::array<double, 3>> const *normals);
template void write_stl_binary<double>(std::ostream &out,
                                       Mesh const &mesh,
                                       vertex_attribute<std::array<double, 3>> const &position,
                                       face_attribute<std::array<double, 3>> const *normals);
template bool read_stl<double>(std::string const &filename, Mesh &mesh, vertex_attribute<std::array<double, 3>> &position, face_attribute<std::array<double, 3>> *normals);
template bool read_stl<double>(std::istream &input, Mesh &mesh, vertex_attribute<std::array<double, 3>> &position, face_attribute<std::array<double, 3>> *normals);
template bool read_stl_binary<double>(std::istream &input, Mesh &mesh, vertex_attribute<std::array<double, 3>> &position, face_attribute<std::array<double, 3>> *normals);
template bool read_stl_ascii<double>(std::istream &input, Mesh &mesh, vertex_attribute<std::array<double, 3>> &position, face_attribute<std::array<double, 3>> *normals);
} // namespace polymesh
