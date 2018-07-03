#pragma once

#include "../Mesh.hh"
#include "../fields.hh"

namespace polymesh
{
/**
 * @brief Implements geodesic distances using the NNF algorithm
 *
 * Usage:
 *
 *   auto nnf = make_geodesic_nnf(mesh, positions);
 */
template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
struct GeodesicNNF
{
public:
    GeodesicNNF(Mesh const& m, vertex_attribute<Vec3> const& position);

private:
    Mesh const& mesh;
    vertex_attribute<Vec3> const& position;
};

template <class Vec3, class Scalar = typename field_3d<Vec3>::Scalar>
GeodesicNNF<Vec3, Scalar> make_geodesic_nnf(Mesh const& m, vertex_attribute<Vec3> const& position);
}

// Implementation
#include "geodesic_nnf.impl.hh"
