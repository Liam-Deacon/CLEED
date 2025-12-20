Chapter 8: Input files
======================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 17-22). Tables require careful
   review against the PDF. See :ref:`manual-table-tasks`.

8.1 Parameter Input
-------------------

Both program versions can use the same input files, whereby :code:`cleed_sym` needs some
 additional information which is ignored by :code:`cleed_nsym`. Table 8.1 shows an example
 for a single parameter input file for a :math:`p(\sqrt{3} \times \sqrt{3})` H2O / Ru(0001)
 overlayer structure.

Each parameter or set of parameters, respectively, is specified by two letters and a colon
 at the beginning of a line. There can only be one parameter specifier per line. Comments
 can either be indicated by a leading ``#`` or by ``c:``. In the first case the comment is
 ignored by the LEED program, in the latter case it is stored by the program and written
 to the output file.

The meaning and the syntax of the parameter specifiers is (n: integer number, f: floating
 point number, c: character):

``al:``, ``a2:``, ``a3: f f f``
  Lattice vectors of the three-dimensional bulk unit cell (x, y, z in \AA). ``a1`` and
  ``a2`` are parallel to the surface plane, i.e. they define the two-dimensional (1 x 1)
  unit cell. ``a3`` must contain a component perpendicular to the surface. For the
  symmetrised program version ``a3`` must not have any parallel components. In some cases
  (e.g. fcc lattice) a non-primitive bulk unit cell must be used in order to fulfill this
  condition.

``m1:``, ``m2: f f``
  Super structure matrix defining the relationship between the superstructure lattice
  vectors ``b1`` and ``b2`` and the (1 x 1) lattice vectors ``a1`` and ``a2``.

.. code-block:: text

   # Sample input file for
   c: Ru(0001) + p(r3xr3)-20
   # bulk unit cell parameters
   a1: 1.3525 -2.3426 0.0000
   a2: 1.3525 2.3426 0.0000
   a3: 0.0000 0.0000 -4.2800
   # superstructure matrix
   m1: 2. 1.
   m2: -1. 1.
   # symmetry
   sr: 3 0.0 0.0
   # Overlayers:
   # O atoms on hcp site
   po: O_H2O 2.7050 0.0000 4.3100 dr3 0.05 0.05 0.05
   po: O_H2O 1.3525 2.3426 4.1900 dr3 0.05 0.05 0.05
   # Ru atoms of the first layer
   po: /home/CLEED/PHASE/Ru.phs 0.0000 0.0000 2.0400 dr1 0.0707
   po: /home/CLEED/PHASE/Ru.phs 2.7050 0.0000 2.0500 dr1 0.0707
   po: /home/CLEED/PHASE/Ru.phs 1.3525 2.3426 2.1100 dr1 0.0707
   # bulk layers:
   pb: /home/CLEED/PHASE/Ru.phs 0.0000 -1.5617 0.0000 dmt 400. 101. 200.
   pb: /home/CLEED/PHASE/Ru.phs 0.0000 -2.1400 0.0000 dmt 400. 101. 200.
   # NON-GEOMETRIC PARAMETERS:
   # optical potentials
   vr: -13.00
   vi: 4.50
   # energies
   ei: 32.
   ef: 260.1
   es: 4.
   # angles of incidence
   it: 0.
   ip: 0.
   # epsilon, lmax
   ep: 1.e-4
   lm: 8

Table 8.1: Sample parameter input file for p(sqrt(3) x sqrt(3)) (H2)O / Ru(0001).

``sr: n f f``
  Rotational symmetry: degree (n-fold axis), position of axis (x, y in \AA).
  (Ignored by the non-symmetrised program version.)

``vr: f``
  Real part of optical potential (in eV).

``vi: f``
  Imaginary part of optical potential (in eV).

``po: <phasestring> f f f c c c f {f f}``
  Atom parameters in overlayer (super structure unit cell):

  ``<phasestring>``: Name of phase shift file. It can either be specified by the full
  path (starting with a leading slash ``/``) or by the file name body (no leading ``/``)
  which will then be expanded into ``CLEED_PHASE/<phasestring>.phs`` by using the
  environment variable ``CLEED_PHASE``.

  If ``CLEED_PHASE = /home/CLEED/PHASE`` in the current example, the input of ``O_H2O``
  is equivalent to ``/home/CLEED/PHASE/O_H2O.phs``.

  ``f f f``: atom position x, y, z (in \AA).

  ``c c c f {f f}``: three character specifier for input of vibrational displacements:

  ``dtm f f f``: input of Debye temperature (\Theta_D in K), temperature (T in K), mass
  (in amu). From these values the isotropic radial root mean square displacement

  .. math::

     \sqrt{\langle \Delta r^2 \rangle} = \sqrt{\frac{9 h^2 T}{2 m k_B \Theta_D^2}}\, .

  will be calculated and used inside the program.

  ``dr1 f``: input of radial root mean square displacement in \AA.

  .. math::

     \sqrt{\langle \Delta r^2 \rangle} = \sqrt{\langle \Delta x^2 \rangle +
     \langle \Delta y^2 \rangle + \langle \Delta z^2 \rangle}

  ``dr3 f f f``: separate input of root mean square displacements along the coordinates
  :math:`\sqrt{\langle \Delta x^2 \rangle}`, :math:`\sqrt{\langle \Delta y^2 \rangle}`, and
  :math:`\sqrt{\langle \Delta z^2 \rangle}` in \AA. From these values the average radial
  root mean square displacement :math:`\sqrt{\langle \Delta r^2 \rangle}` (see above) will
  be calculated and used as isotropic displacement inside the program, thus the inputs
  ``dr1 0.0866`` and ``dr3 0.05 0.05 0.05`` are equivalent.

  ``nd3 f f f``: separate input of root mean square displacements along the coordinates
  :math:`\sqrt{\langle \Delta x^2 \rangle}`, :math:`\sqrt{\langle \Delta y^2 \rangle}`, and
  :math:`\sqrt{\langle \Delta z^2 \rangle}` in \AA (only non-symmetric version). The
  displacements are used in order to set up a non-diagonal atomic t-matrix which is used
  in the program.

