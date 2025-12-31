Chapter 4: R factors: Comparing Experiment and Theory
======================================================

.. note::
   This chapter has been reconstructed based on standard LEED R-factor methodology,
   particularly Pendry's R-factor [23]_. Cross-reference with the original PDF for
   CLEED-specific implementations.

4.1 Introduction
----------------

The comparison between experimental and theoretical I-V curves is quantified using
reliability factors (R-factors). A good agreement between experiment and theory is
indicated by a low R-factor value. The R-factor serves as the objective function
for structural optimization, guiding the search for the best-fit surface geometry.

Various R-factors have been proposed, each with different sensitivities to particular
features of the I-V curves. CLEED implements several standard R-factors, with
Pendry's R-factor (:math:`R_P`) being the most widely used.

4.2 Pendry's R-Factor
---------------------

Pendry's R-factor [23]_ is specifically designed for LEED I-V analysis and is
relatively insensitive to experimental uncertainties in absolute intensities.
It is based on a logarithmic derivative, the L-function:

.. math::

   L(E) = \frac{I'(E)}{I(E)} = \frac{d \ln I(E)}{dE}
   \tag{4.1}

where :math:`I(E)` is the intensity and :math:`I'(E)` its energy derivative.

To avoid numerical instabilities at intensity minima, Pendry introduced the Y-function:

.. math::

   Y(E) = \frac{L(E)}{1 + V_i^2 L(E)^2}
   \tag{4.2}

where :math:`V_i` is the imaginary part of the optical potential (typically 4-5 eV).

Pendry's R-factor is then defined as:

.. math::

   R_P = \frac{\int (Y_{\text{exp}} - Y_{\text{th}})^2 \, dE}
              {\int (Y_{\text{exp}}^2 + Y_{\text{th}}^2) \, dE}
   \tag{4.3}

The range of :math:`R_P` is from 0 (perfect agreement) to approximately 2 (anti-correlation).
Typical values for good structural determinations are:

- :math:`R_P < 0.2`: Excellent agreement
- :math:`R_P \approx 0.2-0.3`: Good agreement
- :math:`R_P > 0.4`: Significant disagreement

4.3 Other R-Factors
-------------------

CLEED also supports alternative R-factors:

**R1 (Zanazzi-Jona R-factor):**

.. math::

   R_1 = \frac{\int |I_{\text{exp}} - c \cdot I_{\text{th}}| \, dE}
              {\int I_{\text{exp}} \, dE}
   \tag{4.4}

where :math:`c` is a scaling constant determined by minimizing :math:`R_1`.

**R2 (Mean-square R-factor):**

.. math::

   R_2 = \frac{\int (I_{\text{exp}} - c \cdot I_{\text{th}})^2 \, dE}
              {\int I_{\text{exp}}^2 \, dE}
   \tag{4.5}

**Rb (Pendry B-factor):**

Similar to :math:`R_P` but using the B-function instead of the Y-function.

4.4 Energy Shift Optimization
-----------------------------

The optimal inner potential (:math:`V_0`) is determined by shifting the energy
scales of experimental and theoretical I-V curves relative to each other to
minimize the R-factor:

.. math::

   R(V_0) = R(E_{\text{th}} - E_{\text{exp}} + V_0)
   \tag{4.6}

This energy shift eliminates one dimension from the parameter space, as the
optimal :math:`V_0` can be determined independently for each trial geometry.

4.5 Averaging and Weighting
---------------------------

When multiple I-V curves are available, the total R-factor is typically a weighted
average:

.. math::

   R_{\text{total}} = \frac{\sum_i w_i \Delta E_i R_i}{\sum_i w_i \Delta E_i}
   \tag{4.7}

where :math:`w_i` is the user-defined weight and :math:`\Delta E_i` is the
energy range of the i-th beam.

4.6 Error Estimation
--------------------

Pendry proposed a variance formula to estimate structural parameter uncertainties:

.. math::

   \text{var}(R_P) \approx R_{P,\min} \sqrt{\frac{8 V_i}{\Delta E_{\text{total}}}}
   \tag{4.8}

where :math:`\Delta E_{\text{total}}` is the cumulative energy range of all
I-V curves. Parameters within the range where :math:`R_P < R_{P,\min} + \text{var}(R_P)`
are considered statistically equivalent.

Typical uncertainties for well-optimized structures are:

- Vertical positions: ±0.02-0.05 Å
- Lateral positions: ±0.05-0.10 Å
- Interlayer spacings: ±0.02-0.04 Å

.. seealso::

   - :doc:`../part-3/ch13-rfactor-general-outline` for CLEED's R-factor program usage
   - :doc:`../part-3/ch14-rfactor-io` for input/output file formats

References
----------

.. [23] J. B. Pendry, J. Phys. C 13 (1980) 937.
