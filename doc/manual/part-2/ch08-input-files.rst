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

TODO (PDF pp. 18-19): Insert Table 8.1 (sample parameter input file).

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

TODO (PDF pp. 21-22): Populate section 8.2 and insert Table 8.2 and Table 8.3.

8.3 Phase shifts input
----------------------

TODO (PDF p22): Populate section 8.3 and insert Table 8.4.
