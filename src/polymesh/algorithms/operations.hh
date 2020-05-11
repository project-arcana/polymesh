#pragma once

#include <polymesh/Mesh.hh>


namespace polymesh
{
/// Removes all faces of a given mesh
/// NOTE: does NOT compactify!
void remove_faces(Mesh& m);

/// Removes all edges and faces of a given mesh
/// NOTE: does NOT compactify!
void remove_edges_and_faces(Mesh& m);

// ======== IMPLEMENTATION ========

inline void remove_faces(Mesh& m)
{
    auto ll = low_level_api(m);

    // set faces to removed
    for (auto f : m.faces())
        ll.set_removed(f);

    // remove all faces from half-edges
    for (auto h : m.halfedges())
        ll.face_of(h) = face_index::invalid;
}

inline void remove_edges_and_faces(Mesh& m)
{
    auto ll = low_level_api(m);

    // set faces to removed
    for (auto f : m.faces())
        ll.set_removed(f);

    // set edges to removed
    for (auto e : m.edges())
        ll.set_removed(e);

    // remove all halfedges from vertices
    for (auto v : m.vertices())
        ll.outgoing_halfedge_of(v) = halfedge_index::invalid;
}
}
