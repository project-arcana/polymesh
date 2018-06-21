#pragma once

namespace polymesh
{
struct EdgeIndex
{
    const int value = -1;

    EdgeIndex() = default;
    explicit EdgeIndex(int idx) : value(idx) {}
};

}