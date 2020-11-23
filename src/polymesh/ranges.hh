#pragma once

#include <array>
#include <cstddef>
#include <map>
#include <set>
#include <vector>

#include "iterators.hh"

namespace polymesh
{
template <class T>
struct minmax_t
{
    T min;
    T max;
};

template <class V, class W>
struct weighted_sample
{
    V value;
    W weight;
};

template <class ElementT, class RangeT, class PredT>
struct filtered_range;

/// Base class for "smart ranges"
/// (i.e. collections with plenty of helpers that encourage a functional programming style)
/// NOTE: this class uses CRTP to reduce runtime overhead
template <class this_t, class ElementT>
struct smart_range
{
    /// returns the first element in this range
    /// returns invalid on empty ranges (or default ctor'd one)
    ElementT first() const;
    /// returns the last element in this range
    /// returns invalid on empty ranges (or default ctor'd one)
    /// TODO: how to make this O(1)
    ElementT last() const;

    /// returns true if the range is empty
    bool empty() const;
    /// returns true if any value satisfying p(v)
    /// also works for boolean attributes
    template <class PredT = tmp::identity>
    bool any(PredT&& p = {}) const;
    /// returns true if all values satisfy p(v)
    /// also works for boolean attributes
    template <class PredT = tmp::identity>
    bool all(PredT&& p = {}) const;

    /// returns the number of elements in this range
    /// NOTE: this is an O(n) operation, prefer size() if available
    /// TODO: maybe SFINAE to implement this via size() if available?
    int count() const;
    /// returns the number of elements satisfying p(v) in this range
    template <class PredT>
    int count(PredT&& p) const;

    /// calculates min(f(e)) over all elements
    /// undefined behavior if range is empty
    /// works for std::min and everything reachable by ADL (calls min(_, _))
    template <class FuncT = tmp::identity>
    auto min(FuncT&& f = {}) const -> tmp::decayed_result_type_of<FuncT, ElementT>;
    /// returns e that minimizes f(e)
    /// undefined behavior if range is empty
    /// requires working comparison operators for the result
    template <class FuncT>
    ElementT min_by(FuncT&& f) const;
    /// calculates max(f(e)) over all elements
    /// undefined behavior if range is empty
    /// works for std::max and everything reachable by ADL (calls max(_, _))
    template <class FuncT = tmp::identity>
    auto max(FuncT&& f = {}) const -> tmp::decayed_result_type_of<FuncT, ElementT>;
    /// returns e that maximizes f(e)
    /// undefined behavior if range is empty
    /// requires working comparison operators for the result
    template <class FuncT>
    ElementT max_by(FuncT&& f) const;
    /// calculates the sum of f(e) over all elements
    /// undefined behavior if range is empty
    /// requires operator+ for the elements
    template <class FuncT = tmp::identity>
    auto sum(FuncT&& f = {}) const -> tmp::decayed_result_type_of<FuncT, ElementT>;
    /// calculates the avg of f(e) over all elements
    /// undefined behavior if range is empty
    /// requires operator+ for the elements as well as operator/(ElementT, int)
    template <class FuncT = tmp::identity>
    auto avg(FuncT&& f = {}) const -> tmp::decayed_result_type_of<FuncT, ElementT>;

    /// calculates the weighted avg of f(e) with weight w(e) over all elements
    /// undefined behavior if range is empty
    /// requires operator+ for the elements and weights as well as operator/(ElementT, WeightT)
    template <class FuncT, class WeightT>
    auto weighted_avg(FuncT&& f, WeightT&& w) const -> tmp::decayed_result_type_of<FuncT, ElementT>;

    /// calculates the f-mean of this range
    /// the f-mean is f_inv(avg(f))
    template <class FuncT, class FuncInvT>
    auto f_mean(FuncT&& f, FuncInvT&& f_inv) const -> tmp::decayed_result_type_of<FuncInvT, tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// calculates the arithmetic mean of f(e) for this range (same as avg)
    template <class FuncT = tmp::identity>
    auto arithmetic_mean(FuncT&& f = {}) const -> tmp::decayed_result_type_of<FuncT, ElementT>;

