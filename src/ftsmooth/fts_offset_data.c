/*********************************************************************
 *                           FTS_OFFSET_DATA.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - creation.
 *********************************************************************/

/*!
 * \file
 * \brief Implementation of fts_offset_data() function.
 */

#include "ftsmooth.h"

/*!
 * Offsets values by a given \p offset amount.
 *
 * \param x array of x values.
 * \param fx array of y values.
 * \param n_x number of values in array.
 * \param offset amount to offset values by.
 * \param offset_type indicates which axis to perform shift.
 * \return integer indicating success.
 * \retval 0 success.
 * \retval -1 failure.
 */
int fts_offset_data(double *x, double *fx, size_t n_x, double offset,
				            int offset_type)
{
  size_t i;
  double xmin = DBL_MAX;
  double ymin = DBL_MAX;

 /* select offset type */
  switch (offset_type)
  {
    case OFFSET_X_TO_VALUE:
      /* offset f(x) by offset */
      for (i=0; i<n_x; i++)
      {
        x[i] = x[i] + offset;
	    }
      break;
	 
    case OFFSET_X_TO_ZERO:
      /* check for lowest value of y */
      #ifdef USE_OPENMP
      #pragma omp parallel for
      #endif
      for (i=0; i<n_x; i++)
	    {
        if (x[i] < xmin) xmin = x[i];
	    }
	 
      /* offset f(x) by ymin */
      #ifdef USE_OPENMP
	    #pragma omp parallel for
      #endif
      for (i=0; i<n_x; i++)
	    {
        x[i] = x[i] - xmin;
	    }
      break;
    
    case OFFSET_Y_TO_VALUE:
      /* offset f(x) by offset */
      for (i=0;i<n_x;i++)
      {
        fx[i] = fx[i] + offset;
      }
      break;
	 
    case OFFSET_Y_TO_ZERO:
      /* check for lowest value of y */
      #ifdef USE_OPENMP
      #pragma omp parallel for
      #endif
      for (i=0; i<n_x; i++)
      {
        if (fx[i] < ymin) ymin = fx[i];
      }
	 
      /* offset f(x) by ymin */
      #ifdef USE_OPENMP
	    #pragma omp parallel for
      #endif
      for (i=0; i<n_x; i++)
      {
        fx[i] = fx[i] - ymin;
      }
      break;
	  
    default:
      return(-1); /* offset_flag not recognised, do not process offset */
  }
 
  return(0);
}
