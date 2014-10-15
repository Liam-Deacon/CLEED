/*********************************************************************
 *                           QM_FUNC.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: basic quantum mechanical functions for the leed programs
 *
 * Changes:
 *   GH/1994.07.28 - creation
 *   LD/2014.10.09 - change prototypes to have l_max as size_t
 *********************************************************************/

#ifndef QM_FUNC_H
#define QM_FUNC_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*********************************************************************
 * Clebsch Gordon (C.G.) coefficients:
 *********************************************************************/

  /* Calculate C.G. coefficients (qmcgc.c) */
int mk_cg_coef(size_t);
  /* Return C.G. coefficient (qmcgc.c) */
double cg(int, int, int, int, int, int);
  /* Return relevant storage information (qmcgc.c) */
double *cg_info(int, int, int, int, int *, int *, int *);
  /* list C.G. coefficients (qmcgc.c) */
void show_cg_coef();

/*********************************************************************
 * Spherical harmonics
 *********************************************************************/

  /* Calculate spherical harmonics (qmylm.c) */
mat r_ylm(mat, real, real, size_t);
mat c_ylm(mat, real, real, real, size_t);
int mk_ylm_coef(size_t);

/*********************************************************************
 * Hankel/Bessel functions
 *********************************************************************/

/* Calculate Hankel and Bessel functions (qmhank.c, qmbess.c) */
mat r_hank1(mat, real, size_t);
mat c_hank1(mat, real, real, size_t);
mat c_bess(mat, real, real, size_t);
mat c_bessm(mat, real, real, size_t);

/*********************************************************************
lower level functions
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
