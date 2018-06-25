#pragma once

#include <glm/glm.hpp>

#include <polymesh/Mesh.hh>

namespace polymesh
{
vertex_property<float> geodesic_fast_marching_linear(Mesh const& m, vertex_property<glm::vec3> const& position)
{
    auto dis = m.vertices().make_property<float>(-1);

    // TODO

    return dis;
}
}
