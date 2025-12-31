Chapter 5: Optimisation: Finding the best agreement
===================================================

.. note::
   This chapter has been reconstructed based on standard optimization methods used
   in LEED analysis. Cross-reference with the original PDF for CLEED-specific details.

5.1 Introduction
----------------

Surface structure determination by LEED is fundamentally an optimization problem:
finding the set of structural parameters that minimizes the R-factor. Given the
complexity of the parameter space and the computational cost of each I-V curve
calculation, choosing an appropriate optimization strategy is crucial.

CLEED implements several optimization algorithms, ranging from local gradient-based
methods to global search strategies.

5.2 The Parameter Space
-----------------------

The structural parameters typically optimized include:

- **Vertical positions** (:math:`z`-coordinates) of surface atoms
- **Lateral positions** (:math:`x, y`-coordinates) for reconstructed or adsorbate atoms
- **Debye temperatures** or vibrational amplitudes
- **Site occupancies** (in disordered systems)

The parameter space can be high-dimensional (10-50+ parameters for complex structures),
with many local minima, making global optimization challenging.

5.3 Local Optimization Methods
------------------------------

5.3.1 Downhill Simplex (Nelder-Mead)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The simplex method [49]_ is a derivative-free optimization algorithm that works by
maintaining a simplex of :math:`n+1` vertices in :math:`n`-dimensional space. At each
iteration, the worst vertex is reflected, expanded, or contracted to improve the
objective function.

Advantages:

- No gradient calculations required
- Robust for noisy objective functions
- Simple to implement

Disadvantages:

- Can converge to local minima only
- Relatively slow convergence for high dimensions

5.3.2 Powell's Method
^^^^^^^^^^^^^^^^^^^^^

Powell's method [50]_ performs sequential one-dimensional optimizations along a set
of conjugate directions. It is more efficient than the simplex method for smooth
objective functions but still finds only local minima.

5.4 Global Optimization Methods
-------------------------------

5.4.1 Simulated Annealing
^^^^^^^^^^^^^^^^^^^^^^^^^

Simulated annealing [48]_ is inspired by the physical annealing process. It allows
uphill moves with a probability that decreases as the "temperature" parameter is
lowered according to a cooling schedule:

.. math::

   P(\Delta R) = \exp\left(-\frac{\Delta R}{T}\right)
   \tag{5.1}

where :math:`\Delta R` is the R-factor increase and :math:`T` is the current temperature.

Key parameters:

- **Initial temperature**: High enough to allow exploration
- **Cooling schedule**: Rate of temperature decrease
- **Final temperature**: When to terminate the search

5.4.2 Genetic Algorithm
^^^^^^^^^^^^^^^^^^^^^^^

Genetic algorithms [51, 52]_ maintain a population of candidate solutions that evolve
through selection, crossover, and mutation operations:

1. **Selection**: Prefer structures with lower R-factors
2. **Crossover**: Combine parameters from two "parent" structures
3. **Mutation**: Random perturbations to maintain diversity

While potentially capable of finding global minima, genetic algorithms require many
more function evaluations than local methods.

5.5 Symmetry Constraints
------------------------

Imposing crystallographic symmetry constraints reduces the effective dimensionality
of the search:

- **Rotational symmetry**: :math:`n`-fold rotation axes
- **Mirror planes**: Reflection symmetries
- **Translational symmetry**: Periodicity within the unit cell

CLEED's ``sr:`` keyword specifies rotational symmetry, and the ``sz:`` parameter
controls whether lateral parameters are varied or only vertical ones.

5.6 Avoiding Unphysical Geometries
----------------------------------

Constraints are applied to prevent searches from exploring unphysical regions:

- **Minimum interatomic distances**: Specified via ``rm:`` (minimum radii)
- **z-range constraints**: Atoms confined to specified vertical range (``zr:``)

When constraints are violated, a penalty is added to the R-factor to repel the
search from these regions.

5.7 Practical Search Strategy
-----------------------------

A typical structure determination workflow:

1. **Initial geometry**: Start from a reasonable model based on chemical intuition
2. **Coarse search**: Use simulated annealing or simplex with large step sizes
3. **Refinement**: Switch to downhill simplex near the minimum
4. **Verification**: Check symmetry-equivalent starting points
5. **Error estimation**: Explore parameter space near the minimum

5.8 Tensor LEED (Note)
----------------------

CLEED does not implement tensor LEED [55, 56]_, which uses linear approximations to
speed up calculations near a reference structure. While tensor LEED can significantly
reduce computation time for local searches, it:

- Is incompatible with global search strategies
- Can introduce errors when the search strays far from the reference
- Requires careful management of the reference structure

Full dynamical calculations, as implemented in CLEED, are more robust for global
searches but more computationally demanding.

.. seealso::

   - :doc:`../part-4/ch15-search-program-description` for CLEED's SEARCH program
   - :doc:`../part-4/ch16-search-general-outline` for command-line usage
   - :doc:`../part-4/ch17-search-input-files` for input file format

References
----------

.. [48] W. H. Press et al., "Numerical Recipes in C", Cambridge University Press, 1988.
.. [49] J. A. Nelder and R. Mead, Computer Journal 7 (1965) 308.
.. [50] R. P. Brent, "Algorithms for Minimization without Derivatives", Prentice-Hall, 1973.
.. [51] J. H. Holland, "Adaptation in Natural and Artificial Systems", U. Michigan Press, 1975.
.. [52] D. E. Goldberg, "Genetic Algorithms in Search, Optimization and Machine Learning", Addison-Wesley, 1989.
.. [55] P. J. Rous and J. B. Pendry, Surf. Sci. 219 (1989) 355.
.. [56] P. J. Rous, M. A. Van Hove, and G. A. Somorjai, Surf. Sci. 226 (1990) 15.