    /// computes the median
    /// NOTE: constructs an intermediate vector!
    template <class FuncT = tmp::identity>
    auto median(FuncT&& f = {}) const -> tmp::decayed_result_type_of<FuncT, ElementT>;
    /// computes the p-order statistic (p in 0..1)
    /// p: 0 is min, 0.5 is median, 1 is max
    /// NOTE: constructs an intermediate vector!
    template <class FuncT = tmp::identity>
    auto order_statistic(float p, FuncT&& f = {}) const -> tmp::decayed_result_type_of<FuncT, ElementT>;

    /// calculates the aabb (min and max) of f(e) over all elements
    /// undefined behavior if range is empty
    /// works for std::min/max and everything reachable by ADL (calls min/max(_, _))
    template <class FuncT = tmp::identity>
    auto aabb(FuncT&& f = {}) const -> polymesh::minmax_t<tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// same as aabb(...)
    template <class FuncT = tmp::identity>
    auto minmax(FuncT&& f = {}) const -> polymesh::minmax_t<tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// returns {e_min, e_max} that minimizes/maximizes f(e)
    /// undefined behavior if range is empty
    /// requires working comparison operators for the result
    template <class FuncT>
    polymesh::minmax_t<ElementT> minmax_by(FuncT&& f) const;

    /// converts this range to a vector containing f(v) entries
    template <class FuncT = tmp::identity>
    auto to_vector(FuncT&& f = {}) const -> std::vector<tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// converts this range to a set containing f(v) entries
    template <class FuncT = tmp::identity>
    auto to_set(FuncT&& f = {}) const -> std::set<tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// converts this range to a map containing {v, f(v)} entries
    template <class FuncT = tmp::identity>
    auto to_map(FuncT&& f = {}) const -> std::map<ElementT, tmp::decayed_result_type_of<FuncT, ElementT>>;
    /// converts this range to a fixed size array containing f(v) entries
    /// NOTE: if less elements are present, array is filled with default constructed elements
    template <size_t N, class FuncT = tmp::identity>
    auto to_array(FuncT&& f = {}) const -> std::array<tmp::decayed_result_type_of<FuncT, ElementT>, N>;

    /// same as to_* but takes the container as a parameter (does NOT clear the container!)
    template <class FuncT = tmp::identity>
    void into_vector(std::vector<tmp::decayed_result_type_of<FuncT, ElementT>>& container, FuncT&& f = {}) const;
    template <class FuncT = tmp::identity>
    void into_set(std::set<tmp::decayed_result_type_of<FuncT, ElementT>>& container, FuncT&& f = {}) const;
    template <class FuncT = tmp::identity>
    void into_map(std::map<ElementT, tmp::decayed_result_type_of<FuncT, ElementT>>& container, FuncT&& f = {}) const;
    template <size_t N, class FuncT = tmp::identity>
    void into_array(std::array<tmp::decayed_result_type_of<FuncT, ElementT>, N>& container, FuncT&& f = {}) const;

    /// returns a new range that consists of all elements where p(x) is true
    template <class PredT>
    auto where(PredT&& p) const -> filtered_range<ElementT, this_t const&, PredT>;
    template <class PredT>
    auto where(PredT&& p) -> filtered_range<ElementT, this_t&, PredT>;
    /// same as where
    template <class PredT>
    auto filter(PredT&& p) const -> filtered_range<ElementT, this_t const&, PredT>;
    template <class PredT>
    auto filter(PredT&& p) -> filtered_range<ElementT, this_t&, PredT>;

    /// returns a uniformly random sampled element from this range
    /// complexity is O(n) (but does not allocate)
    template <class Generator, class FuncT = tmp::identity>
    auto random(Generator& g, FuncT&& f = {}) const -> tmp::decayed_result_type_of<FuncT, ElementT>;

