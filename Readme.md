# PolyMesh

A lightweight half-edge data structure.

Best used with glm and glow.


## Function parameter order guide

The following types of parameters exist:

* M (`Mesh const& m` or `Mesh& m`) reference to mesh (depending on topology is modified)
* RO-A (`attribute<T> const&`) read-only attributes such as position
* RW-A (`attribute<T>&`) read-write attributes
* OPT-A (`attribute<T>* = nullptr` or `attribute<T> const* = nullptr`) optional attributes
* H (`handle h`) handle
* M-P: other mandatory parameters
* OPT-P: other optional parameters
* OUT-P: output parameters (that don't fit in the return value)

Free functions that perform mesh-related algorithms follow these rules for their parameters:

* Mesh (M) or handle (H) is first parameter ("what does this function operates on?")
* Followed by all required RO-A such as `position`
* Followed by all mandatory parameters M-P
* Followed by all additional outputs RW-A and OUT-P
* Finalized by all optional parameters OPT-A and OPT-P (least frequently used parameter should be last)


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