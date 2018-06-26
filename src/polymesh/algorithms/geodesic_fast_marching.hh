#pragma once

#include <glm/glm.hpp>

#include <polymesh/Mesh.hh>

namespace polymesh
{
vertex_attribute<float> geodesic_fast_marching_linear(Mesh const& m, vertex_attribute<glm::vec3> const& position)
{
    auto dis = m.vertices().make_attribute<float>(-1);

    // TODO

    return dis;
}
}
