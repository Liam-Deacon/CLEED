/************************************************************************
 *                            FTSMOOTH.H
 *
 *  Copyright 1995-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.05.30 - Creation
 *   GH/1995.06.01 - Start transformation from f(x=0) = 0 and
 *                   interpolate values from -x_0 to x_0.
 *   GH/1995.06.07 - x_0 = n_x/4 * x_step
 *   LD/2013.06.18 - Allow trimming of datasets with
 *                   '--range <arg1> <arg2>'
 *   LD/2014.07.04 - Added VERSION*, BUG* and PROG* defines
 *                 - Added C linkage with C++ compiler
 *   LD/2014.09.30 - OpenMP support and initial Doxygen markup
 ************************************************************************/

/*! \file
 *
 * Header file for \c ftsmooth (Fourier Transformation Smooth)
 */

#ifndef FTSMOOTH_H
#define FTSMOOTH_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/* define version information */
#define FTSMOOTH_VERSION 1.2
#define FTSMOOTH_VERSION_INFO "FTSMOOTH - version 1.2 LD/18.06.13"
#define FTSMOOTH_NOTES "ftsmooth is part of the CLEED package by Georg Held" \
"\n\t(contact g.held@reading.ac.uk)"
#ifndef BUG_CONTACT 
#define BUG_CONTACT "Liam Deacon (liam.deacon@diamond.ac.uk)"
#endif

#define FTSMOOTH_PROG "ftsmooth"
#define FTSMOOTH_PROG_COPYRIGHT "Copyright (C) 2008-2014 Georg Held\n" \
"Copyright (C) 2014 Liam Deacon"
#define FTSMOOTH_PROG_LICENSE "GNU General Public License v3"
#define FTSMOOTH_PROG_VERSION "2014.07.04 Development"
#define FTSMOOTH_PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define FTSMOOTH_PROG_MAINTAINER "Liam Deacon <liam.deacon@diamond.ac.uk>"
#define FTSMOOTH_PROG_SHORTDESC "Manipulate IV curves"

/* platform information */
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
#define FTSMOOTH_PROG_PLAT "Win32"
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define FTSMOOTH_PROG_PLAT "Win32_Cygwin"
#elif defined(__ANDROID__)
#define FTSMOOTH_PROG_PLAT "Android"
#elif defined(__linux__) || defined(__linux)
#define FTSMOOTH_PROG_PLAT "Linux"
#else
#define FTSMOOTH_PROG_PLAT "Unix"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <strings.h>
#include <float.h>
#include <ctype.h>

#if defined(_USE_OPENMP) && !defined(DEBUG)
#include <omp.h>            /* add openMP support if desired */
#endif


#ifndef STRSZ
#define STRSZ 128
#endif

#ifndef PI
#define PI 3.141592653589793
#endif

#ifndef SQRT_PI
#define SQRT_PI 0.79788456080286541644     /*!< sqrt(2/PI) */
#endif

/*! \typedef offset
 *  \brief indicates the offset operation type
 */
typedef enum {
  OFFSET_X_TO_VALUE=1,        /*!< make x += offset */
  OFFSET_X_TO_ZERO,           /*!< make x_min = 0 */
  OFFSET_Y_TO_VALUE,          /*!< make f(x) += offset */
  OFFSET_Y_TO_ZERO            /*!< make y_min = 0 */
} offset;

#define INVALID_ARGUMENT_ERROR -1

/* function prototypes */
int ftsmooth(FILE *out_stream, double *x, double *fx, size_t n_x,
        double cutoff, double tailoff, int stdout_flag);

void ftsmooth_usage(FILE *output);

void ftsmooth_info();

/* ftsmooth data manipulation functions */
size_t fts_read_data(FILE *, FILE *, double *, double *);

int fts_offset_data(double *, double *, size_t, double, int);

size_t fts_trim_data(double *, double *, size_t, double *, double *, size_t);

size_t fts_rm_neg_data(double *, double *, size_t);

int fts_print_data(FILE *, double *, double *, size_t);

int decode_ranges(double *lbound, double *ubound, size_t *i_r, char *argv);

void ftsmooth_debug(char *debug_str, char *tag, int argc, char *argv[],
    FILE *in_stream, FILE *out_stream,
    int *stdin_flag, int *stdout_flag,
    double *cutoff, double *tailoff, char *mode,
    int *offset_flag, double *offset, int *range_flag, int *i_r,
    double *lbound, double *ubound, int *del_flag);
      
int parse_args(int argc, char *argv[], 
    FILE *in_stream, FILE *out_stream,
    int *stdin_flag, int *stdout_flag,
    double *cutoff, double *tailoff, char *mode,
    int *offset_flag, double *offset, int *range_flag, size_t *i_r,
    double *lbound, double *ubound, int *del_flag);
      
/* globals */
char line_buffer[STRSZ];

/* macros */
#define ARG_PARSE_DBL(x)                                        \
if (i_arg < argc)                                               \
{                                                               \
  x = (double)atof(argv[i_arg]);                                \
}                                                               \
else                                                            \
{                                                               \
  fprintf(stderr, "***error (ftsmooth): "                       \
      "no argument value for option '%s'\n",                    \
      argv[i_arg-1]);                                           \
  exit(INVALID_ARGUMENT_ERROR);                                 \
}

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* FTSMOOTH_H */