    // TODO: (requires new ranges)
    // - filter (or where?)
    // - map
    // - skip
    // - only_valid
    // - conversions from vector/set/map
};


// ================= FILTER + MAP =================

template <class ElementT, class RangeT, class PredT>
struct filtered_range : smart_range<filtered_range<ElementT, RangeT, PredT>, ElementT>
{
    using IteratorT = decltype(std::declval<RangeT>().begin());

    filtered_range(IteratorT it, PredT p) : it_begin(std::forward<IteratorT>(it)), pred(std::forward<PredT>(p)) {}

    filtering_iterator<IteratorT, PredT> begin() const { return {it_begin, pred}; }
    end_iterator end() const { return {}; }

    IteratorT it_begin;
    PredT pred;

    filtered_range(filtered_range const&) = delete;
    filtered_range(filtered_range&&) = delete;
    filtered_range& operator=(filtered_range const&) = delete;
    filtered_range& operator=(filtered_range&&) = delete;
};

// TODO: mapped_range


// ================= COLLECTION =================

template <class mesh_ptr, class tag, class iterator>
struct smart_collection : smart_range<smart_collection<mesh_ptr, tag, iterator>, typename primitive<tag>::handle>
{
    template <class AttrT>
    using attribute = typename primitive<tag>::template attribute<AttrT>;
    using handle = typename primitive<tag>::handle;
    using index = typename primitive<tag>::index;

    /// Number of primitives (includes those marked for deletion if all_xyz collection is used)
    /// O(1) computation
    int size() const;

    /// Ensures that a given number of primitives can be stored without reallocation
    void reserve(int capacity) const;

    /// Creates a new primitive attribute (optionally with a default value)
    template <class AttrT>
    attribute<AttrT> make_attribute(AttrT const& def_value = AttrT()) const;
    template <class AttrT>
    [[deprecated("use make_attribute instead")]] attribute<AttrT> make_attribute_with_default(AttrT const& def_value) const;
    /// Creates a new primitive attribute and copies the given data
    template <class AttrT>
    attribute<AttrT> make_attribute_from_data(std::vector<AttrT> const& data) const;
    /// Creates a new primitive attribute and copies the given data
    template <class AttrT>
    attribute<AttrT> make_attribute_from_data(AttrT const* data, int cnt) const;
    /// same as make_attribute(f, def_value)
    template <class FuncT, class AttrT = tmp::decayed_result_type_of<FuncT, handle>>
    attribute<AttrT> map(FuncT&& f, AttrT const& def_value = AttrT()) const;

    // Iteration:
    iterator begin() const;
    end_iterator end() const { return {}; }

    /// returns a handle chosen uniformly at random
    /// Complexity is usually O(1)
    /// NOTE: when only valid handles are allowed, this will use rejection sampling
    ///       and thus get very slow if a lot of data is invalid
    template <class Generator>
    handle random(Generator& g) const;

    /// converts the given integer index into a handle
    /// CAUTION: always includes primitives marked for deletion, this is the numerical value of the handle
    handle operator[](int idx) const;
    handle operator[](index idx) const;

protected:
    /// Backreference to mesh
    mesh_ptr m;

    friend class Mesh;

public:
    /// returns reference to mesh
    std::remove_pointer_t<mesh_ptr>& mesh() const { return *m; }
};

/// Collection of all vertices of a mesh
/// Basically a smart std::vector
template <class iterator>
struct vertex_collection : smart_collection<Mesh*, vertex_tag, iterator>
{
    /// Adds a new vertex and returns its handle
    /// Does NOT invalidate any iterator!
    vertex_handle add() const;

    /// Collapsed the given vertex by removing it and merging the adjacent faces
    /// Preserves half-edge properties but not face ones
    void collapse(vertex_handle v) const;

    /// Removes a vertex (and all adjacent faces and edges)
    /// (marks them as removed, compactify mesh to actually remove them)
    void remove(vertex_handle v) const;

