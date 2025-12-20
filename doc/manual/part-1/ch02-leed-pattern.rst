Chapter 2: Qualitative Information: The LEED Pattern
====================================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 6-8). Figure references
   are placeholders pending review.

2.1 LEED Pattern
---------------

Since the electrons do not penetrate into the crystal bulk far enough to experience its
 three-dimensional periodicity, the diffraction pattern is determined by the two-dimensional
 surface periodicity described by the lattice vectors :math:`\vec{a}_1` and :math:`\vec{a}_2`,
 which are parallel to the surface plane. A general lattice point within the surface is an
 integer multiple of these lattice vectors:

.. math::

   \vec{R} = m_1 \vec{a}_1 + m_2 \vec{a}_2
   \tag{2.1}

The two-dimensional Bragg condition leads to the definition of reciprocal lattice vectors
 :math:`\vec{a}^{\ast}_1` and :math:`\vec{a}^{\ast}_2` which fulfil the set of equations:

.. math::

   \vec{a}_1 \cdot \vec{a}_1^{\ast} = \vec{a}_2 \cdot \vec{a}_2^{\ast} = 2\pi,
   \quad
   \vec{a}_1 \cdot \vec{a}_2^{\ast} = \vec{a}_2 \cdot \vec{a}_1^{\ast} = 0
   \tag{2.2}

These reciprocal lattice vectors, which have units of \AA\ :sup:`-1` and are also parallel to
 the surface, define the LEED pattern in k-space. Each diffraction spot corresponds to the
 sum of integer multiples of :math:`\vec{a}^{\ast}_1` and :math:`\vec{a}^{\ast}_2`.

.. math::

   \vec{G}(n_1, n_2) = n_1 \vec{a}^{\ast}_1 + n_2 \vec{a}^{\ast}_2
   \tag{2.3}

The integer numbers :math:`(n_1, n_2)` are used as indices to label the spot. The parallel
 component of the corresponding wave vector is:

.. math::

   \vec{k}_{\parallel}(n_1, n_2) = \vec{k}_{\parallel}(0, 0) + \vec{G}(n_1, n_2)
   \tag{2.4}

where :math:`\vec{k}_{\parallel}(0, 0)` is the parallel component of the wave vector of the
 incoming electron beam. The vertical component, :math:`k_\perp`, of the back-diffracted
 electrons is defined by energy conservation:

.. math::

   k_\perp(n_1, n_2) = \sqrt{\frac{2 m_e E_{\mathrm{kin}}}{\hbar^2} - k_{\parallel}^2(n_1, n_2)}
   \tag{2.5}

This equation also limits the set of observable LEED spots by the condition that the
 expression inside the brackets must be greater or equal to zero. With increasing electron
 energy the number of LEED spots increases while the polar emission angle with respect
 to the surface normal, :math:`\theta = \arctan(k_{\parallel}/k_\perp)`, decreases for each
 spot except for the specular spot (0,0) whose position does not change. Figure TODO (PDF p13)
 shows examples of common surface unit cells and the corresponding LEED patterns.

In many cases (adsorption, reconstruction) the periodicity at the surface is larger than
 it is expected for a bulk-truncated surface of the given crystal which leads to additional
 (superstructure) spots in the LEED pattern for which fractional indices are used. The
 lattice vectors :math:`\vec{b}_1` and :math:`\vec{b}_2` of such superstructures can be
 expressed as multiples of the (1 x 1) lattice vectors :math:`\vec{a}_1` and
 :math:`\vec{a}_2`:

.. math::

   \vec{b}_1 = m_{11} \vec{a}_1 + m_{12} \vec{a}_2
   \tag{2.6}

.. math::

   \vec{b}_2 = m_{21} \vec{a}_1 + m_{22} \vec{a}_2
   \tag{2.6}

where the numbers :math:`m_{ij}` are the coefficients of the superstructure matrix, which
 is a straightforward way of characterising the superstructure. The positions and indices
 of the additional LEED spots can be calculated directly from this matrix [45] according to
 the formulae:

