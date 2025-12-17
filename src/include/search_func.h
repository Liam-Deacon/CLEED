/*********************************************************************
 *                       SEARCH_FUNC.H
 *
 *  Public function prototypes for SEARCH (csearch).
 *
 *  Historical versions used non-redistributable third-party routines for
 *  optimisation and for vector/matrix helpers. Those implementations
 *  have been removed; the remaining interfaces are provided by
 *  GPL-compatible, in-tree implementations.
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_FUNC_H
#define SEARCH_FUNC_H

#include <stdio.h>

/* Optimisers (derivative-free) */
int sr_amoeba(real **p, real *y, int ndim, real ftol, real (*funk)(real *), int *nfunk);
int sr_amebsa(real **p, real *y, int ndim, real *pb, real *yb, real ftol,
              real (*funk)(real *), int *iter, real temptr);
int sr_powell(real *p, real **xi, int n, real ftol, int *iter, real *fret,
              real (*func)(real *));

/* Drivers */
void sr_sa(int ndim, real dpos, const char *bak_file, const char *log_file);
void sr_sx(int ndim, real dpos, const char *bak_file, const char *log_file);
void sr_po(int ndim, const char *bak_file, const char *log_file);
void sr_er(int ndim, real dpos, const char *bak_file, const char *log_file);

/* file input|output */
real sr_ckgeo(real *);
int  sr_ckrot(struct sratom_str *, struct search_str *);
real sr_evalrf(real *);
int  sr_mkinp(real *, int, char *);
int  sr_rdinp(const char *);
int  sr_rdver(const char *, real *, real **, int);

/* debye temperature */
real leed_inp_debye_temp(real , real , real );

/* help */
void search_usage(FILE * );
void search_info();

#endif /* SEARCH_FUNC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
