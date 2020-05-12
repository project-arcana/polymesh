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


Delaunay
--------

An incomplete collection of algorithms for Delaunay triangulations.

::

    #include <polymesh/algorithms/delaunay.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();
    load(...);

    // makes mesh surface delaunay via flipping
    pm::make_delaunay(m, pos);

.. doxygenfunction:: polymesh::make_delaunay

There is also a 2D version that starts with a vertex-only mesh and creates faces via 2D Delaunay:

.. doxygenfunction:: polymesh::create_delaunay_triangulation


Edge Split
----------

A generic edge splitting routine.

::

    #include <polymesh/algorithms/edge_split.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();
    load(...);

    auto const target_edge_length = 0.1f;
 
    // split all edges longer than 0.1 in descending length order
    pm::split_edges_trimesh(m,
        // function to provide a priority value and signal if the edge should not be split
        [&](pm::edge_handle e) -> tg::optional<float> {
            auto const l = pm::edge_length(e, pos);
            if (l < target_edge_length)
                return {};
            return l;
        },
        // a function performing the split
        [&](pm::vertex_handle v, pm::halfedge_handle, pm::vertex_handle v_from, pm::vertex_handle v_to) {
            pos[v] = mix(pos[v_to], pos[v_from], 0.5f);
            // .. and propagate other attributes if desire
        });

For polygonal meshes, splitting edges can either be very simple (only insert a vertex) or complex (re-triangulate polygons, ensure user constraints).
The following function provides a generic interface:

.. doxygenfunction:: polymesh::split_edges

If the mesh is known to be triangular (and should stay triangular), then the split is well defined and the split function only has to fill in missing attributes:

.. doxygenfunction:: polymesh::split_edges_trimesh

These functions guarantee that meshes stay *compact* (see :ref:`memory-model`) and are, in general, quite fast.


Normal Estimation
-----------------

A simple estimation algorithm for computing smoothed vertex normals.
Edges that should not be smoothed over can be declared in a generic interface.

::

    #include <polymesh/algorithms/normal_estimation.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();
    load(...);

    auto fnormals = pm::face_normals(pos);

    // compute smooth vertex normals without smoothing over more than 60° edges
    auto vnormals = pm::normal_estimation(fnormals, [&](pm::edge_handle e) {
        return dot(fnormals[e.faceA()], fnormals[e.faceB()]) < 0.5;
    });

    // .. or just smooth over everything
    auto vnormals2 = pm::normal_estimation(pos, [](auto) { return false; });

This function has two versions, one based on face normals, the other on vertex positions (which internally computes face normals):

.. doxygenfunction:: polymesh::normal_estimation(face_attribute<Vec3> const&, IsHardEdgeF&&)

.. doxygenfunction:: polymesh::normal_estimation(vertex_attribute<Pos3> const&, IsHardEdgeF&&)


Normalization
-------------

An incomplete collection of helpers for ensuring different normalization constraints.

::

    #include <polymesh/algorithms/normalize.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();
    load(...);

    // translates and uniformly rescales the mesh
    // so it fits in the [-1, 1] cube and is centered at the origin
    pm::normalize(pos);

.. doxygenfunction:: polymesh::normalize


Cache Optimization
------------------

An incomplete collection of algorithms that reorder the internal memory layout to improve cache coherence in different scenarios.

::

    #include <polymesh/algorithms/cache-optimization.hh>

    pm::Mesh m;
    load(...);

    // reorder memory layout for improved performance when performing vertex-traversal algorithms
    pm::optimize_for_vertex_traversal(m);


.. doxygenfunction:: polymesh::optimize_for_face_traversal

.. doxygenfunction:: polymesh::optimize_for_vertex_traversal



TODO: preserve line breaks in doxygen

TODO: decimate, subdivision, interpolation, iteration, sampling, smoothing, stats, topology, tracing, triangulate