    /// applies an index remapping to all vertex indices
    /// p[curr_idx] = new_idx
    /// NOTE: invalidates all affected handles/iterators
    void permute(std::vector<int> const& p) const;
};

/// Collection of all faces of a mesh
/// Basically a smart std::vector
template <class iterator>
struct face_collection : smart_collection<Mesh*, face_tag, iterator>
{
    /// Adds a face consisting of N vertices
    /// The vertices must already be sorted in CCW order
    /// (note: trying to add already existing halfedges triggers assertions)
    template <size_t N>
    face_handle add(const vertex_handle (&v_handles)[N]) const;
    face_handle add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const;
    face_handle add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const;
    face_handle add(std::vector<vertex_handle> const& v_handles) const;
    face_handle add(std::vector<vertex_index> const& v_indices) const;
    face_handle add(vertex_handle const* v_handles, int vcnt) const;
    face_handle add(vertex_index const* v_indices, int vcnt) const;
    template <size_t N>
    face_handle add(const halfedge_handle (&half_loop)[N]) const;
    face_handle add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const;
    face_handle add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const;
    face_handle add(std::vector<halfedge_handle> const& half_loop) const;
    face_handle add(std::vector<halfedge_index> const& half_loop) const;
    face_handle add(halfedge_handle const* half_loop, int vcnt) const;
    face_handle add(halfedge_index const* half_loop, int vcnt) const;

    /// Returns true if face can be added
    template <size_t N>
    bool can_add(const vertex_handle (&v_handles)[N]) const;
    bool can_add(vertex_handle v0, vertex_handle v1, vertex_handle v2) const;
    bool can_add(vertex_handle v0, vertex_handle v1, vertex_handle v2, vertex_handle v3) const;
    bool can_add(std::vector<vertex_handle> const& v_handles) const;
    bool can_add(std::vector<vertex_index> const& v_indices) const;
    bool can_add(vertex_handle const* v_handles, int vcnt) const;
    bool can_add(vertex_index const* v_indices, int vcnt) const;
    template <size_t N>
    bool can_add(const halfedge_handle (&half_loop)[N]) const;
    bool can_add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2) const;
    bool can_add(halfedge_handle h0, halfedge_handle h1, halfedge_handle h2, halfedge_handle h3) const;
    bool can_add(std::vector<halfedge_handle> const& half_loop) const;
    bool can_add(std::vector<halfedge_index> const& half_loop) const;
    bool can_add(halfedge_handle const* half_loop, int vcnt) const;
    bool can_add(halfedge_index const* half_loop, int vcnt) const;

    /// Splits a face by inserting a vertex (which is returned) and creating triangles towards it
    /// Preserves half-edge attributes
    /// The face itself is deleted and multiple new ones are created
    vertex_handle split(face_handle f) const;
    /// same as before but splits at a given ISOLATED vertex
    void split(face_handle f, vertex_handle v) const;

    /// Cuts a face topologically into two parts
    /// Inserts an edge between h0.vertex_to() and h1.vertex_to()
    /// Creates a new face as well
    /// The returned halfedge is of the new edge and goes from h0.vertex_to() to h1.vertex_to()
    /// (and thus points towards f)
    halfedge_handle cut(face_handle f, halfedge_handle h0, halfedge_handle h1) const;

    /// Fills the half-edge ring of a given boundary half-edge
    /// Returns the new face
    face_handle fill(halfedge_handle h) const;

    /// Removes a face (adjacent edges and vertices are NOT removed)
    /// (marks it as removed, compactify mesh to actually remove it)
    void remove(face_handle f) const;

    /// applies an index remapping to all face indices
    /// p[curr_idx] = new_idx
    /// NOTE: invalidates all affected handles/iterators
    void permute(std::vector<int> const& p) const;
};

/// Collection of all edges of a mesh
/// Basically a smart std::vector
template <class iterator>
struct edge_collection : smart_collection<Mesh*, edge_tag, iterator>
{
    /// Adds an edge between two existing, distinct vertices
    /// if edge already exists, returns it
    edge_handle add_or_get(vertex_handle v_from, vertex_handle v_to) const;
    /// Adds an edge between two existing, distinct halfedges
    /// if edge already exists, returns it
    edge_handle add_or_get(halfedge_handle h_from, halfedge_handle h_to) const;

