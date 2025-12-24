.. _bulk_file:

**************************
``*.bul`` (bulk input)
**************************

Purpose
=======

``.bul`` files define bulk geometry and non-geometric parameters that remain
fixed during a search. They also carry LEED calculation parameters such as
energy ranges and optical potentials.

Structure
=========

A typical ``.bul`` contains:

- Bulk lattice vectors (``a1``, ``a2``, ``a3``).
- Optional superstructure vectors (``b1``, ``b2``) or matrix (``m1``, ``m2``).
- Bulk atom positions (``pb`` lines) with optional thermal displacement data.
- Optical potentials (``vr``, ``vi``).
- LEED calculation parameters (``ei``, ``ef``, ``es``, ``it``, ``ip``, ``ep``, ``lm``).

Keyword reference (used in shipped examples)
============================================

.. list-table::
   :header-rows: 1
   :widths: 12 48 20

   * - Key
     - Meaning
     - Units / notes
   * - ``c``
     - Comment or label line.
     - Free text.
   * - ``a1`` / ``a2`` / ``a3``
     - Bulk lattice vectors (x y z).
     - Angstrom; ``a3`` should point into the crystal.
   * - ``b1`` / ``b2``
     - Superstructure unit-cell vectors.
     - Angstrom; optional.
   * - ``m1`` / ``m2``
     - Superstructure matrix rows.
     - Dimensionless.
   * - ``pb``
     - Bulk atom line: ``pb: <type> x y z [dr1|dr3|nd3|dmt] ...``
     - Angstrom; displacement tokens described below.
   * - ``vr`` / ``vi``
     - Optical potential (real and imaginary parts).
     - eV; ``vr`` should be negative, ``vi`` positive.
   * - ``ei`` / ``ef`` / ``es``
     - Energy range (initial, final, step).
     - eV.
   * - ``it`` / ``ip``
     - Polar and azimuthal incidence angles.
     - Degrees.
   * - ``ep``
     - Wavefunction convergence criterion.
     - Dimensionless; default 1e-4.
   * - ``lm``
     - Maximum l quantum number.
     - Integer; if omitted, CLEED computes a value.
   * - ``ve``
     - Exponent for imaginary optical potential.
     - Float; optional.

Displacement tokens for ``pb``
==============================

``pb`` lines accept optional displacement specifiers after the coordinates:

- ``dr1 <rms>``: isotropic root-mean-square displacement.
- ``dr3 <dx> <dy> <dz>``: directional RMS displacements (diagonal t-matrix).
- ``nd3 <dx> <dy> <dz>``: directional RMS displacements (non-diagonal t-matrix).
- ``dmt <theta_D> <mass> [temperature]``: Debye temperature, atomic mass, and
  optional temperature.

Example (from ``examples/models/nicu/Ni111_Cu.bul``)
===================================================

.. literalinclude:: ../../examples/models/nicu/Ni111_Cu.bul
   :language: none
   :linenos:

Minimal template
================

.. code-block:: none

   c: Example bulk
   a1: 1.000 0.000 0.000
   a2: 0.000 1.000 0.000
   a3: 0.000 0.000 -2.000
   m1: 1. 0.
   m2: 0. 1.
   vr: -8.0
   vi: 4.0
   pb: X 0.000 0.000 0.000 dr1 0.050
   ei: 50.0
   ef: 300.0
   es: 5.0
   it: 0.0
   ip: 0.0
   ep: 1.e-2
   lm: 8
