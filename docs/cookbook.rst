Polymesh Cookbook
=================

This chapter collects various simple recipes and samples that answer common "How do I do XYZ?" questions.
`Typed Geometry <https://graphics.rwth-aachen.de:9000/ptrettner/typed-geometry>`_ is used as math library.

.. note:: Many examples are already given in the :doc:`algorithms` and  :doc:`objects` chapters.


Loading a Mesh from a File
--------------------------

::

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();

    load("/path/to/file.ext", m, pos);


Laplacian Smoothing with Feature Edges
--------------------------------------

TODO


Extracting Edges as Segments
----------------------------

::

    // given:
    // pm::Mesh m;
    // pm::vertex_attribute<tg::pos3> pos;
    
    // functional style
    auto edges = m.edges().map([&](pm::edge_handle e) {
            return tg::segment3(pos[e.vertexA()], pos[e.vertexB()]);
        }).to_vector();
    
    // or
    
    // classical style
    std::vector<tg::segment3> edges;
    edges.reserve(m.edges().size());
    for (auto e : m.edges())
        edges.emplace_back(pos[e.vertexA()], pos[e.vertexB()]);


Add Triangle Faces to Mesh
--------------------------

::

    // given:
    // std::vector<tg::triangle3> tris;

    pm::Mesh m;
    auto pos = pm::vertex_attribute<tg::pos3>(m);
    
    for (auto const& tri : tris)
    {
        /// add vertices to topology
        const auto vh0 = m.vertices().add();
        const auto vh1 = m.vertices().add();
        const auto vh2 = m.vertices().add();

        /// add vertex positions
        pos[vh0] = tri.pos0;
        pos[vh1] = tri.pos1;
        pos[vh2] = tri.pos2;

        /// add face to topology
        m.faces().add(vh0, vh1, vh2);
    }
    
    // note that this will generate a triangle soup only. to obtain a sound mesh, deduplicate it
    // triangles that share a certain position will then be properly connected
    pm::deduplicate(m, pos);
    m.compactify();
