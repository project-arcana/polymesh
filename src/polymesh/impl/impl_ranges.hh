#pragma once

#include "../Mesh.hh"

namespace polymesh
{
namespace detail
{
template <class T>
T helper_min(T const &a, T const &b)
{
    using namespace std;
    return min(a, b);
}
template <class T>
T helper_max(T const &a, T const &b)
{
    using namespace std;
    return max(a, b);
}
}

template <class this_t, class ElementT>
ElementT smart_range<this_t, ElementT>::first() const
{
    for (auto h : *static_cast<this_t const *>(this))
        return h;
    return {};
}
template <class this_t, class ElementT>
ElementT smart_range<this_t, ElementT>::last() const
{
    ElementT result;
    for (auto h : *static_cast<this_t const *>(this))
        result = h;
    return result;
}

template <class this_t, class ElementT>
bool smart_range<this_t, ElementT>::empty() const
{
    return static_cast<this_t const *>(this)->begin() == static_cast<this_t const *>(this)->end();
}

template <class this_t, class ElementT>
template <class PredT>
bool smart_range<this_t, ElementT>::any(PredT &&p) const
{
    for (auto h : *static_cast<this_t const *>(this))
        if (p(h))
            return true;
    return false;
}

template <class this_t, class ElementT>
template <class PredT>
bool smart_range<this_t, ElementT>::all(PredT &&p) const
{
    for (auto h : *static_cast<this_t const *>(this))
        if (!p(h))
            return false;
    return true;
}

template <class this_t, class ElementT>
template <class PredT>
int smart_range<this_t, ElementT>::count(PredT &&p) const
{
    auto cnt = 0;
    for (auto h : *static_cast<this_t const *>(this))
        if (p(h))
            ++cnt;
    return cnt;
}

template <class this_t, class ElementT>
int smart_range<this_t, ElementT>::count() const
{
    auto cnt = 0;
    for (auto h : *static_cast<this_t const *>(this))
    {
        (void)h; // unused
        ++cnt;
    }
    return cnt;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::min(FuncT &&f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto v = f(*it_begin);
    ++it_begin;
    while (it_begin != it_end)
    {
        v = detail::helper_min(v, f(*it_begin));
        ++it_begin;
    }
    return v;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::max(FuncT &&f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto v = f(*it_begin);
    ++it_begin;
    while (it_begin != it_end)
    {
        v = detail::helper_max(v, f(*it_begin));
        ++it_begin;
    }
    return v;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::sum(FuncT &&f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto s = f(*it_begin);
    ++it_begin;
    while (it_begin != it_end)
    {
        s = s + f(*it_begin);
        ++it_begin;
    }
    return s;
}

template <class this_t, class ElementT>
template <class FuncT>
ElementT smart_range<this_t, ElementT>::min_by(FuncT &&f) const
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto e_min = *it_begin;
    auto v_min = f(e_min);
    ++it_begin;
    while (it_begin != it_end)
    {
        auto e = *it_begin;
        auto v = f(*it_begin);
        if (v < v_min)
        {
            v_min = v;
            e_min = e;
        }
        ++it_begin;
    }
    return e_min;
}

template <class this_t, class ElementT>
template <class FuncT>
ElementT smart_range<this_t, ElementT>::max_by(FuncT &&f) const
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto e_max = *it_begin;
    auto v_max = f(e_max);
    ++it_begin;
    while (it_begin != it_end)
    {
        auto e = *it_begin;
        auto v = f(*it_begin);
        if (v > v_max)
        {
            v_max = v;
            e_max = e;
        }
        ++it_begin;
    }
    return e_max;
}

template <class this_t, class ElementT>
template <class FuncT>
polymesh::aabb<ElementT> smart_range<this_t, ElementT>::minmax_by(FuncT &&f) const
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto e_min = *it_begin;
    auto e_max = e_min;
    auto v_min = f(e_min);
    auto v_max = v_min;
    ++it_begin;
    while (it_begin != it_end)
    {
        auto e = *it_begin;
        auto v = f(*it_begin);
        if (v < v_min)
        {
            v_min = v;
            e_min = e;
        }
        if (v > v_max)
        {
            v_max = v;
            e_max = e;
        }
        ++it_begin;
    }
    return {e_min, e_max};
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::avg(FuncT &&f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto s = f(*it_begin);
    auto cnt = 1;
    static_assert(tmp::can_divide_by<decltype(s), decltype(cnt)>::value,
                  "Cannot divide sum by an integer. (if glm is used, including <glm/ext.hpp> might help)");
    ++it_begin;
    while (it_begin != it_end)
    {
        s = s + f(*it_begin);
        ++cnt;
        ++it_begin;
    }
    return s / cnt;
}

template <class this_t, class ElementT>
template <class FuncT, class WeightT>
auto smart_range<this_t, ElementT>::weighted_avg(FuncT &&f, WeightT &&w) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto e = *it_begin;
    auto s = f(e);
    auto ws = w(e);
    static_assert(tmp::can_divide_by<decltype(s), decltype(ws)>::value,
                  "Cannot divide sum by weight. (if glm is used, including <glm/ext.hpp> might help)");
    ++it_begin;
    while (it_begin != it_end)
    {
        auto ee = *it_begin;
        s = s + f(ee);
        ws = ws + w(ee);
        ++it_begin;
    }
    return s / ws;
}

template <class this_t, class ElementT>
template <class FuncT, class FuncInvT>
auto smart_range<this_t, ElementT>::f_mean(FuncT &&f, FuncInvT &&f_inv) const
    -> tmp::decayed_result_type_of<FuncInvT, tmp::decayed_result_type_of<FuncT, ElementT>>
{
    return f_inv(this->avg(f));
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::arithmetic_mean(FuncT &&f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    return this->avg(f);
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::geometric_mean(FuncT &&f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    using std::exp;
    using std::log;
    auto ff = [&f](ElementT const &e) { return log(f(e)); };
    auto ff_inv = [](decltype(ff(std::declval<ElementT>())) const &d) { return exp(d); };
    return this->f_mean(ff, ff_inv);
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::aabb(FuncT &&f) const -> polymesh::aabb<tmp::decayed_result_type_of<FuncT, ElementT>>
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    assert(it_begin != it_end && "requires non-empty range");
    auto v = f(*it_begin);
    polymesh::aabb<tmp::decayed_result_type_of<FuncT, ElementT>> r = {v, v};
    ++it_begin;
    while (it_begin != it_end)
    {
        auto vv = f(*it_begin);
        r.min = detail::helper_min(r.min, vv);
        r.max = detail::helper_max(r.max, vv);
        ++it_begin;
    }
    return r;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::minmax(FuncT &&f) const -> polymesh::aabb<tmp::decayed_result_type_of<FuncT, ElementT>>
{
    return aabb(f);
}

template <class this_t, class ElementT>
std::vector<ElementT> smart_range<this_t, ElementT>::to_vector() const
{
    std::vector<ElementT> v;
    for (auto h : *static_cast<this_t const *>(this))
        v.push_back(h);
    return v;
}

template <class this_t, class ElementT>
std::set<ElementT> smart_range<this_t, ElementT>::to_set() const
{
    std::set<ElementT> s;
    for (auto h : *static_cast<this_t const *>(this))
        s.insert(h);
    return s;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::to_vector(FuncT &&f) const -> std::vector<tmp::decayed_result_type_of<FuncT, ElementT>>
{
    std::vector<tmp::decayed_result_type_of<FuncT, ElementT>> v;
    for (auto h : *static_cast<this_t const *>(this))
        v.push_back(f(h));
    return v;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::to_set(FuncT &&f) const -> std::set<tmp::decayed_result_type_of<FuncT, ElementT>>
{
    std::set<tmp::decayed_result_type_of<FuncT, ElementT>> s;
    for (auto h : *static_cast<this_t const *>(this))
        s.insert(f(h));
    return s;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::to_map(FuncT &&f) const -> std::map<ElementT, tmp::decayed_result_type_of<FuncT, ElementT>>
{
    std::map<ElementT, tmp::decayed_result_type_of<FuncT, ElementT>> m;
    for (auto h : *static_cast<this_t const *>(this))
        m[h] = f(h);
    return m;
}


template <class this_t, class ElementT>
template <class PredT>
auto smart_range<this_t, ElementT>::where(PredT &&p) const -> filtered_range<ElementT, this_t const &, PredT>
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    return {it_begin, it_end, p};
}

template <class this_t, class ElementT>
template <class PredT>
auto smart_range<this_t, ElementT>::where(PredT &&p) -> filtered_range<ElementT, this_t &, PredT>
{
    auto it_begin = static_cast<this_t *>(this)->begin();
    auto it_end = static_cast<this_t *>(this)->end();
    return {it_begin, it_end, p};
}

template <class this_t, class ElementT>
template <class PredT>
auto smart_range<this_t, ElementT>::filter(PredT &&p) const -> filtered_range<ElementT, this_t const &, PredT>
{
    auto it_begin = static_cast<this_t const *>(this)->begin();
    auto it_end = static_cast<this_t const *>(this)->end();
    static_assert(std::is_same<decltype(it_begin), typename filtered_range<ElementT, this_t const &, PredT>::IteratorT>::value, "");
    return filtered_range<ElementT, this_t const &, PredT>(it_begin, it_end, p);
}

template <class this_t, class ElementT>
template <class PredT>
auto smart_range<this_t, ElementT>::filter(PredT &&p) -> filtered_range<ElementT, this_t &, PredT>
{
    auto it_begin = static_cast<this_t *>(this)->begin();
    auto it_end = static_cast<this_t *>(this)->end();
    return {it_begin, it_end, p};
}

template <class this_t, class tag>
int primitive_ring<this_t, tag>::size() const
{
    auto cnt = 0;
    for (auto v : *static_cast<this_t const *>(this))
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

template <class this_t, class tag>
bool primitive_ring<this_t, tag>::contains(handle v) const
{
    for (auto v2 : *static_cast<this_t const *>(this))
        if (v == v2)
            return true;
    return false;
}

template <class mesh_ptr, class tag, class iterator>
int smart_collection<mesh_ptr, tag, iterator>::size() const
{
    return iterator::primitive_size(*mesh);
}

template <class mesh_ptr, class tag, class iterator>
void smart_collection<mesh_ptr, tag, iterator>::reserve(int capacity) const
{
    return primitive<tag>::reserve(*mesh, capacity);
}

template <class mesh_ptr, class tag, class iterator>
template <class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute() const
{
    return typename primitive<tag>::template attribute<AttrT>(mesh, AttrT());
}
template <class mesh_ptr, class tag, class iterator>
template <class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute_with_default(AttrT const &def_value) const
{
    return typename primitive<tag>::template attribute<AttrT>(mesh, def_value);
}

template <class mesh_ptr, class tag, class iterator>
template <class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute_from_data(std::vector<AttrT> const &data) const
{
    auto attr = make_attribute<AttrT>();
    attr.copy_from(data);
    return attr; // copy elison
}

template <class mesh_ptr, class tag, class iterator>
template <class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute_from_data(AttrT const *data, int cnt) const
{
    auto attr = make_attribute<AttrT>();
    attr.copy_from(data, cnt);
    return attr; // copy elison
}

template <class mesh_ptr, class tag, class iterator>
template <class FuncT, class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute(FuncT &&f, AttrT const &def_value) const
{
    auto attr = make_attribute_with_default<AttrT>(def_value);
    for (auto h : *this)
        attr[h] = f(h);
    return attr; // copy elison
}

template <class mesh_ptr, class tag, class iterator>
template <class FuncT, class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::map(FuncT &&f, AttrT const &def_value) const
{
    return this->make_attribute(f, def_value);
}

template <class mesh_ptr, class tag, class iterator>
iterator smart_collection<mesh_ptr, tag, iterator>::begin() const
{
    return {{this->mesh, typename primitive<tag>::index(0)}};
}

template <class mesh_ptr, class tag, class iterator>
iterator smart_collection<mesh_ptr, tag, iterator>::end() const
{
    return {{this->mesh, typename primitive<tag>::index(primitive<tag>::all_size(*this->mesh))}};
}

template <class mesh_ptr, class tag, class iterator>
template <class Generator>
typename primitive<tag>::handle smart_collection<mesh_ptr, tag, iterator>::random(Generator &g) const
{
    auto s = primitive<tag>::all_size(*this->mesh);
    assert(s > 0 && "Cannot chose from empty mesh");
    std::uniform_int_distribution<> uniform(0, s - 1);

    typename primitive<tag>::handle h = {this->mesh, typename primitive<tag>::index(uniform(g))};

    if (iterator::is_valid_iterator)
        while (h.is_removed())
            h = {this->mesh, typename primitive<tag>::index(uniform(g))};

    return h;
}

template <class iterator>
void vertex_collection<iterator>::remove(vertex_handle v) const
{
    low_level_api(this->mesh).remove_vertex(v.idx);
}

template <class iterator>
void face_collection<iterator>::remove(face_handle f) const
{
    low_level_api(this->mesh).remove_face(f.idx);
}

template <class iterator>
void edge_collection<iterator>::remove(edge_handle e) const
{
    low_level_api(this->mesh).remove_edge(e.idx);
}

template <class iterator>
void halfedge_collection<iterator>::remove_edge(halfedge_handle h) const
{
    low_level_api(this->mesh).remove_edge(low_level_api(this->mesh).edge_of(h.idx));
}

template <class iterator>
void face_collection<iterator>::permute(std::vector<int> const &p) const
{
    low_level_api(this->mesh).permute_faces(p);
}

template <class iterator>
void edge_collection<iterator>::permute(std::vector<int> const &p) const
{
    low_level_api(this->mesh).permute_edges(p);
}

template <class iterator>
void vertex_collection<iterator>::permute(std::vector<int> const &p) const
{
    low_level_api(this->mesh).permute_vertices(p);
}


inline valid_vertex_collection Mesh::vertices()
{
    valid_vertex_collection c;
    c.mesh = this;
    return c;
}
inline valid_vertex_const_collection Mesh::vertices() const
{
    valid_vertex_const_collection c;
    c.mesh = this;
    return c;
}
inline all_vertex_collection Mesh::all_vertices()
{
    all_vertex_collection c;
    c.mesh = this;
    return c;
}
inline all_vertex_const_collection Mesh::all_vertices() const
{
    all_vertex_const_collection c;
    c.mesh = this;
    return c;
}
inline valid_face_collection Mesh::faces()
{
    valid_face_collection c;
    c.mesh = this;
    return c;
}
inline valid_face_const_collection Mesh::faces() const
{
    valid_face_const_collection c;
    c.mesh = this;
    return c;
}
inline all_face_collection Mesh::all_faces()
{
    all_face_collection c;
    c.mesh = this;
    return c;
}
inline all_face_const_collection Mesh::all_faces() const
{
    all_face_const_collection c;
    c.mesh = this;
    return c;
}
inline valid_edge_collection Mesh::edges()
{
    valid_edge_collection c;
    c.mesh = this;
    return c;
}
inline valid_edge_const_collection Mesh::edges() const
{
    valid_edge_const_collection c;
    c.mesh = this;
    return c;
}
inline all_edge_collection Mesh::all_edges()
{
    all_edge_collection c;
    c.mesh = this;
    return c;
}
inline all_edge_const_collection Mesh::all_edges() const
{
    all_edge_const_collection c;
    c.mesh = this;
    return c;
}
inline valid_halfedge_collection Mesh::halfedges()
{
    valid_halfedge_collection c;
    c.mesh = this;
    return c;
}
inline valid_halfedge_const_collection Mesh::halfedges() const
{
    valid_halfedge_const_collection c;
    c.mesh = this;
    return c;
}
inline all_halfedge_collection Mesh::all_halfedges()
{
    all_halfedge_collection c;
    c.mesh = this;
    return c;
}
inline all_halfedge_const_collection Mesh::all_halfedges() const
{
    all_halfedge_const_collection c;
    c.mesh = this;
    return c;
}

template <class iterator>
vertex_handle vertex_collection<iterator>::add() const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_vertex());
}

template <class iterator>
face_handle face_collection<iterator>::add(const vertex_handle *v_handles, int vcnt) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(v_handles, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(const vertex_index *v_indices, int vcnt) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(v_indices, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(const halfedge_handle *half_loop, int vcnt) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(half_loop, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(const halfedge_index *half_loop, int vcnt) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(half_loop, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<vertex_handle> const &v_handles) const
{
    return add(v_handles.data(), v_handles.size());
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<vertex_index> const &v_indices) const
{
    return add(v_indices.data(), v_indices.size());
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<halfedge_handle> const &half_loop) const
{
    return add(half_loop.data(), (int)half_loop.size());
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<halfedge_index> const &half_loop) const
{
    return add(half_loop.data(), (int)half_loop.size());
}

template <class iterator>
face_handle face_collection<iterator>::add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    halfedge_index hs[3] = {
        low_level_api(this->mesh).add_or_get_halfedge(v0.idx, v1.idx), //
        low_level_api(this->mesh).add_or_get_halfedge(v1.idx, v2.idx), //
        low_level_api(this->mesh).add_or_get_halfedge(v2.idx, v0.idx), //
    };
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(hs, 3));
}

template <class iterator>
face_handle face_collection<iterator>::add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    halfedge_index hs[4] = {
        low_level_api(this->mesh).add_or_get_halfedge(v0.idx, v1.idx), //
        low_level_api(this->mesh).add_or_get_halfedge(v1.idx, v2.idx), //
        low_level_api(this->mesh).add_or_get_halfedge(v2.idx, v3.idx), //
        low_level_api(this->mesh).add_or_get_halfedge(v3.idx, v0.idx), //
    };
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(hs, 4));
}

template <class iterator>
face_handle face_collection<iterator>::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const
{
    halfedge_index hs[3] = {h0.idx, h1.idx, h2.idx};
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(hs, 3));
}

template <class iterator>
face_handle face_collection<iterator>::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const
{
    halfedge_index hs[4] = {h0.idx, h1.idx, h2.idx, h3.idx};
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(hs, 4));
}

template <class iterator>
template <size_t N>
face_handle face_collection<iterator>::add(const vertex_handle (&v_handles)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0; i < N; ++i)
        hs[i] = low_level_api(this->mesh).find_halfedge(v_handles[i].idx, v_handles[(i + 1) % N].idx);
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(hs, N));
}

