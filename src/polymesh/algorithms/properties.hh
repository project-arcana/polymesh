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

/// returns the area of a given triangle
float triangle_area(face_handle f, vertex_attribute<glm::vec3> const& position);

/// returns the center of gravity for a given triangle
glm::vec3 triangle_centroid(face_handle f, vertex_attribute<glm::vec3> const& position);

/// ======== IMPLEMENTATION ========

inline int valence_of(vertex_handle v)
{
    return v.adjacent_vertices().size();
}

inline float triangle_area(face_handle f, vertex_attribute<glm::vec3> const& position)
{
    auto h = f.any_halfedge();
    auto p0 = position[h.vertex_from()];
    auto p1 = position[h.vertex_to()];
    auto p2 = position[h.next().vertex_to()];

    return 0.5f * length(cross(p0 - p1, p0 - p2));
}

inline glm::vec3 triangle_centroid(face_handle f, vertex_attribute<glm::vec3> const& position)
{
    auto h = f.any_halfedge();
    auto p0 = position[h.vertex_from()];
    auto p1 = position[h.vertex_to()];
    auto p2 = position[h.next().vertex_to()];

    return (p0 + p1 + p2) / 3.0f;
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
        p_prev = p_curr;
    } while (h.vertex_to() != v0);

    return length(varea) * 0.5f;
}

inline glm::vec3 face_centroid(face_handle f, vertex_attribute<glm::vec3> const& position)
{
    // TODO: make correct for non-convex polygons!

    float area = 0.0f;
    glm::vec3 centroid;

    auto h = f.any_halfedge();

    auto v0 = h.vertex_from();
    auto p0 = v0[position];

    auto p_prev = h.vertex_to()[position];
    h = h.next();

    do
    {
        auto p_curr = h.vertex_to()[position];

        auto a = length(cross(p_prev - p0, p_curr - p0));
        area += a;
        centroid += (p_prev + p_curr + p0) * a;

        // circulate
        h = h.next();
        p_prev = p_curr;
    } while (h.vertex_to() != v0);

    return centroid / (3.0f * area);
}
}
