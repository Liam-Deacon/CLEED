/*********************************************************************
 *													gsl_aux.h
 *
 *  Copyright (C) 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.11.2014 - creation
 *********************************************************************/

/*!
 * \file gsl_aux.h
 * \author Liam Deacon
 *
 * Provides mat2gsl() , gsl2mat() , cblas2gsl() and gsl2cblas() utility
 * functions for converting between matrix formats used in native CLEED,
 * CBLAS and the GNU Scientific Library.
 */


#ifndef __gsl_aux_H__
#define __gsl_aux_H__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */
extern C {
#endif /* __cplusplus */

/* includes */
#include <gsl/gsl_matrix.h>
#include "mat.h"

/* defines */


/* enums, structs and typedefs */


/* functions */
int mat2gsl(gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx, int gsl_num, mat Mx);

void gsl2mat(gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx, int mat_type, mat Mx);

void cblas2gsl(real *cblas_mx, real *cblas_mx_cmplx,
               gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx,
               int gsl_type, size_t cols, size_t rows);

void gsl2cblas(real *cblas_mx, real *cblas_mx_cmplx,
               gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx,
               int cblas_type);

void blas2gsl(real *cblas_mx, real *cblas_mx_cmplx,
              gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx,
              int gsl_type, size_t cols, size_t rows);

void gsl2blas(real *cblas_mx, real *cblas_mx_cmplx,
              gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx,
              int cblas_type);


#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */
namespace cleed {

/* classes */

} /* namespace cleed */

}
#endif /* __cplusplus */

#endif /* __gsl_aux_H__ */
