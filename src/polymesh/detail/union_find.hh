#pragma once

#include <vector>

namespace polymesh
{
namespace detail
{
struct disjoint_set
{
public:
    disjoint_set(int size) : entries(size)
    {
        for (auto i = 0; i < size; ++i)
        {
            auto& e = entries[i];
            e.parent = i;
            e.size = 1;
        }
    }

    int size_of(int idx) { return entries[find(idx)].size; }
    bool is_representative(int idx) { return find(idx) == idx; }

    int find(int idx)
    {
        auto& e = entries[idx];
        if (e.parent != idx)
            e.parent = find(e.parent);
        return e.parent;
    }

    bool do_union(int x, int y)
    {
        // union by size
        auto x_root = find(x);
        auto y_root = find(y);

        if (x_root == y_root)
            return false;

        if (entries[x_root].size < entries[y_root].size)
            std::swap(x_root, y_root);
        // |X| > |Y|

        entries[y_root].parent = x_root;
        entries[x_root].size += entries[y_root].size;

        return true;
    }

private:
    struct entry
    {
        int parent;
        int size;
    };

private:
    std::vector<entry> entries;
};
}
}
