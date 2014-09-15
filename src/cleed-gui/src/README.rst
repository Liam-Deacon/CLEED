=========
cleed-gui
=========

This package provides a PyQt GUI frontend to the `CLEED <http://>`_ 
used for computing I(V) curves in low energy electron diffraction (LEED).

Install
=======

The `cleed-gui <https://pypi.python.org/pypi/cleed-gui/>`_ package 
requires CPython 2.6 or later and also uses the `numpy 
<http://www.scipy.org/scipylib/download.html>`_, `scipy 
<http://www.scipy.org/scipylib/download.html>`_ and `cython 
<http://https://pypi.python.org/pypi/cython>`_ packages. 
Currently, it has only been tested extensively with Python 2.7 on Windows, so 
there are no guarantees with other platforms. To perform a setup follow the 
steps below.

 1. Install the numpy, scipy and cython packages. 
    
    On systems compatible with PyPI this can be done using the command::
         
      pip install numpy scipy cython

    Or if you have the easy_install package::
         
      easy_install install numpy scipy cython

    Older versions of numpy & scipy did not allow simultaneous installation -
    if you experience problems then try first installing numpy before 
    attempting to install scipy. 
    
    Alternatively download and install these packages manually following the
    instructions provided for the respective packages.

 2. To install the cleed-gui package::
         
      python setup.py install  

    With any luck the package has been installed successfully. A set of test scripts
    are provided, however a simple check may suffice using an interactive session of 
    the python interpreter:

      >>> from cleed-gui import *

    If this executes without errors then it is likely that all is well, but in case of 
    problems or bugs please use the contact provided below and I will do my best to 
    address the problem quickly.

.. tip:: On Windows systems it may be easier to install a scientific python distibution 
         rather than install the dependencies from source - `Python(x,y) 
         <http://code.google.com/p/pythonxy>`_ with mingw (gcc & gfortran) 
         installed is highly recommended.


About the code
==============

The example source codes provided in this package are intended to be 
instructional in calculating phase shifts. While it is not recommended to 
use the example code in production, the code
should be sufficient to explain the general use of the library.

         
Acknowledgements
================

As with all scientific progress, we stand on the shoulders of giants. If this 
package is of use to you in publishing papers then please acknowledge the 
following people who have made this package a reality:

 - **G. Held** for developing the CLEED programs.

 - **W. Braun** for the symmetry extensions.

 - **Z. Zheleeva** for the angle of incidence extensions.
 
 I would also be grateful if you acknowledge this Python package (*cleed-gui*) as: 
   "L.M. Deacon, *private communication.*"


Thanks
------

A special thanks is due for Georg Held, who allowed CLEED to be freely available 
under the GNU Public License and whose enlightening discussions 
have helped guide the development of both the core CLEED package and this 
GUI front end.


Contact
=======

This package is developed/maintained in my spare time so any bug reports, patches, 
or other feedback are very welcome and should be sent to: liam.deacon@diamond.ac.uk

The project is in the early developmental stages and so anyone who wishes to get 
involved are most welcome (simply contact me using the email above).

To do
=====


 #. Full implementation of GUI front end (Qt ui files are provided in the `res/`` directory for anyone 
    wishing to undertake this challenge). Other frontends are welcome (I use Qt 
    due to familiarity/experience). For those wishing a sneak preview, try executing
    ``main.pyw``
    
 #. Embedding of a model visualisation tool such as 
    `jmol <http://jmol.sourceforge.net/>_, 
    `gdis <http://gdis.sourceforge.net/>_` or 
    `Avogadro <http://avogadro.cc/wiki/Python_PyQt4>_

 #. Wrapping CLEED functions with Python and call as a script (allow automation 
    and streamlining of the LEED calculation process and data processing)
    
 #. Documentation - the manual has been started, but is not complete and thus is a 
    high priority. The current aim is to use sphinx to generate html and latex documents
    for semi-automated generation of both the tutorial and supporting website. If
    you have the cleed-gui source and the `sphinx <https://pypi.python.org/pypi/Sphinx>`_ 
    and the `numpydoc <https://pypi.python.org/pypi/numpydoc>`_ PyPi packages then you 
    can try making html or latex manuals using ``make html`` or ``make latexpdf`` commands 
    from the ``doc/`` directory.

 #. Test suit to verify the package is working as expected.

Author list
===========

  - Liam Deacon - *author & current maintainer*