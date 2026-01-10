.. _algorithm-powell:

================
Powell's Method
================

Powell's conjugate direction method :cite:`Powell1964` is a derivative-free
optimization algorithm that builds up a set of conjugate directions
through successive line searches.

.. contents:: On this page
   :local:
   :depth: 2

Introduction
============

Powell's method performs sequential one-dimensional minimizations along
a set of search directions. After each cycle, the directions are updated
to approximate conjugate directions, which accelerates convergence for
quadratic-like objective functions.

**Advantages for LEED-IV:**

* No derivatives required
* Fast convergence for smooth functions
* Efficient line search utilization
* Good for medium-dimensional problems

**Limitations:**

* Local optimizer
* Directions can become linearly dependent
* May fail on very rough objective functions

Mathematical Formulation
========================

Direction Set Method
--------------------

Starting from initial directions :math:`\mathbf{d}_1, \ldots, \mathbf{d}_n`
(typically the coordinate directions), the algorithm performs:

1. Sequential line minimizations along each direction
2. Update of the direction set to improve convergence

For iteration :math:`k`, starting at point :math:`\mathbf{x}_0^{(k)}`:

.. math::

   \mathbf{x}_i^{(k)} = \mathbf{x}_{i-1}^{(k)} + \lambda_i^* \mathbf{d}_i, \quad i = 1, \ldots, n

where :math:`\lambda_i^* = \arg\min_\lambda f(\mathbf{x}_{i-1}^{(k)} + \lambda \mathbf{d}_i)`.

Direction Update
----------------

After completing the cycle, a new direction is formed:

.. math::

   \mathbf{d}_{new} = \mathbf{x}_n^{(k)} - \mathbf{x}_0^{(k)}

The direction that produced the largest decrease is replaced by :math:`\mathbf{d}_{new}`.

Modified Powell's Method
------------------------

To prevent linear dependence of directions, the update rule checks:

.. math::

   f(\mathbf{x}_0) - 2f(\mathbf{x}_n) + f(\mathbf{x}_e) \geq \frac{1}{2} \Delta_{max}^2

where :math:`\mathbf{x}_e = 2\mathbf{x}_n - \mathbf{x}_0` is the extrapolated point
and :math:`\Delta_{max}` is the largest decrease in any direction.

Line Search
-----------

The line search finds the minimum along direction :math:`\mathbf{d}`:

.. math::

   \lambda^* = \arg\min_\lambda f(\mathbf{x} + \lambda \mathbf{d})

This is typically done using Brent's method or golden section search.

Algorithm Pseudocode
====================

.. code-block:: text

   Algorithm: Powell's Conjugate Direction Method
   Input: objective function f, initial point x0, tolerance
   Output: best solution x*, best fitness f*

   1. Initialize direction set D = {e_1, e_2, ..., e_n} (coordinate directions)
   2. x = x0
   3. REPEAT
   4.   x_start = x
   5.   delta_max = 0, i_max = 0
   6.   FOR i = 1 TO n DO
   7.     f_old = f(x)
   8.     Minimize f along direction d_i -> x = x + lambda* * d_i
   9.     delta = f_old - f(x)
   10.    IF delta > delta_max THEN
   11.      delta_max = delta
   12.      i_max = i
   13.    END IF
   14.  END FOR
   15.  d_new = x - x_start
   16.  x_e = 2*x - x_start (extrapolated point)
   17.  IF f(x_e) < f(x_start) AND meets update criterion THEN
   18.    Minimize f along d_new
   19.    Replace d_{i_max} with d_new
   20.  END IF
   21. UNTIL ||x - x_start|| < tolerance
   22. RETURN x, f(x)

Parameter Configuration
=======================

.. list-table:: Powell's Method Parameters
   :header-rows: 1
   :widths: 25 15 60

   * - Parameter
     - Default
     - Description
   * - Tolerance
     - 1e-6
     - Convergence tolerance on displacement
   * - Max iterations
     - 1000
     - Maximum number of outer iterations
   * - Line search tolerance
     - 1e-4
     - Precision of 1D line searches
   * - Reset frequency
     - n
     - Reset directions to coordinate axes every n cycles

Implementation Notes
====================

CLEED implementation details:

1. **Brent's line search**: Robust 1D minimization with parabolic interpolation
2. **Direction reset**: Periodic reset to prevent direction degeneracy
3. **Bound handling**: Truncated line searches at boundaries
4. **Scaling**: Automatic parameter scaling for numerical stability

References
==========

.. bibliography::
   :filter: docname in docnames
