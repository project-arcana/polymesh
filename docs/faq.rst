FAQ
===

.. topic:: How do I get an existing halfedge from one vertex to another?

    ::

        #include <polymesh/properties.hh>

        pm::halfedge_handle h = pm::halfedge_from_to(v_from, v_to);
