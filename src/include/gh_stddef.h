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

/*********************************************************************
 * output channels
 *********************************************************************/

#define STDOUT stdout     /*!< alias for stdout file stream */
#define STDERR stderr     /*!< alias for stderr file stream */
#define STDWAR stderr     /*!< output redirection for warnings */
#define STDCTR stdout     /*!< output redirection for normal messages */
#define STDCPU stdout     /*!< output redirection for CPU messages */

/*********************************************************************
 * printing verbosity
 *********************************************************************/

/*! 
 * \define EXIT_ON_ERROR
 * \brief Exits program if error occurs.
 *
 * \define ERROR
 * \brief Adds error messages to #STDERR output.
 *
 * \define WARNING
 * \brief Adds warning messages to #STDWAR output.
 *
 * \define CONTROL
 * \brief Adds control messages for debugging to #STDCTR
 *
 * \define CONTROL_ALL
 * \brief Adds maximum level of control information for debugging.
 */
 
#ifdef DEBUG
#define CONTROL_ALL     /*!< Maximum verbosity for control output */
#define EXIT_ON_ERROR   /*!< Exits program if error occurs. */
#define ERROR           /*!< Enable error logging to stderr */
#define WARNING         /*!< Enable warning logging to #STDWAR */
#endif

#ifdef CONTROL_ALL
#define CONTROL
#define CONTROL_X
#define CONTROL_FLOW
#define CONTROL_IO
#define CONTROL_MBG
#define CONTROL_LSUM
#define CONTROL_MATB
#define CONTROL_ALL       /*!< The most verbose mode - includes all others */
#endif

#if VERBOSE == 1
#  ifndef WARNING 1
#  define WARNING 1
#  endif /* WARNING */
#  ifndef ERROR 1
#  define ERROR 1
#  endif /* ERROR */
#endif

/*********************************************************************
 * general mathematical definitions / constants
 *********************************************************************/

#ifdef _XOPEN_SOURCE
#define PI  M_PI
#else
#define PI  3.1415926535897932385
#endif

#define DEG_TO_RAD 0.017453293         /*!< conversion degree to radian */
#define RAD_TO_DEG 57.29578            /*!< conversion radian to degree */

#define M2_H      0.2631894506957162   /*!< 2*m/h       [eV^-1   A^-2] */
#define SQRT_M2_H 0.5130199320647456   /*!< sqrt(2*m/h) [eV^-0.5 A^-1] */

/*********************************************************************
 * general other definitions / constants
 *********************************************************************/

#define KBYTE 1024
#define MBYTE 1048576

/*********************************************************************
 * special definitions
 *********************************************************************/

#define STRSZ 256                /*!< maximum length of strings */

#define I_END_OF_LIST   -9999    /*!< list terminator (integer)*/
#define F_END_OF_LIST   -9999.   /*!< list terminator (float)  */

#define IEND_OF_LIST   I_END_OF_LIST  /*!< alias for list terminator (integer)*/
#define FEND_OF_LIST   F_END_OF_LIST  /*!< alias for list terminator (float)  */

/*********************************************************************
 * macros:
 *********************************************************************/

#define MAX(x,y)  ((x)>(y))?(x):(y) /*!< returns maximum value */
#define MIN(x,y)  ((x)<(y))?(x):(y) /*!< returns minimum value */
#define SQUARE(x) (x)*(x)           /*!< returns the value squared*/

#define ODD(n)    ((n)%2)           /*!< evaluates whether integer is odd */
#define M1P(n)    (((n)%2)?(-1.):(1.))

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* GH_STDDEF_H */
