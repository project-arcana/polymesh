#pragma once

#include <iostream>

#include "../Mesh.hh"
#include "../fields.hh"

#include "components.hh"

namespace polymesh
{
/// Prints statistics for the given mesh, including:
/// - number of primitives
/// - components
/// - aabb
template <class Vec3 = void>
void print_stats(std::ostream& out, Mesh const& m, vertex_attribute<Vec3> const* position = nullptr);

/// ======== IMPLEMENTATION ========
template <class Vec3>
void print_stats(std::ostream& out, Mesh const& m, vertex_attribute<Vec3> const* position)
{
    auto ln = "\n";

    out << "[Mesh]:" << ln;

    // # verts, faces, edges, hedges
    out << "  Vertices: " << m.vertices().size();
    if (m.vertices().size() != m.all_vertices().size())
        out << " (" << m.all_vertices().size() - m.vertices().size() << " removed)";
    out << ln;
    out << "  Faces: " << m.faces().size();
    if (m.faces().size() != m.all_faces().size())
        out << " (" << m.all_faces().size() - m.faces().size() << " removed)";
    out << ln;
    out << "  Edges: " << m.edges().size();
    if (m.edges().size() != m.all_edges().size())
        out << " (" << m.all_edges().size() - m.edges().size() << " removed)";
    out << ln;
    out << "  Half-edges: " << m.halfedges().size();
    if (m.halfedges().size() != m.all_halfedges().size())
        out << " (" << m.all_halfedges().size() - m.halfedges().size() << " removed)";
    out << ln;

    if (m.vertices().empty())
        return; // no vertices, no further stats
    out << ln;

    // # isolated, components, boundaries, genus
    int face_comps;
    int vertex_comps;
    vertex_components(m, &vertex_comps);
    face_components(m, &face_comps);
    out << "  Vertex Components: " << vertex_comps << ln;
    out << "  Face Components: " << face_comps << ln;
    // TODO: genus
    // TODO: boundaries
    // TODO: isolated verts, edges

    if (position)
    {
        out << ln;

        auto const& pos = *position;

        auto aabb = m.vertices().aabb(pos);
        auto min = aabb.min;
        auto max = aabb.max;
        out << "  AABB Min:  " << field_3d<Vec3>::to_string(min) << ln;
        out << "  AABB Max:  " << field_3d<Vec3>::to_string(max) << ln;
        out << "  AABB Size: " << field_3d<Vec3>::to_string(max - min) << ln;

        auto avg = m.vertices().avg(pos);
        out << "  Vertex Centroid: " << field_3d<Vec3>::to_string(avg) << ln;
    }
}
}
