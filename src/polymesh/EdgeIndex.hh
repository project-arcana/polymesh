#pragma once

namespace polymesh
{
struct EdgeIndex
{
    int value = -1;

    EdgeIndex() = default;
    explicit EdgeIndex(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    static EdgeIndex invalid() { return {}; }

    bool operator==(EdgeIndex const& rhs) const { return value == rhs.value; }
    bool operator!=(EdgeIndex const& rhs) const { return value != rhs.value; }
};
}
