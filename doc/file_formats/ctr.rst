.. _control_file:

***************************
``*.ctr`` (control file)
***************************

Purpose
=======

``.ctr`` files map experimental I(V) curves to theoretical beam indices for
R-factor evaluation by :ref:`crfac`. Each non-comment line defines one curve
comparison.

Format
======

Each line is a set of ``key=value`` segments separated by ``:``.

Supported keys:

- ``ef``: experimental input file path.
- ``ti``: theoretical beam indices. Multiple beams can be summed using ``+``.
- ``id``: group identifier (integer).
- ``wt``: relative weight in the R-factor calculation.
- ``e0``: energy of beam appearance (optional; if omitted, CLEED uses the first
  energy in the theoretical data).

Example (from ``examples/models/nicu/Ni111_Cu.ctr``)
===================================================

.. literalinclude:: ../../examples/models/nicu/Ni111_Cu.ctr
   :language: none
   :linenos:

Minimal template
================

.. code-block:: none

   # control file for crfac
   ef=expt.iv:ti=(1.00,0.00):id=01:wt=1.