template <class iterator>
template <size_t N>
face_handle face_collection<iterator>::add(const halfedge_handle (&half_loop)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0; i < N; ++i)
        hs[i] = half_loop[i].idx;
    return this->mesh->handle_of(low_level_api(this->mesh).add_face(hs, N));
}

template <class iterator>
bool face_collection<iterator>::can_add(const vertex_handle *v_handles, int vcnt) const
{
    return low_level_api(this->mesh).can_add_face(v_handles, vcnt);
}

template <class iterator>
bool face_collection<iterator>::can_add(const vertex_index *v_indices, int vcnt) const
{
    return low_level_api(this->mesh).can_add_face(v_indices, vcnt);
}

template <class iterator>
bool face_collection<iterator>::can_add(const halfedge_handle *half_loop, int vcnt) const
{
    return low_level_api(this->mesh).can_add_face(half_loop, vcnt);
}

template <class iterator>
bool face_collection<iterator>::can_add(const halfedge_index *half_loop, int vcnt) const
{
    return low_level_api(this->mesh).can_add_face(half_loop, vcnt);
}

template <class iterator>
bool face_collection<iterator>::can_add(std::vector<vertex_handle> const &v_handles) const
{
    return can_add(v_handles.data(), v_handles.size());
}

template <class iterator>
bool face_collection<iterator>::can_add(std::vector<vertex_index> const &v_indices) const
{
    return can_add(v_indices.data(), v_indices.size());
}

