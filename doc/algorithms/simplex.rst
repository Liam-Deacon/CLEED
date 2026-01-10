.. _algorithm-simplex:

========================
Simplex (Nelder-Mead)
========================

The Nelder-Mead simplex algorithm :cite:`NelderMead1965` is a derivative-free
local optimization method that uses a geometric simplex to explore the
parameter space.

.. contents:: On this page
   :local:
   :depth: 2

Introduction
============

The simplex method maintains a simplex of :math:`n+1` vertices in
:math:`n`-dimensional space. The simplex adapts its shape through
reflection, expansion, contraction, and shrinkage operations to
move toward the optimum.

**Advantages for LEED-IV:**

* No derivatives required
* Very efficient for final refinement
* Simple and robust
* Low computational overhead

**Limitations:**

* Local optimizer (may get stuck in local minima)
* Performance degrades in high dimensions (>20 parameters)

Mathematical Formulation
========================

Simplex Operations
------------------

Let the simplex vertices be ordered by function value:
:math:`f(\mathbf{x}_1) \leq f(\mathbf{x}_2) \leq \cdots \leq f(\mathbf{x}_{n+1})`

The centroid of the best :math:`n` vertices is:

.. math::

   \bar{\mathbf{x}} = \frac{1}{n} \sum_{i=1}^{n} \mathbf{x}_i

**Reflection**: Reflect the worst vertex through the centroid:

.. math::

   \mathbf{x}_r = \bar{\mathbf{x}} + \alpha (\bar{\mathbf{x}} - \mathbf{x}_{n+1})

where :math:`\alpha = 1` is the reflection coefficient.

**Expansion**: If reflection is the best point, try expansion:

.. math::

   \mathbf{x}_e = \bar{\mathbf{x}} + \gamma (\mathbf{x}_r - \bar{\mathbf{x}})

where :math:`\gamma = 2` is the expansion coefficient.

**Contraction**: If reflection is poor, contract toward the best:

.. math::

   \mathbf{x}_c = \bar{\mathbf{x}} + \rho (\mathbf{x}_{n+1} - \bar{\mathbf{x}})

where :math:`\rho = 0.5` is the contraction coefficient.

**Shrinkage**: If contraction fails, shrink toward the best vertex:

.. math::

   \mathbf{x}_i = \mathbf{x}_1 + \sigma (\mathbf{x}_i - \mathbf{x}_1), \quad i = 2, \ldots, n+1

where :math:`\sigma = 0.5` is the shrinkage coefficient.

Algorithm Pseudocode
====================

.. code-block:: text

   Algorithm: Nelder-Mead Simplex
   Input: objective function f, initial point x0, step sizes
   Output: best solution x*, best fitness f*

   1. Initialize simplex with n+1 vertices around x0
   2. Evaluate f at all vertices
   3. REPEAT
   4.   Order vertices: f(x_1) <= f(x_2) <= ... <= f(x_{n+1})
   5.   Compute centroid of best n vertices
   6.   Reflect worst vertex through centroid -> x_r
   7.   IF f(x_r) < f(x_1) THEN
   8.     Try expansion -> x_e
   9.     Replace x_{n+1} with better of x_r, x_e
   10.  ELSE IF f(x_r) < f(x_n) THEN
   11.    Replace x_{n+1} with x_r
   12.  ELSE
   13.    Contract toward best/centroid -> x_c
   14.    IF f(x_c) < f(x_{n+1}) THEN
   15.      Replace x_{n+1} with x_c
   16.    ELSE
   17.      Shrink simplex toward x_1
   18.    END IF
   19.  END IF
   20. UNTIL convergence criteria met
   21. RETURN x_1, f(x_1)

.. mermaid::

   flowchart TD
       A[Start Iteration] --> B[Order vertices by f value]
       B --> C[Compute centroid]
       C --> D[Reflect worst vertex]
       D --> E{f_r < f_1?}
       E -->|Yes| F[Try Expansion]
       F --> G{f_e < f_r?}
       G -->|Yes| H[Accept expansion]
       G -->|No| I[Accept reflection]
       E -->|No| J{f_r < f_n?}
       J -->|Yes| I
       J -->|No| K[Contract]
       K --> L{f_c < f_{n+1}?}
       L -->|Yes| M[Accept contraction]
       L -->|No| N[Shrink simplex]
       H --> O[Next iteration]
       I --> O
       M --> O
       N --> O

Parameter Configuration
=======================

.. list-table:: Simplex Parameters
   :header-rows: 1
   :widths: 20 15 15 50

   * - Parameter
     - Symbol
     - Default
     - Description
   * - Reflection
     - :math:`\alpha`
     - 1.0
     - Reflection coefficient
   * - Expansion
     - :math:`\gamma`
     - 2.0
     - Expansion coefficient
   * - Contraction
     - :math:`\rho`
     - 0.5
     - Contraction coefficient
   * - Shrinkage
     - :math:`\sigma`
     - 0.5
     - Shrinkage coefficient
   * - Tolerance
     - tol
     - 1e-6
     - Convergence tolerance on simplex size

Convergence Criteria
--------------------

Typical convergence tests include:

1. **Function value spread**: :math:`\max_i f(\mathbf{x}_i) - \min_i f(\mathbf{x}_i) < \epsilon`
2. **Simplex size**: :math:`\max_{i,j} \|\mathbf{x}_i - \mathbf{x}_j\| < \delta`
3. **Maximum iterations reached**

Implementation Notes
====================

CLEED enhancements:

1. **Adaptive restart**: Reinitialize simplex if stuck
2. **Bound handling**: Project vertices to feasible region
3. **Scaling**: Automatic parameter scaling for better conditioning
4. **Multiple restarts**: Option for multi-start optimization

References
==========

.. bibliography::
   :filter: docname in docnames
