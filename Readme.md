# PolyMesh

A lightweight half-edge data structure.

Best used with glm and glow.

## TODO

* Properties
* Algorithms
* Tests
* std::less and std::hash for _index (and maybe _handle)
* attribute transformations (also between different types)
* Debug: store compactify generation in handles to check for invalidation
* Debug: insert is_removed assertions into handle access
* Switch primitives and valid_primitives, check if compact flag is inlined
* Test self-adjacent faces
* smart ranges: filter, map
* vector, set, map -> range
* opposite edges (from vertex)
* cotangens weights etc.
* smoothing
* make handle.<primitives>() contain only valid ones and provide an all_<primitives>() version
* _copy versions of topological operations that copy attributes
* vertex split?
* half-edge collapse