#pragma once

namespace polymesh
{
struct HalfedgeIndex
{
    int value = -1;

    HalfedgeIndex() = default;
    explicit HalfedgeIndex(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    static HalfedgeIndex invalid() { return {}; }

    bool operator==(HalfedgeIndex const& rhs) const { return value == rhs.value; }
    bool operator!=(HalfedgeIndex const& rhs) const { return value != rhs.value; }
};

}
