Chapter 13: General outline (R factor program)
==============================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 38-39). Verify against the PDF.

13.1 Program description
------------------------

The R factor program performs the comparison of calculated and experimental IV curves.
The agreement is quantified in terms of the R (reliability) factor. It offers the choice
between four different R factors that can be used for the search: R1, R2, Rb, and Rp.
The output R factor value is the optimum achieved by shifting the energy axes of
experimental and theoretical I-V curves with respect to each other. This shift acts as
a correction for any non-optimum value of the optical potential V0 in the LEED
calculations, which need therefore not be optimized by the search program. In this way,
one dimension is eliminated from the search parameter space on which the search program
operates, hence reducing the number of LEED calculations to be performed. The assignment
of experimental and theoretical I-V curves (or average of curves) for comparison, and of
the relative weight that a particular I-V curve has in the overall R factor, is performed
by the user prior to the search.

13.2 Syntax
-----------

The general calling syntax of the R factor program is:

.. code-block:: text

   crfac -a <ID flag> -c <control file> -h -o <output file> \
         -r <R factor> -s <shift1,shift2,shift3> -t <theoretical file> \
         -v <optical potential> -w <IV output file>

The options ``-c <control file>`` and ``-t <theoretical file>`` are mandatory for normal
use of the program.

13.3 UNIX-Environment
---------------------

The program has been developed in a UNIX environment and uses a few UNIX specific
features. The most important is the use of environment variables:

``RF_HELP_FILE``
  File to be shown when the ``-h`` option is chosen. This variable has to be set using the
  ``export`` or ``setenv`` UNIX commands, respectively, before the program is called for the
  first time.

Argument details:

- ``-a <ID flag>`` defines whether only the average R factor is calculated (argument
  ``average``, default) or partial R factors for each subset of IV curves sharing a common
  ID number (argument ``all``). Only the first two characters of the argument are
  significant.
- ``-c <control file>`` specifies the control file which defines the correlation between
  experimental and theoretical IV curves. A sample control file is shown in Table 14.1.
- ``-h`` causes the program to show a short list of arguments.
- ``-o <output file>`` specifies the output file where the R factor values are written to
  (default: standard output).
- ``-r <R factor>`` specifies the R factor to be calculated. Valid arguments are: ``r1``
  (R1), ``r2`` (R2), ``rb`` (Rb), and ``rp`` (Rp, default).
- ``-s <shift1,shift2,shift3>`` defines the range (arguments shift1 and shift2) and
  stepwidth (shift3) of energy shifts between experimental and theoretical IV curves.
- ``-t <theoretical file>`` specifies the file containing the theoretical IV curves, i.e.
  the results file from the LEED program.
- ``-v <optical potential>`` specifies the value of the optical potential V0 used in the
  evaluation of Pendry's R factor (default: 4 eV).
- ``-w <IV output file>`` causes the program to write all normalised IV curves as
  energy/intensity pairs to separate files so that they can be plotted. ``<IV output file>``
  specifies the body of the file names to which the letters ``e`` (experimental) or ``t``
  (theoretical) and the number of the pair of curves is added.

