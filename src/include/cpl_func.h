/*********************************************************************
GH/23.08.94

functions for complex operations
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CPL_FUNC_H
#define CPL_FUNC_H

  /* multiplication (cribasfun.c) */
void cri_mul(real *, real *, real, real, real, real);
  /* division (cribasfun.c) */
void cri_div(real *, real *, real, real, real, real);
  /* square root (cribasfun.c) */
void cri_sqrt(real *, real *, real, real);
  /* square exponent (cribasfun.c) */
void cri_exp(real *, real *, real, real);
void cri_expi(real *, real *, real, real);
  /* integer powers of i */
void cri_powi(real *, real *, int);

/*********************************************************************
lower level functions
*********************************************************************/

#endif /* CPL_FUNC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif