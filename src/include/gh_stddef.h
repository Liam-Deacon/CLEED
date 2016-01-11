/*********************************************************************
 *                           GH_STDDEF.H
 *
 *  Copyright 2006-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1997.09.03 - creation
 ********************************************************************/

/*!
 * \file
 * \author Georg Held <g.held@reading.ac.uk>
 * \brief Header file for constants and macros generally used
 * throughout the CLEED code.
 *
 * \note When compiling with \c DEBUG defined, the maximum amount of
 * program control information will be printed to the log during execution.
 */

#ifndef STD_DEF_H
#define STD_DEF_H

#if __cplusplus
#include <cmath>
#else
#include <math.h> /* use for C compatibility */
#endif

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define true 1
#define false 0
#else
#include <stdbool.h>
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
using namespace std;

namespace cleed {
#endif

/*********************************************************************
 * define machine and set compiler options
 *********************************************************************/

/* set compiler specific options */
#if (!__GNUC__)
# define __attribute__(x) /* ignore GCC style attributes */
#endif

#if defined(_MSC_VER)
# define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#else
# if __GNUC__
#   define DLL_PUBLIC __attribute__((dllexport))
# else
#   define DLL_PUBLIC
# endif
#endif

/* Alternatives:
#define DEC
#define IBM
*/

#define IBM
/*
#define free(x) free((void *)(x))
*/

/*********************************************************************
 * output channels
 *********************************************************************/

#define STDOUT stdout /*!< alias for stdout file stream */
#define STDERR stderr /*!< alias for stderr file stream */
#define STDWAR stderr /*!< output redirection for warnings */
#define STDCTR stdout /*!< output redirection for normal messages */
#define STDCPU stdout /*!< output redirection for CPU messages */

/*********************************************************************
 * printing verbosity and debugging messages
 *********************************************************************/

/*! 
 * \define EXIT_ON_ERROR
 * \brief Exits program if error occurs.
 *
 * \define ERROR_LOG
 * \brief Error messages flag for debugging to #STDERR output.
 *
 * \define WARNING_LOG
 * \brief Warning messages flag for debugging to #STDWAR .
 *
 * \define CONTROL
 * \brief Control messages flag for debugging to #STDCTR
 *
 * \define CONTROL_ALL
 * \brief Adds maximum level of control information for debugging.
 *
 * \define ERROR_RETURN
 * \brief Exits with integer code 1 if #EXIT_ON_ERROR is defined, otherwise
 * the function will return with the given error code.
 */
 
#ifdef DEBUG
# define EXIT_ON_ERROR  1 /*!< Exits program if error occurs. */
# define ERROR_LOG      1 /*!< Enable error logging to stderr */
# define WARNING_LOG    1 /*!< Enable warning logging to #STDWAR */
#endif

#if VERBOSE == 1
# ifndef WARNING_LOG 1
#   define WARNING_LOG 1
# endif /* WARNING_LOG */
# ifndef ERROR_LOG 1
#   define ERROR_LOG 1
# endif /* ERROR_LOG */
#endif

#ifndef CONTROL_ALL
# define CONTROL       1
# define CONTROL_X     2
# define CONTROL_FLOW  3
# define CONTROL_IO    4
# define CONTROL_MBG   5
# define CONTROL_LSUM  6
# define CONTROL_MATB  7
# define CONTROL_ALL   8   /*!< The most verbose mode - includes all others */
# define CONTROL_LEVEL CONTROL_ALL
#endif

#ifndef CONTROL
# define CONTROL 0
#endif
#ifndef CONTROL_X
# define CONTROL_X 0
#endif
#ifndef CONTROL_FLOW
# define CONTROL_FLOW 0
#endif
#ifndef CONTROL_IO
# define CONTROL_IO 0
#endif
#ifndef CONTROL_MBG
# define CONTROL_MBG 0
#endif
#ifndef CONTROL_LSUM
# define CONTROL_LSUM 0
#endif
#ifndef CONTROL_MATB
# define CONTROL_MATB 0
#endif
#ifndef CONTROL_ALL
# define CONTROL_ALL 0
#endif
#ifndef CONTROL_LEVEL
# define CONTROL_LEVEL -1
#endif

#ifdef EXIT_ON_ERROR
# define ERROR_RETURN(i) exit(1)
# define ERROR_EXIT_RETURN(a,b)  exit((a))
#else
# define ERROR_RETURN(i) return((i))
# define ERROR_EXIT_RETURN(a,b)  return((b))
#endif

