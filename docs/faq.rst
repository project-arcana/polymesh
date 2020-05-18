FAQ
===

.. topic:: How do I get an existing halfedge from one vertex to another?

    ::

        #include <polymesh/properties.hh>

        pm::halfedge_handle h = pm::halfedge_from_to(v_from, v_to);


.. topic:: How do I get the (numerical) index of a vertex / edge / face / halfedge?

    ::

        pm::vertex_handle v = ...;
        int idx = int(v); // explicit conversion


.. topic:: How do I get a handle given the (numerical) index?

    ::

        pm::Mesh m;
        int idx = ...;
        pm::vertex_handle v = m.vertices()[idx];