template <class iterator>
bool face_collection<iterator>::can_add(std::vector<halfedge_handle> const &half_loop) const
{
    return can_add(half_loop.data(), (int)half_loop.size());
}

template <class iterator>
bool face_collection<iterator>::can_add(std::vector<halfedge_index> const &half_loop) const
{
    return can_add(half_loop.data(), (int)half_loop.size());
}

template <class iterator>
bool face_collection<iterator>::can_add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    halfedge_index hs[3] = {
        low_level_api(this->mesh).can_add_or_get_halfedge(v0.idx, v1.idx), //
        low_level_api(this->mesh).can_add_or_get_halfedge(v1.idx, v2.idx), //
        low_level_api(this->mesh).can_add_or_get_halfedge(v2.idx, v0.idx), //
    };
    return low_level_api(this->mesh).can_add_face(hs, 3);
}

template <class iterator>
bool face_collection<iterator>::can_add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    halfedge_index hs[4] = {
        low_level_api(this->mesh).can_add_or_get_halfedge(v0.idx, v1.idx), //
        low_level_api(this->mesh).can_add_or_get_halfedge(v1.idx, v2.idx), //
        low_level_api(this->mesh).can_add_or_get_halfedge(v2.idx, v3.idx), //
        low_level_api(this->mesh).can_add_or_get_halfedge(v3.idx, v0.idx), //
    };
    return low_level_api(this->mesh).can_add_face(hs, 4);
}