  /* remove annoying rest arguments warning with ISO C99 & GCC */
#if __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#endif /* GCC */

#if ERROR_LOG && __STDC_VERSION__ >= 199901L
# ifdef _MSC_VER
#   define ERROR_MSG(fmt, ...)                                                \
  do {fprintf(STDERR, "***error [%s:%d:%s()]: "                               \
      fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while(0)
# else /* assume GNU extension */
#   define ERROR_MSG(fmt, ...)                                                \
  do {fprintf(STDERR, "***error [%s:%d:%s()]: "                               \
      fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while(0)
# endif
#else
# define ERROR_MSG(fmt, ...) {} /* dummy message: will warn of unused value */
#endif

#if WARNING_LOG && __STDC_VERSION__ >= 199901L
# ifdef _MSC_VER
#   define WARNING_MSG(fmt, ...)                                              \
  do {fprintf(STDWAR, "*warning [%s:%d:%s()]: "                               \
      fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while(0)
# else /* assume GNU extension */
#   define WARNING_MSG(fmt, ...)                                              \
  do {fprintf(STDWAR, "*warning [%s:%d:%s()]: "                               \
      fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while(0)
# endif
#else
# define WARNING_MSG(fmt, ...) {} /* dummy message: will warn of unused value */
#endif

#if CONTROL && __STDC_VERSION__ >= 199901L
# ifdef _MSC_VER
#   define CONTROL_MSG(level, fmt, ...)                                       \
  do {if (level <= CONTROL_LEVEL) {                                           \
        fprintf(STDCTR, "%s(): " fmt, __func__, __VA_ARGS__);}                \
     } while(0)
# else /* assume GNU extension */
#   define CONTROL_MSG(level, fmt, ...)                                       \
  do {if (level <= CONTROL_LEVEL) {                                           \
        fprintf(STDCTR, "%s(): " fmt, __func__, ##__VA_ARGS__);}              \
     } while(0)
# endif
#else
# define CONTROL_MSG(level, fmt, ...) {} /* dummy message */
#endif

#if __GNUC__
#pragma GCC diagnostic pop /* restore rest aguments warning */
#endif /* __GNUC__ */

/*********************************************************************
 * general mathematical definitions / constants
 *********************************************************************/

#ifdef M_PI
static const double PI = M_PI;
#else
static const double PI = 3.1415926535897932385;
#endif

static const double DEG_TO_RAD = 0.017453293;  /*!< conversion degree to radian */
static const double RAD_TO_DEG = 57.29578;     /*!< conversion radian to degree */

static const double M2_H = 0.2631894506957162; /*!< 2*m/h       [eV^-1   A^-2] */
static const double SQRT_M2_H = 0.5130199320647456; /*!< sqrt(2*m/h) [eV^-0.5 A^-1] */

/*********************************************************************
 * general other definitions / constants
 *********************************************************************/

static const size_t KBYTE = 1024;
static const size_t MBYTE = 1048576;

/*********************************************************************
 * special definitions
 *********************************************************************/

enum {
  STRSZ = 256  /*!< maximum length of strings */
};

enum {
  NAMSZ = 128  /*!< maximum length of name */
};

enum {
  NO = 0,
  YES = 1
};

enum { U_END_OF_LIST = (sizeof(size_t) - 1) }; /*!< list terminator (unsigned int) */
enum { I_END_OF_LIST = -9999 };                /*!< list terminator (integer)*/
static const double F_END_OF_LIST = -9999.;    /*!< list terminator (float)  */

/*
#define IEND_OF_LIST   I_END_OF_LIST  //!< alias for list terminator (integer)
#define FEND_OF_LIST   F_END_OF_LIST  //!< alias for list terminator (float)
*/

/*********************************************************************
 * macros:
 *********************************************************************/
#if defined(__GNUC__)
# define MAX(a,b) /*!< returns maximum value */                                \
__extension__ ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

# define MIN(a,b) /*!< returns minimum value */                                \
__extension__ ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

#else /* use standard C */

# define MAX(a,b) ((a) > (b) ? (a) : (b))
# define MIN(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifdef __cplusplus /* use function templates */

template <typename T> inline T square(T x)
{
  return static_cast<T>(std::pow(x, 2));
}

template <typename T> inline T int_round(T x)
{
  return static_cast<int>(std::round(x));
}

template <typename T> inline T int_roundup(T x)
{
  return static_cast<int>(std::ceil(x+5*DBL_EPSILON));
}

template <typename T> inline bool is_odd(T n)
{
  return (n % 2);
}

#elif __STDC_VERSION__ >= 201112L

__attribute__ ((pure))
static inline long double squarel(long double x) { return ((x)*(x)); } /*!< returns the value squared*/
__attribute__ ((pure))
static inline double square(double x) { return ((x)*(x)); } /*!< returns the value squared*/
__attribute__ ((pure))
static inline float squaref(float x) { return ((x)*(x)); } /*!< returns the value squared*/
__attribute__ ((pure))
static inline int squarei(int x) { return ((x)*(x)); } /*!< returns the value squared*/
__attribute__ ((pure))
static inline size_t squareu(size_t x) { return ((x)*(x)); } /*!< returns the value squared*/

__attribute__ ((pure))
static inline bool is_odd(int n) {return (n % 2); }
__attribute__ ((pure))
static inline bool is_oddu(size_t n) {return (n % 2); }

__attribute__ ((pure))
static inline double m1p(double n) { return (((int)n % 2)?(-1.):(1.)); }
__attribute__ ((pure))
static inline double m1pf(float n) { return (((int)n % 2)?(-1.):(1.)); }
__attribute__ ((pure))
static inline double m1pi(int n) { return ((n % 2)?(-1.):(1.)); }
__attribute__ ((pure))
static inline double m1pu(size_t n) { return ((n % 2)?(-1.):(1.)); }

__attribute__ ((pure))
static inline int int_min(int x, int y) { return ( (x < y) ? x : y); }
__attribute__ ((pure))
static inline int int_max(int x, int y) { return ( (x > y) ? x : y); }
__attribute__ ((pure))
static inline size_t uint_min(size_t x, size_t y) { return ( (x < y) ? x : y); }
__attribute__ ((pure))
static inline size_t uint_max(size_t x, size_t y) { return ( (x > y) ? x : y); }
__attribute__ ((pure))
static inline float minf(float x, float y) { return ( (x < y) ? x : y); }
__attribute__ ((pure))
static inline float maxf(float x, float y) { return ( (x > y) ? x : y); }
__attribute__ ((pure))
static inline double mind(double x, double y) { return ( (x < y) ? x : y); }
__attribute__ ((pure))
static inline double maxd(double x, double y) { return ( (x > y) ? x : y); }
__attribute__ ((pure))
static inline int irnd(double x) { return ((int)( (x) + .5 )); }
__attribute__ ((pure))
static inline int irndf(float x) { return ((int)( (x) + .5 )); }

__attribute__ ((pure))
static inline double irndup(double x) { return ((int)( (x) + 1.)); }
__attribute__ ((pure))
static inline double irndupf(float x) { return ((int)( (x) + 1.)); }

#define SQUARE(X) _Generic( (X),  \
    long double: squarel,         \
    default: square,              \
    float: squaref,               \
    int: squarei,                 \
    size_t: squareu               \
) (X)

#define ODD(X) _Generic( (X),     \
    default: is_odd,              \
    size_t:  is_oddu              \
) (X)

#define M1P(X) _Generic( (X),     \
    default: m1p,                 \
    float:  m1pf,                 \
    int:    m1pi,                 \
    size_t: m1pu                  \
) (X)

#ifndef min
#define min(X,Y) _Generic( (X),   \
    default: mind,                \
    float:  minf,                 \
    int:    int_min,              \
    size_t: uint_min              \
) (X, Y)
#endif

#ifndef max
#define max(X,Y) _Generic( (X),     \
      default: maxd,                \
      float:  maxf,                 \
      int:    int_max,              \
      size_t: uint_max              \
  ) (X, Y)
#endif

#define int_round(X) _Generic( (X),     \
    default: irnd,                      \
    float: irndf                        \
) (X)

#define int_roundup(X) _Generic( (X),   \
    default: irndup,                    \
    float: irndupf                      \
) (X)

#else /* do not use type-generic macros */

#define square(x) ((x)*(x))
#define is_odd(n) (((n) % 2))
#define m1p(n) (((n) % 2)?(-1.):(1.))
#define int_round(x) ((int)(round((x))))
#define int_roundup(x) ((int)( ceil( (x) + 5*DBL_EPSILON )))

/* aliases */
#define SQUARE square
#define ODD is_odd
#define M1P m1p

#ifndef min
#define min MIN
#endif

#ifndef max
#define max MAX
#endif

#endif

#define IS_END_OF_LIST(var)

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif

#endif /* GH_STDDEF_H */
