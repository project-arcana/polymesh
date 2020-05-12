Mesh and Topology
=================

Motivating example: ::

    // creates an empty mesh
    pm::Mesh m;

    // add some vertices
    pm::vertex_handle v0 = m.vertices().add();
    pm::vertex_handle v1 = m.vertices().add();
    pm::vertex_handle v2 = m.vertices().add();

    // add a face
    pm::face_handle f = m.faces().add(v0, v1, v2);

    // iterate over edges
    for (pm::edge_handle e : m.edges())
        std::cout << "v" << int(e.vertexA()) << " -> v" << int(e.vertexB()) << std::endl;

    // split face
    pm::vertex_handle v = m.faces().split(f);

    // navigate the mesh
    v = v.any_outgoing_halfedge().next().vertex_to();


Mesh Class
-------------

The core data structure of polymesh is :class:`polymesh::Mesh`, a half-edge data structure for managing mesh topology.

For an introduction to this type of data structure please refer to:

* https://en.wikipedia.org/wiki/Doubly_connected_edge_list
* http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
* https://www.openmesh.org/media/Documentations/OpenMesh-Doc-Latest/a03930.html

The Mesh only contains pure topology, no positions, no normals.
:doc:`attributes` are externally stored and behave like value types.
Because attributes and handles have a pointer to the mesh they belong to, a :class:`polymesh::Mesh` is non-copyable and non-movable.
Functions returning a mesh with new ownership should use ``std::unique_ptr<pm::Mesh>`` or ``std::shared_ptr<pm::Mesh>``.

:func:`polymesh::Mesh::create` is a static helper function to create a ``unique_ptr<Mesh>`` though the typical way to just use ``pm::Mesh`` as a member or as a local variable.

.. _memory-model:

Memory Model
------------

High-performance is one of the primary goals of polymesh and a cache-friendly contiguous data layout is often key to good performance.
Polymesh stores its topology in 6 arrays of ints: halfedge per face, outgoing halfedge per vertex, and vertex, face, next/prev halfedge per halfedge.
Primitives (vertices, edges, faces, halfedges) are strongly typed indices that point into these arrays (see next subsection).

These arrays behave like a ``std::vector`` with size and capacity, using exponential reallocation for amortized O(1) to add a new primitive.
Deleting primitives does not invalidate any other handle or index nor does it move data.
The primitive is simply marked as "deleted" and is basically a hole in the array.
Iterating over primitives ignores deleted ones by default.
The function :func:`polymesh::Mesh::compactify()` can be used to make the mesh "compact" again, i.e. permuting all primitives such that no holes are left.
This invalidates handles.

:doc:`attributes` mirror the memory layout of their respective primitive and are thus also affected by ``compactify()``.


Handles and Indices
-------------------

Each primitive has a handle and an index version:

* :class:`polymesh::vertex_index` and :class:`polymesh::vertex_handle`
* :class:`polymesh::face_index` and :class:`polymesh::face_handle`
* :class:`polymesh::edge_index` and :class:`polymesh::edge_handle`
* :class:`polymesh::halfedge_index` and :class:`polymesh::halfedge_handle`

Roughly spoken, an index is a strongly typed integer representing the index of a primitive in a :class:`polymesh::Mesh`.
Handles are a combination of pointer to mesh and index such that topological operations like getting the face belonging to a halfedge can be done on the handle.

Because handles contain a pointer to the mesh, they consume more memory than their corresponding index.
As a rule of thumb, if primitives are stored on the heap (e.g. in an ``std::vector`` or an ``std::set``) it makes sense to store the index.
For single primitives or local variables, handles are safer and more comfortable.

Handles and indices can be explicitly cast to integer (``int(my_handle)``) to get the index.
Invalid indices and handles can be created (e.g. ``vertex_index::invalid``), which correspond to index ``-1``.

Indices can be converted to handles.
For example, given ``Mesh m`` and ``vertex_index v`` (or as ``int i``), the following are handles (and equivalent): ``v.of(m)``, ``m[v]``, ``m.vertices()[i]``.

