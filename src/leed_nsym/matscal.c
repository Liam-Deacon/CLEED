/*********************************************************************
 *                        MATSCAL.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/12.04.95 - Creation
 *********************************************************************/

/*! \file
 *
 * Multiplies a matrix by a complex number.
 */

#include <math.h>   
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"

/*!
 * Multiplies a matrix with a complex number: \p Mr = \p num * \p Mi
 * \param[out] Mr Pointer to the matrix containing the result of the
 * multiplication. If \c NULL, the pointer will be created and returned.
 * \param[in] Mi Pointer to the matrix to be multiplied.
 * \param num_r Real part of the scaling factor.
 * \param num_i Imaginary part of the scaling factor.
 * \return Pointer to the result matrix.
 * \retval \c NULL if unsuccessful.
 */
mat matscal(mat Mr, const mat Mi, real num_r, real num_i )
{
  /* check input matrix */

  /* check validity of the input matrices */
  if (matcheck(Mi) < 1)
  {
    ERROR_MSG("invalid input matrix\n");
    ERROR_RETURN(NULL);
  }

  /* copy Mi to Mr */
  Mr = matcopy(Mr, Mi);

  /* Perform multiplication */
  CONTROL_MSG(CONTROL, "start multiplication \n");

  switch(Mr->num_type)
  {
    /* real matrix */
    case (NUM_REAL):
    {
      if( IS_EQUAL_REAL(num_i, 0.))         /* num is real */
      {
        register real *ptr_end;
        register real *ptrr;

        CONTROL_MSG(CONTROL, "entering real loop\n");

        for(ptrr = Mr->rel + 1, ptr_end = Mr->rel + Mr->cols*Mr->rows;
            ptrr <= ptr_end; ptrr ++)
        {
          *ptrr *= num_r;
        }
      } /* if num_i == 0. */
      else /* num is complex */
      {
        register real *ptr_end;
        register real *ptrr, *ptri;

        Mr->iel = (real*)calloc( Mr->cols*Mr->rows * 1, sizeof(real));
        Mr->num_type = NUM_COMPLEX;

        for( ptrr = Mr->rel + 1, ptri = Mr->iel + 1,
             ptr_end = Mr->rel + Mr->cols*Mr->rows;
             ptrr <= ptr_end; ptrr ++, ptri ++)
        {
          *ptrr = *ptrr * num_r;   /* real part */
          *ptri = *ptrr * num_i;   /* imaginary part */
        }
      }

      break;
    }  /* case REAL */

    /* complex matrix */
    case (NUM_COMPLEX):
    {
      CONTROL_MSG(CONTROL, "entering complex loop\n");

      register real faux;
      register real *ptr_end;
      register real *ptrr, *ptri;

      CONTROL_MSG(CONTROL, "entering real loop\n");

      for( ptrr = Mr->rel + 1, ptri = Mr->iel + 1,
           ptr_end = Mr->rel + Mr->cols*Mr->rows;
           ptrr <= ptr_end; ptrr ++, ptri ++)
      {
        faux  = *ptrr * num_r - *ptri * num_i;   /* real part */
        *ptri = *ptrr * num_i + *ptri * num_r;   /* imaginary part */
        *ptrr = faux;
      }

      break;
    }  /* case COMPLEX */
  }   /* switch */

  return(Mr);
}  /* end of function matscal */
