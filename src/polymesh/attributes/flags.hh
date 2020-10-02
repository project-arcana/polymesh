#pragma once

#include <polymesh/attributes.hh>

namespace polymesh
{
template <class enum_t, class tag>
struct flags;

template <class enum_t = bool, class mesh_ptr, class tag, class iterator>
flags<enum_t, tag> make_flags(smart_collection<mesh_ptr, tag, iterator> const& c, enum_t initial_value = (enum_t)0);

/**
 * Datatype for flags
 *
 * Usage (boolean):
 *      auto p = make_flags(m.vertices());
 *      p[v0].set();
 *      p[v0].unset();
 *      p[v0].is_set();
 *      p.clear();
 *
 * Usage (enum):
 *      enum my_flags {
 *          None = 0x00,
 *          BitA = 0x01,
 *          BitB = 0x02,
 *          ...
 *      };
 *
 *      auto p = make_flags(m.vertices(), None);
 *      p[v0].set(BitA);
 *      p[v0].unset(BitB);
 *      p[v0].is_set(BitB);
 *      p.clear();
 */
template <class enum_t, class tag>
struct flags
{
    using index_t = typename primitive<tag>::index;
    template <class AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;

    enum values
    {
        NONE = (enum_t)0,
        ALL = (enum_t)~0
    };

    // methods
public:
    void clear(enum_t value = (enum_t)0);

    // partition
public:
    struct flag
    {
        // methods
    public:
        bool is_set(enum_t value = ALL) { return (p.entries[i] & value) != 0; }
        void set(enum_t value = ALL) { p.entries[i] |= value; }
        void unset(enum_t value = NONE) { p.entries[i] &= ~value; }

        bool operator==(enum_t const& f) { return f == *this; }
        bool operator!=(enum_t const& f) { return f != *this; }

        /// conversion
        operator enum_t() const { return p.entries[i]; }

    private:
        flags& p;
        index_t i;

    public:
        flag(flags& p, index_t i) : p(p), i(i) {}
    };

    struct const_flag
    {
        // methods
    public:
        bool is_set(enum_t value = ALL) { return (p.entries[i] & value) != 0; }

        bool operator==(enum_t const& f) { return f == *this; }
        bool operator!=(enum_t const& f) { return f != *this; }

        /// conversion
        operator enum_t() const { return p.entries[i]; }

    private:
        flags const& p;
        index_t i;

    public:
        const_flag(flags const& p, index_t i) : p(p), i(i) {}
    };

    flag operator[](index_t i) { return {*this, i}; }
    const_flag operator[](index_t i) const { return {*this, i}; }

    // ctor
public:
    flags(Mesh const& m, enum_t initial_value) : entries(m, initial_value) {}

private:
    attribute<enum_t> entries;
};

// ======== IMPLEMENTATION ========

template <class enum_t, class mesh_ptr, class tag, class iterator>
flags<enum_t, tag> make_flags(smart_collection<mesh_ptr, tag, iterator> const& c, enum_t initial_value)
{
    return {c.mesh(), initial_value};
}

template <class enum_t, class tag>
void flags<enum_t, tag>::clear(enum_t value)
{
    entries.clear(value);
}
} // namespace polymesh
