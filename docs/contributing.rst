Contributing
============

How to Build the Documentation
------------------------------

* navigate to ``docs/``
* optionally execute ``pip install -r requirements.txt`` to install required dependencies
* execute ``doxygen`` to generate from-source part of the documentation
* execute ``make html`` to build the Sphinx docs
* open ``docs/_build/index.html``

Types of Contribution
---------------------

* Any typo fix is heavily welcome. It does not matter if the PR changes only a single character.
* Updates to the documentation, especially the FAQ or the Cookbook are always welcome. If you are unsure about the scope, style, or anything: just open an issue first to discuss what should be changed.
* Extending existing formats or adding a new format. Be sure to read :doc:`serialization` and add your format to the generic load/save in ``formats.cc``.
* Adding new algorithms or changing core aspects of polymesh should probably be discussed in an issue before but we are always happy to extend the library.

Function Parameter Order Guide
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