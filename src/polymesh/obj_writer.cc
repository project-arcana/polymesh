#include "obj_writer.hh"

#include <fstream>

#include "Mesh.hh"

using namespace polymesh;

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
                *out << base_n + i;
                *out << "/";
            }
        }
        *out << "\n";
    }
}
