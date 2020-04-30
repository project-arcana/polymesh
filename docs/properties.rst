Properties
==========

Properties are the commonly used free functions found in ``<polymesh/properties.hh>``.
They are helpers for answering simple topological questions or computing basic geometric properties.
When they use geometric :doc:`attributes`, they are written using generic :doc:`vector-math` to transparently support different math libraries.
The properties interact well with :doc:`smart-ranges` and are commonly used together.
Some properties transform complete attributes, like ``pm::vertex_normals_by_area`` which computes vertex normals for the whole mesh.

Property functions live in the ``polymesh::`` namespace and can thus often be found via `argument-dependent lookup <https://en.cppreference.com/w/cpp/language/adl>`_.
For example, ``valence(v)`` is the same as ``pm::valence(v)`` for ``pm::vertex_handle v``.

All defined properties can be found in :ref:`topological-properties-ref`, :ref:`geometric-properties-ref`, or directly in ``<polymesh/properties.hh>``.

Motivating example: ::

    #include <polymesh/properties.hh>

    // load some mesh
    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();
    load("/path/to/some/file.obj", m, pos);

    // number of triangles
    auto tri_count = m.faces().count(pm::is_triangle);

    // remove isolated vertices
    for (auto v : m.vertices())
        if (is_isolated(v))
            m.vertices().remove(v);

    // compute total mesh area
    // (requires a lambda because face_area needs to know which position attribute to use)
    auto mesh_area = m.faces().sum([&](auto f) { return face_area(f, pos); });

    // get vertex normals with uniform weighting
    auto vnormals = vertex_normals_uniform(pos);


Topological Properties
----------------------

The topological are independent of the geometric interpretation of a mesh and thus also work for :ref:`simple-graphs-ref`.
Some of these properties are just free function versions of members available in :ref:`handles-ref` to make it slightly easier to use them with :doc:`smart-ranges`:

.. doxygenfunction:: polymesh::is_vertex_isolated

::

    // number of isolated vertices
    auto isolated_cnt = m.vertices().count(pm::is_vertex_isolated);

Other properties are helpful when checking if certain topological operations are allowed:

.. doxygenfunction:: polymesh::can_collapse

And some properties compute otherwise useful mesh information such as

.. doxygenfunction:: polymesh::euler_characteristic

All defined topological properties can be found in :ref:`topological-properties-ref` or directly in ``<polymesh/properties.hh>``.


Geometric Properties
--------------------

While topological properties can be computed directly ony a ``pm::Mesh``, geometric properties usually require at least one attribute, such as per-vertex positions.
These properties are written in a generic, sometimes peculiar way.
For example:

::

    template <class Pos3, class Scalar = typename field3<Pos3>::scalar_t>
    Scalar face_area(face_handle f, vertex_attribute<Pos3> const& position);

This ``pm::face_area`` function takes a ``face_handle`` and a 3D position attribute and compute the area of the polygonal face (assuming that it is planar).
The ``Scalar`` template parameter defaults to the scalar type of the position attribute but can be explicitly provided if the result is desired in a different type (e.g. a ``float`` based position but a ``double`` face area).

Similar to the topological ones, there are per-primitive properties, like 

.. doxygenfunction:: polymesh::edge_length(edge_handle, vertex_attribute<Pos3> const&)

and there are properties that compute new attributes for the whole mesh, like

.. doxygenfunction:: polymesh::cotan_weights

Polymesh tries to support (planar) polygons where possible, though some computations are considerably faster when specialized on triangles.
Generally, ``face_`` versions work on polygons while ``triangle_`` versions assume a triangular face.
For example 

.. doxygenfunction:: polymesh::face_centroid

versus

.. doxygenfunction:: polymesh::triangle_centroid

Finally, some properties answer geometry-related queries, like

.. doxygenfunction:: polymesh::is_delaunay

or

.. doxygenfunction:: polymesh::can_collapse_without_flips

All defined geometric properties can be found in :ref:`geometric-properties-ref` or directly in ``<polymesh/properties.hh>``.
