Chapter 10: LEED-specific Functions
===================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 26-29). This chapter includes
   dense equations that need careful verification.

10.1 Introduction
-----------------

The multiple scattering within a single Bravais layer can be written in a compact matrix
notation (see Ref. [43]). The following sections list the relevant functions and the
expressions they implement.

10.2 Multiple scattering Matrix: Single Bravais layer
------------------------------------------------------

Function definitions:

.. code-block:: text

   (lmsbravl.c:)
   int ms_bravl(mat *p_Tpp, mat *p_Rpm,
               struct var_str *v_par, struct layer_str *layer,
               struct beam_str *beams)

   (lmsbravlnd.c:)
   int ms_bravl_nd(mat *p_Tpp, mat *p_Tmm, mat *p_Rpm, mat *p_Rmp,
                  struct var_str *v_par, struct layer_str *layer,
                  struct beam_str *beams)

   (lmslsumii.c:)
   mat ms_lsum_ii(mat Llm, real k_r, real k_i, real *k_in, real *a,
                 int l_max, real epsilon)

   (lmstmatii.c:)
   mat ms_tmat_ii(mat Tii, mat Llm, mat Tl, int l_max)

   (lmstmatndii.c:)
   mat ms_tmat_nd_ii(mat Tii, mat Llm, mat Tlm_in, int l_max)

   (lmsymat.c:)
   mat ms_ymat(mat Ymat, int l_max, struct beam_str *beams, int n_beams)

   (lmsypy.c:)
   mat ms_yp_yxp(mat Yxmat, mat Ymat)
   mat ms_yp_yxm(mat Yxmat, mat Ymat)
   mat ms_yp_ym(mat Ymmat, mat Ypmat)

The function ``ms_bravl`` calculates the multiple scattering matrix for a single Bravais
layer of scatterers. According to equation (??) in Ref. [43], this equation can be written
in matrix notation as:

.. math::

   M = \frac{1}{\kappa \cdot A} Y^{\mathrm{out}\,\pm} \left\{(1 - \tau \cdot G)^{-1}\tau\right\}
   Y^{\mathrm{in}\,\pm}
   \tag{10.1}

The matrices :math:`Y^{\mathrm{in}}` and :math:`Y^{\mathrm{out}}` perform the transformation from
plane-wave representation (used between the layers) into the spherical wave representation
(used inside the layers). They are defined as:

.. math::

   Y^{\mathrm{in}\,\pm}_{lm,\vec{k}} = Y^{\ast}_{lm}(\vec{k}^{\pm})
   \tag{10.2}

computed by the functions ``ms_ymat`` and ``ms_yp_ym``, and

.. math::

   Y^{\mathrm{out}\,\pm}_{\vec{k},lm} = \frac{8i}{k_z} Y_{lm}(\vec{k}^{\pm})
   \tag{10.3}

computed by the functions ``ms_yp_yxp`` and ``ms_yp_yxm``.

The expression embraced by the :math:`Y` matrices describes the multiple scattering
process inside the layer. :math:`G` is the propagator matrix:

.. math::

   G_{l_1m_1,l_2m_2} = \sum_{l_3m_3} i^{l_1 - l_2} (-1)^{(l_2-l_1-l_3)/2 - m_1}
   \, C_{l_1m_1,l_3m_3,l_2m_2} \cdot L_{l_3m_3}
   \tag{10.4}

with :math:`C_{l_1m_1,l_3m_3,l_2m_2}` being a Clebsch-Gordan coefficient and
:math:`L_{l_3m_3}` the lattice sum:

.. math::

   L_{l_3m_3} = (-1)^{m_3} 4\pi \cdot Y_{l_3m_3}\left(\cos \frac{\pi}{2}, 0\right)
   \cdot \sum_{\vec{R}_j} \exp\left[i\vec{k}_{\mathrm{in}}\vec{R}_j - i\varphi(\vec{R}_j)\right]
   \cdot h^{(1)}_{l_3}(\kappa \cdot |\vec{R}_j|)
   \tag{10.5}

computed in the function ``ms_lsum_ii``. :math:`\vec{R}_j` is the position of the *j*th atom
inside the layer; :math:`h^{(1)}_{l_3}` the Hankle function of the first kind, and
:math:`\kappa = \sqrt{2E}` the length of the wave vector.

:math:`\tau` is the atomic scattering matrix *t* times :math:`-\kappa`. The functions
``ms_bravl`` and ``ms_bravl_sym`` assume isotropic scatterers with a diagonal scattering
matrix which only depends on *l* and not on the *m* quantum numbers.

.. math::

   \tau_{l_1m_1,l_2m_2} = -\kappa \cdot t_{l_1m_1,l_2m_2} = e^{i\delta_{l_1}}\sin(\delta_{l_1})
   \, \delta_{l_1l_2}\,\delta_{m_1m_2}
   \tag{10.6}

The diagonality of :math:`\tau` is explicitly used in the function ``ms_tmat_ii`` called by
``ms_bravl`` and ``ms_bravl_sym`` which computes the matrix expression:

.. math::

   (1 - \tau \cdot G)^{-1}\tau
   \tag{10.7}

The function ``ms_tmat_nd_ii`` and the calling function ``ms_bravl_nd`` treat the more
general case of anisotropic scatterers (e.g. anisotropic thermal vibrations) with
non-diagonal scattering matrices which can also depend on *m* (see below).

Before performing any calculations, the function checks whether some quantities needed in
the calculation can be reused from the last call of ``ms_bravl``. In the next steps the
function calculates the lattice sum (if needed) by calling ``ms_lsum_ii``,
:math:`(1 - t \cdot G)^{-1}t` by calling ``ms_tmat_ii``, and the transformation matrices
:math:`Y^{\pm}` by calling ``ms_ymat`` first and then ``ms_yp_yxp`` and ``ms_yp_yxm``,
respectively. After multiplying the outgoing wavefunctions (:math:`Y^{+}`) with
:math:`i 8\pi/(|k|A k'_{\perp})`, the reflection and transmission are put together.

.. TODO: Equation verification screenshots (PDF p27-28):
..   doc/manual/verify/equations/eq-10-1.png (10.1)
..   doc/manual/verify/equations/eq-10-2-3.png (10.2-10.3)
..   doc/manual/verify/equations/eq-10-4.png (10.4)
..   doc/manual/verify/equations/eq-10-5.png (10.5)
..   doc/manual/verify/equations/eq-10-6.png (10.6)
..   doc/manual/verify/equations/eq-10-7.png (10.7)

10.3 Multiple scattering: combined space method
-----------------------------------------------

.. TODO (PDF p29): The section heading is present but no body text appears on the page.

10.4 Layer Doubling
-------------------

.. TODO (PDF p29): The section heading is present but no body text appears on the page.
