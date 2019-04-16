#pragma once

#include <unordered_set>

#include <polymesh/std/hash.hh> // only depends on <utility>

/// Primitive sets are sets optimized for storing primitive indices
/// They support these operations:
///   .insert(T t) -> bool   // true iff actually inserted
///   .contains(T t) -> bool // true iff contained
///   .clear()               // removes all contained elements
///   .size() -> int         // returns number of contained elements
///   .begin() / .end()      // iteration
///
/// Notes:
///   - they should also have value-semantics and proper copy behavior
///   - they are designed to hold int-like objects (must be convertible to int via (int)obj)

namespace polymesh
{
namespace detail
{
template <class T>
struct primitive_set
{
public:
    using iterator = typename std::unordered_set<T>::iterator;
    using const_iterator = typename std::unordered_set<T>::const_iterator;

public:
    bool insert(T t);
    void clear();
    bool contains(T t) const;
    int size() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

private:
    std::unordered_set<T> mElements;
};

template <class T>
bool primitive_set<T>::insert(T t)
{
    return mElements.insert(t).second;
}

template <class T>
void primitive_set<T>::clear()
{
    mElements.clear();
}

template <class T>
bool primitive_set<T>::contains(T t) const
{
    return mElements.count(t);
}

template <class T>
int primitive_set<T>::size() const
{
    return (int)mElements.size();
}

template <class T>
typename primitive_set<T>::iterator primitive_set<T>::begin()
{
    return mElements.begin();
}

template <class T>
typename primitive_set<T>::iterator primitive_set<T>::end()
{
    return mElements.end();
}

template <class T>
typename primitive_set<T>::const_iterator primitive_set<T>::begin() const
{
    return mElements.begin();
}

template <class T>
typename primitive_set<T>::const_iterator primitive_set<T>::end() const
{
    return mElements.end();
}
}
}
