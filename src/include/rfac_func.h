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

/*! \file rfac_func.h
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
rfac_args rf_rdargs(int, char **);            /*!< read argument list */

size_t rf_lines(const char *);               /*!< count lines */
size_t rf_clines(const char *);              /*!< count comment lines */
size_t rf_nclines(const char *);             /*!< count data lines */

char *rf_version(char *);                    /*!< current program version */
void rf_help(FILE *);                        /*!< help function */
void rf_info();                              /*!< version information, etc. */

/*!
 * line interpreter
 */
void rf_intindl(char *, rfac_spot *, size_t);

/*********************************************************************
 * data output
 *********************************************************************/

/*!
 * copy control file to output
 */
int rf_ctr2out(char *, FILE * );

/*! general output function */
int rf_output(rfac_args *,  rfac *, size_t, size_t);

/*********************************************************************
 * data input
 *********************************************************************/

/* general input routine */
rfac_ivcur *rf_input(const char *, const char *);

rfac_iv_data *rf_rdcleed(rfac_ivcur *, char *, char *);

/* read input of experimental data */
rfac_iv_data *rf_rdexpt(rfac_ivcur *, const char *);

/* read input of theoretical data */
rfac_iv_data *rf_rdthe(rfac_ivcur *, char *);

/* read input of theoretical data */
rfac_iv *rf_rdvh_beams(rfac_ivcur *, char *, char *);

/*********************************************************************
 * sort, spline, smooth
 *********************************************************************/
int rf_sort(rfac_ivcur *);                     /* sort theoretical IV curve */
int rf_lorentz(rfac_ivcur *, real, const char *);
                                               /* smooth IV curves */
void rf_spline(rfac_iv *);                     /* prepare cubic spline */
real rf_splint(real, rfac_iv_data *, size_t);  /* prepare cubic spline */

/*********************************************************************
 * r_factors
 *********************************************************************/
size_t rf_mklide(real *, real *, real *, real , real,
                 rfac_iv_data *, size_t, rfac_iv_data *, size_t);

/* prepare comparison */
size_t rf_mklist(real *, real *, real *, real,
                 rfac_iv_data *, size_t, rfac_iv_data *, size_t);

/* R1 factor */
real rf_r1(const real *, const real *, const real *);

/* R2 factor */
real rf_r2(const real *, const real *, const real *);

/* Rb1 factor */
real rf_rb(const real *, const real *, const real *);

/* Pendry's R factor */
real rf_rp(const real *, const real *, const real *, real);

/* Minimum R factor */
real rf_rmin(rfac_ivcur *, rfac_args *, real *, real *, real *);

/*********************************************************************
 * low-level routines
 *********************************************************************/

rfac_iv *rfac_iv_init();
rfac_iv *rfac_iv_alloc(size_t n_eng);
void rfac_iv_free(rfac_iv*);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_FUNC_H */
