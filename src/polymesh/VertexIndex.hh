#pragma once

namespace polymesh
{
struct VertexIndex
{
    const int value = -1;

    VertexIndex() = default;
    explicit VertexIndex(int idx) : value(idx) {}
};

}