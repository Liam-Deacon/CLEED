/*********************************************************************
 *                           REAL.H
 *
 *  Copyright 1994-2014 Georg Held <georg.held@diamond.ac.uk>
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

#ifndef CLEED_REAL_H
#define CLEED_REAL_H

/* define DOUBLE if neither FLOAT nor DOUBLE have been defined before. */
#ifndef CLEED_REAL_IS_DOUBLE
#ifndef CLEED_REAL_IS_FLOAT
#define CLEED_REAL_IS_DOUBLE 1
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
#ifdef CLEED_REAL_IS_DOUBLE
#include <float.h>

typedef double real;

static const char* CLEED_REAL_FMT = "l";

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

#endif  /* CLEED_REAL_IS_DOUBLE */

/************************************************
 * REAL IS FLOAT
 ************************************************/

#ifdef CLEED_REAL_IS_FLOAT
#include <float.h>

static const char* CLEED_REAL_FMT = "";

typedef float real;

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

inline float R_cos(double x) { return (float)cos(x); }
inline float R_cosd(double x) { return (float)cosd(x); }
inline float R_sin(double x)  { return (float)sin(x); }
inline float R_sind(double x) { return (float)sind(x); }
inline float R_atan2(double x, double y) { return (float)atan2(x, y); }

inline float R_cosh(x) { return (float)cosh(x); }
inline float R_sinh(x) { return (float)sinh(x); }

inline float R_exp(x) { (float) exp(x); }
inline float R_log(x) { (float) log(x); }

inline float R_fabs(x)  (float) fabs ((double)(x))
inline float R_sqrt(x)  (float) sqrt ((double)(x))

inline float R_cabs(x,y)   (float) hypot ((double)(x), (double)(y))
inline float R_hypot(x,y)  (float) hypot ((double)(x), (double)(y))

inline float R_nint(x) { return (float)rint((double)(x));}

#endif

#define IS_EQUAL_REAL(x,y) (fabs(x-y) < FLT_EPSILON)

#endif  /* CLEED_REAL_IS_FLOAT */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* CLEED_REAL_H */
