.. _csearch:

csearch
=======

csearch performs the LEED model structure optimisation. It uses standard 
optimisation  algorithms such as the downhill simplex method, Powell's method, 
simulated annealing and the genetic algorithm, which can be selected by the 
use in order to perform the search for the best fit geometry in N-dimensional 
parameter space (coordinates, angles, but not vibrational amplitudes). 
While the first two algorithms are strictly downhill-orientated, i.e. will 
find only the nearest R factor minimum, the latter two algorithms should in 
principle provide a means of locating the global R factor minimum within
the  given constrains, at the expense of many more search steps. Within
each search step a set of geometrical parameters is chosen by the algorithm 
depending on the R factor values achieved previously and in accordance with 
user-specified symmetry constraints. The parameters are written to a file 
serving as input for the LEED program whose output is then fed into the R factor 
program, :ref:`crfac`, in order to calculate an R factor value for the current 
parameter set.

.. _csearch_pso:

Particle Swarm Optimisation (PSO)
---------------------------------

The PSO algorithm is a population-based stochastic optimizer inspired by
social behaviour of bird flocking. Each particle in the swarm represents a
candidate solution that moves through parameter space according to its own
experience and the swarm's collective knowledge.

**Algorithm (Kennedy & Eberhart, 1995)**

For each particle *i* with position :math:`\mathbf{x}_i` and velocity
:math:`\mathbf{v}_i`:

1. **Velocity update:**

   .. math::

      \mathbf{v}_i(t+1) = \omega \mathbf{v}_i(t)
                        + c_1 r_1 (\mathbf{p}_i - \mathbf{x}_i(t))
                        + c_2 r_2 (\mathbf{g} - \mathbf{x}_i(t))

   where :math:`\omega` is the inertia weight, :math:`c_1` and :math:`c_2`
   are cognitive and social coefficients, :math:`r_1` and :math:`r_2` are
   uniform random values in [0,1], :math:`\mathbf{p}_i` is the particle's
   personal best, and :math:`\mathbf{g}` is the global best.

2. **Position update:**

   .. math::

      \mathbf{x}_i(t+1) = \mathbf{x}_i(t) + \mathbf{v}_i(t+1)

3. **Velocity clamping:** :math:`|\mathbf{v}_i| \le v_{\max}` to prevent
   divergence.

The implementation uses the constriction coefficient variant with defaults
tuned for LEED optimisation: :math:`\omega = 0.729`, :math:`c_1 = c_2 = 1.494`.

**References:**

- Kennedy, J., & Eberhart, R. (1995). Particle swarm optimization.
  *Proceedings of ICNN'95*, 4, 1942–1948.
  `doi:10.1109/ICNN.1995.488968 <https://doi.org/10.1109/ICNN.1995.488968>`_

- Shi, Y., & Eberhart, R. (1998). A modified particle swarm optimizer.
  *Proceedings of IEEE CEC 1998*, 69–73.
  `doi:10.1109/ICEC.1998.699146 <https://doi.org/10.1109/ICEC.1998.699146>`_

.. _csearch_de:

Differential Evolution (DE)
---------------------------

Differential Evolution is a population-based stochastic optimizer that
evolves candidate solutions using vector differences. The implementation
uses the classic DE/rand/1/bin variant.

**Algorithm (Storn & Price, 1997)**

For each target vector :math:`\mathbf{x}_i` in the population:

1. **Mutation:** Generate a mutant vector by combining three randomly
   selected distinct individuals :math:`a`, :math:`b`, :math:`c`:

   .. math::

      \mathbf{v}_i = \mathbf{x}_a + F (\mathbf{x}_b - \mathbf{x}_c)

   where :math:`F \in (0, 2]` is the differential weight (scaling factor).

2. **Crossover:** Create a trial vector :math:`\mathbf{u}_i` using binomial
   crossover:

   .. math::

      u_{i,j} = \begin{cases}
        v_{i,j} & \text{if } r_j < CR \text{ or } j = j_{\text{rand}} \\
        x_{i,j} & \text{otherwise}
      \end{cases}

   where :math:`CR \in [0, 1]` is the crossover probability and
   :math:`j_{\text{rand}}` ensures at least one component comes from the
   mutant.