    /// Returns the edge handle between two vertices (invalid if not found)
    /// O(valence) computation
    [[deprecated("use pm::edge_between instead")]] edge_handle find(vertex_handle v_from, vertex_handle v_to) const;
    /// Returns true iff an edge exists between the vertices
    /// O(valence) computation
    [[deprecated("use pm::are_adjacent instead")]] bool exists(vertex_handle v_from, vertex_handle v_to) const;

    /// Splits this edge in half by inserting a vertex (which is returned)
    /// Preserves face attributes
    /// The edge itself is deleted and two new ones are created
    vertex_handle split(edge_handle e) const;
    /// same as before but splits at a given ISOLATED vertex
    void split(edge_handle e, vertex_handle v) const;

    /// Splits this edge in half by inserting a vertex (which is returned)
    /// All adjacent faces are triangulated
    /// Works on boundaries
    /// NOTE:
    ///   - on triangle meshes, this corresponds to a 2-4 split
    ///   - is currently only implemented for triangles
    vertex_handle split_and_triangulate(edge_handle e) const;
    /// same as before but splits at a given ISOLATED vertex
    void split_and_triangulate(edge_handle e, vertex_handle v) const;

    /// Moves both half-edges vertices to their next half-edge vertex
    /// Equivalent to an edge flip if both faces are triangular
    /// Preserves all attributes
    /// NOTE: does not work on boundaries!
    /// TODO: image
    void rotate_next(edge_handle e) const;
    /// Same as rotate_next but with the previous half-edge
    void rotate_prev(edge_handle e) const;

    /// performs an edge flip
    /// NOTE: does not work on boundaries
    /// NOTE: only works if both faces are triangles
    /// Preserves all attributes
    void flip(edge_handle e) const;

    /// Removes an edge (and both adjacent faces, vertices are NOT removed)
    /// (marks them as removed, compactify mesh to actually remove them)
    void remove(edge_handle e) const;

    /// applies an index remapping to all edge indices
    /// p[curr_idx] = new_idx
    /// NOTE: invalidates all affected handles/iterators
    void permute(std::vector<int> const& p) const;
};

/// Collection of all half-edges of a mesh
/// Basically a smart std::vector
template <class iterator>
struct halfedge_collection : smart_collection<Mesh*, halfedge_tag, iterator>
{
    /// Adds an half-edge between two existing, distinct vertices
    /// if half-edge already exists, returns it
    /// (always adds opposite half-edge as well)
    halfedge_handle add_or_get(vertex_handle v_from, vertex_handle v_to) const;
    /// Adds an half-edge between two existing, distinct hafledges
    /// if half-edge already exists, returns it
    /// (always adds opposite half-edge as well)
    halfedge_handle add_or_get(halfedge_handle h_from, halfedge_handle h_to) const;

    /// Returns the half-edge handle between two vertices (invalid if not found)
    /// O(valence) computation
    [[deprecated("use pm::halfedge_from_to instead")]] halfedge_handle find(vertex_handle v_from, vertex_handle v_to) const;
    /// Returns true iff an edge exists between the vertices
    /// O(valence) computation
    [[deprecated("use pm::are_adjacent instead")]] bool exists(vertex_handle v_from, vertex_handle v_to) const;

    /// Collapsed the given half-edge by removing it, keeping the to_vertex
    /// Preserves half-edge properties but not face or vertex ones
    /// This function never creates, only deletes faces, edges, vertices (depending on case)
    /// NOTE: this should work for all topologies as long as the result is manifold
    ///       can be checked with pm::can_collapse(h)
    void collapse(halfedge_handle h) const;

