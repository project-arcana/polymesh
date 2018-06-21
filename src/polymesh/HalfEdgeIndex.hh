#pragma once

namespace polymesh
{
struct HalfEdgeIndex
{
    const int value = -1;

    HalfEdgeIndex() = default;
    explicit HalfEdgeIndex(int idx) : value(idx) {}
};

}