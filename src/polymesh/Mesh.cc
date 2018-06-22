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

    // check sizes
    assert(mHalfedges.size() % 2 == 0); ///< even number of halfedges

    // check correct counts

    // check prev-next heh

    // check topology consistencies

    // check iterators
}
