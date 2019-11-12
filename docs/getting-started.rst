Getting Started
===============

Quickstart
----------

::

    // this header includes the most important parts of polymesh
    #include <polymesh/pm.hh>

    // polymesh is often used with a math library, like typed-geometry
    #include <typed-geometry/tg.hh>

    // a Mesh is purely topological, like a graph with vertices, edges, faces
    // polymesh is a half-edge data structure
    pm::Mesh m;

    // attributes are associated with a certain primitive type
    // they behave like a std::vector<T> but automatically resize on topology changes
    auto pos = pm::vertex_attribute<tg::pos3>(m);

    // non-core functionality is implemented as free functions
    // like loading from file
    pm::load("/path/to/mesh.ext", m, pos);

    // topological iteration is supported with a strong functional flavor
    // "handles" are "smart" and can be used to navigate the mesh
    auto smoothed_pos = pm::vertex_attribute<tg::pos3>(m);
    for (auto v : m.vertices())
        smoothed_pos[v] = v.adjacent_vertices().avg(pos);


CMake Integration
-----------------

Assuming that polymesh was cloned or copied into ``extern/polymesh``, the following CMake snippet will configure and set up the library.

.. code-block:: cmake

    # example math library
    add_subdirectory(extern/typed-geometry)

    # include polymesh CMakeLists.txt
    add_subdirectory(extern/polymesh)

    # make polymesh library available to MyProject
    target_link_libraries(MyProject PUBLIC polymesh)

A math library is optional but recommended (see :doc:`vector-math`).


Header Structure
----------------

``polymesh/fwd.hh``
    Forward declarations of all important types.

``polymesh/pm.hh``
    Most important types and functions, including meshes, attributes, IO, properties.

``polymesh/pm-std.hh``
    Same as ``/pm.hh`` but also includes useful interop with the C++ standard library. Individual interop is located in ``polymesh/std/*``.

``polymesh/Mesh.hh``
    The central data structure, ``pm::Mesh``. Due to dependencies also includes attributes.

``polymesh/formats.hh``
    Basic versions of all input/output formats.
    Individual formats are located in ``polymesh/formats/*``.

``polymesh/objects.hh``
    Helper functions for creating primitives like cubes and spheres.

``polymesh/algorithms.hh``
    Umbrella header for all built-in mesh algorithms. 
    Individual algorithms can be found in ``polymesh/algorithms/*``.

``polymesh/algorithms/properties.hh``
    Basic topological and geometrical properties like face area, valence, and angle defect.

``polymesh/objects.hh``
    Helper functions for creating primitives like cubes and spheres.
    Individual objects are located in ``polymesh/objects/*``.

``polymesh/ext/attribute-collection.hh``
    Type-erased collection of named attributes. Convenient for generic attribute save/load.

``polymesh/attributes/*``
    Special attribute types like flags and partitionings.


Documentation Structure
-----------------------
