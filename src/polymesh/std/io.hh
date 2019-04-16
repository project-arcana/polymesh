#pragma once

#include <iostream>

#include <polymesh/Mesh.hh>

namespace polymesh
{
template <class tag>
std::ostream& operator<<(std::ostream& out, primitive_index<tag> const& v)
{
    out << primitive<tag>::name << " " << v.value;
    if (v.is_invalid())
        out << " (invalid)";
    return out;
}

template <class tag>
std::ostream& operator<<(std::ostream& out, primitive_handle<tag> const& v)
{
    out << v.idx;
    return out;
}
}
