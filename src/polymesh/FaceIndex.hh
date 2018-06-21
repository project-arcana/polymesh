#pragma once

namespace polymesh
{
struct FaceIndex
{
    const int value = -1;

    FaceIndex() = default;
    explicit FaceIndex(int idx) : value(idx) {}
};

}