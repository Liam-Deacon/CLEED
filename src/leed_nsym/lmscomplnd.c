/*********************************************************************
 *                      LMSCOMPL.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 * (leed_ms_compl)
 *   GH/03-13.04.95 - Creation (it took only 6 days to create the world
 *                 but it took more than 2 months to create this function!)
 *   GH/12.07.95 - correct signs in giant matrix (Mbg) and wave propagators.
 *   GH/14.07.95 - change directions (z is opposite to VHT convention)
 *   GH/28.07.95 - change parameter list (layer instead of atoms etc.).
 *                 replace matrix array Tii by pointer p_Tii.
 *                 find smallest l_max necessary.
 *   GH/04.08.95 - order atoms such that the most populated plane comes first.
 *   GH/07.08.95 - perform matrix inversion by partitioning (function
 *                 ms_partinv instead of matinv).
 * (leed_ms_compl_nd)
 *   GH/23.09.00 - extension for non-diagonal T matrix.
 *   GH/17.07.02 - bug fixes for non-diagonal T matrix:
 *                 = Copy atom information by memcpy.
 *                 = Set l_max equal to v_par->l_max for T_NOND.
 *********************************************************************/

/*! \file
 *
 * Implements calculation of scattering matrix for a composite layer using the
 * combined space method.
 */

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"

#define CPUTIME

#ifdef CPUTIME
static char cpu_msg[STRSZ] = "";
#define CTIME(x) do {snprintf(cpu_msg, STRSZ-1, "%s: %s", __func__, (x)); \
  cpu_msg[STRSZ-1]="\-0"; leed_cpu_time(STDCPU, cpu_msg);} while(0)
#else
#define CTIME(x) 
#endif

/*!
 * Multiplies \p M with cols of \p R and rows with \p L
 */
__attribute__((nonnull))
static inline void leed_ms_matmul_Rx_Lx_helper(mat M, const mat R, const mat L)
{
  real ptr_r, ptr_i, ptr_end;
  size_t k, l;
  for(k = 1; k <= M->rows; k++)         /* loop over row No's (1st index) */
  {
    for(ptr_r = M->rel + (k-1)*M->cols + 1,
        ptr_i = M->iel + (k-1)*M->cols + 1,
        ptr_end = ptr_r + M->cols, l = 1;
        ptr_r < ptr_end;
        ptr_r ++, ptr_i ++, l ++)         /* loop over col No's (2nd index) */
    {
      cri_mul(ptr_r, ptr_i, *(L->rel+k), *(L->iel+k), *ptr_r, *ptr_i);
      cri_mul(ptr_r, ptr_i, *(R->rel+l), *(R->iel+l), *ptr_r, *ptr_i);
    }
  }
}

/*!
 * Calculates the scattering matrix for a composite layer using the combined
 * space method.
 *
 * The algorithm design is based on the following:
 *
 * \f[  R/T(k'k) = pref_i * |k'><l,m|M(lm,l'm')|l'm'><k| + dkk' \f]
 *
 * \f [ Y_{lm} =  |k'><l,m| = Y_{lm}(k') \f]
 * for (k_max rows) x ((l_max+1)^2 columns)
 *
 * pref_i = \f$ -i \times 16 \pi^2 \times EXP(A k' z+) \f$
 * EXP = exp[i(-k(+)r1 + k'(+)rN)] for ++
 *       exp[i(-k(+)r1 - k'(-)r1)] for -+
 *       exp[i(+k(-)rN - k'(-)r1)] for --
 *       exp[i(+k(-)rN + k'(+)rN)] for +-
 *
 * r1 = (0, 0, (atoms+0)->pos[3]).
 *      Projection of the origin to the atomic sub-plane with the greatest
 *      z coordinate (atom No. 0):
 *
 * rN = (0, 0, (atoms+n_atoms-1)->pos[3]).
 *      Projection of the origin to the atomic sub-plane with the largest
 *      z coordinate (atom No. n_atoms-1):
 *
 * \param[out] p_Tpp Pointer to the composite layer diffraction matrix in
 * k-space for the k(+) -> k(+) transmission matrix \f$ \textbf{T}^{++} \f$
 * \param[out] p_Tmm Pointer to the composite layer diffraction matrix in
 * k-space for the k(-) -> k(-) transmission matrix \f$ \textbf{T}^{--} \f$
 * \param[out] p_Rpm Pointer to the composite layer diffraction matrix in
 * k-space for the k(-) -> k(+) reflection matrix \f$ \textbf{R}^{-+} \f$
 * \param[out] p_Rmp Pointer to the composite layer diffraction matrix in
 * k-space for the k(+) -> k(-) reflection matrix \f$ \textbf{R}^{+-} \f$
 * \param[in,out] v_par Pointer to #leed_var structure containing parameters
 * which vary during the energy loop. The members
 * @leed_var::k_in (parallel component of the incoming beam),
 * @leed_var::l_max (maximum angular momentum quantum number),
 * @leed_var::tl (atomic scattering factors) and @leed_var::epsilon
 * (smallest relative wave amplitude to be considered) are used.
 * \param[in] layer Pointer to array of #leed_layer structures containing all
 * relevant information about the composite layer. The members
 * @leed_layer::n_atoms (number of atoms in the layer),
 * @leed_layer::a_lat (lattice vectors) and
 * @leed_layer::rel_area (area of unit cell relative to 1x1) are used.
 * \param[in] beams Pointer to array of #leed_beam structures containing
 * additional information about the k-space vectors involved
 * (in this case: \f$ {A kz}^{-1} \f$ ). The order of beams must be equal to
 * the first dimension of \f$ Y_{kl} \f$ (not checked).
 * \return Error code indicating success (note: C function convention is used).
 * \retval 0 if successful.
 * \retval -1 if failed and #EXIT_ON_ERROR not defined.
 * \see matarralloc() , matalloc() , matfree() , matcheck() , matins() ,
 * matinv() , matmul() , leed_ms_lsum_ii() , leed_ms_lsum_ij() ,
 * leed_ms_tmat_ii() , leed_ms_tmat_ij() and ms_partinv()
 */
