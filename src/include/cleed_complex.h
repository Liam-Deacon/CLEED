/*********************************************************************
 *                          cleed_complex.h
 *
 *  Copyright (C) 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/28.11.2014 - creation
 *********************************************************************/

/*!
 * \file mat_aux.h
 * \author Liam Deacon
 *
 * Provides a complex data type using an abstract factory design pattern.
 */


#ifndef CLEED_COMPLEX_H
#define CLEED_COMPLEX_H

#include "cleed_real.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

#if USE_CBLAS || USE_MKL || USE_LAPACK

typedef real cleed_complex[2];

# define CLEED_COMPLEX_INIT(i,r)               {(real)(i), (real)(r)}
# define CLEED_COMPLEX_SET(z,r,i) do {z[0] = (real)r; z[1] = (real)i;} while (0)

#elif USE_GSL

# if CLEED_REAL_IS_FLOAT
typedef gsl_complex_float cleed_complex;
# else /* assume double precision floating point */
typedef gsl_complex cleed_complex;
# endif

# define CLEED_COMPLEX_INIT(i,r)               {(real)(i), (real)(r)}
# define CLEED_COMPLEX_SET(z,r,i)              GSL_COMPLEX_SET((z),(r),(i))

#else /* USE_NATIVE */

typedef cleed_real cleed_complex[2];

# define CLEED_COMPLEX_INIT(r,i) {(r), (i)}
# define CLEED_COMPLEX_SET(z,r,i)                                              \
do {(z) = (z)[0] = (real)(r); (z)[1] = (real)(i);} while (0)

#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif


#endif /* CLEED_COMPLEX_H */
