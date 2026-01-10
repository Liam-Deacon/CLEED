Chapter 7: General Outline
==========================

This chapter provides an overview of running the CLEED programs, including
command-line syntax, environment configuration, and typical workflow.

.. _ch07-syntax:

7.1 Syntax
----------

The CLEED package consists of several executables that work together:

**Main LEED Calculation Programs**

.. code-block:: bash

   # Non-symmetrized calculation
   cleed_nsym -i <input_file> -o <output_file>
   
   # Symmetry-adapted calculation (faster for symmetric surfaces)
   cleed_sym -i <input_file> -o <output_file>

**R-factor Calculation**

.. code-block:: bash

   crfac -t <theoretical_file> -e <experimental_file> [-r <rfactor_type>]

Where ``<rfactor_type>`` can be:

- ``rp`` - Pendry R-factor (default)
- ``r1`` - R1 factor
- ``r2`` - R2 factor
- ``rb`` - Rb factor (Zanazzi-Jona)

**Structural Search**

.. code-block:: bash

   csearch -i <control_file> [-v]

The search program iteratively calls the LEED and R-factor programs to
optimize structural parameters.

**Common Options**

Most programs accept:

- ``-h, --help`` - Display usage information
- ``-v, --verbose`` - Verbose output
- ``-V, --version`` - Display version information

.. _ch07-environment:

7.2 Environment Variables
-------------------------

CLEED uses environment variables to locate data files and executables.
These can be set manually or using the provided ``set_env.sh`` script.

**Required Variables**

``CLEED_HOME``
   Base directory for the CLEED installation. Used to locate executables
   and default data paths.

``CLEED_PHASE``
   Directory containing atomic phase shift files (``.phs``). Phase shifts
   are essential for scattering calculations and are element-specific.
   
   Example:
   
   .. code-block:: bash
   
      export CLEED_PHASE=/path/to/cleed/data/phase

**Search Program Variables**

``CSEARCH_LEED``
   Path to the LEED calculation executable called by ``csearch``.
   Defaults to ``cleed_nsym`` if not set.

``CSEARCH_RFAC``
   Path to the R-factor executable called by ``csearch``.
   Defaults to ``crfac`` if not set.

**Angle-of-Incidence Variables**

For angle-of-incidence (AOI) calculations:

``CAOI_LEED``
   Path to AOI-capable LEED executable.

``CAOI_RFAC``
   Path to R-factor program for AOI analysis.

**Setting Up the Environment**

The easiest way to configure the environment is using the provided script:

.. code-block:: bash

   source set_env.sh --check

This script:

1. Locates CLEED executables in PATH or CLEED_HOME
2. Sets all required environment variables
3. Optionally verifies that all paths are valid (with ``--check``)

For Windows users, ``set_env.cmd`` provides equivalent functionality.

.. _ch07-workflow:

7.3 Typical Workflow
--------------------

A standard LEED-IV analysis follows this workflow:

1. **Prepare Phase Shifts**
   
   Obtain or calculate atomic phase shift files for all elements in
   your surface structure. Place them in the ``CLEED_PHASE`` directory.

2. **Create Input Files**
   
   Write the bulk and surface input files describing your trial structure
   (see :ref:`Chapter 8 <ch08-input-files>`).

3. **Run LEED Calculation**
   
   .. code-block:: bash
   
      cleed_nsym -i surface.inp -o surface.res
   
   This generates theoretical I-V curves.

4. **Compare with Experiment**
   
   .. code-block:: bash
   
      crfac -t surface.res -e experimental.iv -r rp
   
   This calculates the R-factor between theory and experiment.

5. **Optimize Structure**
   
   Create a search control file and run:
   
   .. code-block:: bash
   
      csearch -i search.ctr
   
   The search iteratively refines structural parameters to minimize
   the R-factor.

6. **Analyze Results**
   
   Extract the optimized structure from the search output and
   calculate error estimates from the R-factor variance.

.. seealso::

   - :doc:`ch08-input-files` for input file format
   - :doc:`../part-3/ch13-program-crfac` for R-factor details
   - :doc:`../part-4/ch15-program-csearch` for search algorithms