3. **Selection:** Replace the target if the trial is at least as good:

   .. math::

      \mathbf{x}_i^{(t+1)} = \begin{cases}
        \mathbf{u}_i & \text{if } f(\mathbf{u}_i) \le f(\mathbf{x}_i^{(t)}) \\
        \mathbf{x}_i^{(t)} & \text{otherwise}
      \end{cases}

The implementation uses defaults tuned for LEED optimisation:
:math:`F = 0.8`, :math:`CR = 0.9`, population size = 10×ndim (minimum 20).

**References:**

- Storn, R., & Price, K. (1997). Differential Evolution – A Simple and
  Efficient Heuristic for Global Optimization over Continuous Spaces.
  *Journal of Global Optimization*, 11, 341–359.
  `doi:10.1023/A:1008202821328 <https://doi.org/10.1023/A:1008202821328>`_

- Storn, R., & Price, K. (1995). Differential Evolution—A Simple and
  Efficient Adaptive Scheme for Global Optimization over Continuous
  Spaces. International Computer Science Institute, Berkeley, CA,
  Technical Report TR-95-012.

.. _csearch_syntax:

Syntax
------
::

    csearch -i <input_file> [ OPTIONS...  ]

.. _csearch_options:
    
Options
^^^^^^^
:code:`-i <input_file>`

  Specifies the parameter input file and is the only mandatory
  option. The file contains all the  geometric  and  non-geometric
  parameters need for the surface structure. The :code:`<input_file>`
  filename without the extension is used as the "project name" by
  the program in order to create names for output files.

:code:`-d <initial_dispacement>`

  Specifies  the initial displacements from the start geometry for
  all parameters.

:code:`-s <search_type>`
  Specifies the search algorithm to be used for the structure
  optimisation. Possible arguments are:

  - :code:`si` / :code:`sx`: simplex (default).
  - :code:`po`: Powell.
  - :code:`sa`: simulated annealing.
  - :code:`ps`: particle swarm optimisation.
  - :code:`de`: differential evolution.
  - :code:`ga`: genetic algorithm (not implemented).

:code:`--max-evals <n>`

  Limits objective evaluations (simplex/PSO/DE). Overrides the default iteration budget.

:code:`--max-iters <n>`

  Limits iterations (Powell/annealing/PSO/DE). Overrides the default iteration budget.

:code:`--seed <n>`

  Sets a deterministic seed for stochastic optimizers (simulated annealing/PSO/DE).

.. note::
   Seeds are parsed as unsigned 64-bit integers. A value of 0 selects the
   built-in default used by the deterministic annealing RNG; non-zero seeds
   reproduce runs across platforms.

:code:`--pso-swarm <n>`

  Sets the PSO swarm size. Default: 10×ndim. Recommended range: 10–100.

:code:`--pso-inertia <n>`

  Sets the PSO inertia weight. Default: 0.729. Recommended range: 0.4–0.9.

:code:`--pso-c1 <n>`

  Sets the PSO cognitive coefficient. Default: 1.494. Recommended range: 1.0–2.5.

:code:`--pso-c2 <n>`

  Sets the PSO social coefficient. Default: 1.494. Recommended range: 1.0–2.5.

:code:`--pso-vmax <n>`

  Sets the PSO velocity clamp. Default: dpos (initial displacement). Recommended range: 0.1–5.0.

:code:`--de-pop <n>`

  Sets the DE population size. Default: 10×ndim (minimum 20). Recommended range: 20–200.

:code:`--de-weight <n>`

  Sets the DE differential weight (scaling factor F). Default: 0.8. Recommended range: 0.4–1.0.

:code:`--de-cr <n>`

  Sets the DE crossover probability. Default: 0.9. Recommended range: 0.5–1.0.

:code:`--de-span <n>`

  Sets the DE initial search span for population initialization. Default: dpos (initial displacement). Recommended range: 0.5–5.0.

