Misc
====

Assertions
----------

::

    #include <polymesh/assert.hh>

    POLYMESH_ASSERT(1 + 2 == 3);

These assertions are a slightly improved version of the `C++ Assertions <https://en.cppreference.com/w/cpp/error/assert>`_.
By default, they are available in ``Debug`` and in ``Release with Debug Info`` modes, only disabled in a pure ``Release``.
Using ``unlikely`` and ``cold`` functions, each assertion has minimal impact on the non-error code path.
The only overhead is evaluating the condition and a single, perfectly predicted conditional branch.
The assembly for the "assertion failed" path is generated outside of the usual function code.

Behavior on assertion failure can be customized using:

.. doxygenfunction:: polymesh::set_assertion_handler



.. _simple-graphs-ref:

Simple Graphs
-------------

While polymesh is primarily a mesh data structure it can also be used to represent simple graphs, i.e. undirected graphs without self loops and without multi-edges.
Half-edge data structures cannot represent most non-manifold geometry but that limitations only applies if faces are added.
A "wireframe mesh" (mesh without faces) can be of arbitrary topology as long as edges are unique (no multi-edge) and start and end vertex are different (no self loops).
