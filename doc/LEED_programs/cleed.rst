.. _cleed:

cleed
=====

.. _cleed_description:

Description
------------

In this **LEED** code, fully dynamical scattering theory has been implemented along 
the lines of algorithms described by Pendry (layer doubling between successive 
layers of atoms [43]) and Van Hove/Tong (combined space method for layers with 
more than one atom per unit cell [44]). The extensive use of dynamical memory 
allocation - an intrinsic feature of the C programming language - allows the 
memory requirements, even for large surface unit cells, to be kept small. It 
also allows a very flexible input format which does not impose any restrictions 
on the number of bulk layers and overlayers, nor on the number of atoms therein 
(other than the physically available memory). The input from the optimisation 
program to the LEED program is simply a set of atomic coordinates from which 
the program creates its own set of Bravais and/or composite layers. Scattering 
matrices are calculated for these layers and used to evaluate the amplitudes for 
multiple scattering between the layers.

While the main emphasis in the first development stage of the code had been 
put on reducing the time needed for the giant matrix inversion, which is 
part of the combined space method, in the second stage the symmetries among 
beams were exploited in order to gain additional reductions in computing 
times.  

The scattering part of our implementation follows the algorithms developed by 
Van Hove and Tong [44]. A completely new development is the automated set up 
of the symmetry reduced list of beams according to the symmetry of the 
surface (most possible combinations of rotational and mirror symmetries).

It does not include any linear approximations, such as tensor LEED [55][56], 
which are, in general, incompatible with global search strategies such as 
simulated annealing or the genetic algorithm, and can lead to errors even 
in downhill-orientated optimisations when the search path enters regimes of 
the parameter space where the approximation is inaccurate. It is, however 
planned to implement this as an option for locally confined searches in a 
future version.

