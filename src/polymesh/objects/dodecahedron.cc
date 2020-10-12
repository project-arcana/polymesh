#include "dodecahedron.hh"

#include <cmath> // std::sqrt

polymesh::unique_array<polymesh::detail::pos3f> polymesh::detail::add_unit_dodecahedron_impl(polymesh::Mesh& m)
{
    polymesh::unique_array<pos3f> position(20);

    auto phi = (1 + std::sqrt(5.0f)) / 2.0f;
    auto phi_inv = 1 / phi;
    auto const sqrt3inv = 1 / std::sqrt(3.0f);
    phi *= sqrt3inv;
    phi_inv *= sqrt3inv;

    pm::vertex_handle vs[20];
    for (size_t i = 0; i < 20; ++i)
        vs[i] = m.vertices().add();

    position[0] = {sqrt3inv, sqrt3inv, sqrt3inv};
    position[1] = {sqrt3inv, sqrt3inv, -sqrt3inv};
    position[2] = {sqrt3inv, -sqrt3inv, sqrt3inv};
    position[3] = {sqrt3inv, -sqrt3inv, -sqrt3inv};
    position[4] = {-sqrt3inv, sqrt3inv, sqrt3inv};
    position[5] = {-sqrt3inv, sqrt3inv, -sqrt3inv};
    position[6] = {-sqrt3inv, -sqrt3inv, sqrt3inv};
    position[7] = {-sqrt3inv, -sqrt3inv, -sqrt3inv};

    position[8] = {0, phi, phi_inv};
    position[9] = {0, phi, -phi_inv};
    position[10] = {0, -phi, phi_inv};
    position[11] = {0, -phi, -phi_inv};

    position[12] = {phi_inv, 0, phi};
    position[13] = {phi_inv, 0, -phi};
    position[14] = {-phi_inv, 0, phi};
    position[15] = {-phi_inv, 0, -phi};

    position[16] = {phi, phi_inv, 0};
    position[17] = {phi, -phi_inv, 0};
    position[18] = {-phi, phi_inv, 0};
    position[19] = {-phi, -phi_inv, 0};

    m.faces().add({vs[8], vs[9], vs[5], vs[18], vs[4]});
    m.faces().add({vs[9], vs[8], vs[0], vs[16], vs[1]});
    m.faces().add({vs[8], vs[4], vs[14], vs[12], vs[0]});
    m.faces().add({vs[9], vs[1], vs[13], vs[15], vs[5]});
    m.faces().add({vs[4], vs[18], vs[19], vs[6], vs[14]});
    m.faces().add({vs[5], vs[15], vs[7], vs[19], vs[18]});
    m.faces().add({vs[13], vs[3], vs[11], vs[7], vs[15]});
    m.faces().add({vs[16], vs[17], vs[3], vs[13], vs[1]});
    m.faces().add({vs[0], vs[12], vs[2], vs[17], vs[16]});
    m.faces().add({vs[14], vs[6], vs[10], vs[2], vs[12]});
    m.faces().add({vs[19], vs[7], vs[11], vs[10], vs[6]});
    m.faces().add({vs[17], vs[2], vs[10], vs[11], vs[3]});

    return position;
}
