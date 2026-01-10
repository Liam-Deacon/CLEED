.. _algorithm-pso:

===========================
Particle Swarm Optimization
===========================

Particle Swarm Optimization (PSO) is a population-based stochastic optimization
algorithm inspired by social behavior of bird flocking or fish schooling,
introduced by Kennedy and Eberhart :cite:`Kennedy1995`.

.. contents:: On this page
   :local:
   :depth: 2

Introduction
============

PSO maintains a swarm of particles, each representing a candidate solution.
Particles move through the search space guided by their own best-known position
and the swarm's best-known position.

**Advantages for LEED-IV:**

* Simple and intuitive algorithm
* Few parameters to tune
* Good balance of exploration and exploitation
* Naturally parallelizable

Mathematical Formulation
========================

Velocity Update
---------------

Each particle's velocity is updated based on cognitive and social components:

.. math::

   v_i^{(t+1)} = w \cdot v_i^{(t)}
                 + c_1 \cdot r_1 \cdot (p_i - x_i^{(t)})
                 + c_2 \cdot r_2 \cdot (g - x_i^{(t)})

where:

* :math:`w` is the inertia weight
* :math:`c_1` is the cognitive coefficient (personal best attraction)
* :math:`c_2` is the social coefficient (global best attraction)
* :math:`r_1, r_2 \sim U(0,1)` are random numbers
* :math:`p_i` is particle :math:`i`'s personal best position
* :math:`g` is the swarm's global best position

Position Update
---------------

Particle positions are updated using their velocities:

.. math::

   x_i^{(t+1)} = x_i^{(t)} + v_i^{(t+1)}

Velocity Clamping
-----------------

To prevent particles from moving too fast, velocities are clamped:

.. math::

   v_{i,j} = \min(\max(v_{i,j}, -v_{max,j}), v_{max,j})

where :math:`v_{max,j} = k \cdot (x_j^{max} - x_j^{min})` with :math:`k \in [0.1, 0.5]`.

Inertia Weight Strategies
-------------------------

**Constant inertia**: :math:`w = 0.729` (constriction coefficient)

**Linear decrease**: :math:`w^{(t)} = w_{max} - \frac{t}{t_{max}}(w_{max} - w_{min})`

Typical values: :math:`w_{max} = 0.9`, :math:`w_{min} = 0.4`

Algorithm Pseudocode
====================

.. code-block:: text

   Algorithm: Particle Swarm Optimization
   Input: objective function f, bounds [lb, ub], swarm_size, w, c1, c2, max_iter
   Output: best solution g, best fitness f(g)

   1. Initialize particle positions x_i randomly in bounds
   2. Initialize velocities v_i = 0 (or small random values)
   3. Evaluate fitness f(x_i) for all particles
   4. Set personal bests p_i = x_i
   5. Set global best g = argmin_i f(p_i)
   6. FOR t = 1 TO max_iter DO
   7.   FOR each particle i DO
   8.     Update velocity:
            v_i = w*v_i + c1*r1*(p_i - x_i) + c2*r2*(g - x_i)
   9.     Clamp velocity to [-v_max, v_max]
   10.    Update position: x_i = x_i + v_i
   11.    Apply bounds constraints
   12.    Evaluate f(x_i)
   13.    IF f(x_i) < f(p_i) THEN
   14.      p_i = x_i
   15.      IF f(x_i) < f(g) THEN g = x_i
   16.    END IF
   17.  END FOR
   18.  Update inertia weight w (if adaptive)
   19.  Check convergence criteria
   20. END FOR
   21. RETURN g, f(g)

Parameter Configuration
=======================

.. list-table:: PSO Parameters
   :header-rows: 1
   :widths: 20 15 15 50

   * - Parameter
     - Symbol
     - Default
     - Description
   * - Swarm size
     - N
     - 30
     - Number of particles. 20-50 typical for most problems.
   * - Inertia weight
     - w
     - 0.729
     - Balances global/local search. Decrease over time for convergence.
   * - Cognitive coefficient
     - c1
     - 1.49
     - Personal best attraction. Higher values increase exploration.
   * - Social coefficient
     - c2
     - 1.49
     - Global best attraction. Higher values increase exploitation.
   * - Velocity max factor
     - k
     - 0.2
     - v_max = k * (ub - lb). Prevents overshooting.

**Constriction Coefficients** (Clerc and Kennedy):

For guaranteed convergence, use :math:`\chi = 0.729` with :math:`c_1 = c_2 = 1.49445`:

.. math::

   \chi = \frac{2}{\left|2 - \phi - \sqrt{\phi^2 - 4\phi}\right|}
   \quad \text{where } \phi = c_1 + c_2 > 4

Implementation Notes
====================

The CLEED implementation includes:

1. **Bound handling**: Positions reflected at boundaries
2. **Local best topology**: Optional ring topology instead of global best
3. **Velocity reinitialization**: Reset particles stuck at boundaries
4. **Hybrid approach**: Can switch to local optimizer near convergence

References
==========

.. bibliography::
   :filter: docname in docnames
