Attributes
==========

:class:`polymesh::Mesh` stores pure topology, no positions, no normals, no other attributes.
All data associated with mesh primitives are stored in so-called external attributes.

Motivating example: ::

    // create empty mesh
    pm::Mesh m;

    // create position as external vertex attribute
    auto pos = pm::vertex_attribute<tg::pos3>(m);

    // create a vertex (pos is automatically resized as well)
    auto v = m.vertices().add();

    // assign position
    pos[v] = {1, 2, 3};

    // compute face normals (functional style)
    auto f_normals = m.faces().map([&](pm::face_handle f) { return pm::face_normal(f, pos); };

    // compute vertex normals (imperative style)
    auto v_normals = m.vertices().make_attribute<tg::vec3>();
    for (auto v : m.vertices())
    {
        auto n = v.faces().sum(f_normals);
        v_normals[v] = normalize(n);
    }

    // compute bounding box
    auto [min, max] = pos.aabb();


External Attributes
-------------------

Polymesh stores in separate objects with value semantics:

* :cpp:struct:`polymesh::vertex_attribute\<T>` stores a ``T`` per vertex
* :cpp:struct:`polymesh::face_attribute\<T>` stores a ``T`` per face
* :cpp:struct:`polymesh::edge_attribute\<T>` stores a ``T`` per edge
* :cpp:struct:`polymesh::halfedge_attribute\<T>` stores a ``T`` per halfedge

These types behave similar to a ``std::vector<T>``, i.e. moving is cheap and copy is a deep copy.
Each attribute stores a reference to the mesh which can be retrieved via :cpp:func:`polymesh::primitive_attribute_base::mesh`.
While a ``std::vector<T>`` can be accessed via ``int`` (or ``size_t``), polymesh attributes can only be accessed via the corresponding primitive ``_handle`` or ``_index`` (e.g. ``my_vertex_attribute[v]`` for :cpp:struct:`polymesh::vertex_handle` ``v``).
The only exception is an edge attribute which can also be accessed via :cpp:struct:`polymesh::halfedge_handle` or :cpp:struct:`polymesh::halfedge_index` because each halfedge belongs to exactly one edge.

If attributes are accessed via handles, a ``POLYMESH_ASSERT`` checks that the handle belongs to the same mesh as the attribute.
This is a security feature and can be circumvented by casting to the appropriate ``_index``.

Usually, attributes are created via their corresponding :ref:`primitive-collection`, e.g. ``m.vertices().make_attribute<int>()`` creates an ``int``-valued attribute for vertices.
Alternatively, the constructors can be used directly, e.g. ``auto pos = pm::vertex_attribute<tg::pos3>(m);``.

Attributes have default values (defaults to ``T{}``) that can be set when creating the attribute.
These are not only the value of all preexisting primitives but also the values of newly created primitives.

All attributes are :doc:`smart-ranges`.
In contrast to the normal topological ranges (such as :func:`polymesh::Mesh::vertices`), attributes do not know which primitives are deleted and thus iterating over an attribute will iterate over *all* values, even those belonging to primitives that are marked for deletion.
This is done for performance reasons.
If only values belonging to valid primitives are desired, either :func:`polymesh::Mesh::compactify` the mesh or iterate over the primitive and use the handle to access the value.

Attributes not only overload ``operator[]`` for their primitive index and handle but also ``operator()``.
Thus, each attributes is also a function object that can map primitives to their stored value.
This makes attributes easy-to-use in a functional context.
For example, ``m.vertices().avg(pos)`` computes the average vertex position.


Integrating Mesh and Attributes
-------------------------------

Sometimes, it is useful to have a class that represents a mesh with a set of default attributes.
An easy way to achieve this is to derive from :class:`polymesh::Mesh` and declare the attributes as members: ::

    class MyMesh : public pm::Mesh
    {
        pm::vertex_attribute<tg::pos3> pos{*this};
        pm::vertex_attribute<tg::dir3> normal{*this};
        pm::halfedge_attribute<tg::pos2> texCoords{*this};
    };

Note that attributes should be initialized with ``{*this}`` to attach them to the mesh.


Advanced Attributes
-------------------

Flags
^^^^^

TODO

Partitionings
^^^^^^^^^^^^^

TODO

Sparse Attributes
^^^^^^^^^^^^^^^^^

TODO


Views
-----

TODO
