#pragma once

namespace polymesh
{
struct FaceIndex
{
    int value = -1;

    FaceIndex() = default;
    explicit FaceIndex(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    static FaceIndex invalid() { return {}; }

    bool operator==(FaceIndex const& rhs) const { return value == rhs.value; }
    bool operator!=(FaceIndex const& rhs) const { return value != rhs.value; }
};

}