    /// Splits this half-edge in half by inserting a vertex (which is returned)
    /// Preserves face attributes
    /// Contrary to edges().split, the edge is preserved and a single new one is inserted AFTER h
    /// (thus h->next() is the newly inserted edge and h->vertex_to() is the returned vertex)
    vertex_handle split(halfedge_handle h) const;
    /// same as before but splits at a given ISOLATED vertex
    void split(halfedge_handle h, vertex_handle v) const;

    /// Given an isolated vertex v, inserts a self-adjacent edge at the to-vertex to v
    void attach(halfedge_handle h, vertex_handle v) const;

    /// Merges the vertex_from into the vertex_to of this halfedge.
    /// From_vertex must have valence 2
    void merge(halfedge_handle h) const;

    /// Moves the to-vertex of this half-edge to the same as the next half-edge
    /// Preserves all attributes
    /// NOTE: does not work on boundaries!
    /// NOTE: does not work on triangles!
    /// TODO: image
    void rotate_next(halfedge_handle h) const;
    /// Same as rotate_next but with the previous half-edge
    void rotate_prev(halfedge_handle h) const;

    /// Removes the edge and both half-edges belonging to it (and both adjacent faces, vertices are NOT removed)
    /// (marks them as removed, compactify mesh to actually remove them)
    void remove_edge(halfedge_handle h) const;
};

// vertices

struct all_vertex_collection : vertex_collection<primitive<vertex_tag>::all_iterator>
{
};

struct all_vertex_const_collection : smart_collection<Mesh const*, vertex_tag, primitive<vertex_tag>::all_iterator>
{
};

struct valid_vertex_collection : vertex_collection<primitive<vertex_tag>::valid_iterator>
{
};

struct valid_vertex_const_collection : smart_collection<Mesh const*, vertex_tag, primitive<vertex_tag>::valid_iterator>
{
};

// faces

struct all_face_collection : face_collection<primitive<face_tag>::all_iterator>
{
};

struct all_face_const_collection : smart_collection<Mesh const*, face_tag, primitive<face_tag>::all_iterator>
{
};

struct valid_face_collection : face_collection<primitive<face_tag>::valid_iterator>
{
};

struct valid_face_const_collection : smart_collection<Mesh const*, face_tag, primitive<face_tag>::valid_iterator>
{
};

// edges

struct all_edge_collection : edge_collection<primitive<edge_tag>::all_iterator>
{
};

struct all_edge_const_collection : smart_collection<Mesh const*, edge_tag, primitive<edge_tag>::all_iterator>
{
};

struct valid_edge_collection : edge_collection<primitive<edge_tag>::valid_iterator>
{
};

struct valid_edge_const_collection : smart_collection<Mesh const*, edge_tag, primitive<edge_tag>::valid_iterator>
{
};

// half-edges

struct all_halfedge_collection : halfedge_collection<primitive<halfedge_tag>::all_iterator>
{
};

struct all_halfedge_const_collection : smart_collection<Mesh const*, halfedge_tag, primitive<halfedge_tag>::all_iterator>
{
};

struct valid_halfedge_collection : halfedge_collection<primitive<halfedge_tag>::valid_iterator>
{
};

struct valid_halfedge_const_collection : smart_collection<Mesh const*, halfedge_tag, primitive<halfedge_tag>::valid_iterator>
{
};

// ================= RINGS =================

template <class this_t, class tag>
struct primitive_ring : smart_range<this_t, typename primitive<tag>::handle>
{
    using handle = typename primitive<tag>::handle;

    face_handle face;

    /// Number of elements
    /// O(result) computation
    int size() const;
    /// Returns true if handle is contained in this ring
    bool contains(handle v) const;
};

template <class tag, class circulator>
struct face_primitive_ring : primitive_ring<face_primitive_ring<tag, circulator>, tag>
{
    face_handle face;
    face_primitive_ring(face_handle f) { face = f; }

    // Iteration:
    circulator begin() const { return {face.any_halfedge()}; }
    end_iterator end() const { return {}; }
};

template <class tag, class circulator>
struct vertex_primitive_ring : primitive_ring<vertex_primitive_ring<tag, circulator>, tag>
{
    vertex_handle vertex;
    vertex_primitive_ring(vertex_handle v) { vertex = v; }

