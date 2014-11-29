/*********************************************************************
 *                             MATMUL.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/16.08.1994 - Mr can be equal to  M1 or M2
 *                  (Diagonal matrices are not included)
 *   GH/26.08.1994 - Error in the multiplication for complex matrices
 *                   corrected.
 *   LD/02.04.2014 - First attempt at OpenCL version of matmul code
 * MGJF/18.07.2014 - Workaround:
 *                   replace native matrix multiplication by cblas_Xgemm
**********************************************************************/

/*! \file
 *
 * Implements matmul() function for matrix multiplication. 
 *
 * The original Numerical Recipes routines have been replaced with a
 * CBLAS one, which gives massive speed gains over the original textbook 
 * version (kindly contributed by Michael Fink <Michael.Fink@uibk.ac.at>).
 * 
 * \note An initial attempt to use OpenCL for GPGPU calculations has been added
 * but is not tested. It can be enabled by defining #USE_OPENCL when compiling.
 */

#include <math.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mat.h"
#include "cblas_aux.h"

#ifdef USE_OPENCL
#include "err_code.h"
#ifndef CL_DEVICE
#define CL_DEVICE CL_DEVICE_TYPE_GPU   /* change as desired */
#endif
#endif

/*!
 * Multiplies two matrices \p Mr = \p M1 * \p M2
 *
 * \param[out] Mr Pointer to the matrix containing the result of the
 * multiplication. If \p Mr is passed in as \c NULL , the matrix will be
 * created and allocated memory.
 * \param[in] M1 Pointer to the first matrix to multiply.
 * \param[in] M2 Pointer to the second matrix to multiply.
 * \return Pointer to the resultant matrix.
 * \retval \c NULL if unsuccessful and #EXIT_ON_ERROR is not defined.
 *
 * \todo add OpenCL version e.g. using CBLAS within conditional \c
 * USE_OPENCL preprocessor block.
 *
 * \note A uniform interface to several matrix multiplication backends (CBLAS,
 * GSL and native) is provided through the use of preprocessor macros, which
 * are defined in cblas_aux.h .
 *
 */
mat matmul(mat Mr, const mat M1, const mat M2)
{
  int result_num_type;

  #if ( USE_CBLAS || USE_GSL || USE_MKL )
  REAL_MATRIX *m1, *m2, *mr;      /* cblas matrices passed to cblas_[sf]gemm */
  COMPLEX_MATRIX *m1c, *m2c, *mr; /* cblas matrices passed to cblas_[cz]gemm */
  #else
  mat m1 = M1, m2 = M2, mr = Mr;
  mat m1c = M1, m2c = M2, mrc = Mr;
  #endif

  /* check input matrices */

  /* check validity of the input matrices */
  if ((matcheck(M1) < 1) || (matcheck(M2) < 1))
  {
    ERROR_MSG("invalid input matrices\n");
    ERROR_RETURN(NULL);
  }

  /* check dimensions of input matrices */
  if (M1->cols != M2->rows)
  {
    ERROR_MSG("dimensions of input matrices do not match\n");
    ERROR_RETURN(NULL);
  }
  
  /* check size of real */
  if ( sizeof(real) != sizeof(float) && sizeof(real) != sizeof(double) )
  {
    ERROR_MSG("unexpected sizeof(real)=%lu\n", sizeof(real));
    ERROR_RETURN(NULL);
  }

  /* Create cblas matrices */
  if((M1->num_type ==  NUM_REAL) && (M2->num_type ==  NUM_REAL) )
  {
    /* In this case we need:
     * - no intermediary storage for operands
     * - no conversion to complex
     */
    mr = REAL_MATRIX_ALLOC(M1->rows, M2->cols);

    /* matrices are stored as row major */
    REAL_MAT2CBLAS(m1, M1);
    REAL_MAT2CBLAS(m2, M2);

    result_num_type = NUM_REAL;
  }
  else
  {
    /* at least one operand is complex */
    mrc = COMPLEX_MATRIX_ALLOC(M1->rows, M2->cols);

    m1c = COMPLEX_MATRIX_ALLOC(M1->rows, M1->cols);
    m2c = COMPLEX_MATRIX_ALLOC(M2->rows, M2->cols);

    COMPLEX_MAT2CBLAS(m1c, M1);
    COMPLEX_MAT2CBLAS(m2c, M2);

    result_num_type = NUM_COMPLEX;
  }

  /* Allocate memory for the resultant native matrix */
  Mr = matalloc(Mr, M1->rows, M2->cols, result_num_type);
  
  /* Perform the multiplication */
  CONTROL_MSG(CONTROL, "start multiplication\n");

  switch(result_num_type)
  {
   case (NUM_REAL):
   {
     real alpha = 1.0 ;
     real beta =  0.0 ;

     REAL_CBLAS_GEMM(alpha, m1, M1->rows, M1->cols,
                            m2, M2->cols, beta, mr);
     REAL_CBLAS2MAT(mrc, Mr);
   }  /* endcase NUM_REAL */
   break;

   case (NUM_COMPLEX):
   {
     COMPLEX(alpha, 1.0, 0.0);
     COMPLEX(beta, 0.0, 0.0);

     COMPLEX_CBLAS_GEMM(alpha, m1c, M1->rows, M1->cols,
                               m2c, M2->cols, beta, mrc);
     COMPLEX_CBLAS2MAT(mrc, Mr);
   }  /* endcase NUM_COMPLEX */
   break ;
  }   /* endswitch */

  /* free memory */
  #ifndef USE_NATIVE
  REAL_MATRIX_FREE(m1);
  REAL_MATRIX_FREE(m2);
  REAL_MATRIX_FREE(mr);

  COMPLEX_MATRIX_FREE(m1c);
  COMPLEX_MATRIX_FREE(m2c);
  COMPLEX_MATRIX_FREE(mrc);
  #endif

  return(Mr);
}  /* end of function matmul */
