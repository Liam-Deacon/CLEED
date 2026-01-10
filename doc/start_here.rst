.. _start_here:

**************************
Start here: LEED-IV in CLEED
**************************

This guide is a short, practical introduction for new LEED-IV users. It
summarizes the modeling loop and gives an end-to-end example you can run from
``examples/``.

What is LEED-IV?
================

Low Energy Electron Diffraction (LEED) compares experimental intensity-versus-
energy curves against theoretical simulations. The goal is to adjust the
surface structure until the theoretical I(V) curves match experiment within a
reliability metric. For background, see :cite:p:`vanhove1984` and the Pendry
R-factor reference :cite:p:`pendry1980`.

Program loop
============

The core CLEED loop is:

1. **csearch**: selects candidate geometries.
2. **cleed_***: computes theoretical I(V) curves.
3. **crfac**: evaluates an R-factor for the current geometry.

This is repeated until convergence.

Inputs you need
===============

- ``*.bul``: bulk geometry and fixed parameters.
- ``*.inp``: surface geometry and search parameters.
- ``*.ctr``: experimental vs theoretical curve mapping.
- ``*.phs``: phase shift files generated per element.

See :ref:`file_formats` for details.

End-to-end example (fixed geometry)
===================================

This example runs a single theoretical calculation and an R-factor evaluation
using the shipped Ni(111) + 2x2 O example:

.. code-block:: console

   cd examples/models/nio
   export CLEED_PHASE="$(pwd)/phase"
   phsh.py -g -b Ni111_2x2O.bul -i Ni111_2x2O.inp -f CLEED
   cleed_nsym -c Ni111_2x2O.ctr -i Ni111_2x2O.inp -b Ni111_2x2O.bul
   crfac -c Ni111_2x2O.ctr -t Ni111_2x2O.res

If you are running a full structure search, ``csearch`` can orchestrate the
loop when provided matching ``.inp/.bul/.ctr`` files.
