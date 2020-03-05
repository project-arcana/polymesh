Smart Ranges
============

Many collections found in ``polymesh::`` derive from :cpp:struct:`polymesh::smart_range\<this_t, ElementT>`.
Iterating over these collections yields elements of type ``ElementT``.
The ``smart_range`` base class provides many convenience functions that encourage a functional programming style.
See :ref:`ranges-ref` for all supported operations.

Example: ::

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();
    // ... load or create mesh

    // get edge lengths as edge attribute
    auto edge_lengths = m.edges().map([&](pm::edge_handle e) { return distance(pos[e.vertexA()], pos[e.vertexB()]); });

    // sum up all edge lengths
    auto total_edge_length = edge_lengths.sum();

    // NOTE: this sums over deleted edges as well if the mesh is not compact
    //       alternatively only over valid edges:
    //       (this works because edge_lengths is also a function (pm::edge_handle) -> float)
    auto total_valid_edge_length = m.edges().sum(edge_lengths);

    // compute avg vertex position per face
    // (is only real center of gravity for triangles)
    auto face_cog = m.faces().map([&](pm::face_handle f) { return f.vertices().avg(pos); });

    // find vertex with maximum y coordinate
    auto max_y_v = m.vertices().max_by([&](pm::vertex_handle v) { return pos[v].y; });

    // iterate over all vertices with valence > 5
    for (auto v : m.vertices().where([&](auto v) { return valence(v) > 5; })
        pos[v] = ...;

    // iterate over faces and unpack triangle vertices
    POLYMESH_ASSERT(is_triangle_mesh(m));
    for (auto f : m.faces())
    {
        // get the three vertices
        auto [v0, v1, v2] = f.vertices().to_array<3>();

        // get the three vertex positions
        auto [p0, p1, p2] = f.vertices().to_array<3>(pos);
    }

:doc:`properties` are designed to interact well with this functional style.

Notable ``smart_range`` collections in polymesh include:

* all :ref:`primitive-collection` (e.g. ``m.vertices()``)
* all :doc:`attributes` (e.g. ``pm::face_attribute<T>``)
* all handle circulators (e.g. ``f.edges()`` for :struct:`polymesh::face_handle` ``f``)
