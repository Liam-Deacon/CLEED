/*********************************************************************
 *                       SEARCH_FUNC.H
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
 *   GH/1995.08.21 - creation
 *   LD/2014.06.15 - added conditional GNU Scientific Library
 *                   interface to code.
 *   LD/2014.10.09 - updated function params to reflect new typedefs
 *********************************************************************/

/*!
 * \file
 * \brief Contains all the function prototypes for the search program.
 * \author Georg Held <g.held@reading.ac.uk>
 * \author Liam Deacon <liam.m.deacon@gmail.com>
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

#if (!__GNUC__)
# define __attribute__(x) /* empty to disable GCC attribute extension */
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include "search_def.h"

/* Drivers */
__attribute__((nonnull)) void sr_ga(size_t, real, const char *, const char *);
__attribute__((nonnull)) void sr_sa(size_t, real, const char *, const char *);
__attribute__((nonnull)) void sr_sx(size_t, real, const char *, const char *);
__attribute__((nonnull)) void sr_po(size_t, const char *, const char *);
__attribute__((nonnull)) void sr_er(size_t, real, const char *);

/* file input|output */
__attribute__((nonnull)) real sr_ckgeo(const cleed_vector *);
__attribute__((nonnull)) size_t sr_ckrot(search_atom *, search *);
__attribute__((nonnull)) real sr_evalrf(cleed_vector *);
__attribute__((nonnull)) int sr_mkinp(const char *, const cleed_vector *, size_t);
__attribute__((nonnull)) int sr_mkinp_mir(const char *, const cleed_vector *, size_t);
__attribute__((nonnull)) void sr_mklog(const char *);
__attribute__((nonnull)) int sr_rdinp(const char *);
__attribute__((nonnull)) int sr_rdver(const char *ver_file,
                                      cleed_vector *y,
                                      cleed_basic_matrix *p,
                                      int n_dim);
__attribute__((nonnull)) int sr_mkver(const cleed_vector *y,
                                      const cleed_basic_matrix *p,
                                      const size_t n_dim);

//#if USE_CBLAS || USE_LAPACK || USE_MKL /* BLAS interface */
//#error "BLAS/LAPACK version not implemented"
#if (USE_CYTHON == 1 || USE_PYTHON == 1 || USE_SCIPY == 1) /* Cython|SciPy inteface */

#include "cysearch_api.h"

#define SR_RF sr_evalrf
#define SR_ER sr_er
#define SR_SX sr_sx
#define SR_SA sr_sa
#define SR_PO sr_po
#define SR_GA sr_ga

static inline SR_AMOEBA_FUNC(cleed_basic_matrix *p, cleed_vector *y, size_t n_dim,
                             cleed_real ftol, cleed_real (*funk)(), size_t *nfunk) {
  import_cysearch();
  cy_simplex(p, y, n_dim, ftol, nfunk);
}

static inline SR_SIMANN_FUNC(cleed_basic_matrix *p, cleed_vector *y, size_t n_dim,
                             cleed_vector *pb, cleed_vector *yb, cleed_real ftol,
                             cleed_real (*funk)(), size_t *iter, cleed_real temptr) {
  import_cysearch();
  cy_simann(p, y, n_dim, pb, yb, ftol, iter, temptr);
}

static inline SR_POWELL_FUNC(cleed_vector *p, cleed_basic_matrix *y, size_t n_dim,
                        cleed_real ftol, size_t *iter, cleed_real *fret,
                        cleed_real (*func)()) {
  import_cysearch();
  cy_powell(p, y, n_dim, ftol, iter, fret);
}
#define SR_GENETIC_FUNC {ERROR_MSG("Genetic Algorithm not implemented\n"); exit(1);}

#elif USE_GSL /* GNU Scientific Library Interface */
#warning "GSL implementation not yet complete"
/* sramoeba_gsl.c - downhill simplex */
int sr_amoeba(cleed_basic_matrix *, cleed_vector *, real , size_t *);
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

#else /* Original CLEED Interface - uses NR routines */

#define SR_RF sr_evalrf
#define SR_ER sr_er
#define SR_SX sr_sx
#define SR_SA sr_sa
#define SR_PO sr_po
#define SR_GA sr_ga

#define SR_AMOEBA_FUNC sr_simplex
#define SR_SIMANN_FUNC sr_amebsa
#define SR_POWELL_FUNC sr_powell
#define SR_GENETIC_FUNC {ERROR_MSG("Genetic Algorithm not implemented\n"); exit(1);}

/* new simplex version */
void sr_simplex(cleed_basic_matrix *p, cleed_vector *y, size_t n_dim,
                cleed_real ftol, cleed_real (*funk)(), size_t *nfunk);

/* sramoeba.c */
__attribute__((nonnull))
void sr_amoeba(cleed_basic_matrix *p, cleed_vector *y, size_t n_dim,
               cleed_real ftol, cleed_real (*funk)(), size_t *nfunk);

__attribute__((nonnull))
cleed_real sr_amotry(cleed_basic_matrix *p, cleed_vector *y, cleed_vector *psum,
               size_t n_dim, cleed_real (*funk)(), size_t ihi,
               size_t *nfunk, cleed_real fac);

/* sramebsa.c */
__attribute__((nonnull))
void sr_amebsa(cleed_basic_matrix *p, cleed_vector *y, size_t n_dim,
               cleed_vector *pb, cleed_vector *yb, cleed_real ftol,
               cleed_real (*funk)(), int *iter, cleed_real temptr);

__attribute__((nonnull))
cleed_real sr_amotsa(cleed_matrix *p, cleed_vector *y, cleed_vector *psum,
               size_t n_dim, cleed_vector *pb, cleed_real *yb,
               cleed_real (*funk)(), size_t ihi, cleed_real *yhi,
               cleed_real fac);

/* srpowell.c */
__attribute__((nonnull))
void sr_powell(cleed_vector *p, cleed_basic_matrix *xi, size_t n,
               cleed_real ftol, size_t *iter, cleed_real *fret,
               cleed_real (*func)() );

#endif 

/* debye temperature */
real leed_inp_debye_temp(real , real , real);

/* help */
void search_usage(FILE *);
void search_info(void);

int search_main(int, char **); /* for python entry into main */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
#endif

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* SEARCH_FUNC_H */
