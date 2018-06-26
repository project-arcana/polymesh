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

/// see http://geomalgorithms.com/a01-_area.html#3D%20Polygons
/// TODO
/// returns the area of the (flat) polygonal face
float face_area(face_handle f, vertex_attribute<glm::vec3> const& position);
/// returns the signed area of the (flat) polygonal face
float signed_face_area(face_handle f, vertex_attribute<glm::vec3> const& position);

/// returns the center of gravity for a given (flat) polygonal face
glm::vec3 face_centroid(face_handle f, vertex_attribute<glm::vec3> const& position);

/// ======== IMPLEMENTATION ========

inline int valence_of(vertex_handle v) 
{ 
    return v.adjacent_vertices().size(); 
}

inline float face_area(face_handle f, vertex_attribute<glm::vec3> const& position)
{
    return glm::abs(signed_face_area(f, position));
}

inline float signed_face_area(face_handle f, vertex_attribute<glm::vec3> const& position)
{
    auto area = 0.0f;

    for (auto h : f.halfedges())
    {
        auto v0 = h.vertex_from()[position];
        auto v1 = h.vertex_to()[position];

        area += cross(v0, v1);
    }

    return area / 2;
}

inline glm::vec3 face_centroid(face_handle f, vertex_attribute<glm::vec3> const& position)
{
    glm::vec3 centroid;

    auto area = 0.0f;

    for (auto h : f.halfedges())
    {
        auto v0 = h.vertex_from()[position];
        auto v1 = h.vertex_to()[position];

        area += cross(v0, v1);

    }

    return centroid;
}

}