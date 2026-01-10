.. _algorithms-index:

======================
Optimization Algorithms
======================

This section provides detailed documentation of the optimization algorithms
implemented in CLEED for LEED-IV structural refinement, including mathematical
formulations, pseudocode, and parameter guidance.

.. contents:: On this page
   :local:
   :depth: 2

Introduction
============

LEED-IV structural analysis requires optimizing atomic positions to minimize
the discrepancy (R-factor) between experimental and calculated I(V) curves.
CLEED implements several optimization algorithms suitable for this multi-dimensional,
non-convex optimization problem.

Algorithm Selection Guide
=========================

.. list-table:: Algorithm Selection Criteria
   :header-rows: 1
   :widths: 20 20 20 40

   * - Scenario
     - Recommended
     - Alternative
     - Notes
   * - Initial exploration
     - DE, PSO
     - SA
     - Global methods explore parameter space
   * - Near-optimal refinement
     - Simplex
     - Powell
     - Local methods converge quickly
   * - Few parameters (<10)
     - Simplex
     - Powell, DE
     - Low dimensional problems
   * - Many parameters (>20)
     - DE, PSO
     - SA
     - Population methods scale better
   * - Noisy objective
     - DE, SA
     - PSO
     - Stochastic methods handle noise

Algorithm Documentation
=======================

.. toctree::
   :maxdepth: 2

   differential_evolution
   particle_swarm
   simplex
   powell
   simulated_annealing


Common Concepts
===============

Objective Function
------------------

All algorithms minimize the R-factor, which quantifies the agreement between
experimental and theoretical LEED I(V) curves. The Pendry R-factor
:cite:`Pendry1980` is commonly used:

.. math::

   R_P = \frac{\int (Y_{exp} - Y_{th})^2 \, dE}{\int (Y_{exp}^2 + Y_{th}^2) \, dE}

where :math:`Y = L^{-1} dL/dE` is the logarithmic derivative of the intensity,
and :math:`L = I / (1 + V_i^2 I^{-2})` includes the imaginary part of the
inner potential.

Parameter Bounds
----------------

All optimizers respect box constraints on parameters:

.. math::

   x_i^{min} \leq x_i \leq x_i^{max} \quad \forall i \in \{1, \ldots, n\}

Convergence Criteria
--------------------

Optimization terminates when any of these conditions is met:

1. R-factor falls below target threshold
2. Maximum iterations/evaluations reached
3. Parameter changes fall below tolerance
4. R-factor improvement stagnates


References
==========

.. bibliography::
   :filter: docname in docnames
