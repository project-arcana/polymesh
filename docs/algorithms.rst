Algorithms
==========

The headers located in ``polymesh/algorithms/*`` or included by ``polymesh/algorithms.hh`` contain useful algorithms operating on meshes.
In contrast to the :doc:`properties` or the functional-inspired :doc:`smart-ranges`, these algorithms are often non-trivial and perform substantial transformations on the mesh.
As most built-in geometry-related methods, the algorithms tend to be very generic, providing extensive customization points and using the :doc:`vector-math` wrapper to support many different math types.

A reference of all algorithms can be found in :ref:`algorithms-ref`.

Components
----------

This category of algorithms contains methods to compute connected components on meshes.

::

    #include <polymesh/algorithms/components.hh>

    pm::Mesh m;
    load(m, /* ... */);

    // computes all vertex components of the mesh
    int vcomp_count;
    auto vcomp = pm::vertex_components(m, vcomp_count);
    for (auto v : m.vertices())
        std::cout << "vertex " << int(v) << " belongs to component " << vcomp[v] << " of " << vcomp_count << std::endl;

    // special iterator to iterate over a single component
    pm::face_handle f = /* ... */;
    for (auto ff : pm::face_component(f))
        std::cout << "face " << int(ff) << " is in the same connected component as face " << int(f) << std::endl;


.. doxygenfunction:: polymesh::vertex_components

.. doxygenfunction:: polymesh::face_components

.. doxygenfunction:: polymesh::vertex_component

.. doxygenfunction:: polymesh::face_component(face_handle)

.. doxygenfunction:: polymesh::face_component(vertex_handle)


Deduplicate
-----------

A small helper that merges vertices based on a user criterion.

::

    #include <polymesh/algorithms/deduplicate.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();
    load("some-file.stl", m, pos);

    // merges all vertices with the same position
    pm::deduplicate(m, pos);

.. doxygenfunction:: polymesh::deduplicate

TODO: preserve line breaks in doxygen

TODO: decimate, dedup, delaunay, edge_split, subdivision, interpolation, iteration, normal-estimation, normalize, operations, optimizations, sampling, smoothing, stats, topology, tracing, triangulate
