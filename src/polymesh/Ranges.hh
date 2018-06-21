#pragma once

#include "Iterators.hh"

namespace polymesh
{
template<typename IteratorT>
struct iterator_range
{
    IteratorT _begin;
    IteratorT _end;

    IteratorT begin() const { return _begin; }
    IteratorT end() const { return _end; }
};

using vertex_range = iterator_range<vertex_iterator>;
using skipping_vertex_range = iterator_range<skipping_vertex_iterator>;
}
