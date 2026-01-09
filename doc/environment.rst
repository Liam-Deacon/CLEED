.. _environment:

*********************
Environment Variables
*********************

This page defines the environment variables referenced throughout the manual.

.. envvar:: CLEED_HOME

   Parent directory of the CLEED distribution.

.. envvar:: PATH

   System search path used to locate executables.

.. envvar:: CLEED_PHASE

   Directory containing phase shift input files (often `phase/`).

.. envvar:: CSEARCH_LEED

   Path to the LEED-IV program executable used by :ref:`csearch` (commonly
   :ref:`cleed_nsym`).

.. envvar:: CSEARCH_RFAC

   Path to the R-factor executable used by :ref:`csearch` (commonly
   :ref:`crfac`).

.. envvar:: CSEARCH_MAX_EVALS

   Optional evaluation budget for :ref:`csearch` (same as ``--max-evals``).

.. envvar:: CSEARCH_MAX_ITERS

   Optional iteration budget for :ref:`csearch` (same as ``--max-iters``).

.. envvar:: CSEARCH_SEED

   Optional deterministic seed for stochastic optimizers (same as ``--seed``).

   Seeds are parsed as unsigned 64-bit integers. A value of 0 uses the built-in
   default seed for the annealing RNG.

.. envvar:: RF_HELP_FILE

   Path to a help file shown when :ref:`crfac` is invoked with ``-h``.

.. envvar:: CAOI_LEED

   Path to the LEED-IV executable used by the angle-of-incidence workflow (if
   configured).

.. envvar:: CAOI_RFAC

   Path to the R-factor executable used by the angle-of-incidence workflow (if
   configured).

.. envvar:: PHASESHIFTS_LEED

   Path to the LEED executable used by the phase shifts tooling (if configured).

.. envvar:: PHASESHIFT_LEED

   Path to the LEED executable invoked by the ``phsh.py`` script (if configured).

.. envvar:: PHASHSHIFTS_LEED

   Legacy/typo variant retained for compatibility in older documentation.