template <class iterator>
bool face_collection<iterator>::can_add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const
{
    halfedge_index hs[3] = {h0.idx, h1.idx, h2.idx};
    return low_level_api(this->mesh).can_add_face(hs, 3);
}

template <class iterator>
bool face_collection<iterator>::can_add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const
{
    halfedge_index hs[4] = {h0.idx, h1.idx, h2.idx, h3.idx};
    return low_level_api(this->mesh).can_add_face(hs, 4);
}

template <class iterator>
template <size_t N>
bool face_collection<iterator>::can_add(const vertex_handle (&v_handles)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0; i < N; ++i)
        hs[i] = low_level_api(this->mesh).find_halfedge(v_handles[i].idx, v_handles[(i + 1) % N].idx);
    return low_level_api(this->mesh).can_add_face(hs, N);
}

template <class iterator>
template <size_t N>
bool face_collection<iterator>::can_add(const halfedge_handle (&half_loop)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0; i < N; ++i)
        hs[i] = half_loop[i].idx;
    return low_level_api(this->mesh).can_add_face(hs, N);
}

template <class iterator>
edge_handle edge_collection<iterator>::add_or_get(vertex_handle v_from, vertex_handle v_to) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_or_get_edge(v_from.idx, v_to.idx));
}

template <class iterator>
halfedge_handle halfedge_collection<iterator>::add_or_get(vertex_handle v_from, vertex_handle v_to) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_or_get_halfedge(v_from.idx, v_to.idx));
}