Handles and indices can be "indexed" via subscript by attributes and functors (taking an index or handle, returning some type).
For example, given a ``vertex_attribute<tg::pos3> pos`` and a ``vertex_handle v``, the following are equivalent and return the position of the vertex: ``pos[v]`` and ``v[pos]``.
(This can be either read as "index the position attribute by v" or "return the position of v")

Handles are "smart" in the sense that they can be directly used to iterate over mesh topology and query certain topological properties.
For example, given a ``face_handle f``, ``f.vertices()`` returns an iterable smart range containing all vertices of this face.
For all operations, see class references for :class:`polymesh::vertex_handle`, :class:`polymesh::face_handle`, :class:`polymesh::edge_handle`, and :class:`polymesh::halfedge_handle`.
For more information about the ranges, see :doc:`smart-ranges`.

.. _primitive-collection:

Primitive Collections
---------------------

Handles can be used to locally navigate the topology of a mesh.
Primitive collections are used for global topology navigation and for changing the topology.
Given ``Mesh m``, each primitive has its own collection:

* :class:`polymesh::vertex_collection` via ``m.vertices()``
* :class:`polymesh::face_collection` via ``m.faces()``
* :class:`polymesh::edge_collection` via ``m.edges()``
* :class:`polymesh::halfedge_collection` via ``m.halfedges()``

By default, iteration over these collections skips deleted primitives.
This is an additional check that can affect performance slightly.
If deleted primitives should be returned as well (or it is known that no deleted primitives exists, e.g. if :func:`polymesh::Mesh::is_compact` is true), then the ``all_<primitive>`` variants such as ``m.all_vertices()`` can be used.

Topological operations such as adding faces, removing vertices, or splitting edges are all performed via their respective primitive collections.
E.g. ``m.faces().add(v0, v1, v2)`` adds a new triangle.

Example: ::

    pm::Mesh m;
    auto pos = pm::vertex_attribute<tg::pos3>(m);

    for (auto v : m.vertices())
        if (should_remove(v))
            m.vertices().remove(v);

    tg::rng rng;
    auto f = m.faces().random(rng);

    auto centroid = f.vertices().avg(pos);
    auto v = m.faces().split(f);
    pos[v] = centroid;


Low-Level API
-------------

Primitive collections and handles are a kind of "high-level API".
They perform many security checks, handle special cases, and sometimes have to do extra work to preserve some useful invariants (like the face-to-halfedge mapping always pointing to a boundary if the face lies on a boundary to have a super fast :func:`polymesh::face_handle::is_boundary` test).

Sometimes, algorithms want to manipulate the half-edge data structure directly and bypass the high-level API.
This can be done by using the helper function :func:`polymesh::low_level_api` which returns a simple wrapper object that can be used for manipulating the internals of a :class:`polymesh::Mesh`.

The API is split into two parts, :class:`polymesh::low_level_api_base` for queries (const Mesh) and :class:`polymesh::low_level_api_mutable` for modifications (mutable Mesh).

It goes without saying that using this API means leaving the hand-holding of the high-level API and there are no guarantees for what happens when the mesh is left in an inconsistent state.

Example: ::

    pm::Mesh m;
    auto ll = low_level_api(m);
    ll.to_vertex_of(m.halfedges()[0]) = m.vertices[](7);


Copying a Mesh
--------------

Handles and attributes (which are external) refer to a mesh by reference, which makes it nontrivial to copy a mesh with all attributes.
For security reasons, handles to one mesh cannot be used to index into a different mesh unless the handle is cast into an index beforehand.

To make copying a mesh less tedious a few helpers exist:

* :func:`polymesh::Mesh::copy_from` clears a mesh and copies over the topology of another mesh
* :func:`polymesh::Mesh::copy` creates a new ``unique_ptr<Mesh>`` and copies over the topology
* ``attribute::copy_from(attribute const&)`` copies attribute data from a different mesh
* ``attribute::copy_to(Mesh const&)`` copies attribute data to a different mesh
* :func:`polymesh::copy` creates a mesh and an arbitrary number of attributes and creates copies of them all (returned as tuple)

Example: ::

    // must be included explicitly because 
    // it needs the relatively expensive header <tuple>
    #include <polymesh/copy.hh>

    pm::Mesh m;
    auto pos = pm::vertex_attribute<tg::pos3>(m);

    auto [m2, pos2] = pm::copy(m, pos);
