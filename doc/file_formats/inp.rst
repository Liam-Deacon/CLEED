.. _input_file:

****************************
``*.inp`` (search input)
****************************

Purpose
=======

``.inp`` files define the surface geometry and search constraints for
:ref:`csearch`. They may also be read by lattice utilities (for example, the
``latt`` program) when generating or inspecting surface cells.

Structure
=========

A typical ``.inp`` contains:

- Lattice vectors (``a1``, ``a2``, optional ``a3``).
- Optional superstructure matrix (``m1``, ``m2``) and overlayer cell vectors
  (``b1``, ``b2``).
- Variable atom positions (``po`` lines) with optional thermal displacement
  data.
- Search constraints and symmetry (``rm``, ``zr``, ``sz``, ``sr``, ``sm``).
- Optional R-factor parameters (``rft``, ``rfr``) for the search workflow.

Keyword reference (used in shipped examples)
============================================

.. list-table::
   :header-rows: 1
   :widths: 12 48 20

   * - Key
     - Meaning
     - Units / notes
   * - ``a1`` / ``a2``
     - In-plane lattice vectors (x y z). ``z`` may be omitted.
     - Angstrom; input for surface cell.
   * - ``a3``
     - Out-of-plane lattice vector.
     - Angstrom; optional in examples.
   * - ``m1`` / ``m2``
     - Superstructure matrix rows.
     - Dimensionless; see :ref:`file_format_conventions`.
   * - ``b1`` / ``b2``
     - Overlayer unit-cell vectors.
     - Angstrom; optional.
   * - ``po``
     - Variable atom line: ``po: <type> x y z [dr1|dr3|dmt] ...``
     - Angstrom; displacement tokens described below.
   * - ``rm``
     - Minimum radius per atom type: ``rm: <type> r_min``
     - Angstrom; used for geometry checks.
   * - ``zr``
     - Search bounds for z: ``zr: z_min z_max``
     - Angstrom.
   * - ``sz``
     - Search mode: ``1`` for z-only, ``0`` for xyz.
     - Integer flag.
   * - ``sr``
     - Rotation symmetry: ``sr: n x y``.
     - ``n`` is order; ``x y`` axis in surface plane.
   * - ``sm``
     - Mirror symmetry: ``sm: x_d y_d x_p y_p``.
     - Direction and point in the surface plane.
   * - ``sa``
     - Angle search on/off: ``sa: 1`` or ``sa: 0``.
     - Integer flag.
   * - ``spn``
     - Number of parameters for manual reference lists.
     - Integer; see ``spp``.
   * - ``spp``
     - Parameter reference list: ``spp: <atom> <x|y|z> <p1> <p2> ...``
     - Used when ``spn`` is set.
   * - ``rft``
     - R-factor type for the search workflow.
     - String token.
   * - ``rfr``
     - R-factor shift range.
     - Float.
   * - ``it`` / ``ip``
     - Polar and azimuthal incidence angles.
     - Degrees; optional.
   * - ``il``
     - Image length (``latt`` parser).
     - Integer; used by ``latt`` inputs.
   * - ``nl``
     - Max layers (``latt`` parser).
     - Integer; used by ``latt`` inputs.
   * - ``uc``
     - Max cells (``latt`` parser).
     - Integer; used by ``latt`` inputs.

Displacement tokens for ``po``
==============================

``po`` lines accept optional displacement specifiers after the coordinates:

- ``dr1 <rms>``: isotropic root-mean-square displacement.
- ``dr3 <dx> <dy> <dz>``: directional RMS displacements.
- ``dmt <theta_D> <mass> [temperature]``: Debye temperature, atomic mass, and
  optional temperature.

Example (from ``examples/models/nio/Ni111_2x2O.inp``)
====================================================

.. literalinclude:: ../../examples/models/nio/Ni111_2x2O.inp
   :language: none
   :linenos:

Minimal template
================

.. code-block:: none

   # surface input
   a1: 1.000 0.000 0.000
   a2: 0.000 1.000 0.000
   m1: 1. 0.
   m2: 0. 1.
   po: X 0.000 0.000 0.000 dr1 0.050
   rm: X 1.00
   zr: 1.00 6.00
   sz: 1
   sr: 3 0.0 0.0
