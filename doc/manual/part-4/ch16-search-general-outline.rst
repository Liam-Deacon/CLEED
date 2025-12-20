Chapter 16: General outline (SEARCH)
====================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 45-46). Verify against the PDF.

16.1 Syntax
-----------

The general calling syntax of the search program is:

.. code-block:: text

   csearch -i <parameter file> -d <initial displacement> \
           -s <search type> -v <vertex file>

``-i <parameter file>`` specifying the parameter input file is the only mandatory option.
The file contains all the geometric and non-geometric parameters needed for the search.
A sample file is shown in Table 17.1. The parameter file name without extension is used
as "project name" by the program in order to create names for output files and the
control file of the R factor program.

``-d <initial displacement>`` specifies the initial displacements from the start geometry
for all parameters.

``-s <search type>`` specifies the search algorithm to be used for the structure
optimisation. Possible arguments are: ``ga`` (genetic algorithm, not implemented yet),
``po`` (Powell's method), ``sa`` (simulated annealing), and ``si``, ``sx`` (both simplex
algorithm, default).

``-v <vertex file>`` allows to restart the search with the current simplex, if the simplex
algorithm is used. The argument ``<vertex file>`` is the ``*.ver`` file produced by the
program.

16.2 UNIX-Environment
---------------------

The program has been developed in a UNIX environment and uses a few UNIX specific
features. The most important is the use of environment variables:

``CSEARCH_LEED``
  Name of the program used for the LEED calculations.

``CSEARCH_RFAC``
  Name of the program used for the R factor evaluation.

These variables have to be set using the ``export`` or ``setenv`` UNIX commands,
respectively, before the program is called for the first time.
