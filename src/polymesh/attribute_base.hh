#pragma once

#include <cstddef>
#include <vector>

// Helper for mesh-based attribute bookkeeping

namespace polymesh
{
class Mesh;

template <class DataT>
struct attribute_data
{
    int size = 0;
    DataT* data = nullptr;

    DataT& operator[](int i) { return data[i]; }
    DataT const& operator[](int i) const { return data[i]; }

    attribute_data() = default;
    attribute_data(attribute_data<DataT> const& rhs) // copy
    {
        size = rhs.size;
        data = new DataT[size];

        for (int i = 0; i < size; ++i)
            data[i] = rhs.data[i];
    }
    attribute_data(attribute_data<DataT>&& rhs) // move
    {
        size = rhs.size;
        data = rhs.data;

        rhs.size = 0;
        rhs.data = nullptr;
    }
    attribute_data<DataT>& operator=(attribute_data<DataT> const& rhs) // copy
    {
        delete[] data;

        size = rhs.size;
        data = new DataT[size];

        for (int i = 0; i < size; ++i)
            data[i] = rhs.data[i];

        return *this;
    }
    attribute_data<DataT>& operator=(attribute_data<DataT>&& rhs) // move
    {
        delete[] data;

        size = rhs.size;
        data = rhs.data;

        rhs.size = 0;
        rhs.data = nullptr;

        return *this;
    }
    ~attribute_data() { delete[] data; }

    void resize(int new_size, DataT const& default_value)
    {
        auto new_data = new DataT[new_size];

        if (new_size < size)
        {
            for (int i = 0; i < new_size; ++i)
                new_data[i] = data[i];
        }
        else
        {
            for (int i = 0; i < size; ++i)
                new_data[i] = data[i];

            for (int i = size; i < new_size; ++i)
                new_data[i] = default_value;
        }

        delete[] data;
        data = new_data;
        size = new_size;
    }
};

struct vertex_attribute_base
{
private:
    vertex_attribute_base* mNextAttribute = nullptr;
    vertex_attribute_base* mPrevAttribute = nullptr;

    void resize(int newSize, bool force)
    {
        if (force)
        {
            mDataSize = newSize;
            on_resize(mDataSize);
            return;
        }

        if (mDataSize < newSize)
        {
            mDataSize = std::max(newSize, 1 + mDataSize + (mDataSize >> 1)); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    int mDataSize = 0;
    Mesh const* mMesh;
    vertex_attribute_base(Mesh const* mesh);
    virtual ~vertex_attribute_base() { deregister_attr(); }
    virtual void on_resize(int newSize) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    void register_attr();
    void deregister_attr();
    friend class Mesh;
};

struct face_attribute_base
{
private:
    face_attribute_base* mNextAttribute = nullptr;
    face_attribute_base* mPrevAttribute = nullptr;

    void resize(int newSize, bool force)
    {
        if (force)
        {
            mDataSize = newSize;
            on_resize(mDataSize);
            return;
        }

        if (mDataSize < newSize)
        {
            mDataSize = std::max(newSize, 1 + mDataSize + (mDataSize >> 1)); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    int mDataSize = 0;
    Mesh const* mMesh;
    face_attribute_base(Mesh const* mesh);
    virtual ~face_attribute_base() { deregister_attr(); }
    virtual void on_resize(int newSize) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    void register_attr();
    void deregister_attr();
    friend class Mesh;
};

struct edge_attribute_base
{
private:
    edge_attribute_base* mNextAttribute = nullptr;
    edge_attribute_base* mPrevAttribute = nullptr;

    void resize(int newSize, bool force)
    {
        if (force)
        {
            mDataSize = newSize;
            on_resize(mDataSize);
            return;
        }

        if (mDataSize < newSize)
        {
            mDataSize = std::max(newSize, 1 + mDataSize + (mDataSize >> 1)); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    int mDataSize = 0;
    Mesh const* mMesh;
    edge_attribute_base(Mesh const* mesh);
    virtual ~edge_attribute_base() { deregister_attr(); }
    virtual void on_resize(int newSize) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    void register_attr();
    void deregister_attr();
    friend class Mesh;
};

struct halfedge_attribute_base
{
private:
    halfedge_attribute_base* mNextAttribute = nullptr;
    halfedge_attribute_base* mPrevAttribute = nullptr;

    void resize(int newSize, bool force)
    {
        if (force)
        {
            mDataSize = newSize;
            on_resize(mDataSize);
            return;
        }

        if (mDataSize < newSize)
        {
            mDataSize = std::max(newSize, 1 + mDataSize + (mDataSize >> 1)); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    int mDataSize = 0;
    Mesh const* mMesh;
    halfedge_attribute_base(Mesh const* mesh);
    virtual ~halfedge_attribute_base() { deregister_attr(); }
    virtual void on_resize(int newSize) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    void register_attr();
    void deregister_attr();
    friend class Mesh;
};
}
