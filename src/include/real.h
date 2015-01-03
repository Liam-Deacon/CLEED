/*********************************************************************
 *                           REAL.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.10.02 - creation
 *   LD/2014.10.09 - added inclusion guards and C linkage for C++
 *********************************************************************/

/*!
 * \file
 * \brief Definitions in connection with the type #real
 *
 * This file defines aliases for floating point operations depending on
 * whether #real is a \c typedef for \c double or \c float type.
 *
 * \note The behavior of changes depending on the CPU of the machine e.g.
 * casting from \c double to \c float may occur.
*/

#ifndef REAL_H
#define REAL_H

/* define DOUBLE if neither FLOAT nor DOUBLE have been defined before. */
#ifndef REAL_IS_DOUBLE
#ifndef REAL_IS_FLOAT
#define REAL_IS_DOUBLE 1
#endif
#endif

/*!
 * \typedef real
 * \brief alias for floating point data type.
 *
 * If #REAL_IS_DOUBLE is defined then #real is of type \c double
 * otherwise if #REAL_IS_FLOAT is defined then #real is of type \c
 * float .
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/************************************************
 * REAL IS DOUBLE
 ************************************************/
#ifdef REAL_IS_DOUBLE
#include <float.h>

#ifndef TYPE_REAL
#define TYPE_REAL double
typedef double real;
#endif

#define REAL_FMT "l"

/*
  Define macros for math operations of type double 
  (standard math library)
*/

#define R_cos cos
#define R_cosd cosd
#define R_sin sin
#define R_sind sind
#define R_atan2 atan2

#define R_cosh cosh
#define R_sinh sinh

#define R_exp exp
#define R_log log

#define R_fabs fabs
#define R_sqrt sqrt

#define R_cabs hypot
#define R_hypot hypot

#define R_nint rint
#define R_rint rint

#define IS_EQUAL_REAL(x,y) (fabs(x-y) < DBL_EPSILON)

#endif  /* REAL_IS_DOUBLE */

/************************************************
 * REAL IS FLOAT
 ************************************************/

#ifdef REAL_IS_FLOAT
#include <float.h>

#define REAL_FMT

#ifndef TYPE_REAL
#define TYPE_REAL
typedef float  real;
#endif

/*
  Define macros for math operations of type float
*/

#ifdef __osf__                                     /* DEC */

#define R_cos cosf
#define R_cosd cosdf
#define R_sin sinf
#define R_sind sindf
#define R_atan2 atan2f

#define R_cosh coshf
#define R_sinh sinhf

#define R_exp expf
#define R_log logf

#define R_fabs fabsf
#define R_sqrt sqrtf

#define R_cabs hypotf
#define R_hypot hypotf

#define R_nint rintf

#else                              /* IBM and others (standard math library) */

#define R_cos(x)   (float) cos  ((double)(x))
#define R_cosd(x)  (float) cosd ((double)(x))
#define R_sin(x)   (float) sin  ((double)(x))
#define R_sind(x)  (float) sind ((double)(x))
#define R_atan2(x,y)  (float)atan2((double)(x), (double)(y))

#define R_cosh(x)  (float) cosh ((double)(x))
#define R_sinh(x)  (float) sinh ((double)(x))

#define R_exp(x)   (float) exp  ((double)(x))
#define R_log(x)   (float) log  ((double)(x))

#define R_fabs(x)  (float) fabs ((double)(x))
#define R_sqrt(x)  (float) sqrt ((double)(x))

#define R_cabs(x,y)   (float) hypot ((double)(x), (double)(y))
#define R_hypot(x,y)  (float) hypot ((double)(x), (double)(y))

#define R_nint(x)  (float) rint ((double)(x))

#endif

#define IS_EQUAL_REAL(x,y) (fabs(x-y) < FLT_EPSILON)

#endif  /* REAL_IS_FLOAT */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* REAL_H */
