.. _file_format_conventions:

******************
Common Conventions
******************

This page describes conventions shared across ``.inp``, ``.bul``, and ``.ctr``
files in this repository.

File naming
===========

CLEED expects matching filename prefixes for the surface, bulk, and control
files. For example:

- ``Ni111_2x2O.inp``
- ``Ni111_2x2O.bul``
- ``Ni111_2x2O.ctr``

Comments and ordering
=====================

- Lines beginning with ``#`` are treated as comments.
- Parameters may appear in any order; if a parameter occurs multiple times,
  the last occurrence is used.

Units and coordinate frame
==========================

The shipped examples use the following conventions:

- Lengths are in Angstrom.
- Energies are in eV.
- Angles are in degrees.

CLEED converts many values internally (for example, bulk lattice vectors and
energies are converted to atomic units). Use the examples as a reference for
input units.

Lattice vectors
===============

``a1`` and ``a2`` define the in-plane surface lattice vectors. ``a3`` is the
out-of-plane vector, expected to point into the crystal for bulk inputs. CLEED
will reorder vectors if needed to enforce a right-handed system with ``a3``
pointing inward.

Superstructure matrix
=====================

The superstructure matrix ``m1`` and ``m2`` maps the surface cell (``a1``, ``a2``)
into a supercell. The transform is:

- ``b1 = m11 * a1 + m12 * a2``
- ``b2 = m21 * a1 + m22 * a2``

Atomic positions
================

Atomic position lines begin with ``pb:`` (bulk atoms) or ``po:`` (overlayer or
variable atoms). The common prefix is:

``<element_or_phase_label> x y z``

Optional displacement specifications can follow on the same line. See the
format-specific pages for details.

