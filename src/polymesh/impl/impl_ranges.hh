#pragma once

#include <polymesh/Mesh.hh>

namespace polymesh
{
namespace detail
{
template <class T>
auto helper_min(T const& a, T const& b) -> decltype(min(a, b))
{
    return min(a, b);
}
template <class T>
auto helper_max(T const& a, T const& b) -> decltype(max(a, b))
{
    return max(a, b);
}
template <class A, class B, class = void>
auto helper_min(A const& a, B const& b) -> decltype(a < b ? a : b)
{
    return a < b ? a : b;
}
template <class A, class B, class = void>
auto helper_max(A const& a, B const& b) -> decltype(!(a < b) ? a : b)
{
    return !(a < b) ? a : b;
}

template <class T>
auto helper_add(T const& a, T const& b) -> decltype(a + b)
{
    return a + b;
}
template <class A, class B>
auto helper_add(A const& a, B const& b) -> decltype(A() + (a - A()) + (b - B()))
{
    return A() + (a - A()) + (b - B());
}
} // namespace detail

template <class this_t, class ElementT>
ElementT smart_range<this_t, ElementT>::first() const
{
    for (auto&& h : *static_cast<this_t const*>(this))
        return h;
    return {};
}
template <class this_t, class ElementT>
ElementT smart_range<this_t, ElementT>::last() const
{
    ElementT result;
    for (auto&& h : *static_cast<this_t const*>(this))
        result = h;
    return result;
}

template <class this_t, class ElementT>
bool smart_range<this_t, ElementT>::empty() const
{
    return !static_cast<this_t const*>(this)->begin().is_valid();
}

template <class this_t, class ElementT>
template <class PredT>
bool smart_range<this_t, ElementT>::any(PredT&& p) const
{
    for (auto&& h : *static_cast<this_t const*>(this))
        if (p(h))
            return true;
    return false;
}

template <class this_t, class ElementT>
template <class PredT>
bool smart_range<this_t, ElementT>::all(PredT&& p) const
{
    for (auto&& h : *static_cast<this_t const*>(this))
        if (!p(h))
            return false;
    return true;
}

template <class this_t, class ElementT>
template <class PredT>
int smart_range<this_t, ElementT>::count(PredT&& p) const
{
    auto cnt = 0;
    for (auto&& h : *static_cast<this_t const*>(this))
        if (p(h))
            ++cnt;
    return cnt;
}

template <class this_t, class ElementT>
int smart_range<this_t, ElementT>::count() const
{
    auto cnt = 0;
    for (auto&& h : *static_cast<this_t const*>(this))
    {
        (void)h; // unused
        ++cnt;
    }
    return cnt;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::min(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto v = f(*it);
    ++it;
    while (it.is_valid())
    {
        v = detail::helper_min(v, f(*it));
        ++it;
    }
    return v;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::max(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto v = f(*it);
    ++it;
    while (it.is_valid())
    {
        v = detail::helper_max(v, f(*it));
        ++it;
    }
    return v;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::sum(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto s = f(*it);
    ++it;
    while (it.is_valid())
    {
        s = s + f(*it);
        ++it;
    }
    return s;
}

template <class this_t, class ElementT>
template <class FuncT>
ElementT smart_range<this_t, ElementT>::min_by(FuncT&& f) const
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto e_min = *it;
    auto v_min = f(e_min);
    ++it;
    while (it.is_valid())
    {
        auto e = *it;
        auto v = f(*it);
        if (v < v_min)
        {
            v_min = v;
            e_min = e;
        }
        ++it;
    }
    return e_min;
}

template <class this_t, class ElementT>
template <class FuncT>
ElementT smart_range<this_t, ElementT>::max_by(FuncT&& f) const
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto e_max = *it;
    auto v_max = f(e_max);
    ++it;
    while (it.is_valid())
    {
        auto e = *it;
        auto v = f(*it);
        if (v > v_max)
        {
            v_max = v;
            e_max = e;
        }
        ++it;
    }
    return e_max;
}

template <class this_t, class ElementT>
template <class FuncT>
polymesh::minmax_t<ElementT> smart_range<this_t, ElementT>::minmax_by(FuncT&& f) const
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto e_min = *it;
    auto e_max = e_min;
    auto v_min = f(e_min);
    auto v_max = v_min;
    ++it;
    while (it.is_valid())
    {
        auto e = *it;
        auto v = f(*it);
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
        ++it;
    }
    return {e_min, e_max};
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::avg(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    decltype(f(*it) + f(*it)) s = f(*it);
    auto cnt = 1;
    static_assert(tmp::can_divide_by<decltype(s), decltype(cnt)>::value, "Cannot divide sum by an integer. (if glm is used, including <glm/ext.hpp> "
                                                                         "might help)");
    ++it;
    while (it.is_valid())
    {
        s = s + f(*it);
        ++cnt;
        ++it;
    }
    return s / cnt;
}

template <class this_t, class ElementT>
template <class FuncT, class WeightT>
auto smart_range<this_t, ElementT>::weighted_avg(FuncT&& f, WeightT&& w) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto e = *it;
    auto ws = w(e);
    decltype(f(e) + f(e)) s = f(e) * ws;
    static_assert(tmp::can_divide_by<decltype(s), decltype(ws)>::value, "Cannot divide sum by weight. (if glm is used, including <glm/ext.hpp> might "
                                                                        "help)");
    ++it;
    while (it.is_valid())
    {
        auto ee = *it;
        auto ww = w(ee);
        s = s + f(ee) * ww;
        ws = ws + ww;
        ++it;
    }
    return s / ws;
}

template <class this_t, class ElementT>
template <class FuncT, class FuncInvT>
auto smart_range<this_t, ElementT>::f_mean(FuncT&& f, FuncInvT&& f_inv) const
    -> tmp::decayed_result_type_of<FuncInvT, tmp::decayed_result_type_of<FuncT, ElementT>>
{
    return f_inv(this->avg(f));
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::arithmetic_mean(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    return this->avg(f);
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::median(FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    return this->order_statistic(0.5f, f);
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::order_statistic(float p, FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto vals = this->to_vector(f);
    POLYMESH_ASSERT(!vals.empty() && "requires non-empty range");
    auto n = int(vals.size() * p + .5f);
    if (n < 0)
        n = 0;
    if (n >= (int)vals.size())
        n = (int)vals.size() - 1;
    auto itn = vals.begin() + n;
    std::nth_element(vals.begin(), itn, vals.end());
    return *itn;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::aabb(FuncT&& f) const -> polymesh::minmax_t<tmp::decayed_result_type_of<FuncT, ElementT>>
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto v = f(*it);
    polymesh::minmax_t<tmp::decayed_result_type_of<FuncT, ElementT>> r = {v, v};
    ++it;
    while (it.is_valid())
    {
        auto vv = f(*it);
        r.min = detail::helper_min(r.min, vv);
        r.max = detail::helper_max(r.max, vv);
        ++it;
    }
    return r;
}

template <class this_t, class ElementT>
template <class Generator, class FuncT>
auto smart_range<this_t, ElementT>::random(Generator& g, FuncT&& f) const -> tmp::decayed_result_type_of<FuncT, ElementT>
{
    auto it = static_cast<this_t const*>(this)->begin();
    POLYMESH_ASSERT(it.is_valid() && "requires non-empty range");
    auto v = f(*it);
    double cnt = 2;
    ++it;
    while (it.is_valid())
    {
        if (double(g()) * cnt < double(g.max()))
            v = f(*it);
        ++it;
        cnt += 1;
    }
    return v;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::minmax(FuncT&& f) const -> polymesh::minmax_t<tmp::decayed_result_type_of<FuncT, ElementT>>
{
    return aabb(f);
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::to_vector(FuncT&& f) const -> std::vector<tmp::decayed_result_type_of<FuncT, ElementT>>
{
    std::vector<tmp::decayed_result_type_of<FuncT, ElementT>> v;
    this->into_vector(v, f);
    return v;
}

template <class this_t, class ElementT>
template <size_t N, class FuncT>
auto smart_range<this_t, ElementT>::to_array(FuncT&& f) const -> std::array<tmp::decayed_result_type_of<FuncT, ElementT>, N>
{
    std::array<tmp::decayed_result_type_of<FuncT, ElementT>, N> a;
    this->into_array(a, f);
    return a;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::to_set(FuncT&& f) const -> std::set<tmp::decayed_result_type_of<FuncT, ElementT>>
{
    std::set<tmp::decayed_result_type_of<FuncT, ElementT>> s;
    this->into_set(s, f);
    return s;
}

template <class this_t, class ElementT>
template <class FuncT>
auto smart_range<this_t, ElementT>::to_map(FuncT&& f) const -> std::map<ElementT, tmp::decayed_result_type_of<FuncT, ElementT>>
{
    std::map<ElementT, tmp::decayed_result_type_of<FuncT, ElementT>> m;
    this->into_map(m, f);
    return m;
}

template <class this_t, class ElementT>
template <class FuncT>
void smart_range<this_t, ElementT>::into_vector(std::vector<tmp::decayed_result_type_of<FuncT, ElementT>>& container, FuncT&& f) const
{
    for (auto&& h : *static_cast<this_t const*>(this))
        container.push_back(f(h));
}

template <class this_t, class ElementT>
template <class FuncT>
void smart_range<this_t, ElementT>::into_set(std::set<tmp::decayed_result_type_of<FuncT, ElementT>>& container, FuncT&& f) const
{
    for (auto&& h : *static_cast<this_t const*>(this))
        container.insert(f(h));
}

template <class this_t, class ElementT>
template <class FuncT>
void smart_range<this_t, ElementT>::into_map(std::map<ElementT, tmp::decayed_result_type_of<FuncT, ElementT>>& container, FuncT&& f) const
{
    for (auto&& h : *static_cast<this_t const*>(this))
        container[h] = f(h);
}

template <class this_t, class ElementT>
template <size_t N, class FuncT>
void smart_range<this_t, ElementT>::into_array(std::array<tmp::decayed_result_type_of<FuncT, ElementT>, N>& container, FuncT&& f) const
{
    auto idx = 0u;
    for (auto&& h : *static_cast<this_t const*>(this))
    {
        if (idx >= N)
            break;

        container[idx] = f(h);

        ++idx;
    }
}


template <class this_t, class ElementT>
template <class PredT>
auto smart_range<this_t, ElementT>::where(PredT&& p) const -> filtered_range<ElementT, this_t const&, PredT>
{
    return {static_cast<this_t const*>(this)->begin(), p};
}

template <class this_t, class ElementT>
template <class PredT>
auto smart_range<this_t, ElementT>::where(PredT&& p) -> filtered_range<ElementT, this_t&, PredT>
{
    return {static_cast<this_t*>(this)->begin(), p};
}

template <class this_t, class ElementT>
template <class PredT>
auto smart_range<this_t, ElementT>::filter(PredT&& p) const -> filtered_range<ElementT, this_t const&, PredT>
{
    return {static_cast<this_t const*>(this)->begin(), p};
}

template <class this_t, class ElementT>
template <class PredT>
auto smart_range<this_t, ElementT>::filter(PredT&& p) -> filtered_range<ElementT, this_t&, PredT>
{
    return {static_cast<this_t*>(this)->begin(), p};
}

template <class this_t, class tag>
int primitive_ring<this_t, tag>::size() const
{
    auto cnt = 0;
    for (auto&& v : *static_cast<this_t const*>(this))
    {
        (void)v; // unused
        cnt++;
    }
    return cnt;
}

template <class this_t, class tag>
bool primitive_ring<this_t, tag>::contains(handle v) const
{
    for (auto&& v2 : *static_cast<this_t const*>(this))
        if (v == v2)
            return true;
    return false;
}

template <class mesh_ptr, class tag, class iterator>
int smart_collection<mesh_ptr, tag, iterator>::size() const
{
    return iterator::primitive_size(*m);
}

template <class mesh_ptr, class tag, class iterator>
void smart_collection<mesh_ptr, tag, iterator>::reserve(int capacity) const
{
    return primitive<tag>::reserve(*m, capacity);
}

template <class mesh_ptr, class tag, class iterator>
typename smart_collection<mesh_ptr, tag, iterator>::handle smart_collection<mesh_ptr, tag, iterator>::operator[](int idx) const
{
    POLYMESH_ASSERT(idx < iterator::primitive_size(*m));
    return (*m)[index(idx)];
}

template <class mesh_ptr, class tag, class iterator>
typename smart_collection<mesh_ptr, tag, iterator>::handle smart_collection<mesh_ptr, tag, iterator>::operator[](index idx) const
{
    return (*m)[idx];
}

template <class mesh_ptr, class tag, class iterator>
template <class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute(AttrT const& def_value) const
{
    return typename primitive<tag>::template attribute<AttrT>(m, def_value);
}

template <class mesh_ptr, class tag, class iterator>
template <class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute_with_default(AttrT const& def_value) const
{
    return typename primitive<tag>::template attribute<AttrT>(m, def_value);
}

template <class mesh_ptr, class tag, class iterator>
template <class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute_from_data(std::vector<AttrT> const& data) const
{
    auto attr = make_attribute<AttrT>();
    attr.copy_from(data);
    return attr; // copy elison
}

template <class mesh_ptr, class tag, class iterator>
template <class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::make_attribute_from_data(AttrT const* data, int cnt) const
{
    auto attr = make_attribute<AttrT>();
    attr.copy_from(data, cnt);
    return attr; // copy elison
}

template <class mesh_ptr, class tag, class iterator>
template <class FuncT, class AttrT>
typename primitive<tag>::template attribute<AttrT> smart_collection<mesh_ptr, tag, iterator>::map(FuncT&& f, AttrT const& def_value) const
{
    auto attr = make_attribute<AttrT>(def_value);
    for (auto&& h : *this)
        attr[h] = f(h);
    return attr; // copy elison
}

template <class mesh_ptr, class tag, class iterator>
iterator smart_collection<mesh_ptr, tag, iterator>::begin() const
{
    return {*this->m, index(0), index(primitive<tag>::all_size(*this->m))};
}

template <class mesh_ptr, class tag, class iterator>
template <class Generator>
typename primitive<tag>::handle smart_collection<mesh_ptr, tag, iterator>::random(Generator& g) const
{
    auto s = primitive<tag>::all_size(*this->m);
    POLYMESH_ASSERT(s > 0 && "Cannot chose from empty mesh");

    typename primitive<tag>::handle h = {this->m, typename primitive<tag>::index(int(g() % s))};

    if constexpr (iterator::is_valid_only_iterator)
    {
        POLYMESH_ASSERT(primitive<tag>::valid_size(*this->m) > 0 && "Cannot chose from empty mesh");
        while (h.is_removed())
            h = {this->m, typename primitive<tag>::index(int(g() % s))};
    }

    return h;
}

template <class iterator>
void vertex_collection<iterator>::remove(vertex_handle v) const
{
    low_level_api(this->m).remove_vertex(v.idx);
}

template <class iterator>
void face_collection<iterator>::remove(face_handle f) const
{
    low_level_api(this->m).remove_face(f.idx);
}

template <class iterator>
void edge_collection<iterator>::remove(edge_handle e) const
{
    low_level_api(this->m).remove_edge(e.idx);
}

template <class iterator>
void halfedge_collection<iterator>::remove_edge(halfedge_handle h) const
{
    low_level_api(this->m).remove_edge(low_level_api(this->m).edge_of(h.idx));
}

template <class iterator>
void face_collection<iterator>::permute(std::vector<int> const& p) const
{
    low_level_api(this->m).permute_faces(p);
}

template <class iterator>
void edge_collection<iterator>::permute(std::vector<int> const& p) const
{
    low_level_api(this->m).permute_edges(p);
}

template <class iterator>
void vertex_collection<iterator>::permute(std::vector<int> const& p) const
{
    low_level_api(this->m).permute_vertices(p);
}


inline valid_vertex_collection Mesh::vertices()
{
    valid_vertex_collection c;
    c.m = this;
    return c;
}
inline valid_vertex_const_collection Mesh::vertices() const
{
    valid_vertex_const_collection c;
    c.m = this;
    return c;
}
inline all_vertex_collection Mesh::all_vertices()
{
    all_vertex_collection c;
    c.m = this;
    return c;
}
inline all_vertex_const_collection Mesh::all_vertices() const
{
    all_vertex_const_collection c;
    c.m = this;
    return c;
}
inline valid_face_collection Mesh::faces()
{
    valid_face_collection c;
    c.m = this;
    return c;
}
inline valid_face_const_collection Mesh::faces() const
{
    valid_face_const_collection c;
    c.m = this;
    return c;
}
inline all_face_collection Mesh::all_faces()
{
    all_face_collection c;
    c.m = this;
    return c;
}
inline all_face_const_collection Mesh::all_faces() const
{
    all_face_const_collection c;
    c.m = this;
    return c;
}
inline valid_edge_collection Mesh::edges()
{
    valid_edge_collection c;
    c.m = this;
    return c;
}
inline valid_edge_const_collection Mesh::edges() const
{
    valid_edge_const_collection c;
    c.m = this;
    return c;
}
inline all_edge_collection Mesh::all_edges()
{
    all_edge_collection c;
    c.m = this;
    return c;
}
inline all_edge_const_collection Mesh::all_edges() const
{
    all_edge_const_collection c;
    c.m = this;
    return c;
}
inline valid_halfedge_collection Mesh::halfedges()
{
    valid_halfedge_collection c;
    c.m = this;
    return c;
}
inline valid_halfedge_const_collection Mesh::halfedges() const
{
    valid_halfedge_const_collection c;
    c.m = this;
    return c;
}
inline all_halfedge_collection Mesh::all_halfedges()
{
    all_halfedge_collection c;
    c.m = this;
    return c;
}
inline all_halfedge_const_collection Mesh::all_halfedges() const
{
    all_halfedge_const_collection c;
    c.m = this;
    return c;
}

template <class iterator>
vertex_handle vertex_collection<iterator>::add() const
{
    return this->m->handle_of(low_level_api(this->m).add_vertex());
}

template <class iterator>
face_handle face_collection<iterator>::add(const vertex_handle* v_handles, int vcnt) const
{
    return this->m->handle_of(low_level_api(this->m).add_face(v_handles, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(const vertex_index* v_indices, int vcnt) const
{
    return this->m->handle_of(low_level_api(this->m).add_face(v_indices, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(const halfedge_handle* half_loop, int vcnt) const
{
    return this->m->handle_of(low_level_api(this->m).add_face(half_loop, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(const halfedge_index* half_loop, int vcnt) const
{
    return this->m->handle_of(low_level_api(this->m).add_face(half_loop, vcnt));
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<vertex_handle> const& v_handles) const
{
    return add(v_handles.data(), int(v_handles.size()));
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<vertex_index> const& v_indices) const
{
    return add(v_indices.data(), int(v_indices.size()));
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<halfedge_handle> const& half_loop) const
{
    return add(half_loop.data(), int(half_loop.size()));
}

template <class iterator>
face_handle face_collection<iterator>::add(std::vector<halfedge_index> const& half_loop) const
{
    return add(half_loop.data(), int(half_loop.size()));
}

template <class iterator>
face_handle face_collection<iterator>::add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    POLYMESH_ASSERT(v0.is_valid() && v1.is_valid() && v2.is_valid());
    POLYMESH_ASSERT(!v0.is_removed() && !v1.is_removed() && !v2.is_removed());
    halfedge_index hs[3] = {
        low_level_api(this->m).add_or_get_halfedge(v2.idx, v0.idx), //
        low_level_api(this->m).add_or_get_halfedge(v0.idx, v1.idx), //
        low_level_api(this->m).add_or_get_halfedge(v1.idx, v2.idx), //
    };
    return this->m->handle_of(low_level_api(this->m).add_face(hs, 3));
}

template <class iterator>
face_handle face_collection<iterator>::add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    POLYMESH_ASSERT(v0.is_valid() && v1.is_valid() && v2.is_valid() && v3.is_valid());
    POLYMESH_ASSERT(!v0.is_removed() && !v1.is_removed() && !v2.is_removed() && !v3.is_removed());
    halfedge_index hs[4] = {
        low_level_api(this->m).add_or_get_halfedge(v3.idx, v0.idx), //
        low_level_api(this->m).add_or_get_halfedge(v0.idx, v1.idx), //
        low_level_api(this->m).add_or_get_halfedge(v1.idx, v2.idx), //
        low_level_api(this->m).add_or_get_halfedge(v2.idx, v3.idx), //
    };
    return this->m->handle_of(low_level_api(this->m).add_face(hs, 4));
}

template <class iterator>
face_handle face_collection<iterator>::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const
{
    POLYMESH_ASSERT(h0.is_valid() && h1.is_valid() && h2.is_valid());
    POLYMESH_ASSERT(!h0.is_removed() && !h1.is_removed() && !h2.is_removed());
    halfedge_index hs[3] = {h0.idx, h1.idx, h2.idx};
    return this->m->handle_of(low_level_api(this->m).add_face(hs, 3));
}

template <class iterator>
face_handle face_collection<iterator>::add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const
{
    POLYMESH_ASSERT(h0.is_valid() && h1.is_valid() && h2.is_valid() && h3.is_valid());
    POLYMESH_ASSERT(!h0.is_removed() && !h1.is_removed() && !h2.is_removed() && !h3.is_removed());
    halfedge_index hs[4] = {h0.idx, h1.idx, h2.idx, h3.idx};
    return this->m->handle_of(low_level_api(this->m).add_face(hs, 4));
}

template <class iterator>
template <size_t N>
face_handle face_collection<iterator>::add(const vertex_handle (&v_handles)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0u; i < N; ++i)
    {
        POLYMESH_ASSERT(v_handles[i].is_valid());
        POLYMESH_ASSERT(!v_handles[i].is_removed());
        hs[i] = low_level_api(this->m).add_or_get_halfedge(v_handles[(i + N - 1) % N].idx, v_handles[i].idx);
    }
    return this->m->handle_of(low_level_api(this->m).add_face(hs, N));
}

template <class iterator>
template <size_t N>
face_handle face_collection<iterator>::add(const halfedge_handle (&half_loop)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0u; i < N; ++i)
    {
        POLYMESH_ASSERT(half_loop[i].is_valid());
        POLYMESH_ASSERT(!half_loop[i].is_removed());
        hs[i] = half_loop[i].idx;
    }
    return this->m->handle_of(low_level_api(this->m).add_face(hs, N));
}

template <class iterator>
bool face_collection<iterator>::can_add(const vertex_handle* v_handles, int vcnt) const
{
    return low_level_api(this->m).can_add_face(v_handles, vcnt);
}

template <class iterator>
bool face_collection<iterator>::can_add(const vertex_index* v_indices, int vcnt) const
{
    return low_level_api(this->m).can_add_face(v_indices, vcnt);
}

template <class iterator>
bool face_collection<iterator>::can_add(const halfedge_handle* half_loop, int vcnt) const
{
    return low_level_api(this->m).can_add_face(half_loop, vcnt);
}

template <class iterator>
bool face_collection<iterator>::can_add(const halfedge_index* half_loop, int vcnt) const
{
    return low_level_api(this->m).can_add_face(half_loop, vcnt);
}

template <class iterator>
bool face_collection<iterator>::can_add(std::vector<vertex_handle> const& v_handles) const
{
    return can_add(v_handles.data(), int(v_handles.size()));
}

template <class iterator>
bool face_collection<iterator>::can_add(std::vector<vertex_index> const& v_indices) const
{
    return can_add(v_indices.data(), int(v_indices.size()));
}

template <class iterator>
bool face_collection<iterator>::can_add(std::vector<halfedge_handle> const& half_loop) const
{
    return can_add(half_loop.data(), int(half_loop.size()));
}

template <class iterator>
bool face_collection<iterator>::can_add(std::vector<halfedge_index> const& half_loop) const
{
    return can_add(half_loop.data(), int(half_loop.size()));
}

template <class iterator>
bool face_collection<iterator>::can_add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const
{
    vertex_index vs[3] = {v0.idx, v1.idx, v2.idx};
    return low_level_api(this->m).can_add_face(vs, 3);
}

template <class iterator>
bool face_collection<iterator>::can_add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const
{
    vertex_index vs[4] = {v0.idx, v1.idx, v2.idx, v3.idx};
    return low_level_api(this->m).can_add_face(vs, 4);
}

template <class iterator>
bool face_collection<iterator>::can_add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const
{
    halfedge_index hs[3] = {h0.idx, h1.idx, h2.idx};
    return low_level_api(this->m).can_add_face(hs, 3);
}

template <class iterator>
bool face_collection<iterator>::can_add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const
{
    halfedge_index hs[4] = {h0.idx, h1.idx, h2.idx, h3.idx};
    return low_level_api(this->m).can_add_face(hs, 4);
}

template <class iterator>
template <size_t N>
bool face_collection<iterator>::can_add(const vertex_handle (&v_handles)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0u; i < N; ++i)
        hs[i] = low_level_api(this->m).find_halfedge(v_handles[i].idx, v_handles[(i + 1) % N].idx);
    return low_level_api(this->m).can_add_face(hs, N);
}

template <class iterator>
template <size_t N>
bool face_collection<iterator>::can_add(const halfedge_handle (&half_loop)[N]) const
{
    halfedge_index hs[N];
    for (auto i = 0u; i < N; ++i)
        hs[i] = half_loop[i].idx;
    return low_level_api(this->m).can_add_face(hs, N);
}

template <class iterator>
edge_handle edge_collection<iterator>::add_or_get(vertex_handle v_from, vertex_handle v_to) const
{
    return this->m->handle_of(low_level_api(this->m).add_or_get_edge(v_from.idx, v_to.idx));
}

template <class iterator>
halfedge_handle halfedge_collection<iterator>::add_or_get(vertex_handle v_from, vertex_handle v_to) const
{
    return this->m->handle_of(low_level_api(this->m).add_or_get_halfedge(v_from.idx, v_to.idx));
}

template <class iterator>
edge_handle edge_collection<iterator>::add_or_get(halfedge_handle h_from, halfedge_handle h_to) const
{
    return this->m->handle_of(low_level_api(this->m).add_or_get_edge(h_from.idx, h_to.idx));
}

template <class iterator>
halfedge_handle halfedge_collection<iterator>::add_or_get(halfedge_handle h_from, halfedge_handle h_to) const
{
    return this->m->handle_of(low_level_api(this->m).add_or_get_halfedge(h_from.idx, h_to.idx));
}

template <class iterator>
edge_handle edge_collection<iterator>::find(vertex_handle v_from, vertex_handle v_to) const
{
    return this->m->handle_of(low_level_api(this->m).edge_of(low_level_api(this->m).find_halfedge(v_from.idx, v_to.idx)));
}

template <class iterator>
bool edge_collection<iterator>::exists(vertex_handle v_from, vertex_handle v_to) const
{
    return low_level_api(this->m).find_halfedge(v_from.idx, v_to.idx).is_valid();
}

template <class iterator>
halfedge_handle halfedge_collection<iterator>::find(vertex_handle v_from, vertex_handle v_to) const
{
    return this->m->handle_of(low_level_api(this->m).find_halfedge(v_from.idx, v_to.idx));
}

template <class iterator>
bool halfedge_collection<iterator>::exists(vertex_handle v_from, vertex_handle v_to) const
{
    return low_level_api(this->m).find_halfedge(v_from.idx, v_to.idx).is_valid();
}

template <class iterator>
vertex_handle face_collection<iterator>::split(face_handle f) const
{
    return this->m->handle_of(low_level_api(this->m).face_split(f.idx));
}

template <class iterator>
halfedge_handle face_collection<iterator>::cut(face_handle f, halfedge_handle h0, halfedge_handle h1) const
{
    return this->m->handle_of(low_level_api(this->m).face_cut(f.idx, h0.idx, h1.idx));
}

template <class iterator>
void face_collection<iterator>::split(face_handle f, vertex_handle v) const
{
    low_level_api(this->m).face_split(f.idx, v.idx);
}

template <class iterator>
face_handle face_collection<iterator>::fill(halfedge_handle h) const
{
    return this->m->handle_of(low_level_api(this->m).face_fill(h.idx));
}

template <class iterator>
vertex_handle edge_collection<iterator>::split(edge_handle e) const
{
    return this->m->handle_of(low_level_api(this->m).edge_split(e.idx));
}

template <class iterator>
void edge_collection<iterator>::split(edge_handle e, vertex_handle v) const
{
    low_level_api(this->m).edge_split(e.idx, v.idx);
}

template <class iterator>
vertex_handle edge_collection<iterator>::split_and_triangulate(edge_handle e) const
{
    return this->m->handle_of(low_level_api(this->m).edge_split_and_triangulate(e.idx));
}

template <class iterator>
void edge_collection<iterator>::split_and_triangulate(edge_handle e, vertex_handle v) const
{
    low_level_api(this->m).edge_split_and_triangulate(e.idx, v.idx);
}

template <class iterator>
void vertex_collection<iterator>::collapse(vertex_handle v) const
{
    low_level_api(this->m).vertex_collapse(v.idx);
}

template <class iterator>
void halfedge_collection<iterator>::collapse(halfedge_handle h) const
{
    low_level_api(this->m).halfedge_collapse(h.idx);
}

template <class iterator>
void edge_collection<iterator>::rotate_next(edge_handle e) const
{
    low_level_api(this->m).edge_rotate_next(e.idx);
}

template <class iterator>
void edge_collection<iterator>::rotate_prev(edge_handle e) const
{
    low_level_api(this->m).edge_rotate_prev(e.idx);
}

template <class iterator>
void edge_collection<iterator>::flip(edge_handle e) const
{
    low_level_api(this->m).edge_flip(e.idx);
}

template <class iterator>
vertex_handle halfedge_collection<iterator>::split(halfedge_handle h) const
{
    return this->m->handle_of(low_level_api(this->m).halfedge_split(h.idx));
}

template <class iterator>
void halfedge_collection<iterator>::split(halfedge_handle h, vertex_handle v) const
{
    low_level_api(this->m).halfedge_split(h.idx, v.idx);
}

template <class iterator>
void halfedge_collection<iterator>::attach(halfedge_handle h, vertex_handle v) const
{
    low_level_api(this->m).halfedge_attach(h.idx, v.idx);
}

template <class iterator>
void halfedge_collection<iterator>::merge(halfedge_handle h) const
{
    low_level_api(this->m).halfedge_merge(h.idx);
}

template <class iterator>
void halfedge_collection<iterator>::rotate_next(halfedge_handle h) const
{
    low_level_api(this->m).halfedge_rotate_next(h.idx);
}

template <class iterator>
void halfedge_collection<iterator>::rotate_prev(halfedge_handle h) const
{
    low_level_api(this->m).halfedge_rotate_prev(h.idx);
}
} // namespace polymesh
