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
  the system :envar:`PATH` variable.

:envvar:`CSEARCH_RFAC`
  Path of the crfac program  used  for the R factor evaluation. This may simply be 'crfac'
  if the parent directory of this program is in the system :envar:`PATH` variable.

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
be to use different angles of incidence. For more information 
see :ref:`caoi_leed`.