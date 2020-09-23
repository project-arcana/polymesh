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


Namespaces
----------

In its implementation, polymesh uses the ``polymesh::`` namespace.
However, long namespace are cumbersome and by default ``pm::`` is provided as an alias.
All examples will use the short version.


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


Philosophy
----------

Polymesh provides a mesh data structure (:class:`polymesh::Mesh`) that contains and manages the mesh topology in a half-edge data structure (see :doc:`mesh-topology`).

A mesh has no positions or normals, no attributes in general.
Instead, attributes are separate value types that behave like a ``std::vector`` but in a type-safe manner with automatic resize on topology change (see :doc:`attributes`).

Algorithms are kept as generic as possible, most are templated on the type of the position.
Polymesh is designed to work with a broad class of math libraries (see :doc:`vector-math` and :doc:`algorithms`).

Except for some core functionality, :doc:`algorithms` and :doc:`properties` are implemented as free functions.
This makes it easy to extend and favors an include-what-you-use approach, reducing compile times in the process.

Working with meshes and attributes involves a lot of iteration.
Often, aggregate statistics like averages, minimums, maximums, and mapped/filtered ranges are needed.
Thus, polymesh provides a clean, composable, and powerful range API (see :doc:`mesh-topology` and :doc:`smart-ranges`).

Polymesh is performance-oriented but still concerned with usability and safety.
C++ Exceptions are not used but many assertions will trigger in Debug or RelWithDebInfo builds if the API is used wrongly.


Header Structure
----------------

A quick guide for which ``#include <...>`` directives are commonly needed:

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

``polymesh/properties.hh``
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

:doc:`mesh-topology`
    Describes how mesh topology is stored in the :class:`polymesh::Mesh` class, how primitives are accessed via handles and indices, how topological iteration works, how memory is managed, and how the low-level API can be used to manipulate the internal half-edge data structure.

:doc:`attributes`
    Introduces the "external, smart attributes" design and more advanced topics like flags, partitionings, sparse attributes, and views.

:doc:`vector-math`
    Polymesh does not provide its own math library but assumes an external one is used.
    This section describes which math libraries are recommended and tested and what requirements must be met if a custom math library is to be used.

:doc:`smart-ranges`
    Geometry processing involves a lot of iteration and polymesh supports this with a clean, functional, and powerful "smart range" approach.

:doc:`properties`
    Many topological and geometrical properties like edge lengths, valences, areas, etc. are free functions usable in a composable and generic manner.

:doc:`algorithms`
    Polymesh is mostly "batteries included" and provides many important algorithms and basic operations like edge splits, decimation, smoothing, triangulation, subdivision, and many more.

:doc:`serialization`
    Saving and loading meshes is supported for several popular file formats.
    There are also type-erased ways to store arbitrary attributes.

:doc:`objects`
    Sometimes, meshes are not loaded from file but created from primitives like cubes, spheres, cylinders, etc.
    Many primitive objects are supported out-of-the-box.

:doc:`misc`
    Polymesh provides some miscellaneous support code like custom high-performance assertions or a ``span`` type.

:doc:`cookbook`
    The "Polymesh Cookbook" contains several recipes for common problems/operations in a "how do I do XYZ?"-fashion.

:doc:`faq`
    A collection of common questions or misconceptions.

:doc:`reference`
    Class reference generated mainly by Doxygen that documents noteworthy individual functions and classes.

:doc:`contributing`
    A small guide of code style, philosophy, and tips should anyone want to contribute to polymesh (which we heavily welcome!).
