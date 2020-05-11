#pragma once

#include <vector>

namespace polymesh
{
namespace detail
{
/// Applies a permutation that is given by a remapping
/// p[curr_idx] = new_idx
/// Calculates the necessary transpositions and calls s(i, j) for each of it
template <class Swap>
void apply_permutation(std::vector<int> const& p, Swap&& s);

/// Returns true if the parameter is actually a permutation
bool is_valid_permutation(std::vector<int> const& p);

/// Returns a list of transpositions that result in the given remapping
/// p[curr_idx] = new_idx
std::vector<std::pair<int, int>> transpositions_of(std::vector<int> const& p);

// ======== IMPLEMENTATION ========

inline bool is_valid_permutation(std::vector<int> const& p)
{
    std::vector<int> r(p.size(), -1);

    for (auto i = 0u; i < p.size(); ++i)
    {
        auto pi = p[i];
        if (pi < 0 || pi >= (int)p.size())
            return false; // out of bound

        if (r[pi] != -1)
            return false; // not injective

        r[pi] = i;
    }

    return true;
}

template <class Swap>
void apply_permutation(std::vector<int> const& p, Swap&& s)
{
    auto size = p.size();
    std::vector<bool> visited(size, false);
    for (auto pi = 0u; pi < size; ++pi)
    {
        auto i = pi;

        if (visited[i])
            continue;

        visited[i] = true;
        i = p[i];
        while (!visited[i])
        {
            // mark
            visited[i] = true;

            // swap
            s(pi, i);

            // advance
            i = p[i];
        }
    }
}

inline std::vector<std::pair<int, int>> transpositions_of(std::vector<int> const& p)
{
    std::vector<std::pair<int, int>> ts;
    apply_permutation(p, [&](int i, int j) { ts.emplace_back(i, j); });
    return ts;
}
}
}
