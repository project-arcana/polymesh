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
