#pragma once

#include <cstddef>
#include <cstdint>

namespace polymesh
{
namespace detail
{
// from https://en.wikipedia.org/wiki/Xorshift
inline uint64_t xorshift64star(uint64_t& state)
{
    uint64_t x = state; /* The state must be seeded with a nonzero value. */
    x ^= x >> 12;       // a
    x ^= x << 25;       // b
    x ^= x >> 27;       // c
    state = x;
    return x * 0x2545F4914F6CDD1D;
}
}
}
