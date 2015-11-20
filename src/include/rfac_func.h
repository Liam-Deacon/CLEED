/*********************************************************************
 *                           RFAC_FUNC.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
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

#include "attributes.h"
#include "rfac_def.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*********************************************************************
 * General routines (utility functions)
 *********************************************************************/

NONNULL()
int bgets(const char * const, long, long, char *);  /*!< get string from buffer */

NONNULL() RETURNS_NONNULL
char *file2buffer(const char * const);              /*!< copy file to buffer */

NONNULL() RETURNS_NONNULL
rfac_args *rfac_rdargs(int, char **);           /*!< read argument list */

NONNULL()
size_t rfac_lines(const char * const);               /*!< count lines */

NONNULL()
size_t rfac_clines(const char * const);              /*!< count comment lines */

NONNULL()
size_t rfac_nclines(const char * const);             /*!< count data lines */

NONNULL() RETURNS_NONNULL
const char *rfac_version(void);                     /*!< current program version */

void rfac_help(FILE *);                             /*!< help function */
void rfac_info(void);                               /*!< version information, etc. */

const char* get_rfactor_name(rfactor_type type);

int rfac_create_control_file_duplicates(const char *control_filename, size_t n);

/* line interpreter */
NONNULL()
void rfac_intindl(char *, rfac_spot *, size_t);

/*********************************************************************
 * data output
 *********************************************************************/
/* copy control file to output */
NONNULL()
int rfac_ctr2out(char *, FILE * );

/* general output function */
NONNULL()
int rfac_output(rfac_args *,  rfac *, size_t, size_t);

/*********************************************************************
 * data input
 *********************************************************************/

/* general input routine */
NONNULL() RETURNS_NONNULL
rfac_ivcur *rfac_ivcur_read(const char *control_file, const char *theory_file);

NONNULL() RETURNS_NONNULL
rfac_iv *rfac_iv_read_cleed(rfac_ivcur *, char *, char *);

/* read input of IV data */
NONNULL() RETURNS_NONNULL
rfac_iv *rfac_iv_read(const char *filename);

/*********************************************************************
 * sort, spline, smooth
 *********************************************************************/
int rfac_ivcur_process(rfac_ivcur *iv_cur, real vi);

NONNULL()
int rfac_lorentz(rfac_ivcur *, real, const char *); /* smooth IV curves */

NONNULL()
real rfac_splint(real, rfac_iv_data *, size_t);  /* prepare cubic spline */

NONNULL()
void rfac_iv_spline(rfac_iv *);                /* prepare cubic spline */

NONNULL()
int rfac_iv_sort(rfac_iv *iv);                 /* sort IV curve by energy */

NONNULL()
int rfac_iv_lorentz_smooth(rfac_iv *iv, real vi);
/*********************************************************************
 * r_factors
 *********************************************************************/
NONNULL()
size_t rfac_mklide(real *, real *, real *, real , real,
                   rfac_iv_data *, size_t, rfac_iv_data *, size_t);

/* prepare comparison */
NONNULL()
size_t rfac_mklist(real *, real *, real *, real,
                   rfac_iv_data *, size_t, rfac_iv_data *, size_t);

/* R1 factor */
NONNULL()
real rfac_r1(const real *, const real *, const real *);

/* R2 factor */
NONNULL()
real rfac_r2(const real *, const real *, const real *);

/* Rb1 factor */
NONNULL()
real rfac_rb(const real *, const real *, const real *);

/* Pendry's R factor */
NONNULL()
real rfac_rp(const real *, const real *, const real *, real);

/* Minimum R factor */
NONNULL()
real rfac_rmin(rfac_ivcur *, rfac_args *, real *, real *, real *);

void rfactor_print(rfactor_type type, real r_min, real rr,
                   real s_min, real e_range);

const char *rfactor_name(rfactor_type type);

/*********************************************************************
 * low-level routines
 *********************************************************************/
RETURNS_NONNULL
rfac_args *rfac_args_init(void);

RETURNS_NONNULL
rfac_iv *rfac_iv_init(void);

RETURNS_NONNULL ALLOC_SIZE(1)
rfac_iv *rfac_iv_alloc(size_t);

NONNULL()
void rfac_iv_free(rfac_iv*);

NONNULL()
void rfac_iv_copy(rfac_iv *dest, const rfac_iv *src);

real rfac_iv_get_energy_range(const rfac_iv *iv);

NONNULL()
void rfac_ivcur_free(rfac_ivcur*);

NONNULL()
void rfac_ivcur_free_all(rfac_ivcur*);

NONNULL(1)
int rfac_iv_to_arrays(const rfac_iv *, double *, double *, size_t *);

void rfac_ivcur_print(const rfac_ivcur *ivs);

size_t rfac_ivcur_get_number_of_datasets(const rfac_ivcur *iv_cur);

int rfac_main(int, char **);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_FUNC_H */
