Chapter 11: Quantum Mechanical Functions
========================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 30-32). This chapter contains
   equations and code-like function signatures that need verification.

11.1 Introduction
-----------------

This chapter summarises the quantum mechanical utility functions used by CLEED,
primarily Clebsch-Gordan coefficients, spherical harmonics, and spherical Hankel
functions.

11.2 Clebsch-Gordan Coefficients
-------------------------------

(qmcgc.c:)

.. code-block:: text

   int mk_cg_coef(int l_max)

   double cg(int l1, int m1, int l2, int m2, int l3, int m3)
   double blm(int l1, int m1, int l2, int m2, int l3, int m3)
   double gaunt(int l1, int m1, int l2, int m2, int l3, int m3)

All necessary Clebsch-Gordan coefficients (or Gaunt coefficients) up to
:math:`l_{\max}` are calculated once by the function ``mk_cg_coef``. The coefficients are
stored in a list and can be read from there by using the functions ``cg``, ``blm``, or
``gaunt``. These functions read from the same list but return different values.

``double cg(int l1, int m1, int l2, int m2, int l3, int m3)`` returns the value of the
integral described in Slater's book:

.. math::

   (-1)^{m_2} \int Y_{l_1m_1}(\Omega) Y^{\ast}_{l_2m_2}(\Omega) Y_{l_3m_3}(\Omega)\,d\Omega

Allowed (non-zero) values:

.. math::

   0 \le l_1 \le 2\cdot l_{\max}, \quad 0 \le l_{2,3} \le l_{\max}

.. math::

   m_1 = m_2 + m_3 \quad \Leftrightarrow \quad m_1 - m_2 - m_3 = 0

.. math::

   |l_2 - l_3| \le l_1 \le l_2 + l_3

Transformations:

.. math::

   cg(l_1,m_1,l_2,m_2,l_3,m_3) = cg(l_1,m_1,l_3,m_3,l_2,m_2)

.. math::

   cg(l_1,m_1,l_2,m_2,l_3,m_3) = cg(l_2,m_2,l_1,m_1,l_3,-m_3)

``double blm(int l1, int m1, int l2, int m2, int l3, int m3)`` returns the value of the
integral used in Pendry's book:

.. math::

   \int Y_{l_1m_1}(\Omega) Y_{l_2m_2}(\Omega) Y_{l_3m_3}(\Omega)\,d\Omega

Allowed (non-zero) values:

.. math::

   0 \le l_2 \le 2\cdot l_{\max}, \quad 0 \le l_{1,3} \le l_{\max}

.. math::

   m_1 + m_2 + m_3 = 0

.. math::

   |l_1 - l_3| \le l_2 \le l_1 + l_3

Transformations:

.. math::

   blm(l_1,m_1,l_2,m_2,l_3,m_3) = cg(l_2,-m_2,l_1,m_1,l_3,m_3)

``double gaunt(int l1, int m1, int l2, int m2, int l3, int m3)`` returns Gaunt's
integral:

.. math::

   (-1)^{m_3} \int Y_{l_1m_1}(\Omega) Y_{l_2m_2}(\Omega) Y^{\ast}_{l_3m_3}(\Omega)\,d\Omega

Allowed (non-zero) values:

.. math::

   0 \le l_2 \le 2\cdot l_{\max}, \quad 0 \le l_{1,3} \le l_{\max}

.. math::

   m_3 = m_1 + m_2 \quad \Leftrightarrow \quad m_1 + m_2 - m_3 = 0

.. math::

   |l_2 - l_3| \le l_1 \le l_2 + l_3

Transformations:

.. math::

   gaunt(l_1,m_1,l_2,m_2,l_3,m_3) = cg(l_2,m_2,l_1,-m_1,l_3,m_3)

The memory requirements of the list created by ``mk_cg_coef`` are for a given value of
:math:`l_{\max}`:

.. math::

   \frac{(2l_{\max}+1)(2l_{\max}+2)}{2} \cdot (l_{\max}+1)^2\left(\frac{l_{\max}}{2} + 1\right)
   \cdot \mathrm{sizeof(double)}

.. list-table:: Memory requirements for Clebsch-Gordan coefficients
   :header-rows: 1

   * - :math:`l_{\max}`
     - Memory (bytes)
   * - 6
     - 142,688
   * - 8
     - 495,720
   * - 10
     - 1,341,648
   * - 12
     - 3,075,800
   * - 14
     - 6,264,000
   * - 16
     - 11,673,288

.. TODO:: Equation verification screenshots (PDF p30-31):
..   doc/manual/verify/equations/eq-11-cg.png (cg integral + allowed values)
..   doc/manual/verify/equations/eq-11-cg-transform.png (cg transformations)
..   doc/manual/verify/equations/eq-11-blm.png (blm integral + allowed values)
..   doc/manual/verify/equations/eq-11-blm-transform.png (blm transformation + gaunt lead-in)
..   doc/manual/verify/equations/eq-11-gaunt.png (gaunt integral + allowed values)
..   doc/manual/verify/equations/eq-11-memory.png (memory requirement + table)

11.3 Spherical Harmonics
------------------------

(qmylm.c:)

.. code-block:: text

   mat r_ylm(mat Ylm, real x, real phi, int l_max)
   mat c_ylm(mat Ylm, real z_r, real z_i, real phi, int l_max)

(lmsymat.c:)

(lmsypy.c:)

11.4 Spherical Hankel Functions
-------------------------------

(qmhank.c:)

.. code-block:: text

   mat r_hankl(mat Hl, real x, int l_max)
   mat c_hankl(mat Hl, real z_r, real z_i, int l_max)