template <class iterator>
edge_handle edge_collection<iterator>::add_or_get(halfedge_handle h_from, halfedge_handle h_to) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_or_get_edge(h_from.idx, h_to.idx));
}

template <class iterator>
halfedge_handle halfedge_collection<iterator>::add_or_get(halfedge_handle h_from, halfedge_handle h_to) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).add_or_get_halfedge(h_from.idx, h_to.idx));
}

template <class iterator>
edge_handle edge_collection<iterator>::find(vertex_handle v_from, vertex_handle v_to) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).edge_of(low_level_api(this->mesh).find_halfedge(v_from.idx, v_to.idx)));
}

template <class iterator>
halfedge_handle halfedge_collection<iterator>::find(vertex_handle v_from, vertex_handle v_to) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).find_halfedge(v_from.idx, v_to.idx));
}

template <class iterator>
vertex_handle face_collection<iterator>::split(face_handle f) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).face_split(f.idx));
}

template <class iterator>
void face_collection<iterator>::split(face_handle f, vertex_handle v) const
{
    low_level_api(this->mesh).face_split(f.idx, v.idx);
}

template <class iterator>
face_handle face_collection<iterator>::fill(halfedge_handle h) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).face_fill(h.idx));
}

template <class iterator>
vertex_handle edge_collection<iterator>::split(edge_handle e) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).edge_split(e.idx));
}

