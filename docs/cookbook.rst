Polymesh Cookbook
=================


Loading a mesh from a file
--------------------------

::

    pm::Mesh m;
    auto pos = m.vertices().make_attribute<tg::pos3>();

    load("/path/to/file.ext", m, pos);