.. math::

   \vec{b}_1^{\ast} = \frac{1}{m_{11} m_{22} - m_{12} m_{21}} (m_{22} \vec{a}_1^{\ast} - m_{21} \vec{a}_2^{\ast})
   \tag{2.7}

.. math::

   \vec{b}_2^{\ast} = \frac{1}{m_{11} m_{22} - m_{12} m_{21}} (-m_{12} \vec{a}_1^{\ast} + m_{11} \vec{a}_2^{\ast})
   \tag{2.7}

The area of the superstructure unit cell, :math:`A`, in units of the (1 x 1) unit cell area
 can also easily be calculated from the coefficients of the superstructure matrix:

.. math::

   A = m_{11} m_{22} - m_{12} m_{21}
   \tag{2.8}

Another, less general way is the notation according to Wood [10] where the lengths of the
 vectors :math:`\vec{b}_1` and :math:`\vec{b}_2` are specified in units of
 :math:`\vec{a}_1` and :math:`\vec{a}_2` together with the rotation angle :math:`\alpha`
 between :math:`\vec{b}_1` and :math:`\vec{a}_1` (only specified if non-zero):

.. math::

   p/c\left(\frac{\lvert \vec{b}_1 \rvert}{\lvert \vec{a}_1 \rvert} \times
   \frac{\lvert \vec{b}_2 \rvert}{\lvert \vec{a}_2 \rvert}\right) R\alpha
   \tag{2.9}

"p" indicates a "primitive" and "c" a "centred" surface unit cell. Examples are
 "p(2 x 2)", "p(\sqrt{3} x \sqrt{3})R30 deg" and "c(2 x 2)". This notation is not
 applicable in all cases but it is more frequently used than the matrix notation because
 it is shorter. Figure TODO (PDF p14) shows examples of common superstructures with the
 corresponding matrix and Wood notations.

2.2 Spot Profile Analysis
-------------------------

While the spot positions carry information about the size of the surface unit cell, the
 shapes and widths of the spots, i.e. the spot profiles, are influenced by the long range
 arrangement and order of the unit cells at the surface. If vertical displacements of the
 surface unit cells are involved (steps, facets), the spot profiles do change as a function
 of electron energy. If all surface unit cells are in the same plane (within the transfer
 width of the LEED optics), the spot profile is constant with energy. A periodic arrangement
 of equal steps at the surface leads to a spot splitting at certain energies from which the
 step height can be determined directly. For a statistical arrangement of steps the analysis
 of energy dependent changes in the spot profiles allows in many cases the determination
 of the mean step height and a characterisation of the step distribution [11]. Facets lead
 to extra spots which move in k upon changes of the kinetic energy. Point defects, static
 disorder, and thermally induced displacements lead to an increase of the background
 intensity between the spots. Depending on the correlation between the scatterers, the
 background is either homogeneous (no correlation) or structured (correlation). If the
 coherently ordered surface areas (islands, domains) are smaller than the transfer width of
 the LEED system and at the same vertical height, the width of these areas, :math:`\Delta d`,
 is directly related to the width of the LEED spots in k-space, :math:`\Delta k`:

.. math::

   \Delta k = \frac{2\pi}{\Delta d}
   \tag{2.10}

This relation holds for each direction parallel to the surface independently. It is
 particularly useful for determining the size of adsorbate islands which lead to extra
 superstructure spots. A good introduction (in German) into spot profile analysis is given
 by Henzler and Gopel in Ref. [39].

.. figure:: /_static/manual/figures/figure-placeholder-p13.png
   :alt: Placeholder for surface unit cells and LEED patterns (PDF p13).
   :align: center

   Figure TODO (PDF p13): Common surface unit cells and corresponding LEED patterns.

.. figure:: /_static/manual/figures/figure-placeholder-p14.png
   :alt: Placeholder for superstructure notation examples (PDF p14).
   :align: center

   Figure TODO (PDF p14): Superstructure matrix vs. Wood notation examples.
