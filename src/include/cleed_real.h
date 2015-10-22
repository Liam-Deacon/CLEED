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

#ifndef CLEED_REAL_H
#define CLEED_REAL_H

/* define DOUBLE if neither FLOAT nor DOUBLE have been defined before. */
#if (!defined(CLEED_REAL_IS_DOUBLE) && !defined(CLEED_REAL_IS_FLOAT))
#define CLEED_REAL_IS_DOUBLE 1
#endif

#if !defined(_USE_MATH_DEFINES) && defined(__cplusplus)
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <stdbool.h>

#if (__GNUC__)
#define PURE __attribute__ ((pure))
#else
#define PURE
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
#include <cmath>

#define _USE_MATH_DEFINES

using std::atan2;

using std::cos;
using std::cosf;
using std::cosh;

using std::sin;
using std::sinf;
using std::sinh;

using std::exp;
using std::log;

using std::sqrt;

using std::hypot;

using std::rint;

using std::fabs;

namespace cleed {
#endif

/************************************************
 * REAL IS DOUBLE
 ************************************************/
#ifdef CLEED_REAL_IS_DOUBLE
# ifdef CLEED_REAL_IS_FLOAT
#   undef CLEED_REAL_IS_FLOAT
# endif
#endif  /* CLEED_REAL_IS_DOUBLE */

/************************************************
 * REAL IS FLOAT
 ************************************************/

#ifdef CLEED_REAL_IS_FLOAT
#include <float.h>

static const char CLEED_REAL_FMT[] = "";

typedef float real;

/*
  Define macros for math operations of type float
*/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

PURE static inline double sind(real x) { return sinf(x * (real)(M_PI / 180)); }
PURE static inline double cosd(real x) { return cosf(x * (real)(M_PI / 180)); }

#ifdef __osf__                                     /* DEC */

PURE static inline real cleed_real_cos(real x) { return (cosf(x)); }
PURE static inline real cleed_real_cosd(real x) { return (cosd(x)); }
PURE static inline real cleed_real_sin(real x)  { return (sinf(x)); }
PURE static inline real cleed_real_sind(real x) { return (sind(x)); }
PURE static inline real cleed_real_atan2(real x, real y) { return atan2f(x, y); }

PURE static inline real cleed_real_cosh(real x) { return coshf(x); }
PURE static inline real cleed_real_sinh(real x) { return sinhf(x); }

PURE static inline real cleed_real_exp(real x) { return expf(x); }
PURE static inline real cleed_real_log(real x) { return logf(x); }

PURE static inline real cleed_real_fabs(real x) { return fabsf(x); }
PURE static inline real cleed_real_sqrt(real x) { return sqrtf(x); }

PURE static inline real cleed_real_cabs(real x, real y) { return hypotf(x, y); }
PURE static inline real cleed_real_hypot(real x, real y) { return hypotf(x, y); }

PURE static inline real cleed_real_nint(real x) { return (rintf(x));}

#else                              /* IBM and others (standard math library) */

PURE static inline real cleed_real_cos(double x) { return ((real) cos(x)); }
PURE static inline real cleed_real_cosd(double x) { return ((real) cosd((real)x)); }
PURE static inline real cleed_real_sin(double x)  { return ((real) sin(x)); }
PURE static inline real cleed_real_sind(double x) { return ((real) sind((real)x)); }
PURE static inline real cleed_real_atan2(double x, double y) { return (real) atan2(x, y); }

PURE static inline real cleed_real_cosh(double x) { return (real) cosh(x); }
PURE static inline real cleed_real_sinh(double x) { return (real) sinh(x); }

PURE static inline real cleed_real_exp(double x) { return (real) exp(x); }
PURE static inline real cleed_real_log(double x) { return (real) log(x); }

PURE static inline real cleed_real_fabs(double x) { return (real) fabs(x); }
PURE static inline real cleed_real_sqrt(double x) { return (real) sqrt(x); }

PURE static inline real cleed_real_cabs(double x, double y) { return (real) hypot(x, y); }
PURE static inline real cleed_real_hypot(double x, double y) { return (real) hypot(x, y); }

PURE static inline real cleed_real_nint(double x) { return ((real)rint(x));}

#endif

PURE static inline bool is_equal_real(real x, real y) { return (fabs(x-y) < FLT_EPSILON); }

#define IS_EQUAL_REAL is_equal_real

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
#endif

#else

#include <float.h>

typedef double real;

static const char CLEED_REAL_FMT[] = "l";

/*
  Define macros for math operations of type double
  (standard math library)
*/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

PURE static inline double sind(double x) { return (sin(x * M_PI / 180.)); }
PURE static inline double cosd(double x) { return (cos(x * M_PI / 180.)); }

PURE static inline real cleed_real_cos(double x) { return ( cos(x)); }
PURE static inline real cleed_real_cosd(double x) { return ( cosd(x)); }
PURE static inline real cleed_real_sin(double x)  { return ( sin(x)); }
PURE static inline real cleed_real_sind(double x) { return ( sind(x)); }
PURE static inline real cleed_real_atan2(double x, double y) { return (atan2(x, y)); }

PURE static inline real cleed_real_cosh(double x) { return (cosh(x)); }
PURE static inline real cleed_real_sinh(double x) { return (sinh(x)); }

PURE static inline real cleed_real_exp(double x) { return (exp(x)); }
PURE static inline real cleed_real_log(double x) { return (log(x)); }

PURE static inline real cleed_real_fabs(double x) { return (fabs(x)); }
PURE static inline real cleed_real_sqrt(double x) { return (sqrt(x)); }

PURE static inline real cleed_real_cabs(double x, double y) { return (hypot(x, y)); }
PURE static inline real cleed_real_hypot(double x, double y) { return (hypot(x, y)); }

PURE static inline real cleed_real_nint(double x) { return ((real)rint(x));}

PURE static inline bool is_equal_real(real x, real y)
{
  return (fabs(((real)x-(real)y)) < DBL_EPSILON);
}

#define IS_EQUAL_REAL is_equal_real

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
#endif

#endif  /* CLEED_REAL_IS_FLOAT */

typedef real cleed_real;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif


#undef PURE

#endif /* CLEED_REAL_H */
