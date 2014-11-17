/**********************************************************************
 *                          QMYLM.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/09.08.94 - creation
 *********************************************************************/

/*! @file
 *
 * Quantum mechanical calculations for the Hankel function.
 *
 * The file contains r_hank1() and c_hank1() which calculate the real
 * and complex parts of the Hankel functions, respectively.
 *
 * @note Functions tested for PI/2, PI, i and -i.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"
#include "qm.h"

/*!
 * Calculate all orders of the Hankel function of the first kind H(1)l up to
 * @f$ l @f$ = @p l_max for a given real argument @p x.
 *
 * The Hankel functions H(1)l are calculated from the recurrence relation:
 *
 * @f[ H^{(1)}_{l}(x) = \frac{(2l-1)}{x H{(1)}_{l-1(x)}} - H^{(1)}_{l-2}(x) @f]
 *
 * knowing the exact formula for \f$ H^{(1)}_0 \f$ and \f$ H^{(1)}_1 \f$:
 *
 * \f[ H^{(1)}_0(x) = - exp(ix)/x \times i
 *                  = \frac{sin(x)}{x} - i \frac{cos(x)}{x} \f]
 *
 * \f[ H^{(1)}_1(x) = - exp(ix)/x \times (\frac{i}{x} + 1)
 *                  = H^{(1)}_0(x) \times (\frac{1}{x} - i) \f]
 *
 * \param[out] Hl Hankel function of the first kind in natural order
 * @f$ l = 0,1, ... @f$
 *
 * \param x real argument.
 * \param l_max maximum angular momentum @f$ l_{max} @f$ for output.
 *
 * \return Pointer to the Henkel function matrix.
 * \retval \c NULL if failed.
 *
 *  Hl (may be different from input parameter). The storage scheme for Hl
 *  is in the natural order:
 *
 *  l      0  1  2  3  4  ...
 *  index  1  2  3  4  5  ...
 *
 *  I.e. index(l,m) = l + 1.
 */
mat r_hank1(mat Hl, real x, size_t l_max )
{
  size_t l;                        /* quantum number l */

  real faux;
  real z_inv;
  real *ptr_r, *ptr_i;

  /* Check arguments x and l_max */

  if(IS_EQUAL_REAL(x, 0.))
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (r_hank1): invalid argument x = 0.\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }
 
  if (l_max < 1) l_max = 1;   /* we need at least that much storage */

  /* Allocate memory for Hl */
  Hl = matalloc( Hl, (l_max+1), 1, NUM_COMPLEX );

  /* Add the offset of 1 to r/ptr_i */
  ptr_r = Hl->rel+1;
  ptr_i = Hl->iel+1;

  /* Some often used values */
  z_inv = 1./x;
 
  /* H0 and H1 are calculated from the exact formula:
   * H0(x) = sin(x)/x - i*cos(x)/x.
   * H1(x) = H0(x) * (1/x - i).
   */
 
  /* H0(x) */
  ptr_r[0] =  z_inv * sin(x);
  ptr_i[0] = -z_inv * cos(x);

  /* H1(x) */
  cri_mul(ptr_r+1, ptr_i+1, ptr_r[0], ptr_i[0], z_inv, -1.);
 
  /* loop over l:
   * Hl (x) = (2*l-1)/x Hl-1(x) - Hl-2(x)
   */
  for(l = 2; l <= l_max; l++ )
  {
    faux = (2*l - 1) * z_inv;
    cri_mul(ptr_r+l, ptr_i+l, faux, 0., ptr_r[l-1], ptr_i[l-1]);
    ptr_r[l] -= ptr_r[l-2];
    ptr_i[l] -= ptr_i[l-2];
  }   /* l */

  return(Hl);

} /* end of function r_hank1 */

/*!
 * Calculate all orders of the Hankel function of the first kind
 * @f$ H^{(1)}_l @f$ up to l = @p l_max for a given complex argument @p z
 *
 * The Hankel functions @f$ H^{(1)}_l @f$ are calculated from the recurrence
 * relation:
 *
 * @f[ H^{(1)}_l(z) = \frac{(2l-1)}{z} H^{(1)}_{l-1}(z) - H^{(1)}_{l-2}(z) @f]
 *
 * Knowing the exact formula for @f$ H^{(1)}_0 @f$ and @f$ H^{(1)}_1 @f$ :
 *
 * @f[ H^{(1)}_0(z) = - \frac{exp(iz)}{z} \times i @f]
 * @f[ H^{(1)}_1(z) = - \frac{exp(iz)}{z} times (\frac{i}{z} + 1)
 *                  = H^{(1)}_0(z) \times (\frac{1}{z} - i) @f]
 *
 * \param[out] Hl Hankel function of the first kind in natural order
 * (l = 0,1, ..., see below)
 * \param z_r real part of the argument.
 * \param z_i imaginary part of the argument.
 * \param l_max maximum angular momentum for output.
 * \return Pointer to matrix of Henkel values.
 * \retval \c NULL if failed
 *
 * @p Hl (may be different from input parameter). The storage scheme for Hl
 * is in the natural order:
 *
 * l      0  1  2  3  4  ...
 * index  1  2  3  4  5  ...
 *
 * I.e. index(l,m) = l + 1.
 */
mat c_hank1(mat Hl, real z_r, real z_i, size_t l_max )
{
  size_t l;                        /* quantum number l */

  real faux_r, faux_i;
  real z_inv_r, z_inv_i;
  real *ptr_r, *ptr_i;


  /* Check arguments z and l_max */
  if( IS_EQUAL_REAL(z_r, 0.) && IS_EQUAL_REAL(z_i, 0.) )
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (r_hank1): invalid argument z = (0., 0.)\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }

  if (l_max < 1) l_max = 1;   /* we need at least that much storage */

  /* Allocate memory for Hl */
  Hl = matalloc( Hl, (l_max+1), 1, NUM_COMPLEX );

  /* Add the offset of 1 to r/ptr_i */
  ptr_r = Hl->rel+1;
  ptr_i = Hl->iel+1;

  /* Some often used values */
  cri_div(&z_inv_r, &z_inv_i, 1., 0., z_r, z_i);
 
  /* H0 and H1 are calculated from the exact formula:
   * H0(z) = - i/z * exp(iz).
   * H1(x) = H0(x) * (1/x - i).
   */
 
  /* H0(x) */
  cri_expi(&faux_r, &faux_i, z_r, z_i);
 
  /* -i * z_inv = z_inv_i - i* z_inv_r */
  cri_mul(ptr_r, ptr_i, z_inv_i, -z_inv_r, faux_r, faux_i);

  /* H1(x) */
  cri_mul(ptr_r+1, ptr_i+1, ptr_r[0], ptr_i[0], z_inv_r, z_inv_i - 1.);
 
  /* loop over l:
   *
   * Hl (z) = (2*l-1)/z Hl-1(z) - Hl-2(z)
   */
  for(l = 2; l <= l_max; l++ )
  {
    faux_r = (2*l - 1) * z_inv_r;
    faux_i = (2*l - 1) * z_inv_i;

    cri_mul(ptr_r+l, ptr_i+l, faux_r, faux_i, ptr_r[l-1], ptr_i[l-1]);
    ptr_r[l] -= ptr_r[l-2];
    ptr_i[l] -= ptr_i[l-2];
  }   /* l */

  return(Hl);
} /* end of function c_hank1 */
