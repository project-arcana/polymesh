#include "obj.hh"

#include <fstream>
#include <sstream>

using namespace polymesh;

void polymesh::write_obj(const std::string &filename,
                         const Mesh &mesh,
                         const vertex_attribute<glm::vec3> &position,
                         const vertex_attribute<glm::vec2> *tex_coord,
                         const vertex_attribute<glm::vec3> *normal)
{
    obj_writer obj(filename);
    obj.write_mesh(mesh, position, tex_coord, normal);
}

obj_writer::obj_writer(const std::string &filename)
{
    tmp_out = new std::ofstream(filename);
    out = tmp_out;
}

obj_writer::obj_writer(std::ostream &out)
{
    this->out = &out;
}

obj_writer::~obj_writer()
{
    delete tmp_out;
}

void obj_writer::write_object_name(std::string object_name)
{
    *out << "o " << object_name << "\n";
}

void obj_writer::write_mesh(const Mesh &mesh,
                            vertex_attribute<glm::vec3> const &position,
                            vertex_attribute<glm::vec2> const *tex_coord,
                            vertex_attribute<glm::vec3> const *normal)
{
    auto base_v = vertex_idx;
    auto base_t = texture_idx;
    auto base_n = normal_idx;

    for (auto v : mesh.vertices())
    {
        auto pos = v[position];
        *out << "v " << pos.x << " " << pos.y << " " << pos.z << "\n";
        ++vertex_idx;
    }

    if (tex_coord)
        for (auto v : mesh.vertices())
        {
            auto t = v[*tex_coord];
            *out << "vt " << t.x << " " << t.y << "\n";
            ++texture_idx;
        }

    if (normal)
        for (auto v : mesh.vertices())
        {
            auto n = v[*normal];
            *out << "vn " << n.x << " " << n.y << " " << n.z << "\n";
            ++normal_idx;
        }

    for (auto f : mesh.valid_faces())
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
                *out << base_n + i;
                *out << "/";
            }
        }
        *out << "\n";
    }
}

obj_reader::obj_reader(const std::string &filename, Mesh &mesh)
  : positions(mesh.vertices().make_attribute<glm::vec4>()),
    tex_coords(mesh.halfedges().make_attribute<glm::vec3>()),
    normals(mesh.halfedges().make_attribute<glm::vec3>())
{
    std::ifstream file(filename);
    if (!file.good())
        std::cerr << "Cannot read from file `" << filename << "'" << std::endl;
    else
        parse(file, mesh);
}

obj_reader::obj_reader(std::istream &in, Mesh &mesh)
  : positions(mesh.vertices().make_attribute<glm::vec4>()),
    tex_coords(mesh.halfedges().make_attribute<glm::vec3>()),
    normals(mesh.halfedges().make_attribute<glm::vec3>())
{
    parse(in, mesh);
}

vertex_attribute<glm::vec4> obj_reader::positions_vec4() const
{
    return positions;
}

vertex_attribute<glm::vec3> obj_reader::positions_vec3() const
{
    return positions.map([](glm::vec4 const &v) { return glm::vec3(v); });
}

halfedge_attribute<glm::vec3> obj_reader::tex_coords_vec3() const
{
    return tex_coords;
}

halfedge_attribute<glm::vec3> obj_reader::normals_vec3() const
{
    return tex_coords;
}

void obj_reader::parse(std::istream &in, Mesh &mesh)
{
    mesh.clear();

    std::vector<glm::vec3> raw_tex_coords;
    std::vector<glm::vec3> raw_normals;

    struct face
    {
        int v = 0;
        int t = 0;
        int n = 0;
        vertex_handle vh;
    };

    std::vector<face> poly;
    std::vector<halfedge_handle> poly_hs;
    std::string fs;

    std::string line_s;
    auto line_nr = 0;
    while (std::getline(in, line_s))
    {
        ++line_nr;
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

            glm::vec4 p;
            p.w = 1.0f;

            line >> p.x;
            line >> p.y;
            line >> p.z;
            float w;
            line >> w;
            if (line.good())
                p.w = w;

            positions[v] = p;
        }

        // textures
        else if (type == "vt")
        {
            glm::vec3 t;
            t.z = 1.0f;

            line >> t.x;
            line >> t.y;
            float z;
            line >> z;
            if (line.good())
                t.z = z;

            raw_tex_coords.push_back(t);
        }

        // normals
        else if (type == "vn")
        {
            glm::vec3 n;
            line >> n.x;
            line >> n.y;
            line >> n.z;
            raw_normals.push_back(n);
        }

        // faces
        else if (type == "f")
        {
            poly_hs.clear();
            poly.clear();

            while (line.good())
            {
                fs.clear();
                line >> fs;
                int sc = 0;
                auto first_s = fs.find_first_of('/');
                auto last_s = fs.find_last_of('/');
                for (auto &c : fs)
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
            }

            if (poly.size() < 3)
            {
                std::cerr << "faces with less than 3 vertices are not supported. Use lines instead." << std::endl;
                continue;
            }

            poly_hs.resize(poly.size());
            for (auto i = 0u; i < poly.size(); ++i)
            {
                auto const &f0 = poly[i];
                auto const &f1 = poly[(i + 1) % poly.size()];
                auto v0 = f0.vh;
                auto v1 = f1.vh;
                auto hh = mesh.halfedges().add_or_get(v0, v1);
                poly_hs[i] = hh;

                if (f1.t > 0)
                    tex_coords[hh] = raw_tex_coords[f1.t - 1];
                if (f1.n > 0)
                    normals[hh] = raw_normals[f1.n - 1];
            }

            mesh.faces().add(poly_hs);
        }

        // lines
        else if (type == "l")
        {
            int i0, i1;
            line >> i0;
            line >> i1;
            while (line.good())
            {
                mesh.edges().add_or_get(mesh[vertex_index(i0 - 1)], mesh[vertex_index(i1 - 1)]);
                i0 = i1;
                line >> i1;
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
}
