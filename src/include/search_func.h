/*********************************************************************
 *                       SEARCH_FUNC.H
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
 *   GH/1995.08.21 - creation
 *   LD/2014.06.15 - added conditional GNU Scientific Library
 *                   interface to code.
 *   LD/2014.10.09 - updated function params to reflect new typedefs
 *********************************************************************/

/*!
 * \file
 * \brief Contains all the function prototypes for the search program.
 * \author Georg Held <g.held@reading.ac.uk>
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 * There are two code interfaces for the search routines. The original
 * interface uses calls to routines taken from the copyrighted
 * "Numerical Recipes in C", therefore for the open source version of the
 * code there is a new interface to the GNU Scientific Library - this is
 * a major re-write of parts of the SEARCH code and should therefore be
 * considered as highly experimental.
 *
 * \warning The open source version of the search program,
 * enabled when compiling with \c USE_GSL defined, is highly experimental
 * and needs significant testing.
 */

#ifndef SEARCH_FUNC_H
#define SEARCH_FUNC_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include "search_def.h"

#if !defined(USE_GSL) && !defined(USE_GSL) /* Original CLEED Interface */

/* sramoeba.c */
void sr_amoeba(real **, real *, int, real, real (*)(), int *);
real sr_amotry(real **, real *, real *, int, real (*)(), int, int *, real);

/* sramebsa.c */
void sr_amebsa(real **, real *, int , real *, real *, real,
               real (*)(), int *, real);
real sr_amotsa(real **, real *, real *, int, real *, real *,
               real (*)(), int, real *, real);
real sr_fluct(long *);

/* srpowell.c */
void sr_powell(real *, real **, int, real, int *, real *, real (*)() );

/* Drivers */
void sr_sa(size_t, real, const char *, const char *);
void sr_sx(size_t, real, const char *, const char *);
void sr_po(size_t, const char *, const char *);
void sr_er(size_t, real, const char *);

/* file input|output */
real sr_ckgeo(const real *);
size_t sr_ckrot(search_atom *, search *);
real sr_evalrf(real *);
int  sr_mkinp(const char *, const real *, size_t);
int  sr_mkinp_mir(const char *, const real *, size_t);
void sr_mklog(const char *);
int  sr_rdinp(const char *);
int  sr_rdver(const char * , real *, real **, int);

#else /* GNU Scientific Interface */

/* sramoeba_gsl.c - downhill simplex */
int sr_amoeba_gsl(gsl_matrix *, gsl_vector *, real , size_t *);
double sr_amoeba_eval(const gsl_vector *, void *);

/* sramebsa_gsl.c - simulated annealing */
void sr_amebsa_gsl(gsl_matrix *, gsl_vector *, gsl_vector *, real *, 
                   real, real (*)(), size_t *, real);
real sr_amotsa(gsl_matrix *, gsl_vector *, gsl_vector *, gsl_vector *, real *,
               real *, real (*)(), int , real *, real);
real sr_fluct(long *);

/* srpowell_gsl.c - Powell's method */
int sr_powell_gsl(gsl_vector *, gsl_matrix *, real, size_t *, real *);
void sr_linmin_gsl(gsl_vector *, gsl_vector *, real *, real (*)());

/* Drivers */
void sr_sa_gsl(size_t, real, char *, char *);     /* Simulated Annealing  */
void sr_sx_gsl(size_t, real, char *, char *);     /* Simplex method       */
void sr_po_gsl(size_t, char *, char *);           /* Powell's method      */
void sr_er_gsl(size_t, real, char *);             /* error evaluation     */

/* file input|output */
real sr_ckgeo(const gsl_vector *);
int sr_ckrot(search_atom *, search *);
double sr_evalrf_gsl(const gsl_vector *);
int sr_mkinp_gsl(const gsl_vector *, int, const char *);
int sr_mkinp_mir(const char *, gsl_vector *, int);
void sr_mklog(const char *);
int sr_rdinp(const char * );
int sr_rdver(const char *, gsl_vector *, gsl_matrix *, int);

#endif 

/* debye temperature */
real leed_inp_debye_temp(real , real , real);

/* help */
void search_usage(FILE *);
void search_info();

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SEARCH_FUNC_H */
