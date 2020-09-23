#pragma once

#include <sstream>

#include <polymesh/Mesh.hh>
#include <polymesh/fields.hh>

#include <polymesh/properties.hh>
#include "components.hh"

namespace polymesh
{
/// Prints statistics for the given mesh, including:
/// - number of primitives
/// - components
/// - aabb
template <class Vec3 = void, class OutStream>
void print_stats(OutStream& out, Mesh const& m, vertex_attribute<Vec3> const* position = nullptr);

// ======== IMPLEMENTATION ========
template <class Vec3, class OutStream>
void print_stats(OutStream& out, Mesh const& m, vertex_attribute<Vec3> const* position)
{
    auto ln = "\n";

    auto to_string = [&](auto const& v) {
        std::stringstream ss;
        ss << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
        return ss.str();
    };

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
    out << "  Isolated Vertices: " << m.vertices().count(is_vertex_isolated);
    out << "  Isolated Edges: " << m.edges().count(is_edge_isolated);

    if (position)
    {
        out << ln;

        auto const& pos = *position;

        auto aabb = m.vertices().aabb(pos);
        auto min = aabb.min;
        auto max = aabb.max;
        out << "  AABB Min:  " << to_string(min) << ln;
        out << "  AABB Max:  " << to_string(max) << ln;
        out << "  AABB Size: " << to_string(max - min) << ln;

        auto avg = m.vertices().avg(pos);
        out << "  Vertex Centroid: " << to_string(avg) << ln;

        auto el_minmax = m.edges().minmax([&](edge_handle e) { return edge_length(e, pos); });
        auto el_avg = m.edges().avg([&](edge_handle e) { return edge_length(e, pos); });
        out << "  Edge Lengths: " << el_minmax.min << " .. " << el_minmax.max << " (avg " << el_avg << ")" << ln;
    }
}
} // namespace polymesh
