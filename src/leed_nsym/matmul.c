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
 */
mat matmul(mat Mr, const mat M1, const mat M2)
{

  long int i;
  long int i_c2, i_r1;
  long int i_cr1, i_cr2;

  int result_num_type;

  #if defined(USE_GSL)
  gsl_matrix *gsl_m1, *gsl_m2, *gsl_mr;
  gsl_matrix_complex *gsl_m1c, *gsl_m2c, *gsl_mr;
  #else
  real *cblas_m1, *cblas_m2, *cblas_mr; /* passed to cblas_Xgemm */
  #endif

  /* check input matrices */

  /* check validity of the input matrices */
  if ((matcheck(M1) < 1) || (matcheck(M2) < 1))
  {

    #ifdef ERROR
    fprintf(STDERR, "*** error (matmul): ivalid input matrices\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }

  /* check dimensions of input matrices */
  if (M1->cols != M2->rows)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matmul): "
            "dimensions of input matrices do not match\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }
  
  /* Create cblas matrices */
  if((M1->num_type ==  NUM_REAL) && (M2->num_type ==  NUM_REAL) )
  {
    #ifdef USE_GSL /* use GNU Scientific Library */

    #else /* use CBLAS method */
    /* In this case we need:
     * - no intermediary storage for operands
     * - no conversion to complex
     */
    cblas_mr = calloc(M1->rows * M2->cols, sizeof(real));

    cblas_m1 = M1->rel + 1;   /* matrices are stored as row major */
    cblas_m2 = M2->rel + 1;

    result_num_type = NUM_REAL;
    #endif /* end USE_GSL */
  }
  else
  {
    /* at least one operand is complex */
    cblas_mr = calloc(M1->rows * M2->cols, 2*sizeof(real));

    cblas_m1 = calloc(M1->rows * M1->cols, 2*sizeof(real));
    cblas_m2 = calloc(M2->rows * M2->cols, 2*sizeof(real));

    mat2cblas ( cblas_m1, NUM_COMPLEX, M1 ) ;
    mat2cblas ( cblas_m2, NUM_COMPLEX, M2 ) ;

    result_num_type = NUM_COMPLEX;
  }
  
  /* Perform the multiplication */
  #ifdef CONTROL
  fprintf(STDCTR, " (matmul) start multiplication \n");
  #endif

  switch(result_num_type)
  {
   case (NUM_REAL):
   {
     real alpha = 1.0 ;
     real beta = 0.0 ;

     if ( sizeof(real) == sizeof(float) || sizeof(real) == sizeof(double) )
     {
       CBLAS_REAL_GEMM(alpha, m1, m1_rows, m1_cols, m2, m2_cols, beta, mr);
     }
     else
     {
       fprintf(stderr, "matmul: unexpected sizeof(real)=%lu\n", sizeof(real));

       #ifdef EXIT_ON_ERROR
       exit(1);
       #else
       return(NULL);
       #endif
     }
   }  /* endcase NUM_REAL */
   break;

   case (NUM_COMPLEX):
   {
     real alpha[2] = { 1.0, 0.0 } ;
     real beta[2] =  { 0.0, 0.0 } ;

     if ( sizeof(real) == sizeof(float) || sizeof(real) == sizeof(double) )
     {
       CBLAS_COMPLEX_GEMM(alpha, m1, m1_rows, m1_cols, m2, m2_cols, beta)
     }
     else
     {
       fprintf(stderr, "matmul: unexpected sizeof(real)=%lu\n", sizeof(real));

       #ifdef EXIT_ON_ERROR
       exit(1);
       #else
       return(NULL);
       #endif
     }
   }  /* endcase NUM_COMPLEX */
   break ;
  }   /* endswitch */

  /* Copy cblas_mr into Mr */
  Mr = matalloc(Mr, M1->rows, M2->cols, result_num_type);
  cblas2mat ( Mr, cblas_mr );

  /* free memory */
  if ( result_num_type == NUM_COMPLEX )
  {
    free(cblas_m1);
    free(cblas_m2);
  }
  free(cblas_mr);

  return(Mr);
}  /* end of function matmul */
