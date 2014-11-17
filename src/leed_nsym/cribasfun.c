/*********************************************************************
 *                        CRIBASFUN.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/18.08.94 - type real instead of double
 *   GH/30.08.97 - all functions are void delete all return statements).
 *********************************************************************/

/*! \file
 *
 * Basic mathematical operations for complex arguments.
 *
 * It contains the functions cri_mul() , cri_div() , cri_sqrt() ,
 * cri_exp() , cri_expi() and cri_powi()
 *
 * The input of arguments is done for real (*_r) and imaginary part
 * (*_i) separately; the output of the result is done by
 * pointers to the respective variables (again real and imaginary
 * part separately). The typical argument list is:
 *
 * \code
 * void cri_<func> ( res_r, res_i, arg1_r, arg1_i [, arg2_r, argi_i] )
 * \endcode
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/*!
 * Performs complex multiplication.
 *
 * \f{eqnarray*}{
 *     z &=& z_1 \times z_2 \\
 *       &=& (a + ib) \times (c + id) \\
 *       &=& ac + ibc + iad - bd \\
 *       &=& (ac - bd) + i(ad + bc)
 * \f}
 *
 * \param res_r Pointer to the real part of the complex result \f$ z \f$
 * \param res_i Pointer to the imaginary part of the complex result \f$ z \f$
 * \param fac1_r Real part of the complex number \f$ z_1 \f$
 * \param fac1_i Imaginary part of the complex number \f$ z_1 \f$
 * \param fac2_r Real part of the complex number \f$ z_2 \f$
 * \param fac2_i Imaginary part of the complex number \f$ z_2 \f$
 *
 * \warning \p res_r and \p res_i must be allocated in memory (i.e. not \c NULL )
 * before entering the function.
 */
void cri_mul(real *res_r, real *res_i,
            real fac1_r, real fac1_i, real fac2_r, real fac2_i)
{
 *res_r = fac1_r*fac2_r - fac1_i*fac2_i;
 *res_i = fac1_r*fac2_i + fac1_i*fac2_r;
}

/*!
 * Calculates the complex result \f$ z \f$ by performing complex division
 *
 * \f{eqnarray*}{
 *    z &=& \frac{z_1}{z_2} \\
 *      &=& \frac{a + ib}{c + id} \\
 *      &=& \frac{(a + ib)(c - id)}{c + id)(c - id)}
 *      &=& \frac{(ac + bd) + i(bc - ad)}{c^2 + d^2}
 * \f}
 *
 * \param[out] res_r Pointer to the real part of the result \f$ z \f$
 * \param[out] res_i Pointer to the imaginary part of the result \f$ z \f$
 * \param num_r The real part of the complex numerator input \f$ a \f$
 * \param num_i The imaginary part of the complex numerator input \f$ i b \f$
 * \param den_r The real part of the complex denominator input \f$ c \f$
 * \param den_i The imaginary part of the complex denominator input \f$ i d \f$
 *
 * \warning \p res_r and \p res_i must be allocated in memory (i.e. not \c NULL )
 * before entering the function.
 */
void cri_div(real *res_r, real *res_i, 
            real num_r, real num_i, real den_r, real den_i)
{
real r, faux;
 if (R_fabs(den_r) >= R_fabs(den_i))
 {
   r = den_i/den_r;
   faux = den_r + r*den_i;
   *res_r = (num_r + r * num_i)/faux;
   *res_i = (num_i - r * num_r)/faux;
 }
 else
 {
   r = den_r/den_i;
   faux = den_i + r*den_r;
   *res_r = ( num_i + r * num_r)/faux;
   *res_i = (-num_r + r * num_i)/faux;
 }
} /* end of function cri_div */

/*!
 * Calculates the complex square root \f$ z = \sqrt{a + ib} \f$
 *
 * \param[out] res_r Pointer to the real part of the result.
 * \param[out] res_i Pointer to the imaginaty part of the result.
 * \param arg_r Real part of the complex input \f$ a \f$ .
 * \param arg_i Imaginary part of the complex input \f$ ib \f$ .
 *
 * \warning \p res_r and \p res_i must be allocated in memory (i.e. not \c NULL )
 * before entering the function.
 */
