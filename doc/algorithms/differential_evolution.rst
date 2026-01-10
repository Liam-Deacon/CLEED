.. _algorithm-de:

======================
Differential Evolution
======================

Differential Evolution (DE) is a population-based stochastic optimization
algorithm introduced by Storn and Price :cite:`Storn1997`. It is particularly
effective for continuous optimization problems and is well-suited for
LEED-IV structural refinement.

.. contents:: On this page
   :local:
   :depth: 2

Introduction
============

DE is a global optimization algorithm that maintains a population of
candidate solutions and evolves them through mutation, crossover, and
selection operations. Unlike gradient-based methods, DE does not require
derivatives and can escape local minima through its stochastic nature.

**Advantages for LEED-IV:**

* No gradient information required
* Good global search capability
* Robust to noise in objective function
* Few control parameters

Mathematical Formulation
========================

Population Initialization
-------------------------

The initial population of :math:`NP` vectors is randomly generated within
the search bounds:

.. math::

   x_{i,j}^{(0)} = x_j^{min} + r_{i,j} \cdot (x_j^{max} - x_j^{min})

where :math:`r_{i,j} \sim U(0,1)` is a uniform random number.

Mutation
--------

For each target vector :math:`\mathbf{x}_i`, a mutant vector :math:`\mathbf{v}_i`
is generated. The classic DE/rand/1 strategy uses:

.. math::

   \mathbf{v}_i = \mathbf{x}_{r1} + F \cdot (\mathbf{x}_{r2} - \mathbf{x}_{r3})

where :math:`r1, r2, r3` are distinct random indices different from :math:`i`,
and :math:`F \in [0, 2]` is the differential weight (mutation factor).

Other mutation strategies include:

* **DE/best/1**: :math:`\mathbf{v}_i = \mathbf{x}_{best} + F \cdot (\mathbf{x}_{r1} - \mathbf{x}_{r2})`
* **DE/rand/2**: :math:`\mathbf{v}_i = \mathbf{x}_{r1} + F \cdot (\mathbf{x}_{r2} - \mathbf{x}_{r3}) + F \cdot (\mathbf{x}_{r4} - \mathbf{x}_{r5})`
* **DE/best/2**: :math:`\mathbf{v}_i = \mathbf{x}_{best} + F \cdot (\mathbf{x}_{r1} - \mathbf{x}_{r2}) + F \cdot (\mathbf{x}_{r3} - \mathbf{x}_{r4})`

Crossover
---------

The trial vector :math:`\mathbf{u}_i` is created by combining the target
and mutant vectors using binomial crossover:

.. math::

   u_{i,j} = \begin{cases}
   v_{i,j} & \text{if } r_j \leq CR \text{ or } j = j_{rand} \\
   x_{i,j} & \text{otherwise}
   \end{cases}

where :math:`CR \in [0, 1]` is the crossover probability, and :math:`j_{rand}`
ensures at least one component comes from the mutant vector.

Selection
---------

The trial vector replaces the target vector if it has equal or better fitness:

.. math::

   \mathbf{x}_i^{(g+1)} = \begin{cases}
   \mathbf{u}_i & \text{if } f(\mathbf{u}_i) \leq f(\mathbf{x}_i^{(g)}) \\
   \mathbf{x}_i^{(g)} & \text{otherwise}
   \end{cases}

Algorithm Pseudocode
====================

.. code-block:: text

   Algorithm: Differential Evolution
   Input: objective function f, bounds [lb, ub], NP, F, CR, max_gen
   Output: best solution x*, best fitness f*

   1. Initialize population P = {x_1, ..., x_NP} randomly in bounds
   2. Evaluate fitness f(x_i) for all i
   3. FOR g = 1 TO max_gen DO
   4.   FOR i = 1 TO NP DO
   5.     Select distinct random indices r1, r2, r3 != i
   6.     Generate mutant: v = x_r1 + F * (x_r2 - x_r3)
   7.     Apply bounds constraints to v
   8.     Generate trial u by crossover of x_i and v with rate CR
   9.     IF f(u) <= f(x_i) THEN
   10.      x_i = u
   11.    END IF
   12.  END FOR
   13.  Update best solution
   14.  Check convergence criteria
   15. END FOR
   16. RETURN best x*, f*

Parameter Configuration
=======================

.. list-table:: DE Parameters
   :header-rows: 1
   :widths: 20 15 15 50

   * - Parameter
     - Symbol
     - Default
     - Description
   * - Population size
     - NP
     - 10 * dim
     - Number of candidate solutions. Larger values improve exploration but increase computation.
   * - Differential weight
     - F
     - 0.8
     - Controls mutation step size. Range [0.4, 1.0] typically works well.
   * - Crossover rate
     - CR
     - 0.9
     - Probability of trial inheriting from mutant. Higher values increase exploration.
   * - Max generations
     - max_gen
     - 1000
     - Maximum number of generations before termination.

**Recommended Settings for LEED-IV:**

* For initial exploration: NP=50, F=0.8, CR=0.9
* For fine-tuning: NP=20, F=0.5, CR=0.7
* For noisy objectives: Increase NP, decrease F

Implementation Notes
====================

The CLEED implementation includes several enhancements:

1. **Bound handling**: Parameters violating bounds are reflected back
2. **Parallel evaluation**: Population members can be evaluated concurrently
3. **Adaptive strategies**: Optional self-adaptive F and CR
4. **Restart mechanism**: Population is reinitialized if stagnation detected

References
==========

.. bibliography::
   :filter: docname in docnames
