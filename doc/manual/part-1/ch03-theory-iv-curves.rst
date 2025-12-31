Chapter 3: Theory: Calculating I-V curves
=========================================

.. note::
   This chapter has been reconstructed based on standard LEED theory as described in
   Pendry [4]_ and Van Hove & Tong [5]_. Cross-reference with the original PDF for
   any specific CLEED implementation details.

3.1 Introduction to Multiple Scattering Theory
----------------------------------------------

The calculation of LEED I-V curves requires a full dynamical scattering theory that
accounts for the strong interaction between low-energy electrons and the crystal
potential. Unlike X-ray diffraction, where kinematic (single-scattering) theory is
often adequate, LEED analysis must include multiple scattering effects due to the
large electron-atom scattering cross-sections at low energies.

The theoretical framework implemented in CLEED follows the approach developed by
Pendry [4]_ using layer-by-layer scattering combined with Van Hove and Tong's
combined space method [5]_ for treating layers with multiple atoms per unit cell.

3.2 The Scattering Problem
--------------------------

The electron wavefunction inside the crystal satisfies the Schrödinger equation:

.. math::

   \left[-\frac{\hbar^2}{2m}\nabla^2 + V(\vec{r})\right]\psi(\vec{r}) = E\psi(\vec{r})
   \tag{3.1}

where :math:`V(\vec{r})` is the crystal potential. For LEED calculations, the potential
is typically approximated using the muffin-tin model, where:

- Inside non-overlapping spheres around each atom, the potential is spherically symmetric
- Between the spheres (the interstitial region), the potential is constant

The optical potential includes both real and imaginary parts:

.. math::

   V = V_r + iV_i
   \tag{3.2}

where:

- :math:`V_r` (real part) accounts for the inner potential, typically -5 to -15 eV
- :math:`V_i` (imaginary part) accounts for inelastic scattering losses, typically 3-5 eV

3.3 Atomic Scattering: Phase Shifts
-----------------------------------

Within the muffin-tin sphere around each atom, scattering is characterized by
energy-dependent phase shifts :math:`\delta_l(E)` for each angular momentum channel
:math:`l`. The atomic t-matrix elements are:

.. math::

   t_l = \frac{e^{i\delta_l}\sin\delta_l}{\kappa}
   \tag{3.3}

where :math:`\kappa = \sqrt{2mE}/\hbar` is the electron wave vector magnitude inside
the crystal.

The phase shifts encode all information about the atomic scattering potential and
are pre-calculated using self-consistent atomic potential programs. CLEED reads
these from ``.phs`` files (see :doc:`ch08-input-files`).

3.4 Layer Scattering Matrices
-----------------------------

For a layer of atoms, the scattering is described by reflection and transmission
matrices :math:`R` and :math:`T` that relate incoming and outgoing plane wave amplitudes.
For a wave incident from above (+) or below (-):

.. math::

   \begin{pmatrix} A^+_{out} \\ A^-_{out} \end{pmatrix} =
   \begin{pmatrix} R^{++} & T^{+-} \\ T^{-+} & R^{--} \end{pmatrix}
   \begin{pmatrix} A^+_{in} \\ A^-_{in} \end{pmatrix}
   \tag{3.4}

3.5 Layer Doubling Algorithm
----------------------------

The layer doubling algorithm efficiently combines the scattering matrices of
successive layers to build up the semi-infinite crystal. Starting with single-layer
matrices, pairs of layers are combined recursively:

.. math::

   R_{12} = R_1 + T_1^{+-}(1 - R_2 R_1)^{-1} R_2 T_1^{-+}
   \tag{3.5}

This geometric series accounts for all multiple scattering between layers.
The algorithm is :math:`O(\log N)` in the number of layers, making it highly
efficient for deep bulk penetration.

3.6 Thermal Effects: Debye-Waller Factor
----------------------------------------

Thermal vibrations of surface atoms reduce diffraction intensities through the
Debye-Waller factor. For isotropic vibrations, the effective phase shift becomes:

.. math::

   t_l^{\text{eff}} = t_l \exp\left(-\frac{1}{2}k^2\langle u^2 \rangle\right)
   \tag{3.6}

where :math:`\langle u^2 \rangle` is the mean-square vibrational amplitude, related
to the Debye temperature :math:`\Theta_D` and sample temperature :math:`T` by:

.. math::

   \langle u^2 \rangle = \frac{9\hbar^2 T}{m k_B \Theta_D^2}
   \tag{3.7}

CLEED supports input of thermal vibrations via Debye temperatures or direct
specification of RMS displacements (see ``dr1``, ``dr3``, ``dtm`` keywords in
:doc:`ch08-input-files`).

3.7 Convergence Considerations
------------------------------

Key convergence parameters in LEED calculations include:

- **Angular momentum cutoff** (:math:`l_{\max}`): Higher energies and larger atoms
  require larger :math:`l_{\max}`. Typical values range from 6-12.
- **Number of beams**: Set automatically based on the energy range and lattice parameters.
- **Epsilon** (convergence criterion): Controls precision of lattice sums and layer
  doubling, typically :math:`10^{-4}`.

.. seealso::

   - :doc:`ch10-leed-specific-functions` for implementation details
   - :doc:`ch11-quantum-mechanical-functions` for the mathematical functions used

References
----------

.. [4] J. B. Pendry, "Low Energy Electron Diffraction", Academic Press, London, 1974.
.. [5] M. A. Van Hove and S. Y. Tong, "Surface Crystallography by LEED", Springer, Berlin, 1979.
