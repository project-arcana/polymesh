# Changelog

## Breaking changes

* heavyweight `std` includes are removed
    * `<memory>`
    * `<vector>`
    * ...
* `Mesh::copy` and `Mesh::create` now work on `pm::unique_ptr`
* `properties.hh` was renamed to `view.hh`
* `algorithms/properties.hh` was moved to `properties.hh`
* deprecated `make_attribute_with_default` and moved functionality to `make_attribute`
