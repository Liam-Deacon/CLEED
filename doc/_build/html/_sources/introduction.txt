.. _package:

*************
CLEED package
*************

.. _Introduction:

Introduction
------------

CLEED is a computational package for Low Energy Electron Diffraction (LEED) IV analysis. It fits 
experimental IV data curves with those simulated by the :ref:`cleed` program and a 'goodness' of fit is made
with the R-Factor program :ref:`crfac`. Optimisation of the input parameters is performed by :ref:`csearch`.

A (still rather incomplete) documentation of the program package can be found 
in the :download:`manual <CLEED_Manual.pdf>`, but it is probably best to send me an email, with all 
input files, to <g.held@reading.ac.uk> when you encounter any problems (I am sure you will ...)

.. _Installation:

Installation
------------

In order to target as many different systems in the simplest way the CLEED package has been 
developed using `CMake <http://www.cmake.org/>`_ as a dependency tracking and makefile generation program. CMake is 
available on all major desktop platforms and a `GUI <http://www.cmake.org/cmake/resources/software.html>`_ is also available for Windows users.

Use the following command in order to install CLEED for your system: ::

    cd /path/to/cleed/source
    cmake -G <generator-name> --build build/ -i
    cd build/
    <make> install

Where :code:`<generator-name>` is the target platform e.g. "Unix Makefiles" or "MinGW Makefiles".
<make> is usually :command:`make` (Unix/GNU Linux) or :command:`mingw32-make` (Windows). The ':code:`-i`' option is
voluntary, but is useful if you wish to be lead through the setup in a step-by-step fashion.

Note Windows users will require `MinGW <http://sourceforge.net/projects/mingw/>`_ installed (other 
compiler tool chains such as `MSVC <http://www.visualstudio.com/>`_ and 
`Cygwin <http://cygwin.com/index.html>`_ have not been tested). 

If you are unable to compile the programs, pre-compiled packages are also available.


Getting Started
---------------

On other operating systems check the compatibility of the trigonometric functions
and data type used for real in :file:`real.h` (in the :file:`include` directory)

We have had a few problems on OSs different from DEC alpha with the
symmetrised version of CLEED (:ref:`cleed_sym`), therefore this is currently not supported
in this distribution, but the non-symmetrised version should be running okay.

A set of sample input files for Er/Si(111) (no expt. IV curves), Benzene on Ru(0001) 
and O and Cu/Ni(111) can be found in under SIER, RUBENZ, NIO and NICU in the :file:`examples/`
directory of the CLEED installation.
 
There are three files for each search (in addition to phase shifts and experimental data)
which have to be created by the user:

#. <bul_file> :code:`*.bul`
   Bulk geometry and non-geometric parameters

#. <inp_file> :code:`*.inp`
   Specifies start geometry for the search and search parameters (symmetry etc.)

#. <ctr_file> :code:`*.ctr`
   Specifies correspondence between theoretical and experimental IV curves.

A detailed description of the file format can be found in :download:`the pdf manual <CLEED_Manual.pdf>`, 
but it is probably easiest to use one of the sample input files and modify it. 
All other files are created by the programs.
The search can be monitored through the :code:`*.log` file; at each point of the search the 
current best fit geometries and IV curves can be found in :code:`*.rmin` and :code:`*.pmin`.

These files are all described in the :download:`CLEED manual <CLEED_manual.pdf>`.

The phase shift input must have the same format as for the `VanHove/Tong programs
<http://www.icts.hkbu.edu.hk/surfstructinfo/SurfStrucInfo_files/leed/leedpack.html>`_ with a 
first line added that contains the number of energies :math:`n_{eng}` and the
maximum angular momentum quantum number :math:`l_{max}`.  For each type of atoms 
there must be a separate file in the directory 'phase/' called <atom>.phs. The directory 
is passed to the LEED program through the environment variable :envvar:`CLEED_PHASE`. 
The Python `phaseshifts <https://pypi.python.org/pypi/phaseshifts>`_ provides a simple 
script (:file:`phsh.py`) to generate the necessary phase shifts from the CLEED :file:`*.inp` and :file:`*.bul` 
files (see the phaseshifts documentation for further information): ::

    phsh.py -i *.inp -b *.bul

Before starting any program 3 environment variables have to be set:

- :envvar:`CLEED_PHASE` (see above)
- :envvar:`CSEARCH_LEED` (path to executable LEED-IV program: normally cleed_nsym)
- :envvar:`CSEARCH_RFAC` (path to executable R factor program: crfac)

A shell script doing this can be found in 'bin/set_env' (again, the directory path
of the parent directory must be changed in this file). For users with a Python 
interpreter installed, the script 'set_env.py' allows much more sophisticated setups,
for more details see: ::
    
    set_env.py --help

The search is started by: ::

    csearch -i <inp_file>

:ref:`csearch` is a master program that calls the LEED-IV program (:ref:`cleed_nsym`) and the 
R factor program (:ref:`crfac`) to calculate IV curves and R factors for a given trial 
structure and optimises the geometry parameters of the surface geometry in order to 
minimise the R factor using the simplex method (other search methods are currently tested).

The LEED-IV program can be called outside the search using: ::

    cleed_nsym -i <par_file> -b <bul_file> -o <res_file> > <out_file>

<par_file> is usually created by the search program. It contains only the
positions of the overlayer atoms that are optimised during the search. An example is 
given in 'examples/NIO'. The LEED-IV program produces a lot of control output (written 
to stdout). Therefore it is best to redirect stdout to a file using the :code:`>` command. 
It is best to check the LEED-IV program separately first, after you have made any 
alterations to the input file.

In order to test the R factor program separately, call it by: ::

    crfac -c <ctr_file> -t <output_file_from_cleed>

On Linux machines, for the search it is recommended to use the :code:`nohup` command together 
with '&' which runs the search in the background: ::

    nohup csearch -i <inp_file> > <out_file> 2> <err_file> &

In the NiO111_2x2O example each iteration takes about 90-100s (2.4 GHz Linux PC), the 
final R factor after convergence should be around 0.1322 and it takes around 183
iteration to get there from the start geometry specified in Ni111_2x2O.inp 
(see attached :code:`*.log` file).

The example in NICU leads to an R factor of 0.0633 after 63 iterations (*ca* 15s cpu 
time per iteration).

All information necessary to restart the search at the current position is stored in
:code:`*.ver`. If the search stops for any reason, e.g. because it has reached the limit
of iterations (currently 2000), it can be restarted by: ::

    nohup csearch -i <inp_file> -v <ver_file> 1> <out_file> 2> <err_file> &

GOOD LUCK

*-- Georg Held --*


