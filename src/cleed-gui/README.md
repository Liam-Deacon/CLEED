# cleed-gui #
This package provides a PyQt GUI frontend to CLEED, used for computing I(V) curves in low energy electron diffraction (LEED).

## Install ##

Installation of this package assumes that you have git and pip installed on the system PATH and a working Python 2.7x distribution with PyQt.

1. Clone the git repo into your desired current working directory using the command line:
    

```
#!bash

cd /path/to/clone/repo/into
git clone https://Liam_Deacon@bitbucket.org/Liam_Deacon/cleed-gui.git
```


2. Change the working directory and install package:

```
#!bash

cd cleed-gui
cd src
python setup.py 

```


## About the code ##

This project has been released under the GNU Public License Version 3.0 with the aim 
of producing key open source tools and encouraging collaboration within LEED-IV community. 
If you wish to be involved, whether it be huge code contributions or filing bug reports, 
please email the developers using the contact information below - Thanks!

         
## Acknowledgements ##

As with all scientific progress, we stand on the shoulders of giants. If this 
package is of use to you in publishing papers then please acknowledge the 
following people who have made this package a reality:

 - **G. Held** for developing the CLEED programs.

 - **W. Braun** for the symmetry extensions.

 - **Z. Zheleeva** for the angle of incidence extensions.
 
 I would also be grateful if you acknowledge this Python package (*cleed-gui*) as: 
   "L.M. Deacon, *private communication.*"


### Thanks ###

A special thanks is due for Georg Held, who allowed CLEED to be freely available 
under the GNU Public License and whose enlightening discussions 
have helped guide the development of both the core CLEED package and this 
GUI front end.


## Contact ##

This package is developed/maintained in my spare time so any bug reports, patches, 
or other feedback are very welcome and should be sent to: liam.deacon@diamond.ac.uk

The project is in the early developmental stages and so anyone who wishes to get 
involved are most welcome (simply contact me using the email above).

## To do ##

 1. Full implementation of GUI front end (Qt ui files are provided in the ``res/`` directory for anyone 
    wishing to undertake this challenge). Other frontends are welcome (Qt was used  
    due to developer familiarity/experience). For those wishing a sneak preview, try executing
    ``main.pyw``
    
 2. Embedding of a model visualisation tool such as 
    [jmol](http://jmol.sourceforge.net), [gdis](http://gdis.sourceforge.net) or 
    [Avogadro](http://avogadro.cc/wiki/Python_PyQt4)

 3. Wrapping CLEED functions with Python and call as a script (allow automation 
    and streamlining of the LEED calculation process and data processing)
    
 4. Documentation - the manual has been started, but is not complete and thus is a 
    high priority. The current aim is to use sphinx to generate html and latex documents
    for semi-automated generation of both the tutorial and supporting website. If
    you have the cleed-gui source and the [sphinx](https://pypi.python.org/pypi/Sphinx)
    and the [numpydoc](https://pypi.python.org/pypi/numpydoc) PyPi packages then you 
    can try making html or latex manuals using ``make html`` or ``make latexpdf`` commands 
    from the ``doc/`` directory.

 5. Test suit to verify the package is working as expected.

## Author list ##

  - Liam Deacon - *author & current maintainer*