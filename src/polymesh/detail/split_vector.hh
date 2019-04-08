#pragma once

#include <algorithm>
#include <limits>

#include <polymesh/assert.hh>

#include "unique_array.hh"

namespace polymesh
{
namespace detail
{
inline void reserve(int, int) {}

template <class TFirst, class... TRest>
void reserve(int old_size, int new_capacity, TFirst& ptr, TRest&... rest_ptrs)
{
    POLYMESH_ASSERT(new_capacity >= old_size && "cannot reserve less than the current number of elements");

    using element_t = typename TFirst::element_type;

    auto new_ptr = new element_t[new_capacity];
    std::copy(ptr.get(), ptr.get() + old_size, new_ptr);

    ptr.reset(new_ptr);

    reserve(old_size, new_capacity, rest_ptrs...);
}

template <class... TS>
void shrink_to_fit(int& size, int& capacity, TS&... ptrs)
{
    if (capacity > size)
    {
        capacity = size;
        reserve(size, capacity, ptrs...);
    }
}

template <class... TS>
bool resize(int& size, int& capacity, int new_size, TS&... ptrs)
{
    if (new_size > capacity)
    {
        capacity = std::max(new_size, std::max(2 * capacity, 16));
        reserve(size, capacity, ptrs...);
        size = new_size;
        return true;
    }

    size = new_size;
    return false;
}

/// Like push_back, but doesn't initialize the added element
template <class... TS>
bool alloc_back(int& size, int& capacity, TS&... ptrs)
{
    POLYMESH_ASSERT(size < std::numeric_limits<int>::max() && "polymesh only supports 2^31 primitives");
    size++;

    if (size > capacity)
    {
        capacity = std::max(2 * capacity, 16);
        reserve(size - 1, capacity, ptrs...);
        return true;
    }

    return false;
}

inline void deallocate() {}

template <class TFirst, class... TRest>
void deallocate(TFirst& ptr, TRest&... rest_ptrs)
{
    ptr.reset();
    deallocate(rest_ptrs...);
}

/// Unlike std::vector::clear, this also deallocates the storage
template <class... TS>
void clear(int& size, int& capacity, TS&... ptrs)
{
    size = 0;
    capacity = 0;
    deallocate(ptrs...);
}

template <class T>
struct split_vector_range
{
    T* mBegin;
    T* mEnd;
    T* begin() { return mBegin; }
    T* end() { return mEnd; }
};

template <class T>
split_vector_range<T> range(int size, unique_array<T>& ptr)
{
    return {ptr.get(), ptr.get() + size};
}
} // namespace detail
} // namespace polymesh
