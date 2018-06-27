#pragma once

#include <glm/glm.hpp>

#include "../Mesh.hh"

// Derived mesh properties, including:
// - valences
// - edge angles
// - face angles
// - face centroids
// - face area
// - mesh volume
// - face normal
// - vertex normal
// - curvature
namespace polymesh
{
/// returns the vertex valence (number of adjacent vertices)
int valence_of(vertex_handle v);

/// returns the area of the (flat) polygonal face
float face_area(face_handle f, vertex_attribute<glm::vec3> const& position);

/// returns the center of gravity for a given (flat) polygonal face
glm::vec3 face_centroid(face_handle f, vertex_attribute<glm::vec3> const& position);

/// ======== IMPLEMENTATION ========

inline int valence_of(vertex_handle v)
{
    return v.adjacent_vertices().size();
}

inline float face_area(face_handle f, vertex_attribute<glm::vec3> const& position)
{
    glm::vec3 varea;

    auto h = f.any_halfedge();

    auto v0 = h.vertex_from();
    auto p0 = v0[position];

    auto p_prev = h.vertex_to()[position];
    h = h.next();

    do
    {
        auto p_curr = h.vertex_to()[position];

        varea += cross(p_prev - p0, p_curr - p0);

        // circulate
        h = h.next();
    } while (h.vertex_to() != v0);

    return length(varea) / 2;
}

inline glm::vec3 face_centroid(face_handle f, vertex_attribute<glm::vec3> const& position)
{
    /*
    glm::vec3 centroid;

    auto area = 0.0f;

    for (auto h : f.halfedges())
    {
        auto v0 = h.vertex_from()[position];
        auto v1 = h.vertex_to()[position];

        area += cross(v0, v1);

    }

    return centroid;
    */
    assert(false);
    return {};
}
}