template <class iterator>
void edge_collection<iterator>::split(edge_handle e, vertex_handle v) const
{
    low_level_api(this->mesh).edge_split(e.idx, v.idx);
}

template <class iterator>
void vertex_collection<iterator>::collapse(vertex_handle v) const
{
    low_level_api(this->mesh).vertex_collapse(v.idx);
}

template <class iterator>
void halfedge_collection<iterator>::collapse(halfedge_handle h) const
{
    low_level_api(this->mesh).halfedge_collapse(h.idx);
}

template <class iterator>
void edge_collection<iterator>::rotate_next(edge_handle e) const
{
    low_level_api(this->mesh).edge_rotate_next(e.idx);
}

template <class iterator>
void edge_collection<iterator>::rotate_prev(edge_handle e) const
{
    low_level_api(this->mesh).edge_rotate_prev(e.idx);
}

template <class iterator>
vertex_handle halfedge_collection<iterator>::split(halfedge_handle h) const
{
    return this->mesh->handle_of(low_level_api(this->mesh).halfedge_split(h.idx));
}

template <class iterator>
void halfedge_collection<iterator>::split(halfedge_handle h, vertex_handle v) const
{
    low_level_api(this->mesh).halfedge_split(h.idx, v.idx);
}

template <class iterator>
void halfedge_collection<iterator>::attach(halfedge_handle h, vertex_handle v) const
{
    low_level_api(this->mesh).halfedge_attach(h.idx, v.idx);
}

template <class iterator>
void halfedge_collection<iterator>::merge(halfedge_handle h) const
{
    low_level_api(this->mesh).halfedge_merge(h.idx);
}

template <class iterator>
void halfedge_collection<iterator>::rotate_next(halfedge_handle h) const
{
    low_level_api(this->mesh).halfedge_rotate_next(h.idx);
}

template <class iterator>
void halfedge_collection<iterator>::rotate_prev(halfedge_handle h) const
{
    low_level_api(this->mesh).halfedge_rotate_prev(h.idx);
}

template <class ElementT, class RangeT, class PredT>
filtered_range<ElementT, RangeT, PredT>::filtered_range(filtered_range::IteratorT begin, filtered_range::IteratorT end, PredT predicate)
  : obegin(begin), oend(end), pred(predicate)
{
}
}
