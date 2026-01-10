.. _algorithm-sa:

====================
Simulated Annealing
====================

Simulated Annealing (SA) is a probabilistic optimization technique inspired
by the annealing process in metallurgy :cite:`Kirkpatrick1983`. It is capable
of escaping local minima through temperature-controlled acceptance of
uphill moves.

.. contents:: On this page
   :local:
   :depth: 2

Introduction
============

SA mimics the physical process of heating a material and slowly cooling it
to decrease defects. At high temperatures, the algorithm accepts worse
solutions with high probability, enabling escape from local minima.
As temperature decreases, the algorithm becomes more selective, converging
toward the global optimum.

**Advantages for LEED-IV:**

* Global optimization capability
* Simple implementation
* Can escape local minima
* Works with noisy objective functions

**Limitations:**

* Slow convergence compared to population methods
* Temperature schedule requires tuning
* Single-solution approach (no population diversity)

Mathematical Formulation
========================

Metropolis Criterion
--------------------

The acceptance probability for a trial solution follows the Boltzmann distribution:

.. math::

   P(\text{accept}) = \begin{cases}
   1 & \text{if } \Delta f \leq 0 \\
   \exp(-\Delta f / T) & \text{if } \Delta f > 0
   \end{cases}

where :math:`\Delta f = f(\mathbf{x}_{trial}) - f(\mathbf{x}_{current})` is the
change in objective function value, and :math:`T` is the temperature.

Temperature Schedule
--------------------

**Geometric cooling** (most common):

.. math::

   T_{k+1} = \alpha \cdot T_k, \quad \alpha \in [0.8, 0.99]

**Logarithmic cooling** (theoretical guarantee but slow):

.. math::

   T_k = \frac{T_0}{\ln(1 + k)}

**Linear cooling**:

.. math::

   T_k = T_0 - k \cdot \frac{T_0 - T_{min}}{k_{max}}

**Adaptive cooling** (Lam schedule):

.. math::

   T_{k+1} = T_k \cdot \exp\left(-\frac{\sigma T_k}{T_k^2}\right)

Move Generation
---------------

New trial points are generated using various perturbation strategies:

**Uniform perturbation**:

.. math::

   x'_i = x_i + r \cdot \delta_i, \quad r \sim U(-1, 1)

**Gaussian perturbation**:

.. math::

   x'_i = x_i + \sigma_i \cdot z, \quad z \sim N(0, 1)

**Cauchy perturbation** (heavier tails, larger jumps):

.. math::

   x'_i = x_i + \gamma \cdot \tan(\pi(r - 0.5)), \quad r \sim U(0, 1)

Algorithm Pseudocode
====================

.. code-block:: text

   Algorithm: Simulated Annealing
   Input: objective function f, initial point x0, T0, T_min, cooling_rate
   Output: best solution x*, best fitness f*

   1. x = x0, x_best = x0
   2. T = T0
   3. WHILE T > T_min DO
   4.   FOR i = 1 TO moves_per_temperature DO
   5.     Generate trial x' by perturbing x
   6.     Apply bounds constraints to x'
   7.     delta_f = f(x') - f(x)
   8.     IF delta_f < 0 THEN
   9.       x = x'  (accept improvement)
   10.      IF f(x) < f(x_best) THEN x_best = x
   11.    ELSE
   12.      r = random(0, 1)
   13.      IF r < exp(-delta_f / T) THEN
   14.        x = x'  (accept with probability)
   15.      END IF
   16.    END IF
   17.  END FOR
   18.  T = cooling_rate * T  (reduce temperature)
   19. END WHILE
   20. RETURN x_best, f(x_best)

.. mermaid::

   flowchart TD
       A[Start] --> B[Initialize x, T = T0]
       B --> C{T > T_min?}
       C -->|No| J[Return best]
       C -->|Yes| D[Generate trial x']
       D --> E{f(x') < f(x)?}
       E -->|Yes| F[Accept x']
       E -->|No| G{rand < exp(-df/T)?}
       G -->|Yes| F
       G -->|No| H[Reject x']
       F --> I[Update best if improved]
       H --> I
       I --> K{More moves at T?}
       K -->|Yes| D
       K -->|No| L[Cool: T = alpha * T]
       L --> C

Parameter Configuration
=======================

.. list-table:: Simulated Annealing Parameters
   :header-rows: 1
   :widths: 25 15 60

   * - Parameter
     - Default
     - Description
   * - Initial temperature
     - auto
     - Starting temperature. Auto-set based on initial objective value range.
   * - Final temperature
     - 1e-8
     - Stopping temperature threshold.
   * - Cooling rate
     - 0.95
     - Geometric cooling factor. Range [0.8, 0.99].
   * - Moves per temperature
     - 100
     - Number of trial moves at each temperature level.
   * - Step size
     - 0.1
     - Initial perturbation magnitude (fraction of parameter range).

Initial Temperature Selection
-----------------------------

A common heuristic sets :math:`T_0` such that ~80% of uphill moves are initially accepted:

1. Generate sample moves and compute :math:`\Delta f` values
2. Set :math:`T_0 = -\langle\Delta f_{positive}\rangle / \ln(0.8)`

Implementation Notes
====================

CLEED enhancements include:

1. **Adaptive step size**: Step size decreases with temperature
2. **Reheating**: Optional temperature increase if stuck
3. **Multiple restarts**: Best solution from multiple independent runs
4. **Hybrid termination**: Switch to local optimizer at low temperature

References
==========

.. bibliography::
   :filter: docname in docnames
