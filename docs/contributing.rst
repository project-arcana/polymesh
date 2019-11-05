Contributing
============

Function parameter order guide
------------------------------

The following types of parameters exist:

* M (``Mesh const& m`` or ``Mesh& m``) reference to mesh (depending on topology is modified)
* RO-A (``attribute<T> const&``) read-only attributes such as position
* RW-A (``attribute<T>&``) read-write attributes
* OPT-A (``attribute<T>* = nullptr`` or ``attribute<T> const* = nullptr``) optional attributes
* H (``handle h``) handle
* M-P: other mandatory parameters
* OPT-P: other optional parameters
* OUT-P: output parameters (that don't fit in the return value)

Free functions that perform mesh-related algorithms follow these rules for their parameters:

#. Mesh (M) or handle (H) is first parameter ("what does this function operate on?")
#. Followed by all required RO-A such as ``position``
#. Followed by all mandatory parameters M-P
#. Followed by all additional outputs RW-A and OUT-P
#. Finalized by all optional parameters OPT-A and OPT-P (least frequently used parameter should be last)