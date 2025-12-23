Chapter 6: Program description
==============================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 13-14). Verify wording against
   the PDF for accuracy.

In our LEED code fully dynamical scattering theory has been implemented along the lines
 of the algorithms described by Pendry (layer doubling for multiple scattering between
 successive layers of atoms [43]) and Van Hove/Tong (combined space method for layers
 with more than one atom per unit cell [44]). The extensive use of dynamical memory
 allocation - an intrinsic feature of the C programming language - allows the memory
 requirements, even for large surface unit cells, to be kept small. It also allows a very
 flexible input format which does not impose any restrictions on the number of bulk layers
 and overlayers nor on the number of atoms therein (other than the physically available
 memory). The input from the optimization program to the LEED program is simply a set of
 atomic coordinates from which the program creates its own set of Bravais and/or
 composite layers. For these layers scattering matrices are calculated and used to
 evaluate the amplitudes for multiple scattering between the layers.

While the main emphasis in the first development stage of the code had been put on
 reducing the time needed for the giant matrix inversion which is part of the combined
 space method, in the second stage the symmetries among beams were exploited in order to
 gain additional reductions in computing times. The scattering part of our implementation
 follows the algorithms developed by Van Hove and Tong [44]. A completely new development
 is the automated setup of the symmetry-reduced list of beams according to the symmetry
 of the surface (most possible combinations of rotational and mirror symmetries).

It does not include any linear approximations such as tensor LEED [55, 56] which are in
 general incompatible with global search strategies such as simulated annealing or the
 genetic algorithm, and can lead to errors even in down-hill oriented optimizations when
 the search path enters regimes of the parameter space where the approximation is
 inaccurate. It is, however, planned to implement this as an option for locally confined
 searches in the future version.

The input information required from the user is kept as little as possible; the program
 creates most of its parameters from the geometry input provided in the input files. The
 distribution of atoms over different layers is performed by the program based on the atom
 coordinates and the minimum possible vertical distance (MIN_DIST = 1.1 \AA) between two
 layers. The list of beams (length of vectors in plane wave representation) is created on
 the basis of the final energy and the minimum distance between the layers.

To date there exist two versions of cleed:

- The symmetrised version :code:`cleed_sym` makes use of mirror and rotational symmetries
  in the plane wave field and works thus with a reduced set of beams which can speed up
  the calculations significantly (up to a factor of 5 with respect to the non-symmetrised
  version). There are however some restrictions, the most important being that only the
  case of normal incidence and isotropic vibrations can be treated by the current version.
- The non-symmetrised version :code:`cleed_nsym` does not have this restriction. It is
  more flexible and can treat most cases of surface geometries, provided, there is at least
  one bulk inter-layer distance greater than MIN_DIST = 1.1 \AA.

Both program versions can use the same input files, whereby :code:`cleed_sym` needs some
 additional information which is ignored by :code:`cleed_nsym`. In any case it is strongly
 recommended to test the same input file with both programs in order to check the
 consistency. The relative intensities of equivalent beams should be identical down to
 :math:`10^{-8}`.
