Class Reference
===============

Mesh
----

.. doxygenclass:: polymesh::Mesh
    :members:

.. _handles-ref:

Handles and Indices
-------------------

.. doxygenstruct:: polymesh::primitive_index
    :members:

.. doxygenstruct:: polymesh::face_index
    :members:

.. doxygenstruct:: polymesh::vertex_index
    :members:

.. doxygenstruct:: polymesh::edge_index
    :members:

.. doxygenstruct:: polymesh::halfedge_index
    :members:

.. doxygenstruct:: polymesh::primitive_handle
    :members:

.. doxygenstruct:: polymesh::face_handle
    :members:

.. doxygenstruct:: polymesh::vertex_handle
    :members:

.. doxygenstruct:: polymesh::edge_handle
    :members:

.. doxygenstruct:: polymesh::halfedge_handle
    :members:

.. _ranges-ref:

Ranges and Collections
----------------------

.. doxygenstruct:: polymesh::smart_range
    :members:

.. doxygenstruct:: polymesh::smart_collection
    :members:

.. doxygenstruct:: polymesh::face_collection
    :members:

.. doxygenstruct:: polymesh::vertex_collection
    :members:

.. doxygenstruct:: polymesh::edge_collection
    :members:

.. doxygenstruct:: polymesh::halfedge_collection
    :members:

Attributes
----------

.. doxygenstruct:: polymesh::vertex_attribute
    :members:

.. doxygenstruct:: polymesh::face_attribute
    :members:

.. doxygenstruct:: polymesh::edge_attribute
    :members:

.. doxygenstruct:: polymesh::halfedge_attribute
    :members:

.. doxygenstruct:: polymesh::primitive_attribute
    :members:

.. doxygenstruct:: polymesh::primitive_attribute_base
    :members:

.. _topological-properties-ref:

Topological Properties
----------------------

.. doxygenfunction:: polymesh::is_vertex_boundary

.. doxygenfunction:: polymesh::is_face_boundary

.. doxygenfunction:: polymesh::is_edge_boundary

.. doxygenfunction:: polymesh::is_halfedge_boundary

.. doxygenfunction:: polymesh::is_vertex_isolated

.. doxygenfunction:: polymesh::is_edge_isolated

.. doxygenfunction:: polymesh::valence

.. doxygenfunction:: polymesh::is_triangle

.. doxygenfunction:: polymesh::is_quad

.. doxygenfunction:: polymesh::edge_between

.. doxygenfunction:: polymesh::halfedge_from_to

.. doxygenfunction:: polymesh::are_adjacent

.. doxygenfunction:: polymesh::is_triangle_mesh

.. doxygenfunction:: polymesh::is_quad_mesh

.. doxygenfunction:: polymesh::euler_characteristic

.. doxygenfunction:: polymesh::is_closed_mesh

.. doxygenfunction:: polymesh::can_collapse

.. doxygenfunction:: polymesh::can_flip

.. doxygenfunction:: can_rotate_next(edge_handle)

.. doxygenfunction:: can_rotate_prev(edge_handle)

.. doxygenfunction:: can_rotate_next(halfedge_handle)

.. doxygenfunction:: can_rotate_prev(halfedge_handle)

.. doxygenfunction:: can_add_or_get_edge(vertex_handle, vertex_handle)

.. doxygenfunction:: can_add_or_get_edge(halfedge_handle, halfedge_handle)

.. _geometric-properties-ref:

Geometric Properties
--------------------

.. doxygenfunction:: polymesh::face_area

.. doxygenfunction:: polymesh::face_centroid

.. doxygenfunction:: polymesh::face_normal

.. doxygenfunction:: polymesh::triangle_area

.. doxygenfunction:: polymesh::triangle_centroid

.. doxygenfunction:: polymesh::triangle_normal

.. doxygenfunction:: polymesh::triangle_normal_unorm

.. doxygenfunction:: polymesh::bary_interpolate

.. doxygenfunction:: polymesh::barycoords_of

.. doxygenfunction:: polymesh::edge_length(edge_handle, vertex_attribute<Pos3> const&)

.. doxygenfunction:: polymesh::edge_length(halfedge_handle, vertex_attribute<Pos3> const&)

