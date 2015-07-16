/************************************************************************
 *                            FTSMOOTH_FUNC.H
 *
 *  Copyright 1995-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 ************************************************************************/

/*! \file
 *
 * Header file for \c ftsmooth (Fourier Transformation Smooth) functions.
 */

#ifndef FTSMOOTH_FUNC_H
#define FTSMOOTH_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "gh_stddef.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {

extern "C" {
#endif

/* function prototypes */
int ftsmooth(FILE *out_stream, double *x, double *fx, size_t n_x,
        double cutoff, double tailoff, int stdout_flag);

void ftsmooth_usage(FILE *output);

void ftsmooth_info(void);

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
static inline void arg_parse_dbl(double *x, int argc, const char *argv[], int i_arg)
{
  if (i_arg < argc)
  {
    *x = (double)atof(argv[i_arg]);
  }
  else
  {
    ERROR_MSG("no argument value for option '%s'\n", argv[i_arg-1]);                                           \
    exit(INVALID_ARGUMENT_ERROR);
  }
}

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
} /* namespace cleed */
#endif

#endif /* FTSMOOTH_FUNC_H */
