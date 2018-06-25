#pragma once

#include <cstddef>

// Helper for mesh-based property bookkeeping

namespace polymesh
{
class Mesh;

struct vertex_property_base
{
private:
    vertex_property_base* mNextProperty = nullptr;
    vertex_property_base* mPrevProperty = nullptr;
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
    vertex_property_base(Mesh const* mesh);
    virtual ~vertex_property_base();
    virtual void on_resize(size_t newSize) = 0;
    void register_prop();
    friend class Mesh;
};

struct face_property_base
{
private:
    face_property_base* mNextProperty = nullptr;
    face_property_base* mPrevProperty = nullptr;
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
    face_property_base(Mesh const* mesh);
    virtual ~face_property_base();
    virtual void on_resize(size_t newSize) = 0;
    void register_prop();
    friend class Mesh;
};

struct edge_property_base
{
private:
    edge_property_base* mNextProperty = nullptr;
    edge_property_base* mPrevProperty = nullptr;
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
    edge_property_base(Mesh const* mesh);
    virtual ~edge_property_base();
    virtual void on_resize(size_t newSize) = 0;
    void register_prop();
    friend class Mesh;
};

struct halfedge_property_base
{
private:
    halfedge_property_base* mNextProperty = nullptr;
    halfedge_property_base* mPrevProperty = nullptr;
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
    halfedge_property_base(Mesh const* mesh);
    virtual ~halfedge_property_base();
    virtual void on_resize(size_t newSize) = 0;
    void register_prop();
    friend class Mesh;
};
}