int leed_ms_compl_nd(mat *p_Tpp, mat *p_Tmm, mat *p_Rpm, mat *p_Rmp,
                     leed_var *v_par, leed_layer *layer, leed_beam *beams)
{
  size_t iaux;
  size_t uaux, off_row, off_col;

  size_t l_max, l_max_2;
  int i_type, t_type, n_type;
  size_t n_atoms, i_atoms, j_atoms;
  size_t n_beams, k;
  size_t l;
  size_t n_plane;


  real d_ij[4];
  real faux_r, faux_i;
  real pref_i;										/* pref_i is used as common prefactor */

  real z_max, z_min;              /* z coordinates of the outer-most atoms */
  real z_plane;                   /* z coordinate ofo the most populated plane */

  real *ptr_r, *ptr_i, *ptr_end;
  
  leed_atom *atoms;         			/* atomic positions and scattering properties */
  
  mat Ylm;                        /* spherical harmonics (for exit beams) */
  mat Llm_ij, Llm_ji;             /* interlayer lattice sums */
  mat Maux, Mbg, Mark;            /* dummy matrices */
  mat L_p, L_m, R_p, R_m;         /* dummy matrices */
  
  mat Tpp, Tmm, Rpm, Rmp;         /* Layer diffraction matrices in k-space
                                   will be copied to output */
  mat * p_Tii;                    /* Array of Bravais layer scattering matrices */

  Ylm = NULL;

  Llm_ij = NULL;
  Llm_ji = NULL;

  Mark = NULL;
  Maux = NULL;
  Mbg = NULL;

  L_p = NULL;
  L_m = NULL;
  R_p = NULL;
  R_m = NULL;

  CTIME("start of function\t\t");

  /* Check the validity of input matrices p_T/R
   * iaux is used as error flag */
  iaux = 0;
  if (matcheck(*p_Tpp)  < 0)
  {
    ERROR_MSG("invalid input matrix (1st argument)\n");
    iaux = 1;
  }
  if (matcheck(*p_Tmm)  < 0)
  {
    ERROR_MSG("invalid input matrix (2nd argument)\n");
    iaux = 1;
  }
  if (matcheck(*p_Rmp)  < 0)
  {
    ERROR_MSG("invalid input matrix (3rd argument)\n");
    iaux = 1;
  }
  if (matcheck(*p_Rpm)  < 0)
  {
    ERROR_MSG("invalid input matrix (4th argument)\n");
    iaux = 1;
  }

  if(iaux != 0)
  {
    ERROR_EXIT_RETURN(1, -1);
  }

  /* Prepare Calculation:
   * (i) Allocate memory for array atoms and copy the relevant information
   * into the list.
   * Find max number of atom types (n_type),
   * outer-most z coordinates (z_min and z_max), and max. l necessary.
   *
   * (ii) Sort atoms according to their z position (pos[3]): smallest z first.
   */

  /* (o) preset often used values */
  Tpp = *p_Tpp;
  Tmm = *p_Tmm;
  Rpm = *p_Rpm;
  Rmp = *p_Rmp;

  /* find number of beams */
  for(n_beams = 0;
      ! IS_EQUAL_REAL((beams + n_beams)->k_par, F_END_OF_LIST);
      n_beams ++);

  /* (i) Copy layer->atoms to atoms,
   * find max number of atom types and outer-most z coordinates
   * find maximum l necessary for all atom types
   */
  n_atoms = layer->n_atoms;
  CLEED_ALLOC_CHECK(atoms = (leed_atom *) calloc(n_atoms+1, sizeof(leed_atom)));

  n_type = atoms[0].type;
  z_min = z_max = atoms[0].pos[3];
  l_max = 1;

  CONTROL_MSG(CONTROL, "z_min = z_max =%f n_atoms = %d\n", z_min, n_atoms);

  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {

    /* copy atom information */
    memcpy( &atoms[i_atoms], &layer->atoms[i_atoms], sizeof(leed_atom));
   
    CONTROL_MSG(CONTROL, "atom%d  z = %.3f\n", i_atoms,
                          layer->atoms[i_atoms].pos[3]);

    /* find n_type and z_min/z_max */
    n_type = MAX( atoms[i_atoms].type, n_type);

    z_max = MAX( atoms[i_atoms].pos[3], z_max);
    z_min = MIN( atoms[i_atoms].pos[3], z_min);

    /* Find maximum l necessary */
    i_type = atoms[i_atoms].type;
    t_type = atoms[i_atoms].t_type;
    if(t_type == T_DIAG)
    {
      for( iaux = v_par->l_max;
           (cri_abs( (v_par->p_tl[i_type])->rel[iaux+1],
                     (v_par->p_tl[i_type])->iel[iaux+1] ) < v_par->epsilon ) &&
           (iaux > 1);
           iaux --)
      {;}
    }
    else
    {
      iaux = v_par->l_max;
    }
    l_max = MAX(l_max, iaux);
  }

  CONTROL_MSG(CONTROL, "z_min = %f  z_max =%f lmax = %d\n", z_min, z_max, l_max);

  atoms[n_atoms].type = I_END_OF_LIST;   /* terminate list atoms */
  n_type ++;                               /* n_type = number of types */

  /* this would reset l_max to its maximum value */

  /* l_max = v_par->l_max; */
  l_max_2 = (l_max+1)*(l_max+1);

  CONTROL_MSG(CONTROL, "l_max = %d, No of beams = %d, "
      "No of atoms = %d\n",  l_max, n_beams, n_atoms);
  CONTROL_MSG(CONTROL, "before sorting:\n");

  /* (ii)a
   * Find plane containing most atoms
   */
  z_plane = atoms[0].pos[3];
  n_plane = 1;

  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {
    iaux = 0;
    for(j_atoms = 0; j_atoms < n_atoms; j_atoms ++)
    {
      if( cleed_real_fabs( (atoms+j_atoms)->pos[3] - atoms[i_atoms].pos[3] )
          < GEO_TOLERANCE )
      { iaux ++; }
    }    /* for j_atoms */
   
    if(iaux > n_plane)
    {
      n_plane = iaux;
      z_plane = atoms[i_atoms].pos[3];
    }

    CONTROL_MSG(CONTROL, "\t(%d) pos: (%5.2f,%5.2f,%5.2f) A type: %d\n",
            i_atoms, atoms[i_atoms].pos[1]*BOHR,
            atoms[i_atoms].pos[2]*BOHR, atoms[i_atoms].pos[3]*BOHR,
            atoms[i_atoms].type);
  }    /* for i_atoms */

  CONTROL_MSG(CONTROL, "z_plane: %.4f, n_plane: %d\n", z_plane*BOHR, n_plane);

  /* (ii)b
   * Move atoms of the most populated plane to the front of atoms list
   */
  for(i_atoms = 0, j_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {
    if( cleed_real_fabs( atoms[i_atoms].pos[3] - z_plane ) < GEO_TOLERANCE )
    {
      /* Exchange atom parameters if i_atoms != j_atoms */
      if( i_atoms != j_atoms )
      {
        iaux = atoms[i_atoms].type;
        atoms[i_atoms].type = atoms[j_atoms].type;
        atoms[j_atoms].type = iaux;
  
        for(iaux = 1; iaux <= 3; iaux ++)
        {
          faux_r = atoms[i_atoms].pos[iaux];
          atoms[i_atoms].pos[iaux] = atoms[j_atoms].pos[iaux];
          atoms[j_atoms].pos[iaux] = faux_r;
        }
      }  /* if i_atoms ... */

      j_atoms ++;
    }  /* if cleed_real_fabs ... */
  }  /* for i_atoms */

  #if CONTROL
  CONTROL_MSG(CONTROL, "after sorting:\n");
  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {
    fprintf(STDCTR, "\t(%d) pos: (%5.2f,%5.2f,%5.2f) A type: %d %d\n",
           i_atoms, atoms[i_atoms].pos[1]*BOHR,
           atoms[i_atoms].pos[2]*BOHR, atoms[i_atoms].pos[3]*BOHR,
           atoms[i_atoms].type, atoms[i_atoms].t_type);
  }  /* for i_atoms */
  #endif

  /* Create Bravais layer scattering matrices Tii
   * - Allocate a matrix array p_Tii n_type long (max. number of different atoms)
   * - Calculate lattice sum for a single Bravais lattice (stored in L_ij).
   * - Calculate the single Bravais lattice scattering matrix for each atom
   *   type and multiply with factor -1/2k0.
   */
  CLEED_ALLOC_CHECK(p_Tii = (mat *) calloc((size_t)n_type, sizeof(mat)));

  CONTROL_MSG(CONTROL_X, "Calculate Bravais lattice sum\n");

  /* Calculate Bravais lattice sum (only once) */
  Llm_ij = leed_ms_lsum_ii(Llm_ij, beams->k_r[0], beams->k_i[0],
                     v_par->k_in, layer->a_lat, 2 * l_max, v_par->epsilon );
 
  CONTROL_MSG(CONTROL_X, "Calculate scattering matrices\n");

  /* Compute scattering matrices Tii[type] for Bravais lattices and store in p_Tii */
  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {
    i_type = atoms[i_atoms].type;
    t_type = atoms[i_atoms].t_type;

    /* t_type = T_NOND;*/
    if( p_Tii [i_type] == NULL )
    {
      CONTROL_MSG(CONTROL, "before leed_ms_tmat_ii (%d): "
              "i_type = %d t_type = %d (%s)\n", i_atoms, i_type, t_type,
              t_type == T_DIAG ? "T_DIAG" : "T_NOND");

      if(t_type == T_DIAG)
      {
        p_Tii[i_type] = leed_ms_tmat_ii(p_Tii[i_type], Llm_ij,
                                        v_par->p_tl[i_type], l_max);
        p_Tii[i_type] = mattrans(p_Tii [i_type], p_Tii [i_type]);
      }
      else if(t_type == T_NOND)
      {
        p_Tii[i_type] = leed_ms_tmat_nd_ii(p_Tii[i_type], Llm_ij,
                                           v_par->p_tl[i_type], l_max);
      }
      else
      {
        ERROR_MSG("unidentified t_type for atom No %d\n", i_atoms);
        ERROR_EXIT_RETURN(1, -1);
      }

      /* Multiply matrix elements of Tii[type] with -1/2k0 */
      cri_div(&faux_r, &faux_i, -0.5, 0., beams->k_r[0], beams->k_i[0]);
      for(ptr_r = (p_Tii[i_type])->rel + 1, ptr_i = (p_Tii[i_type])->iel + 1,
          ptr_end = (p_Tii[i_type])->rel +
                    (p_Tii[i_type])->cols * (p_Tii[i_type])->rows;
          ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
      {
        cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
      }

    } /* if == NULL */

  } /* for i_atoms */

  /* Giant Matrix Inversion
   * - Allocate giant matrix Mbg to be inverted.
   * - Create interlayer propagators Gij/Gji (Maux)
   * - Calculate -Tii * Gij and -Tjj * Gji and copy into Mbg.
   * - Add identity to Mbg and invert giant matrix.
   * - free storage space for interlayer lattice sums.
   */
  iaux = l_max_2 * n_atoms;
  Mbg  = matalloc(Mbg, iaux, iaux, NUM_COMPLEX);
  Mark = matalloc(Mark, n_atoms, n_atoms, NUM_REAL);

  for(i_atoms = 0, off_row = 1; i_atoms < n_atoms;
      i_atoms ++, off_row += l_max_2)
  {
    for(j_atoms = i_atoms + 1, off_col = off_row + l_max_2;
        j_atoms < n_atoms;
        j_atoms ++, off_col += l_max_2)
      if( ! IS_EQUAL_REAL(RMATEL(i_atoms+1, j_atoms+1, Mark), 1.))
      {
        /* d_ij = vector rj - ri */
        d_ij[1] = atoms[j_atoms].pos[1] - atoms[i_atoms].pos[1];
        d_ij[2] = atoms[j_atoms].pos[2] - atoms[i_atoms].pos[2];
        d_ij[3] = atoms[j_atoms].pos[3] - atoms[i_atoms].pos[3];

        CONTROL_MSG(CONTROL, "d(%d->%d) = (%5.2f, %5.2f, %5.2f) A\n",
                i_atoms, j_atoms, d_ij[1]*BOHR, d_ij[2]*BOHR, d_ij[3]*BOHR);

        leed_ms_lsum_ij(&Llm_ij, &Llm_ji, beams->k_r[0], beams->k_i[0],
                  v_par->k_in, layer->a_lat, d_ij, 2 * l_max, v_par->epsilon );

        #ifdef CONTROL_LSUM
        Maux = mattrans(Maux, Llm_ij);
        fprintf(STDCTR, "%s: Lij\n%s", __func__, matshow(Maux));
        #endif

        /* Copy matrix Tjj * Gji to position (j,i) = (off_col,off_row)
         * and matrix Tii * Gij to position (i,j) = (off_row,off_col)
         */
        Maux = leed_ms_tmat_ij(Maux, Llm_ij, p_Tii[(atoms+j_atoms)->type], l_max);
        Mbg  = matins(Mbg, Maux, off_col, off_row);

        Maux = leed_ms_tmat_ij(Maux, Llm_ji, p_Tii[atoms[i_atoms].type], l_max);
        Mbg  = matins(Mbg, Maux, off_row, off_col);

        /* - Mark the positions in Mark.
         * - Check for other occurences of the same interlayer vector
         */
        RMATEL(i_atoms+1, j_atoms+1, Mark) = 1.;
        RMATEL(j_atoms+1, i_atoms+1, Mark) = 1.;

        for( k = i_atoms + 1; k < n_atoms; k ++)
          for( l = i_atoms + 1; l < n_atoms; l ++)
          {
            if((cleed_real_fabs((atoms+l)->pos[1]-(atoms+k)->pos[1]-d_ij[1]) < GEO_TOLERANCE)
               && (cleed_real_fabs((atoms+l)->pos[2]-(atoms+k)->pos[2]-d_ij[2]) < GEO_TOLERANCE)
               && (cleed_real_fabs((atoms+l)->pos[3]-(atoms+k)->pos[3]-d_ij[3]) < GEO_TOLERANCE))
            {
              /* Maux = matmul(Maux, p_Tii[(atoms+k)->type] , Maux1);
               * Mbg  = matins(Mbg, Maux, k*l_max_2+1, l*l_max_2+1);
               * Maux = matmul(Maux, p_Tii[(atoms+l)->type] , Maux2);
               * Mbg  = matins(Mbg, Maux, l*l_max_2+1, k*l_max_2+1);
               * RMATEL(k+1, l+1, Mark) = 1.;
               * RMATEL(l+1, k+1, Mark) = 1.;
               */
              ;
              CONTROL_MSG(CONTROL, "         = d(%d->%d)\n", k, l);
            } /* if cleed_real_fabs ... */
          } /* for k,l */
      } /* for j_atoms / if not marked */
  } /* for i_atoms */
 
  /* free memory */
  matfree(Llm_ij);
  matfree(Llm_ji);
  matfree(Mark);

  /* Add identity to Mbg */
  for(ptr_r = Mbg->rel+1, ptr_end = Mbg->rel + Mbg->cols*Mbg->rows;
      ptr_r <= ptr_end; ptr_r += Mbg->cols +1)
  {
    *ptr_r += 1.;
  }

  CONTROL_MSG(CONTROL, "giant matrix inversion (%d x %d), "
      "E = %.1f eV ...\n", Mbg->cols, Mbg->rows, v_par->eng_v*HART);

  #ifdef CONTROL_MBG
  matnattovht(Mbg, l_max, n_atoms);
  #endif

  CTIME("before giant matrix inversion");

  Mbg = ms_partinv(Mbg, Mbg, n_plane, l_max);

  /*  ALTERNATIVES
   Mbg = matinv(Mbg, Mbg);
   Mbg = ms_partinv(Mbg, Mbg, n_plane, l_max);
  */

  CONTROL_MSG(CONTROL, " ... completed\n");
  CTIME("after giant matrix inversion");

  /* Prepare matrices for conversion into plane waves:
   *
   * L_p(g',jlm) = Ylm(g'+) * exp(- ikg'(+) * rj)
   *  L_m(g',jlm) = Ylm(g'-) * exp(+ ikg'(-) * rj)
   * to be multiplied with Mbg from the l.h.s. and
   *  R_p(ilm',g)  = exp(+ ikg(+) * ri) *Tii * Ylm'*(g+)
   *  R_m(ilm',g)  = exp(- ikg(-) * ri) *Tii * Ylm'*(g-)
   * to be multiplied from the r.h.s.
   *
   * Ylm = spherical harmonics.
   * kg(+/-) = (kp, +/-kz)
   *   where kp = parallel componets of k
   *         kz = sqrt(2E - kp^2) (positive)
   */

  /* prefactor including relative area of unit cell */
  pref_i = -16.*PI*PI / layer->rel_area;

  CONTROL_MSG(CONTROL, "relative u.c. area: %.3f\n", layer->rel_area);

  /* calculate spherical harmonics Ylm */
  Ylm = leed_ms_ymat(Ylm, l_max, beams, n_beams);

  /* allocate storage space (Ylm->rows = number of beams) */
  iaux = l_max_2 * n_atoms;
  L_p = matalloc(L_p, n_beams, iaux, NUM_COMPLEX);
  L_m = matalloc(L_m, n_beams, iaux, NUM_COMPLEX);

  R_p = matalloc(R_p, iaux, n_beams, NUM_COMPLEX);
  R_m = matalloc(R_m, iaux, n_beams, NUM_COMPLEX);

  CONTROL_MSG(CONTROL, "Prepare matrices R_x and L_x (%d x %d)\n", n_beams, iaux);

  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {
    off_row = i_atoms * l_max_2 + 1;

    /* R_p(ilm',g)  = exp(+ ikg(+) * ri) *Tii * Ylm'*(g+) */
    Maux = leed_ms_yp_yxm(Maux, Ylm);
    Maux = matmul(Maux, p_Tii[atoms[i_atoms].type], Maux);

    /* Multiply the cols of Maux with exp(- ikg(+) * ri) */
    for(k = 0; k < Maux->cols; k ++)
    {
      faux_r = +beams[k].k_r[1] * atoms[i_atoms].pos[1]
               +beams[k].k_r[2] * atoms[i_atoms].pos[2]
               +beams[k].k_r[3] * atoms[i_atoms].pos[3];
      faux_i = +beams[k].k_i[3] * atoms[i_atoms].pos[3];
      cri_expi(&faux_r, &faux_i, faux_r, faux_i);
      for(ptr_r = Maux->rel + k + 1,
          ptr_i = Maux->iel + k + 1,
          ptr_end = Maux->rel + Maux->cols*Maux->rows;
          ptr_r <= ptr_end; ptr_r += Maux->cols, ptr_i += Maux->cols)
      {
        cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
      }
    } /* k */

    #ifdef CONTROL_XXX
    if(i_atoms == 0)
    {
      fprintf(STDCTR, "%s: R_p (atom%d)\n%s", __func__, i_atoms, matshow(Maux);
    }
    #endif

    R_p  = matins(R_p, Maux, off_row, 1);

    /* R_m(ilm',g)  = exp(+ ikg(-) * ri) *Tii * Ylm'*(g-) */
    Maux = leed_ms_yp_yxp(Maux, Ylm);
    Maux = matmul(Maux, p_Tii[atoms[i_atoms].type], Maux);

    /* Multiply the cols of Maux with exp(- ikg(-) * ri) */
    for(k = 0; k < Maux->cols; k ++)
    {
      faux_r = +beams[k].k_r[1] * atoms[i_atoms].pos[1]
               +beams[k].k_r[2] * atoms[i_atoms].pos[2]
               -beams[k].k_r[3] * atoms[i_atoms].pos[3];
      faux_i = -beams[k].k_i[3] * atoms[i_atoms].pos[3];
      cri_expi(&faux_r, &faux_i, faux_r, faux_i);
      for(ptr_r = Maux->rel + k + 1,
          ptr_i = Maux->iel + k + 1,
          ptr_end = Maux->rel + Maux->cols*Maux->rows;
          ptr_r <= ptr_end; ptr_r += Maux->cols, ptr_i += Maux->cols)
      {
        cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
      }
    } /* k */

    R_m  = matins(R_m, Maux, off_row, 1);

    /* L_p(g',jlm) = Ylm(g'+) * exp(- ikg'(+) * rj) */
    Maux = leed_ms_yp_ym(Maux, Ylm);
   
    /* Multiply the rows of Maux with exp(- ikg(+) * ri) */
    for(k = 0; k < Maux->rows; k ++)
    {
      faux_r = -beams[k].k_r[1] * atoms[i_atoms].pos[1]
               -beams[k].k_r[2] * atoms[i_atoms].pos[2]
               -beams[k].k_r[3] * atoms[i_atoms].pos[3];
      faux_i = -beams[k].k_i[3] * atoms[i_atoms].pos[3];

      cri_expi(&faux_r, &faux_i, faux_r, faux_i);
      cri_mul (&faux_r, &faux_i, faux_r, faux_i,
               beams[k].Akz_r, beams[k].Akz_i);
      cri_mul (&faux_r, &faux_i, faux_r, faux_i, 0., pref_i);

      for(ptr_r = Maux->rel + k*Maux->cols + 1,
          ptr_i = Maux->iel + k*Maux->cols + 1,
          ptr_end = ptr_r + Maux->cols;
          ptr_r < ptr_end; ptr_r ++,  ptr_i ++)
      {
        cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
      }
    } /* k */

    #ifdef CONTROL_XXX
    if(i_atoms == 0)
    {
      fprintf(STDCTR, "%s: R_p (atom%d)\n%s%s", __func__, i_atoms,
              matshow(Maux), matshowabs(Maux));
    }
    #endif

    L_p  = matins(L_p, Maux, 1, off_row);

    /* L_m(g',jlm) = Ylm(g'-) * exp(- ikg'(-) * rj) */
    Maux = matcopy(Maux, Ylm);

    /* Multiply the rows of Maux with exp(- ikg'(-) * ri) */
    for(k = 0; k < Maux->rows; k ++)
    {
      faux_r = -beams[k].k_r[1] * atoms[i_atoms].pos[1]
               -beams[k].k_r[2] * atoms[i_atoms].pos[2]
               +beams[k].k_r[3] * atoms[i_atoms].pos[3];
      faux_i = +beams[k].k_i[3] * atoms[i_atoms].pos[3];
      cri_expi(&faux_r, &faux_i, faux_r, faux_i);
      cri_mul (&faux_r, &faux_i, faux_r, faux_i,
               beams[k].Akz_r, beams[k].Akz_i);
      cri_mul (&faux_r, &faux_i, faux_r, faux_i, 0., pref_i);

      for(ptr_r = Maux->rel + k*Maux->cols + 1,
          ptr_i = Maux->iel + k*Maux->cols + 1,
          ptr_end = ptr_r + Maux->cols;
          ptr_r < ptr_end; ptr_r ++,  ptr_i ++)
      {
        cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
      }
    } /* k */

    L_m  = matins(L_m, Maux, 1, off_row);

  }

  CTIME("after preparation of R_p ... ");
  matfree(Ylm);

  /* Multiply matrices: L*Mbg*R */
  Maux = matmul(Maux, Mbg, R_p);
  Tpp = matmul(Tpp, L_p, Maux);
  Rmp = matmul(Rmp, L_m, Maux);

  Maux = matmul(Maux, Mbg, R_m);
  Tmm = matmul(Tmm, L_m, Maux);
  Rpm = matmul(Rpm, L_p, Maux);

  CTIME("after multiplication R * Mbg * L");

  CONTROL_MSG(CONTROL, "... completed\n");

  matfree(Maux);
  matfree(Mbg);

  /* Extrapolation of origin and Prefactor:
   * Multiply the matrix elements with
   * EXP = exp[i(+k'(+)rN - k(+)r1)] for ++
   *       exp[i(+k'(-)r1 - k(+)r1)] for -+
   *       exp[i(+k'(-)r1 - k(-)rN)] for --
   *       exp[i(+k'(+)rN - k(-)rN)] for +-
   *
   * r1 = (0, 0, (atoms+0)->pos[3]).
   *     Projection of the origin to the atomic subplane with the smallest
   *     z coordinate (atom No. 0):
   *
   * rN = (0, 0, (atoms+n_atoms-1)->pos[3]).
   *     Projection of the origin to the atomic subplane with the largest
   *     z coordinate (atom No. n_atoms-1):
   *
   * =>
   * EXP = exp[i(+k'[3]rN - k[3]r1)] for ++
   *       exp[i(-k'[3]r1 - k[3]r1)] for -+
   *       exp[i(-k'[3]r1 + k[3]rN)] for --
   *       exp[i(+k'[3]rN + k[3]rN)] for +-
   *
   * Set up vectors L_p/m and R_p/m containing the appropriate factors and
   * multiply the above matrices R/T with those vectors from l.h.s and r.h.s.,
   * respectively.
   * Loop over k' (exit beams): rows of T/Rk'k, i.e. l.h.s
   * - Multiply with factor exp[- ik'(+/-)rN1]
   * - reuse L_p/m
   * Loop over k (incoming beams): cols of T/Rk'k, i.e. r.h.s
   * - Multiply with factor exp[+ ik(+/-)r1N]
   * - reuse R_p/m
   */

  CONTROL_MSG(CONTROL, "origin shift ... \n");

  L_p = matalloc(L_p, 1, n_beams, NUM_COMPLEX);
  L_m = matalloc(L_m, 1, n_beams, NUM_COMPLEX);

  R_p = matalloc(R_p, n_beams, 1, NUM_COMPLEX);
  R_m = matalloc(R_m, n_beams, 1, NUM_COMPLEX);

  /* Set up vectors L_p/m and R_p/m */
  for(k = 0; k < n_beams; k ++)
  {
    /* R_m (exp[- ikz(-)zmax) = L_p (exp[+ ikz(+)zmax) */
    faux_r = +beams[k].k_r[3] * z_max;
    faux_i = +beams[k].k_i[3] * z_max;
    cri_expi(R_m->rel+k+1, R_m->iel+k+1, faux_r, faux_i);
    L_p->rel[k+1] = R_m->rel[k+1];
    L_p->iel[k+1] = R_m->iel[k+1];

    /* R_p (exp[- ik(+)zmin) = L_m (exp[+ ik(-)zmin) */
    faux_r = -beams[k].k_r[3] * z_min;
    faux_i = -beams[k].k_i[3] * z_min;
    cri_expi(R_p->rel+k+1, R_p->iel+k+1, faux_r, faux_i);
    L_m->rel[k+1] = R_p->rel[k+1];
    L_m->iel[k+1] = R_p->iel[k+1];

  } /* k */

  /* Final multiplications of matrix elements:
   * Multiply cols with R_x and rows with L_x
   */

  /* Tpp */
  for(k = 1; k <= Tpp->rows; k++)         /* loop over row No's (1st index) */
    for(ptr_r = Tpp->rel + (k-1)*Tpp->cols + 1,
        ptr_i = Tpp->iel + (k-1)*Tpp->cols + 1,
        ptr_end = ptr_r + Tpp->cols, l = 1;
        ptr_r < ptr_end;
        ptr_r ++, ptr_i ++, l ++)         /* loop over col No's (2nd index) */
    {
      cri_mul(ptr_r, ptr_i, *(L_p->rel+k), *(L_p->iel+k), *ptr_r, *ptr_i);
      cri_mul(ptr_r, ptr_i, *(R_p->rel+l), *(R_p->iel+l), *ptr_r, *ptr_i);
    }

  /* Tmm */
  for(k = 1; k <= Tmm->rows; k++)         /* loop over row No's (1st index) */
    for(ptr_r = Tmm->rel + (k-1)*Tmm->cols + 1,
        ptr_i = Tmm->iel + (k-1)*Tmm->cols + 1,
        ptr_end = ptr_r + Tmm->cols, l = 1;
        ptr_r < ptr_end;
        ptr_r ++, ptr_i ++, l ++)         /* loop over col No's (2nd index) */
    {
      cri_mul(ptr_r, ptr_i, *(L_m->rel+k), *(L_m->iel+k), *ptr_r, *ptr_i);
      cri_mul(ptr_r, ptr_i, *(R_m->rel+l), *(R_m->iel+l), *ptr_r, *ptr_i);
    }

  /* Rpm */
  for(k = 1; k <= Rpm->rows; k++)         /* loop over row No's (1st index) */
    for(ptr_r = Rpm->rel + (k-1)*Rpm->cols + 1,
        ptr_i = Rpm->iel + (k-1)*Rpm->cols + 1,
        ptr_end = ptr_r + Rpm->cols, l = 1;
        ptr_r < ptr_end;
        ptr_r ++, ptr_i ++, l ++)         /* loop over col No's (2nd index) */
    {
      cri_mul(ptr_r, ptr_i, *(L_p->rel+k), *(L_p->iel+k), *ptr_r, *ptr_i);
      cri_mul(ptr_r, ptr_i, *(R_m->rel+l), *(R_m->iel+l), *ptr_r, *ptr_i);
    }

  /* Rmp */
  for(k = 1; k <= Rmp->rows; k++)         /* loop over row No's (1st index) */
    for(ptr_r = Rmp->rel + (k-1)*Rmp->cols + 1,
        ptr_i = Rmp->iel + (k-1)*Rmp->cols + 1,
        ptr_end = ptr_r + Rmp->cols, l = 1;
        ptr_r < ptr_end;
        ptr_r ++, ptr_i ++, l ++)         /* loop over col No's (2nd index) */
    {
      cri_mul(ptr_r, ptr_i, *(L_m->rel+k), *(L_m->iel+k), *ptr_r, *ptr_i);
      cri_mul(ptr_r, ptr_i, *(R_p->rel+l), *(R_p->iel+l), *ptr_r, *ptr_i);
    }

  /* Add propagator of the unscattered wave to Tpp/Tmm:
   * exp[-ikz(+) * (zn - z1)]
   */
  for(uaux = 1, k = 0; k < n_beams; uaux += n_beams + 1, k ++)
  {
    /* exp[-ikz(+) * (zn - z1)] */
    cri_mul(&faux_r, &faux_i,
            L_p->rel[k+1], L_p->iel[k+1], R_p->rel[k+1], R_p->iel[k+1]);

    pref_i = cri_abs(faux_r, faux_i);
    CONTROL_MSG(CONTROL_X, "(%2d) propagator: (%.3f, %.3f) [%.3f]\n",
                    k, faux_r, faux_i, pref_i);

    Tmm->rel[uaux] += faux_r;
    Tmm->iel[uaux] += faux_i;
    Tpp->rel[uaux] += faux_r;
    Tpp->iel[uaux] += faux_i;
  }

  CONTROL_MSG(CONTROL, "... completed\n");

  /* Free dummy matrices and copy results to p_R/T** */
  matfree(R_p);
  matfree(R_m);
  matfree(L_p);
  matfree(L_m);

  free(atoms);

  for(iaux = (size_t)n_type-1; iaux >= 0; iaux--)
  {
    if(p_Tii[iaux] != NULL) matfree( p_Tii[iaux] );
  }
  free(p_Tii);

  *p_Tpp = Tpp;
  *p_Tmm = Tmm;
  *p_Rpm = Rpm;
  *p_Rmp = Rmp;

  CTIME("end of function");

  return(0);
} /* end of function leed_ms_compl_nd */