.. doxygenfunction:: polymesh::edge_vector

.. doxygenfunction:: polymesh::edge_dir

.. doxygenfunction:: polymesh::angle_to_next

.. doxygenfunction:: polymesh::angle_to_prev

.. doxygenfunction:: polymesh::angle_defect

.. doxygenfunction:: polymesh::cotan_weight

.. doxygenfunction:: polymesh::vertex_voronoi_areas

.. doxygenfunction:: polymesh::vertex_normals_uniform

.. doxygenfunction:: polymesh::vertex_normals_by_area

.. doxygenfunction:: polymesh::face_normals

.. doxygenfunction:: polymesh::triangle_normals

.. doxygenfunction:: polymesh::triangle_areas

.. doxygenfunction:: polymesh::cotan_weights

.. doxygenfunction:: polymesh::barycentric_coordinates

.. doxygenfunction:: polymesh::is_delaunay

.. doxygenfunction:: polymesh::can_collapse_without_flips

.. _algorithms-ref:

Algorithms
----------

.. doxygenfunction:: polymesh::vertex_components

.. doxygenfunction:: polymesh::face_components

.. doxygenfunction:: polymesh::vertex_component

.. doxygenfunction:: polymesh::face_component(face_handle)

.. doxygenfunction:: polymesh::face_component(vertex_handle)

.. doxygenfunction:: polymesh::deduplicate

.. doxygenfunction:: polymesh::make_delaunay

.. doxygenfunction:: polymesh::create_delaunay_triangulation

.. doxygenfunction:: polymesh::split_edges

.. doxygenfunction:: polymesh::split_edges_trimesh

.. doxygenfunction:: polymesh::normal_estimation(face_attribute<Vec3> const&, IsHardEdgeF&&)

.. doxygenfunction:: polymesh::normal_estimation(vertex_attribute<Pos3> const&, IsHardEdgeF&&)

.. doxygenfunction:: polymesh::normalize

.. doxygenfunction:: polymesh::remove_faces

.. doxygenfunction:: polymesh::remove_edges_and_faces

.. doxygenfunction:: polymesh::optimize_for_face_traversal

.. doxygenfunction:: polymesh::optimize_for_vertex_traversal

.. doxygenfunction:: polymesh::optimize_edges_for_faces

.. doxygenfunction:: polymesh::optimize_vertices_for_faces

.. doxygenfunction:: polymesh::optimize_edges_for_vertices

.. doxygenfunction:: polymesh::optimize_faces_for_vertices

.. doxygenfunction:: polymesh::cache_coherent_face_layout

.. doxygenfunction:: polymesh::cache_coherent_vertex_layout

.. doxygenfunction:: polymesh::triangulate_naive

.. doxygenfunction:: polymesh::farthest_face

.. doxygenfunction:: polymesh::smoothing_iteration

.. doxygenfunction:: polymesh::print_stats

.. doxygenfunction:: polymesh::decimate

.. doxygenfunction:: polymesh::decimate_down_to

.. doxygenfunction:: polymesh::decimate_up_to_error

.. _objects-ref:

Objects
-------

.. doxygenfunction:: polymesh::objects::add_quad

.. doxygenfunction:: polymesh::objects::add_cube(Mesh&, CubeF&&)

.. doxygenfunction:: polymesh::objects::add_cube(Mesh&, vertex_attribute<Pos3>&)

.. doxygenfunction:: polymesh::objects::add_uv_sphere

.. doxygenfunction:: polymesh::objects::add_cylinder

.. doxygenfunction:: polymesh::objects::add_cone



Low-Level API
-------------

.. doxygenstruct:: polymesh::low_level_api_base
    :members:

.. doxygenstruct:: polymesh::low_level_api_mutable
    :members:

Helper
------

.. doxygenfunction:: polymesh::copy

.. doxygenstruct:: polymesh::unique_ptr

.. doxygenstruct:: polymesh::unique_array


Internals
---------

.. doxygenstruct:: polymesh::vertex_tag

.. doxygenstruct:: polymesh::face_tag

.. doxygenstruct:: polymesh::edge_tag

.. doxygenstruct:: polymesh::halfedge_tag
