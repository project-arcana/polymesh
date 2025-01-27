#include "obj.hh"

#include <fstream>
#include <iostream>
#include <sstream>

namespace polymesh
{
template <class ScalarT>
void write_obj(cc::string_view filename, vertex_attribute<std::array<ScalarT, 3>> const& position)
{
    obj_writer<ScalarT> obj(filename);
    obj.write_mesh(position);
}

template <class ScalarT>
bool read_obj(cc::string_view filename, Mesh& mesh, vertex_attribute<std::array<ScalarT, 3>>& position)
{
    obj_reader<ScalarT> reader(filename, mesh);
    position = reader.get_positions().map([](std::array<ScalarT, 4> const& p) { return std::array<ScalarT, 3>{{p[0], p[1], p[2]}}; });
    return reader.error_faces() == 0;
}

template <class ScalarT>
obj_writer<ScalarT>::obj_writer(cc::string_view filename)
{
    tmp_out = new std::ofstream(std::string(filename.begin(), filename.end()));
    out = tmp_out;
}

template <class ScalarT>
obj_writer<ScalarT>::obj_writer(std::ostream& out)
{
    this->out = &out;
}

template <class ScalarT>
obj_writer<ScalarT>::~obj_writer()
{
    delete tmp_out;
}

template <class ScalarT>
void obj_writer<ScalarT>::write_object_name(std::string object_name)
{
    *out << "o " << object_name << "\n";
}

template <class ScalarT>
void obj_writer<ScalarT>::write_mesh(vertex_attribute<std::array<ScalarT, 3>> const& position,
                                     vertex_attribute<std::array<ScalarT, 2>> const* tex_coord,
                                     vertex_attribute<std::array<ScalarT, 3>> const* normal)
{
    auto const& mesh = position.mesh();

    auto base_v = vertex_idx;
    auto base_t = texture_idx;
    auto base_n = normal_idx;

    for (auto v : mesh.all_vertices())
    {
        auto pos = v[position];
        *out << "v " << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
        ++vertex_idx;
    }

    if (tex_coord)
        for (auto v : mesh.all_vertices())
        {
            auto t = v[*tex_coord];
            *out << "vt " << t[0] << " " << t[1] << "\n";
            ++texture_idx;
        }

    if (normal)
        for (auto v : mesh.all_vertices())
        {
            auto n = v[*normal];
            *out << "vn " << n[0] << " " << n[1] << " " << n[2] << "\n";
            ++normal_idx;
        }

    for (auto f : mesh.faces())
    {
        *out << "f";
        for (auto v : f.vertices())
        {
            auto i = v.idx.value;
            *out << " ";
            *out << base_v + i;
            if (tex_coord || normal)
                *out << "/";
            if (tex_coord)
                *out << base_t + i;
            if (normal)
            {
                *out << "/";
                *out << base_n + i;
            }
        }
        *out << "\n";
    }
}

template <class ScalarT>
void obj_writer<ScalarT>::write_mesh(vertex_attribute<std::array<ScalarT, 4>> const& position,
                                     halfedge_attribute<std::array<ScalarT, 3>> const* tex_coord,
                                     halfedge_attribute<std::array<ScalarT, 3>> const* normal)
{
    auto const& mesh = position.mesh();

    auto base_v = vertex_idx;
    auto base_t = texture_idx;
    auto base_n = normal_idx;

    for (auto v : mesh.all_vertices())
    {
        auto pos = v[position];
        *out << "v " << pos[0] << " " << pos[1] << " " << pos[2] << " " << pos[3] << "\n";
        ++vertex_idx;
    }

    if (tex_coord)
        for (auto h : mesh.all_halfedges())
        {
            auto t = h[*tex_coord];
            *out << "vt " << t[0] << " " << t[1] << " " << t[2] << "\n";
            ++texture_idx;
        }

    if (normal)
        for (auto h : mesh.all_halfedges())
        {
            auto n = h[*normal];
            *out << "vn " << n[0] << " " << n[1] << " " << n[2] << "\n";
            ++normal_idx;
        }

    for (auto f : mesh.faces())
    {
        *out << "f";
        for (auto h : f.halfedges())
        {
            auto vi = int(h.vertex_to());
            auto hi = int(h);
            *out << " ";
            *out << base_v + vi;
            if (tex_coord || normal)
                *out << "/";
            if (tex_coord)
                *out << base_t + hi;
            if (normal)
            {
                *out << "/";
                *out << base_n + hi;
            }
        }
        *out << "\n";
    }
}

template <class ScalarT>
obj_reader<ScalarT>::obj_reader(cc::string_view filename, Mesh& mesh) : positions(mesh), tex_coords(mesh), normals(mesh)
{
    auto const filename_str = std::string(filename.begin(), filename.end());
    std::ifstream file(filename_str);
    if (!file.good())
        std::cerr << "Cannot read from file `" << filename_str << "'" << std::endl;
    else
        parse(file, mesh);
}

template <class ScalarT>
obj_reader<ScalarT>::obj_reader(std::istream& in, Mesh& mesh) : positions(mesh), tex_coords(mesh), normals(mesh)
{
    parse(in, mesh);
}

template <class ScalarT>
void obj_reader<ScalarT>::parse(std::istream& in, Mesh& mesh)
{
    mesh.clear();

    std::vector<std::array<ScalarT, 3>> raw_tex_coords;
    std::vector<std::array<ScalarT, 3>> raw_normals;

    struct face
    {
        int v = 0;
        int t = 0;
        int n = 0;
        vertex_handle vh;
    };

    std::vector<face> poly;
    std::vector<halfedge_handle> poly_hs;
    std::vector<vertex_index> poly_vs;
    std::string fs;

    std::string line_s;
    auto line_nr = 0;
    while (std::getline(in, line_s))
    {
        ++line_nr;
        while (line_s.size() > 0 && (line_s.back() == '\r' || line_s.back() == ' ' || line_s.back() == '\t'))
            line_s.pop_back();
        std::istringstream line(line_s);
        std::string type;

        line >> type;

        // empty lines
        if (type.empty())
            continue;

        // comments
        else if (type[0] == '#')
            continue;

        // vertices
        else if (type == "v")
        {
            auto v = mesh.vertices().add();

            std::array<ScalarT, 4> p;
            p[3] = 1.0f;

            line >> p[0];
            line >> p[1];
            line >> p[2];
            ScalarT w;
            line >> w;
            if (line.good())
                p[3] = w;

            positions[v] = p;
        }

        // textures
        else if (type == "vt")
        {
            std::array<ScalarT, 3> t;
            t[2] = 1.0f;

            line >> t[0];
            line >> t[1];
            ScalarT z;
            line >> z;
            if (line.good())
                t[2] = z;

            raw_tex_coords.push_back(t);
        }

        // normals
        else if (type == "vn")
        {
            std::array<ScalarT, 3> n;
            line >> n[0];
            line >> n[1];
            line >> n[2];
            raw_normals.push_back(n);
        }

        // faces
        else if (type == "f")
        {
            poly_hs.clear();
            poly_vs.clear();
            poly.clear();

            while (line.good())
            {
                fs.clear();
                line >> fs;
                int sc = 0;
                auto first_s = fs.find_first_of('/');
                auto last_s = fs.find_last_of('/');
                for (auto& c : fs)
                    if (c == '/')
                    {
                        c = ' ';
                        ++sc;
                    }

                std::istringstream ss(fs);
                face f;
                switch (sc)
                {
                case 0:
                    ss >> f.v;
                    break;

                case 1:
                    ss >> f.v;
                    ss >> f.t;
                    break;

                case 2:
                    ss >> f.v;
                    if (first_s + 1 != last_s) // "1//2"
                        ss >> f.t;
                    ss >> f.n;
                    break;
                }

                f.vh = mesh.handle_of(vertex_index(f.v - 1));
                poly.push_back(f);
                poly_vs.push_back(f.vh);
            }

            if (poly.size() < 3)
            {
                std::cerr << "faces with less than 3 vertices are not supported. Use lines instead." << std::endl;
                continue;
            }

            if (!mesh.faces().can_add(poly_vs))
            {
                n_error_faces++;
                continue;
            }

            poly_hs.resize(poly.size());
            for (auto i = 0u; i < poly.size(); ++i)
            {
                auto const& f0 = poly[i];
                auto const& f1 = poly[(i + 1) % poly.size()];
                auto v0 = f0.vh;
                auto v1 = f1.vh;
                auto hh = mesh.halfedges().add_or_get(v0, v1);
                poly_hs[i] = hh;

                if (f1.t > 0)
                    tex_coords[hh] = raw_tex_coords[size_t(f1.t - 1)];
                if (f1.n > 0)
                    normals[hh] = raw_normals[size_t(f1.n - 1)];
            }

            mesh.faces().add(poly_hs);
        }

        // lines
        else if (type == "l")
        {
            int i0, i1;
            line >> i0;
            line >> i1;
            mesh.edges().add_or_get(mesh[vertex_index(i0 - 1)], mesh[vertex_index(i1 - 1)]);

            while (line.good())
            {
                i0 = i1;
                line >> i1;
                mesh.edges().add_or_get(mesh[vertex_index(i0 - 1)], mesh[vertex_index(i1 - 1)]);
            }
        }

        // not implemented
        else if (type == "s")
            continue;
        else if (type == "o")
            continue;
        else if (type == "g")
            continue;
        else if (type == "usemtl")
            continue;
        else if (type == "mtllib")
            continue;

        else
        {
            std::cerr << "Unable to parse line " << line_nr << ": " << line_s << std::endl;
        }
    }

    if (n_error_faces > 0)
    {
        std::cerr << "skipped " << n_error_faces << " face(s) because mesh would become non-manifold" << std::endl;
    }
}

template void write_obj<float>(cc::string_view filename, vertex_attribute<std::array<float, 3>> const& position);
template bool read_obj<float>(cc::string_view filename, Mesh& mesh, vertex_attribute<std::array<float, 3>>& position);
template struct obj_reader<float>;
template struct obj_writer<float>;

template void write_obj<double>(cc::string_view filename, vertex_attribute<std::array<double, 3>> const& position);
template bool read_obj<double>(cc::string_view filename, Mesh& mesh, vertex_attribute<std::array<double, 3>>& position);
template struct obj_reader<double>;
template struct obj_writer<double>;

} // namespace polymesh
