.. _phsh:

phsh
====

:code:`phsh.py` is a Python based program used to quickly and easily 
generate the :file:`*.phs` files necessary for calculating the LEED-IV 
curves using :ref:`cleed`.

The :code:`phsh.py` script is placed into the system :envvar:`PATH` during installation of the 
phaseshifts package. It can then be used from the command line, e.g. :code:`phsh.py --help` 
will produce a list of command line options::

.. _phsh_syntax:

Syntax
------
::
  
    phsh.py [-h] -b <bulk_file> -i <slab_file> [-t <temp_dir>] [-l <lmax>]
            [-f <format>] [-S <subdir>] [-v] [-V]

  
.. _phsh_options:
  
Options
^^^^^^^

:code:`-h`

:code:`--help`
  Show help message and exit.

:code:`-b <bulk_file>` 

:code:`--bulk <bulk_file>`
  Path to MTZ bulk or CLEED :file:`*.bul` input file.
  
:code:`-i <slab_file>`

:code:`--slab <slab_file>`
  Path to MTZ slab or CLEED :file:`*.inp` input file.
  
:code:`-g`

:code:`--generate-only`
  Generate phase shifts, but do not launch :envvar:`PHASHSHIFTS_LEED` 
  sub-process.
    
:code:`-t <temp_dir>`

:code:`--tmpdir <temp_dir>`
  Temporary directory for intermediate file generation.
  
:code:`-l <lmax>`

:code:`--lmax <lmax>`
  Maximum angular momentum quantum number [default=10].
  
:code:`-f <format>`

:code:`--format <format>`
  Use specific phase shift format i.e. 'CLEED' [default=None].
  
:code:`-S <subdir>`

:code:`--store <subdir>`
  Keep intermediate files in subdir when done.
  
:code:`-v`

:code:`--verbose`
  Set verbosity level [default: None].
  
:code:`-V`

:code:`--version`
  Show program's version information and exit.

.. _phsh_compatibility:
  
CLEED compatibility
-------------------
It is possible to use this script to generate phase shift files iteratively 
during a geometry search for the CLEED package. In this manner phase shifts 
will be generated at the beginning of each cycle of the search.

For this to work, the environment variable :envvar:`CSEARCH_LEED` must point to the 
:code:`phsh.py` script, which will invoke the LEED program in :envvar:`PHASESHIFT_LEED`
after execution. When operating in this mode, the following assumptions are made:

 1. :code:`-b <bulk_file>` option not needed and the filename is assumed by 
    changing the file extension of `<slab_file>` to '*.bul*'
 2. `-f CLEED` format is implied.
 3. The generated phase shifts are stored in the directory set by the `CLEED_PHASE` 
    environment variable.
 4. :code:`<lmax>` is equal to 10, unless additional parameter syntax is given in the CLEED 
    :file:`\*.inp` file.

.. note::
  If the :envvar:`PHASESHIFT_LEED` environment variable is not found, but 
  :envvar:`CLEED_PHASE` is, however, found then the program will place the 
  generated files in this directory unless a specific `-S <subdir>` is provided.
  
.. _phsh_notes:
  
Notes
-----
Copyright 2013-2014 Liam Deacon. All rights reserved.

Licensed under the MIT license (see LICENSE file included in the 
`phaseshifts <https://pypi.python.org/pypi/phaseshifts>`_ package for details)

Please send your feedback, including bugs notifications
and fixes, to: liam.m.deacon@gmail.com
