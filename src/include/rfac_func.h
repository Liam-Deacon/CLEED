/*********************************************************************
 *                           RFAC_FUNC.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.11 - creation
 *   LD/2014.10.09 - changed struct declarations to their typedef
 *                   equivalents.
 *                 - added some Doxygen markup
 *********************************************************************/

/*! \file
 *  \brief Header file for function prototypes used in the R factor program.
 */

#ifndef RFAC_FUNC_H
#define RFAC_FUNC_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include "rfac_def.h"

/*********************************************************************
 * General routines (utility functions)
 *********************************************************************/

int bgets(const char *, long, long, char *);  /*!< get string from buffer */
char *file2buffer(const char *);              /*!< copy file to buffer */
rfac_args *rfac_rdargs(int, char **);           /*!< read argument list */

size_t rfac_lines(const char *);               /*!< count lines */
size_t rfac_clines(const char *);              /*!< count comment lines */
size_t rfac_nclines(const char *);             /*!< count data lines */

char *rfac_version(char *);                    /*!< current program version */
void rfac_help(FILE *);                        /*!< help function */
void rfac_info();                              /*!< version information, etc. */

/* line interpreter */
void rfac_intindl(char *, rfac_spot *, size_t);

/*********************************************************************
 * data output
 *********************************************************************/
/* copy control file to output */
int rfac_ctr2out(char *, FILE * );

/* general output function */
int rfac_output(rfac_args *,  rfac *, size_t, size_t);

/*********************************************************************
 * data input
 *********************************************************************/

/* general input routine */
rfac_ivcur *rfac_ivcur_read(const char *control_file, const char *theory_file);

rfac_iv_data *rfac_iv_data_read_cleed(rfac_ivcur *, char *, char *);

/* read input of IV data */
rfac_iv_data *rfac_iv_read(const char *filename);

/*********************************************************************
 * sort, spline, smooth
 *********************************************************************/
int rfac_lorentz(rfac_ivcur *, real, const char *); /* smooth IV curves */

real rfac_splint(real, rfac_iv_data *, size_t);  /* prepare cubic spline */

void rfac_iv_spline(rfac_iv *);                /* prepare cubic spline */
int rfac_iv_sort(rfac_iv *iv);                 /* sort IV curve by energy */

int rfac_iv_lorentz_smooth(rfac_iv *iv, real vi);
/*********************************************************************
 * r_factors
 *********************************************************************/
size_t rfac_mklide(real *, real *, real *, real , real,
                 rfac_iv_data *, size_t, rfac_iv_data *, size_t);

/* prepare comparison */
size_t rfac_mklist(real *, real *, real *, real,
                 rfac_iv_data *, size_t, rfac_iv_data *, size_t);

/* R1 factor */
real rfac_r1(const real *, const real *, const real *);

/* R2 factor */
real rfac_r2(const real *, const real *, const real *);

/* Rb1 factor */
real rfac_rb(const real *, const real *, const real *);

/* Pendry's R factor */
real rfac_rp(const real *, const real *, const real *, real);

/* Minimum R factor */
real rfac_rmin(rfac_ivcur *, rfac_args *, real *, real *, real *);

/*********************************************************************
 * low-level routines
 *********************************************************************/
rfac_args *rfac_args_init();

rfac_iv *rfac_iv_init();
rfac_iv *rfac_iv_alloc(size_t);
void rfac_iv_free(rfac_iv*);
void rfac_iv_copy(rfac_iv *dest, const rfac_iv *src);

void rfac_ivcur_free(rfac_ivcur*);
void rfac_ivcur_free_all(rfac_ivcur*);

int rfac_iv_to_arrays(const rfac_iv *, double *, double *, size_t *);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_FUNC_H */