void cri_sqrt(real *res_r, real *res_i, real arg_r, real arg_i)
{
  real x, y, w, r;
  if (IS_EQUAL_REAL(arg_r, 0.0) && IS_EQUAL_REAL(arg_i, 0.0))
  {
    *res_i = *res_r = 0.;
  }
  else
  {
    x = R_fabs(arg_r);
    y = R_fabs(arg_i);

    if (x >= y)
    {
      r = y/x;
      w = R_sqrt(x)*R_sqrt(0.5*(1.0 + R_sqrt(1.0 + r*r)));
    }
    else
    {
      r = x/y;
      w = R_sqrt(y)*R_sqrt(0.5*(r+R_sqrt(1.0+r*r)));
    }
    if (arg_r > 0.0)
    {
      *res_r = w;
      *res_i = arg_i/(2. * w);
    }
    else
    {
      *res_i = (arg_i >= 0.) ? w:-w;
      *res_r = arg_i/(2. * *res_i);
    }
  }
} /* end of function cri_sqrt */

/*!
 * Calculates the complex exponent
 *
 * \f[ z = e^{a + ib} \f]
 *
 * \param res_r Pointer to the real part of the result \f$ z \f$ .
 * \param res_i Pointer to the imaginary part of the result \f$ z \f$ .
 * \param arg_r Real part of the exponential function argument \f$ a \f$ .
 * \param arg_i Imaginary part of the exponential function argument \f$ ib \f$ .
 *
 * \warning \p res_r and \p res_i must be allocated in memory (i.e. not \c NULL )
 * before entering the function.
 */
void cri_exp(real *res_r, real *res_i, real arg_r, real arg_i)
{
  real faux;
  if (IS_EQUAL_REAL(arg_i, 0.))
  {
    *res_r = R_exp(arg_r);
    *res_i = 0.;
  }
  else
  {
    if (IS_EQUAL_REAL(arg_r, 0.))
    {
      *res_r = R_cos(arg_i);
      *res_i = R_sin(arg_i);
    }
    else
    {
      faux = R_exp(arg_r);
      *res_r = faux * R_cos(arg_i);
      *res_i = faux * R_sin(arg_i);
    }
  }
} /* end of function cri_exp */

/*!
 * Calculates the complex exponent
 *
 * \f{eqnarray*}{
 *    z &=& \exp(a + ib)
 * \f}
 *
 * \param res_r Pointer to real part of the result \f$ z \f$
 * \param res_i Pointer to imaginary part of the result \f$ z \f$ .
 * \param arg_r Real part of the exponential function argument \f$ a \f$.
 * \param arg_i Imaginary part of the exponential function argument \f$ ib \f$ .
 *
 * \warning \p res_r and \p res_i must be allocated in memory (i.e. not \c NULL )
 * before entering the function.
 */
void cri_expi(real *res_r, real *res_i, real arg_r, real arg_i)
{
  real faux;

  /* Exchange values of arg_r and arg_i */
  faux = arg_i;
  arg_i = arg_r;
  arg_r = -faux;

  /* From now on expi is identical to exp. */
  if (IS_EQUAL_REAL(arg_i, 0.))
  {
    *res_r = R_exp(arg_r);
    *res_i = 0.;
  }
  else
  {
    if (IS_EQUAL_REAL(arg_r, 0.))
    {
      *res_r = R_cos(arg_i);
      *res_i = R_sin(arg_i);
    }
    else
    {
      faux = R_exp(arg_r);
      *res_r = faux * R_cos(arg_i);
      *res_i = faux * R_sin(arg_i);
    }
  }
} /* end of function cri_expi */

/*!
 * Calculates the result of a complex number to the power i:
 *
 * \f{eqnarray*}{
 *    z &=& {(a + ib)}^k
 * \f}
 *
 * \param res_r Pointer to the real part of the complex result \f$ z \f$ .
 * \param res_i Pointer to the imaginary part of the result \f$ z \f$ .
 * \param arg_int exponent as an integer argument \f$ k \f$ .
 *
 * \warning \p res_r and \p res_i must be allocated in memory (i.e. not \c NULL )
 * before entering the function.
 */
void cri_powi(real *res_r, real *res_i, int arg_int)
{
  arg_int = arg_int%4;
  switch(arg_int)
  {
    case(0):
    {
      *res_r = 1.;
      *res_i = 0.;
      break;
    }
    case(1): case(-3):
    {
      *res_r = 0.;
      *res_i = 1.;
      break;
    }
    case(2): case(-2):
    {
      *res_r =-1.;
      *res_i = 0.;
      break;
    }
    case(3): case(-1):
    {
      *res_r = 0.;
      *res_i =-1.;
      break;
    }
  }

} /* end of function cri_expi */
