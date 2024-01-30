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
#if (!defined(CLEED_REAL_IS_DOUBLE) && !defined(CLEED_REAL_IS_FLOAT))
#define CLEED_REAL_IS_DOUBLE 1
#endif

#if !defined(_USE_MATH_DEFINES) && (defined(__cplusplus) || defined(_MSC_VER))
#define _USE_MATH_DEFINES
#endif

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include "cc_attribute.h"

/*!
 * \typedef real
 * \brief alias for floating point data type.
 *
 * If #REAL_IS_DOUBLE is defined then #real is of type \c double
 * otherwise if #REAL_IS_FLOAT is defined then #real is of type \c
 * float .
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
#define _USE_MATH_DEFINES
#include <cmath>

#if __cplusplus >= 201103L
#include <tgmath.h>

using std::hypot;
using std::rint;
#else
using ::hypot;
using ::rint;
#endif

using std::atan2;

using ::cosf;
using std::cos;
using std::cosh;

using ::sinf;
using std::sin;
using std::sinh;

using std::exp;
using std::log;

using std::sqrt;
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

static const char CLEED_REAL_FMT[] = "";

typedef float real;

/*
  Define macros for math operations of type float
*/

#ifdef __alpha__ || __osf__        /* DEC ALPHA */
# include "_cleed_real_dec_alpha.h"
#else                              /* IBM and others (standard math library) */
# include "_cleed_real_ibm.h"
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

PURE static inline bool is_equal_real(real x, real y) { return (fabs(x-y) < FLT_EPSILON); }

#define IS_EQUAL_REAL is_equal_real

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
#endif

#else

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
