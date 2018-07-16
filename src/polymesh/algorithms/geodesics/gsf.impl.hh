#pragma once

#include "gsf.hh"

namespace polymesh
{
template <class Vec3, class Scalar>
GeodesicNNF<Vec3, Scalar> make_geodesic_nnf(Mesh const& m, vertex_attribute<Vec3> const& position)
{
    return {m, position};
}

template <class Vec3, class Scalar>
GeodesicNNF<Vec3, Scalar>::GeodesicNNF(const Mesh& m, const vertex_attribute<Vec3>& position) : mesh(m), position(position)
{
}
}
