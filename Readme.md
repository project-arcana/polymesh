# PolyMesh

A lightweight half-edge data structure.

Best used with glm and glow.

## TODO

* Properties
* Algorithms
* Tests
* std::less and std::hash for _index (and maybe _handle)
* Debug: store compactify generation in handles to check for invalidation
* Debug: insert is_removed assertions into handle access
* Test self-adjacent faces
* smart ranges: filter, map
* vector, set, map -> range
* opposite edges (from vertex)
* cotangents weights etc.
* smoothing
* _copy versions of topological operations that copy attributes
* vertex split?
* half-edge collapse
* normal, tangent, bitangent computation
* primitive sort functions, better remap function, cache optimization
* lowlevel API that allows direct half-edge manipulation and does not fix boundaries (but also mirrors high level one)
* primitive collection sort and sort_by functions
* paired_with function for smart range
* operator +-*/ for attributes (similar to valarray)
* dual mesh construction
* cast<>, reinterpret<> function
* surface tracing