``pb: <phasestring> f f f c c c f {f f}``
  Atom parameters in bulk layers ((1 x 1) unit cell); for syntax see ``po``.

``ei: f``
  First energy in energy loop for which intensities are calculated (in eV).

``ef: f``
  Last energy in loop (in eV).

``es: f``
  Energy step in loop (in eV).

``it: f``
  Polar angle of incidence with respect to surface normal (:math:`0^\\circ < \\theta < 90^\\circ`).

``ip: f``
  Azimuthal angle of incidence with respect to x-axis (:math:`0^\\circ < \\varphi < 360^\\circ`).

``ep: f``
  Epsilon: criterion for convergence of lattice sums and layer doubling.

``lm: n``
  Maximum angular momentum.

8.2 Separate bulk and overlayer parameter input
------------------------------------------------

If the parameter input is split into two files, all parameters except for the overlayer atom
parameters are read from the bulk parameter input file specified by the ``-b`` option. The
overlayer atom parameters (``po:``) are read from the parameter input file (``-i``). The
two files corresponding to the above example of :math:`p(\sqrt{3} 	imes \sqrt{3})` H2O / Ru(0001)
are shown in Tables 8.2 and 8.3.

This way of input is used within the automated search where only the optimised ``po:``
parameters are supplied by the search program whereas the unchanged parameters are read
from a separate bulk file provided by the user.

.. code-block:: text

   # sample bulk geometry input file
   c: Ru(0001) + p(r3xr3)
   # bulk unit cell parameters
   a1: 1.3525 -2.3426 0.0000
   a2: 1.3525 2.3426 0.0000
   a3: 0.0000 0.0000 -4.2800
   # superstructure matrix
   m1: 2. 1.
   m2: -1. 1.
   # symmetry
   sr: 3 0.0 0.0
   # NON-GEOMETRIC PARAMETERS:
   # optical potentials
   vr: -13.00
   vi: 4.50
   # energies
   ei: 32.
   ef: 260.1
   es: 4.
   # angles of incidence
   it: 0.
   ip: 0.
   # epsilon, lmax
   ep: 1.e-4
   lm: 8

Table 8.2: Sample bulk input file for any p(sqrt(3) x sqrt(3)) Ru(0001).

.. code-block:: text

   # sample overlayer geometry input file
   c: Ru(0001) + p(r3xr3)-20
   # Overlayers:
   # O atoms on hcp site
   po: O_H2O 2.7050 0.0000 4.3100 dr3 0.05 0.05 0.05
   po: O_H2O 1.3525 2.3426 4.1900 dr3 0.05 0.05 0.05
   # Ru atoms of the first layer
   po: /home/CLEED/PHASE/Ru.phs 0.0000 0.0000 2.0400 dr1 0.0707
   po: /home/CLEED/PHASE/Ru.phs 2.7050 0.0000 2.0500 dr1 0.0707
   po: /home/CLEED/PHASE/Ru.phs 1.3525 2.3426 2.1100 dr1 0.0707

Table 8.3: Sample overlayer input file for p(sqrt(3) x sqrt(3)) (H2)O / Ru(0001).

8.3 Phase shifts input
----------------------

The energy dependent atomic phase shifts :math:`\delta_l(E)` are read from the files
specified in the atom parameter sets (``po:`` and ``pb:``). These files have the format as
shown in Table 8.4.

.. code-block:: text

   40 9 neng, lmax (Ru, m=101)
   0.5000
   .2748 -0.3054 -0.5527 0.0180 0.0007 0.0000 0.0000 0.0000 0.0000 0.0000
   1.0000
   .8705 -0.5465 -0.6358 0.1689 0.0115 0.0009 0.0001 0.0000 0.0000 0.0000
   1.5000
   .5527 -0.7538 -0.7190 0.5972 0.0497 0.0056 0.0005 0.0000 0.0000 0.0000
   2.0000
   .2909 -0.9420 -0.7769 1.1747 0.1270 0.0190 0.0024 0.0003 0.0000 0.0000
   ...
   19.0000
   -1.2839 -3.0896 -1.8887 2.6390 1.6041 1.0384 0.6948 0.4697 0.3216 0.2165
   19.5000
   -1.3173 -3.1180 -1.9057 2.6399 1.6129 1.0490 0.7059 0.4795 0.3300 0.2243
   20.0000
   -1.3499 -3.1458 -1.9223 2.6407 1.6212 1.0593 0.7166 0.4891 0.3382 0.2319

Table 8.4: Sample phase shifts input file for Ru.
