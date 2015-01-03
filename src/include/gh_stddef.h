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

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*********************************************************************
 * define machine
 *********************************************************************/

/* Alternatives:
#define DEC
#define IBM
*/

#define IBM
/*
#define free(x) free((void *)(x))
*/

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <limits.h>

/*********************************************************************
 * output channels
 *********************************************************************/

#define STDOUT stdout     /*!< alias for stdout file stream */
#define STDERR stderr     /*!< alias for stderr file stream */
#define STDWAR stderr     /*!< output redirection for warnings */
#define STDCTR stdout     /*!< output redirection for normal messages */
#define STDCPU stdout     /*!< output redirection for CPU messages */

/*********************************************************************
 * printing verbosity and debugging messages
 *********************************************************************/

/*! 
 * \define EXIT_ON_ERROR
 * \brief Exits program if error occurs.
 *
 * \define ERROR
 * \brief Error messages flag for debugging to #STDERR output.
 *
 * \define WARNING
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
# define ERROR          1 /*!< Enable error logging to stderr */
# define WARNING        1 /*!< Enable warning logging to #STDWAR */
#endif

#if VERBOSE == 1
# ifndef WARNING 1
#   define WARNING 1
# endif /* WARNING */
# ifndef ERROR 1
#   define ERROR 1
# endif /* ERROR */
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

#if ERROR && __STDC_VERSION__ >= 199901L
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

#if WARNING && __STDC_VERSION__ >= 199901L
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



/*********************************************************************
 * general mathematical definitions / constants
 *********************************************************************/

#ifdef _XOPEN_SOURCE
# define PI  M_PI
#else
# define PI  3.1415926535897932385
#endif

static const double DEG_TO_RAD = 0.017453293; /*!< conversion degree to radian */
static const double RAD_TO_DEG = 57.29578;    /*!< conversion radian to degree */

static const double M2_H = 0.2631894506957162;/*!< 2*m/h       [eV^-1   A^-2] */
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

static const size_t U_END_OF_LIST = (sizeof(size_t) - 1); /*!< list terminator (unsigned int) */
static const int I_END_OF_LIST = -9999;       /*!< list terminator (integer)*/
static const double F_END_OF_LIST = -9999.;   /*!< list terminator (float)  */

#define IEND_OF_LIST   I_END_OF_LIST  /*!< alias for list terminator (integer)*/
#define FEND_OF_LIST   F_END_OF_LIST  /*!< alias for list terminator (float)  */

/*********************************************************************
 * macros:
 *********************************************************************/
#define MAX(a,b) /*!< returns maximum value */                                \
({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

#define MIN(a,b) /*!< returns minimum value */                                \
({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })


#define SQUARE(x) ((x)*(x))           /*!< returns the value squared*/

#define ODD(n)    ((n)%2)             /*!< evaluates whether integer is odd */
#define M1P(n)    (((n)%2)?(-1.):(1.))

#define IS_END_OF_LIST(var)

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* GH_STDDEF_H */
