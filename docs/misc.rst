Misc
====

Assertions
----------


Simple Graphs
-------------

While polymesh is primarily a mesh data structure it can also be used to represent simple graphs, i.e. undirected graphs without self loops and without multi-edges.
Half-edge data structures cannot represent most non-manifold geometry but that limitations only applies if faces are added.
A "wireframe mesh" (mesh without faces) can be of arbitrary topology as long as edges are unique (no multi-edge) and start and end vertex are different (no self loops).
