#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

// Helper for mesh-based attribute bookkeeping

namespace polymesh
{
class Mesh;

template <class tag>
struct primitive_attribute_base
{
private:
    primitive_attribute_base* mNextAttribute = nullptr;
    primitive_attribute_base* mPrevAttribute = nullptr;

    void resizeFrom(int new_size)
    {
        on_resize_from(new_size);
        return;
    }


protected:
    Mesh const* mMesh;
    primitive_attribute_base(Mesh const* mesh) : mMesh(mesh) {} // no registration, it's too early!
    virtual void on_resize_from(int new_size) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    virtual void apply_transpositions(std::vector<std::pair<int, int>> const& ts) = 0;
    void register_attr();
    void deregister_attr();
    friend class Mesh;

public:
    virtual ~primitive_attribute_base() { deregister_attr(); }

    Mesh const& mesh() const { return *mMesh; }
};
} // namespace polymesh
