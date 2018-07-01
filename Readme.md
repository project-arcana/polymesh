# PolyMesh

A lightweight half-edge data structure.

Best used with glm and glow.

## TODO

* Properties
* Algorithms
* Tests
* std::less and std::hash for _index (and maybe _handle)
* attribute transformations (also between different types)
* lambda to attribute (from attribute to attribute or from make_attribute to attribute)
* Debug: store compactify generation in handles to check for invalidation
* Debug: insert is_removed assertions into handle access
* Switch primitives and valid_primitives, check if compact flag is inlined
* Test self-adjacent faces
* smart ranges: filter, map
* mid-level topo API: edge-rotate-next/prev, edge-split, edge-collapse, halfedge-split, halfedge-collapse, vertex-collapse
* annotate property preservation for mid-level topo API
* vector, set, map -> range
* opposite edges (from vertex)
* cotangens weights etc.
* smoothing