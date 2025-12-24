.. _yaml_format:

*******************************
YAML alternative (cleedpy draft)
*******************************

Status
======

CLEED does not yet ship a YAML parser. The YAML format referenced in issue
`#36 <https://github.com/Liam-Deacon/CLEED/issues/36>`_ lives in the
``cleedpy`` ecosystem and can be used to generate ``.inp``/``.bul``/``.ctr``
inputs externally.

Mapping overview
================

The YAML mapping is intended to group the legacy formats into a single document
with three top-level sections:

.. list-table::
   :header-rows: 1
   :widths: 24 36 36

   * - YAML section (conceptual)
     - Legacy mapping
     - Notes
   * - ``bulk``
     - ``.bul``
     - Bulk lattice vectors, optical potentials, energy range, bulk atoms.
   * - ``surface``
     - ``.inp``
     - Surface lattice vectors, superstructure, variable atoms, search flags.
   * - ``control``
     - ``.ctr``
     - Experimental vs theoretical beam mapping and weights.

Until the YAML schema is finalized, refer to the ``cleedpy`` documentation and
issue `#36 <https://github.com/Liam-Deacon/CLEED/issues/36>`_ for the latest
field names and examples.
