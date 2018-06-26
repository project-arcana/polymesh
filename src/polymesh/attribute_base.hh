#pragma once

#include <vector>
#include <cstddef>

// Helper for mesh-based attribute bookkeeping

namespace polymesh
{
class Mesh;

struct vertex_attribute_base
{
private:
    vertex_attribute_base* mNextAttribute = nullptr;
    vertex_attribute_base* mPrevAttribute = nullptr;
    size_t mDataSize = 0;

    void resize(size_t newSize)
    {
        if (mDataSize < newSize)
        {
            mDataSize = 1 + newSize + (newSize >> 1); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    Mesh const* mMesh;
    vertex_attribute_base(Mesh const* mesh);
    virtual ~vertex_attribute_base();
    virtual void on_resize(size_t newSize) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    void register_attr();
    friend class Mesh;
};

struct face_attribute_base
{
private:
    face_attribute_base* mNextAttribute = nullptr;
    face_attribute_base* mPrevAttribute = nullptr;
    size_t mDataSize = 0;

    void resize(size_t newSize)
    {
        if (mDataSize < newSize)
        {
            mDataSize = 1 + newSize + (newSize >> 1); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    Mesh const* mMesh;
    face_attribute_base(Mesh const* mesh);
    virtual ~face_attribute_base();
    virtual void on_resize(size_t newSize) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    void register_attr();
    friend class Mesh;
};

struct edge_attribute_base
{
private:
    edge_attribute_base* mNextAttribute = nullptr;
    edge_attribute_base* mPrevAttribute = nullptr;
    size_t mDataSize = 0;

    void resize(size_t newSize)
    {
        if (mDataSize < newSize)
        {
            mDataSize = 1 + newSize + (newSize >> 1); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    Mesh const* mMesh;
    edge_attribute_base(Mesh const* mesh);
    virtual ~edge_attribute_base();
    virtual void on_resize(size_t newSize) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    void register_attr();
    friend class Mesh;
};

struct halfedge_attribute_base
{
private:
    halfedge_attribute_base* mNextAttribute = nullptr;
    halfedge_attribute_base* mPrevAttribute = nullptr;
    size_t mDataSize = 0;

    void resize(size_t newSize)
    {
        if (mDataSize < newSize)
        {
            mDataSize = 1 + newSize + (newSize >> 1); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    Mesh const* mMesh;
    halfedge_attribute_base(Mesh const* mesh);
    virtual ~halfedge_attribute_base();
    virtual void on_resize(size_t newSize) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    void register_attr();
    friend class Mesh;
};
}
