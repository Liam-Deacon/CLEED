/*********************************************************************
 *                           CPL_FUNC.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: Header file declaring prototype functions for
 *              complex operations.
 *
 * Changes:
 *   GH/1994.08.23 - creation
 *
 ********************************************************************/

#ifndef CPL_FUNC_H
#define CPL_FUNC_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*! multiplication (cribasfun.c) */
void cri_mul(real *, real *, real, real, real, real);

/*! division (cribasfun.c) */
void cri_div(real *, real *, real, real, real, real);

/*! square root (cribasfun.c) */
void cri_sqrt(real *, real *, real, real);

/*! square exponent (cribasfun.c) */
void cri_exp(real *, real *, real, real);
void cri_expi(real *, real *, real, real);

/*! integer powers of i */
void cri_powi(real *, real *, int);

/*********************************************************************
lower level functions
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* CPL_FUNC_H */