The input information required from the user is kept to a minimum; the 
program creates most of its parameters from the geometry input provided in 
the input files (:code:`*.bul` & :code:`*.inp`). The distribution of atoms 
over different layers is performed by the program based on the atom 
coordinates and the minimum possible vertical distance (:code:`MIN_DIST = 1.1` 
:math:`\text{\AA}`) between two layers. The list of beams (length of 
vectors in plane wave representation is created on the basis of the final 
energy and the minimum distance between the layers.

To date there exists two versions of the :code:`cleed` program.

.. _cleed_sym:

cleed_sym
^^^^^^^^^

The symmetrised version :code:`cleed_sym` makes use of mirror and rotational 
symmetries in the plane wave field and works thus with a reduced set of 
beams which can speed up the calculations significantly (up to a factor 
of 5 with respect to the non-symmetrised version). There are however some 
restrictions, the most important being that only the case of normal incidence 
and isotropic vibrations can be treated by the current version.

.. _cleed_nsym:

cleed_nsym
^^^^^^^^^^

The non-symmetrised version :code:`cleed_nsym` does not have these restictions. 
It is more flexible and can treat most cases of surface geometries, provided, 
there is at least one bulk inter-layer distance greater than :code:`MIN_DIST` 
= 1.1 :math:`\text{\AA}`.

Both program versions can use the same input files, whereby :code:`cleed_sym` 
needs some additional information which is ignored by the vanilla 
:code:`cleed` (or sometimes :code:`cleed_nsym` or :code:`nsym`). In any case, 
it is strongly recommended to test the same input file with both programs in 
order to check the consistency. The relative intensities of equivalent beams 
should be identical down to 10 :sup:`-8`.

.. _cleed_syntax:

Syntax
------

The general calling syntax of the LEED program is::

    cleed -i <parameter_file> -b <bulk_parameter_file> -o <results_file> 
      -r <storage_file> -w <storage_file>

The first argument (:code:`-i <parameter_file>`) specifying the parameter 
input file is the only mandatory argument. The file contains all the geometric 
and non-geometric parameters for the LEED calculations. A sample file is shown 
:ref:`here <cleed_input_file>`. Alternatively, the input can be split into two files, the parameter 
file and the bulk parameter file. The latter file (
:code:`-b <bulk_parameter_file>`) contains all the parameters which are not 
varied during the optimisation. Consequently, the search program has to produce 
only the parameter file containing the optimised atom positions of the 
overlayer in each iteration step of an automated search.


.. _cleed_options:

Options
^^^^^^^

:code:`-a <ID_flag>`

  defines whether only the average R factor is calculated (argument '*average*' 
  is the default) or partial R factors for each subset of IV curves sharing 
  a common ID number (argument '*all*'). Only the first two characters are significant.

:code:`-c <control_file>`
 
  specifies the control file which defines the correlation between experimental 
  and theoretical IV curves. Further information regarding the control file 
  structure is provided in the CONTROL_FILE section.

:code:`-h` 

  causes the program to show a short list of arguments. 

:code:`-o <output_file>`

  specifies the output file where the R factor values are written to (default: standard output).

:code:`-r <R_factor>`

  specifies the R factor type to be calculated. Valid arguments are:

  + :code:`r1` : calculate :math:`R_1`

  + :code:`r2` : calculate the mean-square displacement 
    :math:`R_2 = {\sum {(c I_{\text{expt}} - I_{\text{theory}})}^ 2} / \sum{I_{\text{theory}}^2}`, 
    where :math:`c` is a scaling factor.

  + :code:`rb` : (calculates :math:`R_{B1}` and :math:`R_{B2}`)

  + :code:`rp` : calculates the Pendry R factor :math:`R_p`. This provides the level of 
    agreement on shape of curves, not the intensity by comparing logarithmic 
    derivatives, :math:`R_p = \frac{1}{I(E)} \times \frac{\delta I(E)}{\delta E}`, and 
    is the standard in I(V) analysis. The Pendry R factor is problematic with 
    experimental noise as it is sensitive to positions of peaks, not intensity and 
    therefore noisy data will result in extra 'peaks'. The workaround for this 
    is to smooth or average the experimental data using a tool such as :ref:`ftsmooth`.

:code:`-s <shift1,shift2,shift3>`

  defines the range (shift1 and shift2) and step width (shift3) of the energy 
  shifts between the experimental and theoretical curves.

:code:`-t <theoretical_file>`

  specifies the file containing the theoretical IV curves. This is the CLEED :file:`*.res`
  results file from the :code:`csearch` program. See :ref:`csearch` for more information. 

:code:`-v <optical_potential>`

  specifies the value of the optical potential :math:`V_i` (in eV) used in the 
  evaluation of Pendry's R-factor (:math:`R_p`). :math:`2V_i` determines 
  smallest resolvable features in the IV curves. The default is 4 eV, however 
  in situations where the interlayer spacings are very small, such as for 
  intermetallic compounds, :math:`V_i` may need to be increased (but 
  should remain <5eV). 

:code:`-V`
 
  give version and additional information about this program. 
  
:code:`-w <IV_output_prefix>`

  causes the program to write all normalised IV curves as energy/intensity 
  pairs to separate files so that they can be plotted. :code:`<IV_output_prefix>`
  specifies the base filename to which the letters '*e*' (experimental) 
  or '*t*' (theoretical) as well as the number of the pair of curves is appended.

.. cleed_environment:

Environment
-----------
The program uses environment variables for calling other processes or for file paths and include:

:envvar:`CLEED_HOME`
  File to be shown when the :code:`-h` option is chosen if set in the system environment. 
 
Each variable has to be set using the :command:`export` or :command:`setenv` UNIX commands, 
for bash and c shells, respectively, before the program is called for the first 
time. In contrast, the :command:`set` is used on Windows machines for the current command window, 
however users of modern Windows operating systems (Win7 and up) may also use the 
:command:`setx` command to permanently store values. 
 
 The :ref:`set_env` program can be used as an aid in setting up the CLEED environment.

.. _cleed_examples:
 
Examples
--------

::

    cleed_nsym -c *.ctr -i *.inp -b *.bul 1>*.out 2>*.err &
    
This will start :ref:`cleed_nsym` as a detached process. Windows users should 
add :code:`START /B` to the beginning of the command instead of using :code:`&`.

::

    cleed_nsym -c *.ctr -i *.inp -b *.bul -v *.ver 1>>*.out 2>>*.err &

This will restart :ref:`cleed_nsym` from the last set of vertices and appends 
output to the :file:`*.out` and :file:`*err` log files.
    
.. note::
  In the preceding examples replace any :code:`*.<ext>` with the actual file name of 
  the models or log files. 
    
.. _cleed_notes:

Notes
-----
Both :file:`*.inp` and :file:`*.var` input files contain the parameters in arbitrary sequence. 
The values are preceded by the parameter names that are mentioned above. 
If a parameter occurs more than one time, the last value is taken. If it doesn't 
occur at all and a sensible value can be preset, the program takes such values. 
Be careful with that option because the program will not mention if you forget a 
value and it is preset by its own value. The used parameter values might be 
controlled in output file. 