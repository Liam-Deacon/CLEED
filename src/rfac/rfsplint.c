/*********************************************************************
 *                       RFSPLINT.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.29 - Creation (copy from splint.c in Numerical Recipes)
 ********************************************************************/

/*! \file
 *
 * Implementation file for rf_splint() function.
 */

#include <stdlib.h>
#include "rfac.h"


/*!
 * \fn rf_splint
 *
 * \brief Cubic spline interpolation.
 *
 *
 * \param[in] eng energy value for which interpolation is to be performed.
 *
 * \param[in,out] *list list of energy/intensity/deriv2 values,
 * the latter of which is generated in function cr_spline.
 *
 * \param[in] leng number of elements in list.
 *
 *
 * For a description see Num. Rec. Chapter 3.3.
 *
 *
 * \return y interpolated intensity.
 *
 * \warning exits with \ref RFAC_BAD_IV_DATA if list is invalid.
 *
 */
real rf_splint(real eng, rfac_iv_data *list, size_t leng)
{
  real y;
  int klo, khi, k;
  real h, b, a;

  /* Find the right place in list by means of bisection. */
  klo = 0;
  khi = leng-1;
  while (khi-klo > 1)
  {
    k = (khi+klo) >> 1;                     /* integer division by 2 */
    if (list[k].energy > eng) khi=k;
    else klo=k;
  }

  h = list[khi].energy - list[klo].energy;
  if (IS_EQUAL_REAL(h, 0.0))
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (cr_splint): Bad list input\n");
    #endif
    exit(RFAC_BAD_IV_DATA);
  }

  a = (list[khi].energy - eng)/h;
  b = (eng - list[klo].energy)/h;

  /* Evaluate cubic spline polynomial */
  y = a*list[klo].intens + b*list[khi].intens +
     ( (a*a*a-a)*list[klo].deriv2 + (b*b*b-b)*list[khi].deriv2 )*(h*h)/6.0;

  return(y);
} /* end of function cr_splint */
