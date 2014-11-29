/*********************************************************************
 *                      CBLAS_AUX.H
 *
 *  Copyright 2014 Michael Fink <Michael.Fink@uibk.ac.at>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 * MGJF/18.07.2014 - creation
 *   LD/21.11.2014 - added condition preprocessor directives for CBLAS
 *********************************************************************/

/*! \file
 *
 * Header file for CBLAS interface to CLEED. The exact interface is determined
 * through set of preprocessor macros depending on the backend to use
 * (e.g. CBLAS, GSL or native) and whether #real type is single or double
 * precision. The backend is selected by defining either \c USE_GSL
 * or \c USE_CBLAS , otherwise the native version using Numerical Recipes
 * will be used.
 */

#ifndef __CBLAS_AUX_H__
#define __CBLAS_AUX_H__

#include "mat.h"

#if defined( USE_GSL )     /* use GNU Scientific Library for CBLAS routines */
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_matrix.h>
#include "gsl_aux.h"
#elif defined( USE_MKL )   /* use Intel Math Kernel Library CBLAS routines */
#include <mkl.h>
#elif defined( USE_CBLAS ) /* use vanilla CBLAS routines */
#include <cblas.h>
#else
#ifndef USE_NATIVE
#define USE_NATIVE 1       /* use original, native CLEED routines */
#endif /* USE_NATIVE */
#endif

#ifdef __cplusplus__ /* Use C linkage if this is a C++ compiler */
extern C {
#endif /* __cplusplus__ */

/* function prototypes */

int mat2cblas(real *cblas_mx, int cblas_num, mat Mx);

int cblas2mat(mat Mx, real *cblas_mx);

int mat2blas(real *cblas_mx, int cblas_num, mat Mx);

int blas2mat(mat Mx, real *cblas_mx);

void info_check(const char *routine, const int info);

#ifdef __cplusplus__ /* Use C linkage if this is a C++ compiler */
} /* extern C */
#endif /* __cplusplus__ */

#endif /* __CBLAS_AUX_H__ */
