#pragma once

namespace polymesh
{
struct VertexIndex
{
    int value = -1;

    VertexIndex() = default;
    explicit VertexIndex(int idx) : value(idx) {}

    bool is_valid() const { return value >= 0; }
    static VertexIndex invalid() { return {}; }

    bool operator==(VertexIndex const& rhs) const { return value == rhs.value; }
    bool operator!=(VertexIndex const& rhs) const { return value != rhs.value; }
};
}
