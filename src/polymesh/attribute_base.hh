#pragma once

#include <algorithm>
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
    attribute_data(attribute_data<DataT> const& rhs) noexcept // copy
    {
        size = rhs.size;
        data = new DataT[size];

        for (int i = 0; i < size; ++i)
            data[i] = rhs.data[i];
    }
    attribute_data(attribute_data<DataT>&& rhs) noexcept // move
    {
        size = rhs.size;
        data = rhs.data;

        rhs.size = 0;
        rhs.data = nullptr;
    }
    attribute_data<DataT>& operator=(attribute_data<DataT> const& rhs) noexcept // copy
    {
        delete[] data;

        size = rhs.size;
        data = new DataT[size];

        for (int i = 0; i < size; ++i)
            data[i] = rhs.data[i];

        return *this;
    }
    attribute_data<DataT>& operator=(attribute_data<DataT>&& rhs) noexcept // move
    {
        delete[] data;

        size = rhs.size;
        data = rhs.data;

        rhs.size = 0;
        rhs.data = nullptr;

        return *this;
    }
    ~attribute_data() { delete[] data; }

    void clear(DataT const& value)
    {
        for (auto i = 0; i < size; ++i)
            data[i] = value;
    }

    void resize(int new_size, DataT const& default_value)
    {
        auto new_data = new DataT[new_size];

        if (new_size < size)
        {
            for (auto i = 0; i < new_size; ++i)
                new_data[i] = data[i];
        }
        else
        {
            for (auto i = 0; i < size; ++i)
                new_data[i] = data[i];

            for (auto i = size; i < new_size; ++i)
                new_data[i] = default_value;
        }

        delete[] data;
        data = new_data;
        size = new_size;
    }
};

template <class tag>
struct primitive_attribute_base
{
private:
    primitive_attribute_base* mNextAttribute = nullptr;
    primitive_attribute_base* mPrevAttribute = nullptr;

    void resize(int new_size, bool force)
    {
        if (force)
        {
            mDataSize = new_size;
            on_resize(mDataSize);
            return;
        }

        if (mDataSize < new_size)
        {
            mDataSize = std::max(new_size, 1 + mDataSize + (mDataSize >> 1)); // 1 + s * 1.5
            on_resize(mDataSize);
        }
    }

protected:
    int mDataSize = 0;
    Mesh const* mMesh;
    primitive_attribute_base(Mesh const* mesh) : mMesh(mesh) {} // no registration, it's too early!
    virtual void on_resize(int new_size) = 0;
    virtual void apply_remapping(std::vector<int> const& map) = 0;
    virtual void apply_transpositions(std::vector<std::pair<int, int>> const& ts) = 0;
    void register_attr();
    void deregister_attr();
    friend class Mesh;

public:
    virtual ~primitive_attribute_base() { deregister_attr(); }

    Mesh const& mesh() const { return *mMesh; }
};
}
