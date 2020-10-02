#pragma once

#include <polymesh/assert.hh>

namespace polymesh
{
/// lightweight replacement for unique_ptr<T[]>
template <class T>
struct unique_array
{
    using element_type = T;

    unique_array() = default;
    explicit unique_array(int size) { ptr = new T[size](); }
    ~unique_array()
    {
        delete[] ptr;
        ptr = nullptr; // for safety
    }

    // proper move
    unique_array(unique_array&& rhs) noexcept
    {
        ptr = rhs.ptr;
        rhs.ptr = nullptr;
    }
    unique_array& operator=(unique_array&& rhs) noexcept
    {
        // self-move results in moved-from state
        delete[] ptr;
        ptr = rhs.ptr;
        rhs.ptr = nullptr;

        return *this;
    }

    // no copy
    unique_array(unique_array const&) = delete;
    unique_array& operator=(unique_array const&) = delete;

    T* get() noexcept { return ptr; }
    T const* get() const noexcept { return ptr; }

    T& operator[](int i) noexcept
    {
        POLYMESH_ASSERT(ptr);
        return ptr[i];
    }
    T const& operator[](int i) const noexcept
    {
        POLYMESH_ASSERT(ptr);
        return ptr[i];
    }

    void reset(T* new_ptr = nullptr)
    {
        delete[] ptr;
        ptr = new_ptr;
    }

private:
    T* ptr = nullptr;
};
}
