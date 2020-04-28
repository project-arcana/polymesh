Properties
==========

Properties are the commonly used free functions found in ``<polymesh/properties.hh>``.
They are helpers for answering simple topological questions or computing basic geometric properties.
When they use geometric :doc:`attributes`, they are written using generic :doc:`vector-math` to transparently support different math libraries.
The properties interact well with :doc:`smart-ranges` and are commonly used together.
Some properties transform complete attributes, like ``pm::vertex_normals_by_area`` which computes vertex normals for the whole mesh.

Property functions live in the ``polymesh::`` namespace and can thus often be found via `argument-dependent lookup <https://en.cppreference.com/w/cpp/language/adl>`_.
For example, ``valence(v)`` is the same as ``pm::valence(v)`` for ``pm::vertex_handle v``.

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

TODO


Geometric Properties
--------------------

TODO
