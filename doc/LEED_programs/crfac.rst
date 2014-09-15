.. _crfac:

crfac
=====

crfac performs a comparison between(theory) and experimental IV curves.  
The agreement is quantified in terms of the reliability factor (or R factor), 
which can vary between 0 (perfect fit) and 1 (uncorrelated). For most applications 
an acceptable R factor is <0.1 for very simple models and <0.3 for complicated 
structures. The program offers the choice between four different R-factors 
that can be used when minimising a structural search. These are :math:`R_1`,
:math:`R_2`, :math:`R_p` and :math:`R_B`. The output R-factor value is the 
optimum achieved by shifting the energy axes of the experimental and theoretical I-V 
curves with respect to each other. This shift acts as a correction for any non-optimum 
value of the optical potential in the LEED calculations, which therefore need not be 
optimised by the csearch program. In this way, one dimension is eliminated from the search 
parameter space on which the search  program operates,  hence reducing the number of 
LEED calculations to be performed. The assignment of experimental and theoretical I-V 
curves  (or average  of  curves) for comparison, and of the relative weight that a
particular I-V curve has in the overall R factor, is performed prior to the search.

.. _crfac_syntax:

Syntax
------

::

    crfac -c <control_file> -t <theoretical_file> [ OPTIONS... ]

.. _crfac_options:

Options
^^^^^^^

:code:`-a <ID_flag>`

  defines whether only the average R-factor is calculated ('average') or partial R factors for
  each subset of IV curves sharing a common ID number ('all') [default: 'average']. Only the
  first two characters are significant.

:code:`-c <control_file>`

  specifies the control file which defines the correlation between
  experimental and theoretical IV curves. Further information
  regarding the control file structure is provided in the :ref:`control_file` section.

:code:`-h`
  
  causes the program to show a short list of arguments.

:code:`-o <output_file>`
  
  specifies  the output file where the R factor values are written
  to [default: standard output].

:code:`-r <R_factor>`
  
  specifies the R-factor type to be calculated. Valid arguments
  are:
  
:code:`-s <shift1,shift2,shift3>`

  defines the range (shift1 and shift2) and step width (shift3) of
  the energy shifts between the experimental and theoretical curves.

:code:`-t <theoretical_file>`

  specifies the file containing the theoretical IV curves. This is
  the CLEED *.res results file from the csearch program. See :ref:`csearch`
  for more information.

:code:`-v  <optical_potential>`

  specifies  the  value of the optical potential :math:`V_i` (in eV)
  used in the evaluation of Pendry's R-factor (:math:`R_p`). :math:`2V_i` 
  determines smallest resolvable features in the IV curves.
  The default is 4 eV, however in situations where the interlayer
  spacings are very small, such as for intermetallic compounds, :math:`V_i` 
  may need to be increased (but should remain < 5eV).

:code:`-V`
  
  give version and additional information about this program.

:code:`-w  <IV_output_prefix>`

  causes the program to write all normalised IV curves as
  energy/intensity pairs to separate files so that they can be
  plotted. <IV_output_prefix> specifies the base filename to which
  the  letters  'e' (experimental) or 't' (theoretical) as well as
  the number of the pair of curves is appended.

Environment
-----------

:envvar:`RF_HELP_FILE`
  File to be shown when the -h option is chosen if set in the system environment.

  This variable has to be set using the export or setenv UNIX commands, respectively, 
  before the program is called for the first time.

.. _crfac_examples:

Examples
--------
::

    crfac -w iv -c *.ctr -t *.res

Creates  a  list  of experimental and theoretical IV curve files
matching the regex pattern *'iv_[et]_[0-9]+?[0-9]+?[0-9]'*. This
command  is  valid  if  there  is  only one :file:`*.ctr` and :file:`*.res` file
within  the  directory (otherwise the full filenames are required). The IV files 
can then plotted using a graphing package such as gnuplot, qtiplot, veusz or xmgrace.

::

    crfac -c *.ctr -t *.res -a all

Prints a list of the individual R factors. Output can  be  redirected  to a file 
(e.g. by appending '>rfac.txt' to the command) for a permanent record.

::

    crfac -c *.ctr -t *.res

Calculates the error (relative error) for the calculated R-factor. The relative error 
is given by: ::math:`RR = (\frac{8 V_i}{\delta E})^{\frac{1}{2}}`, where :math:`V_i` 
is the imaginary component of the optical potential and :math:`\delta E` is the energy 
range of the data. This provides the error bars, :math:`E = (1 + RR) * R_{\text{min}}`, where
:math:`R_{\text{min}}` is the minimum R factor for the data.
  