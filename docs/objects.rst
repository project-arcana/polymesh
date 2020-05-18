Objects
=======

The headers located in ``polymesh/objects/*`` or included by ``polymesh/objects.hh`` contain built-in objects that can be added to a polymesh.
As these objects may not be piecewise linear, they tend to include tessellation factors.
Many objects generated quads, not triangles.
See :ref:`algo-triangulation` if you need a triangle mesh.

A reference of all built-in objects can be found in :ref:`objects-ref`.

.. note:: the collection of objects still has many missing / improvable parts. :doc:`contributing` is heavily welcome.


Quad
----

A regularly subdivided quad.

::

    #include <polymesh/objects/quad.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();

    // creates a 32x32 quad from xz coordinates -1..1
    pm::objects::add_cube(m, [&](pm::vertex_handle v, float x, float y) {
        pos[v] = {x * 2 - 1, 0, y * 2 - 1};
    }, 32, 32);

.. doxygenfunction:: polymesh::objects::add_quad


Cube
----

A simple cube consisting of 6 quads.

::

    #include <polymesh/objects/cube.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();

    // create cube from -1..1
    pm::objects::add_cube(m, [&](pm::vertex_handle v, float x, float y, float z) {
        pos[v] = tg::pos3(x, y, z) * 2 - 1;
    });

.. doxygenfunction:: polymesh::objects::add_cube(Mesh&, CubeF&&)

.. doxygenfunction:: polymesh::objects::add_cube(Mesh&, vertex_attribute<Pos3>&)


Sphere
------

A UV Sphere consisting of rings of quads and two rings of triangles (for top and bottom).

::

    #include <polymesh/objects/uv_sphere.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();

    // create a unit sphere (using 32 rings in each dir)
    pm::objects::add_uv_sphere(m, [&](pm::vertex_handle v, float x, float y) {
        auto [sx, cx] = tg::sin_cos(360_deg * x);
        auto [sy, cy] = tg::sin_cos(180_deg * y);
        pos[v] = {
            sy * cx,
            cy,
            sy * sx
        };
    }, 32, 32);

.. doxygenfunction:: polymesh::objects::add_uv_sphere


Cylinder
--------

A regularly subdivided cylinder, optionally with caps.

::

    #include <polymesh/objects/cylinder.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();

    // create a unit cylinder (using 32 segments, with caps)
    pm::objects::add_cylinder(m, [&](pm::vertex_handle v, float x, float y) {
        auto [sx, cx] = tg::sin_cos(360_deg * x);
        pos[v] = {sx, y * 2 - 1, cx};
    }, 32, true);

.. doxygenfunction:: polymesh::objects::add_cylinder


Cone
----

A regularly subdivided cone, optionally with a base cap.

::

    #include <polymesh/objects/cone.hh>

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();

    // create a unit cone (using 32 segments, with base cap)
    pm::objects::add_cone(m, [&](pm::vertex_handle v, float x, float y) {
        auto [sx, cx] = tg::sin_cos(360_deg * x);
        pos[v] = {sx, y, cx};
    }, 32, true);

.. doxygenfunction:: polymesh::objects::add_cone


.. todo:: add graphical examples of the objects