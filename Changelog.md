# Changelog

## Breaking changes

* heavyweight `std` includes are removed
    * `<memory>`
    * `<vector>`
    * ...
* `Mesh::copy` and `Mesh::create` now work on `pm::unique_ptr`
