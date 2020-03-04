Polymesh Cookbook
=================


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
--------------------------

::

    // given:
    // pm::Mesh m;
    // pm::vertex_attribute<tg::pos3> pos;
    
    // functional style
    auto edges0 = m.edges().map([&](pm::edge_handle e) {
            return tg::segment3(pos[e.vertexA()], pos[e.vertexB()]);
        }).to_vector();
    
    // or
    
    // classical style
    std::vector<tg::segment3> edges;
    edges.reserve(m.edges().size());
    for (auto const& e : m.edges())
        edges.emplace_back(pos[e.vertexA()], pos[e.vertexB()]);


Add Triangle Faces to Mesh
--------------------------

::

    pm::Mesh m;
    auto pos = pm::vertex_attribute<tg::pos3>(m);
    
    const auto n = 10;
    std::vector<tg::triangle3> tris;
    tris.reserve(n);

    // some box to sample random positions
    const auto box = tg::box3(tg::aabb3({-n, -n, -n}, {n, n, n}));
    
    tg::rng rng;
    // create random triangles
    for (auto i = 0u; i < n; ++i)
        tris.emplace_back(tg::triangle3(tg::uniform(rng, box), tg::uniform(rng, box), tg::uniform(rng, box)));

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
