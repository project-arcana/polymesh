#pragma once

// TODO: include "algorithm/..."s

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
#include "algorithms/properties.hh"

// Basic mesh operations, including:
// - elementary subdivision
// - intersections
#include "algorithms/remeshing/triangulate.hh"

// Mesh statistics
#include "algorithms/components.hh"
// WIP: #include "algorithms/stats.hh"

// Geodesics
// WIP: #include "algorithms/geodesic_fast_marching.hh"
// WIP: #include "algorithms/geodesic_nnf.hh"

// TODO:
// - decimation
// - subdivision
// - smoothing
// - cutting
// - intersections
// - dualization
// - triangulation
// - geodesics
// - topological information (as free functions)
// - subdivision-to-acute
