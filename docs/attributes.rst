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


External Attributes
-------------------


Integrating Mesh and Attributes
-------------------------------

TODO: by deriving


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
