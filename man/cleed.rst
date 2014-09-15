.. Process this file with 'groff -man -Tascii cleed.1'
.. man page author: Liam Deacon (liam.deacon@diamond.ac.uk)
.. cleed 1 "MARCH 2014" CLEED User Manuals
.. ===================================================================

Introduction
============

In this *LEED* code, fully dynamical scattering theory has been implemented along 
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
coordinates and the minimum possible vertical distance (:code:`MIN_DIST` = 1.1 
::math::`\text{\AA}`) between two layers. The list of beams (length of 
vectors in plane wave representation is created on the basis of the final 
energy and the minimum distance between the layers.

To date there exists two versions of cleed.

The symmetrised version :code:`cleed_sym` makes use of mirror and rotational 
symmetries in the plane wave field and works thus with a reduced set of 
beams which can speed up the calculations significantly (up to a factor 
of 5 with respect to the non-symmetrised version). There are however some 
restrictions, the most important being that only the case of normal incidence 
and isotropic vibrations can be treated by the current version.

The non-symmetrised version :code:`cleed` does not have these restictions. 
It is more flexible and can treat most cases of surface geometries, provided, 
there is at least one bulk inter-layer distance greater than :code:`MIN_DIST` 
= 1.1 ::math::`\text{\AA}`.

Both program versions can use the same input files, whereby :code:`cleed_sym` 
needs some additional information which is ignored by the vanilla 
:code:`cleed` (or sometimes :code:`cleed_nsym` or :code:`nsym`). In any case, 
it is strongly recommended to test the same input file with both programs in 
order to check the consistency. The relative intensities of equivalent beams 
should be identical down to 10 :sup:`-8`.

Syntax
======

The general calling syntax of the LEED program is::

cleed -i <parameter_file> -b <bulk_parameter_file> -o <results_file> 
      -r <storage_file> -w <storage_file>

The first argument (:code:`-i <parameter_file>`) specifying the parameter 
input file is the only mandatory argument. The file contains all the geometric 
and non-geometric parameters for the LEED calculations. A sample file is shown 
in ??. Alternatively, the input can be split into two files, the parameter 
file and the bulk parameter file. The latter file (
:code:`-b <bulk_parameter_file>`) contains all the parameters which are not 
varied during the optimisation. Consequently, the search program has to produce 
only the parameter file containing the optimised atom positions of the 
overlayer in each iteration step of an automated search.


OPTIONS
-------

*-a <ID_flag>*

  defines whether only the average R factor is calculated (argument 'average' is the default) or partial R factors for each subset of IV curves sharing a common ID number (argument 'all'). Only the first two characters are significant.

*-c <control_file>*
 
  specifies the control file which defines the correlation between experimental and theoretical IV curves. Further information regarding the control file structure is provided in the CONTROL_FILE section.

*-h* 

  causes the program to show a short list of arguments. 

*-o <output_file>*

  specifies the output file where the R factor values are written to (default: standard output).

*-r <R_factor>*

  specifies the R factor type to be calculated. Valid arguments are:

  'r1' : calculate $R sub 1$

  'r2' : calculate the mean-square displacement $R sub 2 = {sum {(c I sub expt - I sub theory)} sup 2} / sum I sub theory sup 2$, where $c$ is a scaling factor.

  'rb' : (calculates $R sub B1$ and $R sub B2$)

  'rp' : calculates the Pendry R factor $R sub p$. This provides the level of agreement on shape of curves, not the intensity by comparing logarithmic derivatives, $R sub p = {1 over I(E)} * {delta I(E) over delta E}$, and is the standard in I(V) analysis. The Pendry R factor is problematic with experimental noise as it is sensitive to positions of peaks, not intensity and therefore noisy data will result in extra 'peaks'. The workaround for this is to smooth or average the experimental data using a tool such as 
**ftsmooth**

*-s <shift1,shift2,shift3>*

  defines the range (shift1 and shift2) and step width (shift3) of the energy shifts between the experimental and theoretical curves.

*-t <theoretical_file>*

  specifies the file containing the theoretical IV curves. This is the CLEED `*.res`
  results file from the csearch program. See <csearch> for more information. 

*-v <optical_potential>*

  specifies the value of the optical potential $V sub i$ (in eV) used in the evaluation of Pendry's R-factor ($R sub p$). $2 V sub i$ determines smallest resolvable features in the IV curves. The default is 4 eV, however in situations where the interlayer spacings are very small, such as for intermetallic compounds, $V sub i$ may need to be increased (but should remain <5eV). 

*-V*
 
  give version and additional information about this program. 
  
*-w <IV_output_prefix>*

  causes the program to write all normalised IV curves as energy/intensity pairs to separate files so that they can be plotted. <IV_output_prefix> specifies the base filename to which the letters 'e' (experimental) or 't' (theoretical) as well as the number of the pair of curves is appended.


Environment
===========
The program has been developed in a UNIX environment and uses a few UNIX 
specific features. The most important is the use of environment variables.

 ``RF_HELP_FILE``
 File to be shown when the -h option is chosen if set in the system environment. 
 This variable has to be set using the ::export:: or ::setenv:: UNIX commands, 
 for bash and c shells, respectively, before the program is called for the first 
 time. Users of modern Windows operating systems (Win7 and up) may use the 
 ::setx:: command.


Examples
========
`cleed -w *"iv"* -c *.ctr -t *.res`


Notes
=====
Both *.inp and *.var input files contain the parameters in arbitrary sequence. 
The values are preceded by the parameter names that are mentioned above. 
If a parameter occurs more than one time, the last value is taken. If it doesn't 
occur at all and a sensible value can be preset, the program takes such values. 
Be careful with that option because the program will not mention if you forget a 
value and it is preset by its own value. The used parameter values might be 
controlled in output file. 

Files
=====

Input Files
-----------
.PP
.I *.ctr
.RS
The control file used by both csearch and cleed. This file contains the experimental curves to be evaluated against the theoretical ones. Each curve is a separate line (UNIX terminated) and take the form:
.br
ef=<experimental_filename>:ti=(<h>,<k>):id=<ID_number>:wt=<weighting>
.PP
The line syntax can be broken down into command character sequences as follows:
.br
.IP ef=
.RS
Experimental Filename: Specifies the path to the experimental IV curve. This is usually a relative path to ease readability. Also note that file paths containing spaces may not be read correctly resulting in an error.
.RE
.IP ti=
.RS
Theoretical Index: Specifies the Miller indices of the spot in terms of h and k. Both indices should be floating point numbers separated with a comma and enclosed in curved braces, i.e. '(' and ')'.
.RE
.IP id=
.RS
Identification: Sets a unique index (the <ID_number>) for each curve and is used primarily to help the user track the curves produced using the 'cleed -w' command. If none of the lines contain id=<ID_number> then the <ID_number> will be in the order in which the curves appear in the control file.
.RE
.IP wt=
.RS
Weighting: Provides a statistical weighting for each curve with a <weighting> value between 0.0 and 1.0 (default is 1.0).
.RE
.PP
.B Notes: 
.br
* Each command sequence must be separated using a colon ':'. 
.br
* Lines starting with an '#' will be regarded as comments.
.br
* Check lines are terminated with '\n' (UNIX end-line character).
.br
* The final line of the file should be blank 
.br 
  (for compatibility with older versions which threw an error).
.RE
.PP
.I *.res
.RS
The results file produced by the csearch program.
.RE

Output Files
------------
.I IV_curve_files
.RS
Generated using the '-w' option in the 
.B cleed
command. Experimental and theoretical curves are indicated with '_e' and '_t', respectively. The number specified by the 'id=' argument will be appended to the filename of each output curve for identification.
.RE


Author
======
Georg Held <g.held@reading.ac.uk>.


Contact
=======
Please email questions, bug fixes or any suggestions to either Georg 
Held, Jacopo Ardini <j.ardini@pgr.reading.ac.uk> or Liam Deacon 
<liam.deacon@diamond.ac.uk>.


Caveats
=======
In older versions of the CLEED package, the control file must end with a blank line
otherwise an error will occur.


Copyright
=========
Copyright (C) 1995-2014 Georg Held


Availability
============
cleed is available as part of the CLEED package and should be available 
on any platform where there is a GNU C compiler (GCC) tool chain. GCC 
is readily available on most Linux distributions, however Windows users 
can install MinGW, the minimalist GNU tool set for Windows.   

See Also
========
caoi(1), crfac(1), cnsym(1), csearch(1), csym(1), debye(1), ftsmooth(1), latt(1),  mkiv(1), phsh(1)  