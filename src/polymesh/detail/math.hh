#pragma once

namespace polymesh::detail
{
template <class ScalarT>
struct pos3
{
    ScalarT x;
    ScalarT y;
    ScalarT z;
};

using pos3f = pos3<float>;
using pos3d = pos3<double>;

}