    // Iteration:
    circulator begin() const { return {vertex.any_outgoing_halfedge(), !vertex.is_isolated()}; }
    end_iterator end() const { return {}; }
};

template <class tag, class circulator>
struct halfedge_primitive_ring : primitive_ring<halfedge_primitive_ring<tag, circulator>, tag>
{
    halfedge_handle halfedge;
    halfedge_primitive_ring(halfedge_handle h) { halfedge = h; }

    // Iteration:
    circulator begin() const { return {halfedge}; }
    end_iterator end() const { return {}; }
};

/// all vertices belonging to a face
struct face_vertex_ring : face_primitive_ring<vertex_tag, face_vertex_circulator>
{
    using face_primitive_ring<vertex_tag, face_vertex_circulator>::face_primitive_ring;
};

/// all halfedges belonging to a face
struct face_halfedge_ring : face_primitive_ring<halfedge_tag, face_halfedge_circulator>
{
    using face_primitive_ring<halfedge_tag, face_halfedge_circulator>::face_primitive_ring;
};

/// all edges belonging to a face
struct face_edge_ring : face_primitive_ring<edge_tag, face_edge_circulator>
{
    using face_primitive_ring<edge_tag, face_edge_circulator>::face_primitive_ring;
};

/// all adjacent faces belonging to a face (DOES NOT INCLUDE invalid ones for boundaries)
struct face_face_ring : face_primitive_ring<face_tag, face_face_circulator>
{
    using face_primitive_ring<face_tag, face_face_circulator>::face_primitive_ring;
};

/// all adjacent faces belonging to a face (INCLUDES invalid ones for boundaries)
struct face_all_face_ring : face_primitive_ring<face_tag, face_all_face_circulator>
{
    using face_primitive_ring<face_tag, face_all_face_circulator>::face_primitive_ring;
};

/// all outgoing half-edges from a vertex
struct vertex_halfedge_out_ring : vertex_primitive_ring<halfedge_tag, vertex_halfedge_out_circulator>
{
    using vertex_primitive_ring<halfedge_tag, vertex_halfedge_out_circulator>::vertex_primitive_ring;
};

/// all incoming half-edges from a vertex
struct vertex_halfedge_in_ring : vertex_primitive_ring<halfedge_tag, vertex_halfedge_in_circulator>
{
    using vertex_primitive_ring<halfedge_tag, vertex_halfedge_in_circulator>::vertex_primitive_ring;
};

/// all adjacent vertices of a vertex
struct vertex_vertex_ring : vertex_primitive_ring<vertex_tag, vertex_vertex_circulator>
{
    using vertex_primitive_ring<vertex_tag, vertex_vertex_circulator>::vertex_primitive_ring;
};

/// all adjacent edges of a vertex
struct vertex_edge_ring : vertex_primitive_ring<edge_tag, vertex_edge_circulator>
{
    using vertex_primitive_ring<edge_tag, vertex_edge_circulator>::vertex_primitive_ring;
};

/// all adjacent faces of a vertex (DOES NOT INCLUDE invalid ones for boundaries)
struct vertex_face_ring : vertex_primitive_ring<face_tag, vertex_face_circulator>
{
    using vertex_primitive_ring<face_tag, vertex_face_circulator>::vertex_primitive_ring;
};

/// all adjacent faces of a vertex (INCLUDES invalid ones for boundaries)
struct vertex_all_face_ring : vertex_primitive_ring<face_tag, vertex_all_face_circulator>
{
    using vertex_primitive_ring<face_tag, vertex_all_face_circulator>::vertex_primitive_ring;
};

/// all half-edges along a ring (next -> next -> ...)
struct halfedge_ring : halfedge_primitive_ring<halfedge_tag, halfedge_ring_circulator>
{
    using halfedge_primitive_ring<halfedge_tag, halfedge_ring_circulator>::halfedge_primitive_ring;
};
}