:code:`-v <vertex_file>`
                     
  Allows the search to be restarted with the current simplex, provided 
  the simplex algorithm is used. The argument :code:`<vertex_file>`
  is the :file:`*.ver` file produced by the program.

.. _csearch_environment:

Environment
-----------

:envvar:`CLEED_HOME`
  Parent directory of CLEED distribution.

:envvar:`CSEARCH_LEED`
  Path of the program used for the LEED calculations. This
  may simply be 'csearch' if the parent directory of this program is in
  the system :envvar:`PATH` variable.

:envvar:`CSEARCH_RFAC`
  Path of the crfac program  used  for the R factor evaluation. This may simply be 'crfac'
  if the parent directory of this program is in the system :envvar:`PATH` variable.

:envvar:`CSEARCH_MAX_EVALS`
  Optional evaluation budget for simplex/PSO/DE searches (same as :code:`--max-evals`).

:envvar:`CSEARCH_MAX_ITERS`
  Optional iteration budget for Powell/annealing/PSO/DE searches (same as :code:`--max-iters`).

:envvar:`CSEARCH_SEED`
  Optional deterministic seed for simulated annealing/PSO/DE (same as :code:`--seed`).

  A value of 0 uses the built-in default seed for the deterministic annealing
  RNG. Non-zero values are parsed as unsigned 64-bit integers.

:envvar:`CSEARCH_PSO_SWARM`
  Optional PSO swarm size (same as :code:`--pso-swarm`).

:envvar:`CSEARCH_PSO_INERTIA`
  Optional PSO inertia weight (same as :code:`--pso-inertia`).

:envvar:`CSEARCH_PSO_C1`
  Optional PSO cognitive coefficient (same as :code:`--pso-c1`).

:envvar:`CSEARCH_PSO_C2`
  Optional PSO social coefficient (same as :code:`--pso-c2`).

:envvar:`CSEARCH_PSO_VMAX`
  Optional PSO velocity clamp (same as :code:`--pso-vmax`).

:envvar:`CSEARCH_DE_POP`
  Optional DE population size (same as :code:`--de-pop`).

:envvar:`CSEARCH_DE_WEIGHT`
  Optional DE weight factor (same as :code:`--de-weight`).

:envvar:`CSEARCH_DE_CR`
  Optional DE crossover rate (same as :code:`--de-cr`).

:envvar:`CSEARCH_DE_SPAN`
  Optional DE initial span (same as :code:`--de-span`).

:envvar:`CLEED_PHASE`
  Directory path of the phase shift files used in  the  surface and bulk models. 
  Please refer to :ref:`phsh` for more information on generating phase shift files.

These  variables  can  be  set automatically by executing the 'set_env' script.

Examples
--------



Files
-----
       
Input files
^^^^^^^^^^^

:file:`*.inp`
  The parameter input for the surface model. See :ref:`input_file` section 
  for more information.

:file:`*.bul`
  The bulk parameter file. See :ref:`bulk_file` for more information.

:file:`*.ctr`
  The  control  parameter  file for R factor evaluation. See 
  :ref:`control_file` for more information.
  
Output files
^^^^^^^^^^^^

:file:`*.bmin`

:file:`*.dum`

:file:`*.ver`

:file:`*.par`

:file:`*.pmin`

Notes
-----
The .inp, .bul and .ctr files all need the same filename prefix before
the file extension. This prefix is also used in the output filenames.

Typically, 50 to 100 eV of data is needed per geometry search parameter, 
e.g. for 1 atom with 3 parameters would require 150-300eV of data.
This  is  because  the precision (error bar) :math:`RR` depends on the energy
range with a larger range leading to greater precision. For complex
adsorbate structures or overlayer superstructures, more data is needed
because there are more parameters to fit. One solution would be to go
to higher experimental energies, however this leads to a loss of sensitivity, 
difficulties in resolving the LEED pattern (i.e. the spots are too close together), 
vastly increased CLEED calculation times due to more scattering, as
well as beam damage to the sample. As such an alternative route would
be to use different angles of incidence. For more information see
``caoi_leed`` (the angle-of-incidence wrapper program).
