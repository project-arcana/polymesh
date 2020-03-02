#pragma once

#include <polymesh/assert.hh>
#include <utility>

namespace polymesh
{
/**
 * A lightweight std::unique_ptr replacement
 *
 * changes to std::unique_ptr<T>:
 * - no custom deleter
 * - no allocators
 * - no operator<
 * - no operator bool
 * - no T[]
 */
template <class T>
struct unique_ptr
{
    unique_ptr() = default;

    unique_ptr(unique_ptr const&) = delete;
    unique_ptr& operator=(unique_ptr const&) = delete;

    unique_ptr(unique_ptr&& rhs) noexcept
    {
        ptr = rhs.ptr;
        rhs.ptr = nullptr;
    }
    unique_ptr& operator=(unique_ptr&& rhs) noexcept
    {
        if (this != &rhs)
        {
            delete ptr;
            ptr = rhs.ptr;
            rhs.ptr = nullptr;
        }
        return *this;
    }

    ~unique_ptr()
    {
        static_assert(sizeof(T) > 0, "cannot delete incomplete type");
        delete ptr;
    }

    void reset(T* p = nullptr)
    {
        POLYMESH_ASSERT(p == nullptr || p != ptr); // no self-reset
        delete ptr;
        ptr = p;
    }

    T* release()
    {
        auto p = ptr;
        ptr = nullptr;
        return p;
    }

    T* get() const { return ptr; }

    T* operator->() const
    {
        POLYMESH_ASSERT(ptr != nullptr);
        return ptr;
    }
    T& operator*() const
    {
        POLYMESH_ASSERT(ptr != nullptr);
        return *ptr;
    }

    bool operator==(unique_ptr const& rhs) const { return ptr == rhs.ptr; }
    bool operator!=(unique_ptr const& rhs) const { return ptr != rhs.ptr; }
    bool operator==(T const* rhs) const { return ptr == rhs; }
    bool operator!=(T const* rhs) const { return ptr != rhs; }

private:
    T* ptr = nullptr;
};

template <class T>
struct unique_ptr<T[]>
{
    static_assert(sizeof(T) >= 0, "unique_ptr does not support arrays, use a vector instead");
};

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args)
{
    unique_ptr<T> p;
    p.reset(new T(std::forward<Args>(args)...));
    return p;
}
}
