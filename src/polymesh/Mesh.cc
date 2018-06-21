#include "Mesh.hh"

#include <cassert>

using namespace polymesh;

void Mesh::compactify()
{
    if (is_compact())
        return;

    /// TODO

    mCompact = true;
}

void Mesh::assert_consistency() const
{
    /// TODO
    /// check compact!
}
