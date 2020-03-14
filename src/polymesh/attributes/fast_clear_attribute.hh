#pragma once

#include <polymesh/attributes.hh>
#include <polymesh/primitives.hh>

namespace polymesh
{
template <class T, class tag, class gen_t = int>
struct fast_clear_attribute;

template <class T, class gen_t = int, class mesh_ptr, class tag, class iterator>
fast_clear_attribute<T, tag, gen_t> make_fast_clear_attribute(smart_collection<mesh_ptr, tag, iterator> const& c, T const& clear_value = {});

/**
 * A wrapper around a pm::attribute that provides a O(1) clear operation
 *
 * Internally, each primitive holds a generation counter.
 * When accessing the attribute, the value is cleared in a lazy manner if the generation mismatches
 */
template <class T, class tag, class gen_t>
struct fast_clear_attribute
{
    using index_t = typename primitive<tag>::index;

    fast_clear_attribute(Mesh const& m, T const& clear_value) : _values(m), _clear_value(clear_value) {}

    void clear(T const& new_value = {})
    {
        ++_gen;
        POLYMESH_ASSERT(_gen != 0 && "generation got wrapped around!");
        _clear_value = new_value;
    }

    T& operator[](index_t i)
    {
        auto& e = _values[i];
        if (e.gen != _gen)
        {
            e.value = _clear_value;
            e.gen = _gen;
        }
        return e.value;
    }
    T const& operator[](index_t i) const
    {
        auto const& e = _values[i];
        return e.gen != _gen ? _clear_value : e.value;
    }
    T& operator()(index_t i) { return operator[](i); }
    T const& operator()(index_t i) const { return operator[](i); }

private:
    struct entry
    {
        T value;
        gen_t gen = 0;
    };

    primitive_attribute<tag, entry> _values;
    gen_t _gen = 1;
    T _clear_value;
};

template <class T, class gen_t, class mesh_ptr, class tag, class iterator>
fast_clear_attribute<T, tag, gen_t> make_fast_clear_attribute(smart_collection<mesh_ptr, tag, iterator> const& c, T const& clear_value)
{
    return {c.mesh(), clear_value};
}
}
