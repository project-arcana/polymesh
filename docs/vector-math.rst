Vector Math
===========

A :class:`polymesh::Mesh` only represents topology (see :doc:`mesh-topology`) and has no knowledge of any embedding into 2D or 3D space.
Any data needed for such embeddings are stored in :doc:`attributes`.
However, :doc:`serialization` and many :doc:`algorithms` work on or with the geometric interpretation of a mesh.

Polymesh is designed to be largely agnostic of which math library is used.
We currently officially support:

* `Typed Geometry <https://graphics.rwth-aachen.de:9000/ptrettner/typed-geometry>`_ (recommended)
* `GLM <https://github.com/g-truc/glm>`_
* `Eigen <http://eigen.tuxfamily.org/>`_


Using Custom Math Types
-----------------------

:doc:`algorithms` are typically templated and written in a way that abstract over the actual math library.
Polymesh assumes that the type used for positions and vectors can be different (e.g. ``tg::pos3`` vs. ``tg::vec3`` in Typed Geometry) or the same (e.g. ``glm::vec3`` in GLM).
Your own math types should work as long as the following operations are provided:

* ``pos - pos -> vec``
* ``pos + vec -> pos``
* ``pos - vec -> pos``
* ``vec + vec -> vec``
* ``vec - vec -> vec``
* ``vec * scalar -> vec``
* ``vec / scalar -> vec``
* ``pos * scalar -> pos``
* ``pos / scalar -> pos``
* ``pos[int-literal] -> scalar``
* ``vec[int-literal] -> scalar``
* ``pos`` and ``vec`` default constructor
* ``pos``, ``vec``, and ``scalar`` behave like value types

(Algorithms should only use right multiplication with scalars. Violations should be reported as defects.)


Contributing Algorithms
-----------------------

Algorithms should be written such that they can be used with custom math types.
They must only rely on the previously mentioned operations.
The utility header ``fields.hh`` contains the type ``pm::field3<Pos3>`` which contains a few helper functions like ``dot``, ``cross``, and ``length``.

NOTE: this only concerns authors of new ``polymesh::`` algorithms.
