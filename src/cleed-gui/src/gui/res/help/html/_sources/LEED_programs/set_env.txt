.. _set_env:

set_env
=======

As :ref:`csearch` requires setting environment variables in order to run, a small 
utility program :code:`set_env` is included to setup the CLEED environment for a 
given search.

.. _set_env_syntax:

Syntax
------
::
  
    set_env.py [-h] [-c] [-x <ext> [<ext> ...]] [-e <env> [<env> ...]]  [-i
    <INI_FILE>]   [-o   <INI_FILE>]   [--aoi-leed  <AOI_LEED>]  [--aoi-rfac
    <AOI_RFAC>] [--leed  <LEED>]  [--search  <SEARCH>]  [--sym  <LEED_SYM>]
    [--rfac <RFAC>] [--phsh <PHSH>] [-v] [-V]

.. _set_env_options:

Options
^^^^^^^

:code:`-h`

:code:`--help`

  Show help message and exit.
  
:code:`-c`

:code:`--check`

  Perform post-processing checks to ensure 
  all paths and executables are set correctly.

:code:`-x <ext> [<ext> ...]`

:code:`--ext <ext> [<ext> ...]`

  Set enabled extensions; can be any of 'aoi', 'sym' or 'phsh'.
  
:code:`-e <env> [<env> ...]`

:code:`--env <env> [<env> ...]`

  Set custom environment values of the form: "<var>:<value>".
  
:code:`-i <INI_FILE>`

:code:`--input <INI_FILE>`

  Load custom environment from :code:`<INI_FILE>`

:code:`-o <INI_FILE>`

:code:`--output <INI_FILE>`

  Save custom environment to :code:`<INI_FILE>` [default: "cleed.ini"].
  
  Note <INI_FILE> is always written.
  
:code:`--aoi-leed <AOI_LEED>`

  Path to angle of incidence LEED wrapper program [default: "caoi_leed"].
  
:code:`--aoi-rfac <AOI_RFAC>`

  Path to angle of incidence R-Factor wrapper program [default: "caoi_rfac"].
  
:code:`--leed <LEED>`

  Path to LEED program [default: "cleed_nsym"].
  
:code:`--search <SEARCH>`

  Path to search program [default: "csearch"].
  
:code:`--sym <LEED_SYM>`

  Path to symmetrised LEED program [default: "cleed_sym"].
  
:code:`--rfac <RFAC>`

  Path to R-Factor program [default: "crfac"].
  
:code:`--phsh <PHSH>`

  Path to phase shifts program [default: "phsh.py"].
  
:code:`-v`

:code:`--verbose`

  Set verbose output.
  
:code:`-V`

:code:`--version`

  Show program version information and exit.
  
.. _set_env_examples:

Examples
--------

::

    set_env --check
    

This will set the :envvar:`CLEED_HOME`, :envvar:`CLEED_PHASE`, 
:envvar:`CSEARCH_LEED` (to 'cleed_nsym') and :envvar:`CSEARCH_RFAC` (to 'crfac')
environment variables and check to verify all paths are correct.

::
      
    set_env --check --ext aoi sym phsh
  
  
In addition to the above example, this will set the :envvar:`CAOI_LEED`,
:envvar:`CAOI_RFAC` and :envvar:`PHASESHIFTS_LEED` environment variables.
      
::

    set_env --env CLEED_PHASE:"~/cleed/phase"

    
This will set the :envvar:`CLEED_PHASE` environment variable to "~/cleed/phase".
      
::

    set_env -i env.ini --env CLEED_PHASE:"%APPDATA%/CLEED/phase" -o env.ini

    
This will load the environment from env.ini, then set the :envvar:`CLEED_PHASE` 
environment variable to "%APPDATA%/CLEED/phase" before re-writing :file:`env.ini`
with the updated value.

.. _set_env_notes:

Notes
-----
A crude bash version of :file:`set_env.py` is included in the CLEED 
package as :file:`set_env.sh` for environments without modern Python support. 
The bash version works in a similar fashion, however, it isn't 
as rigorously tested and therefore your mileage may vary.