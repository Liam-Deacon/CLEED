/*********************************************************************
GH/28.07.94

basic quantum mechanical functions for the leed programs
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef QM_FUNC_H
#define QM_FUNC_H

/*
  C.G.-coefficients:
*/
  /* Calculate C.G. coefficients (qmcgc.c) */
int mk_cg_coef(int);
  /* Return C.G. coefficient (qmcgc.c) */
double cg(int ,int ,int ,int ,int ,int );
  /* Return relevant storage information (qmcgc.c) */
double * cg_info (int , int , int , int , int* , int* , int* );
  /* list C.G. coefficients (qmcgc.c) */
void show_cg_coef();

/*
  Spherical harmonics
*/

  /* Calculate spherical harmonics (qmylm.c) */
mat r_ylm(mat, real, real, int);
mat c_ylm(mat, real, real, real, int);
int mk_ylm_coef(int);

/*
  Hankel/Bessel functions
*/

  /* Calculate Hankel and Bessel functions (qmhank.c, qmbess.c) */
mat r_hank1 (mat, real, int);
mat c_hank1 (mat, real, real, int);
mat c_bess  (mat, real, real, int);
mat c_bessm (mat, real, real, int);

/*********************************************************************
lower level functions
*********************************************************************/

#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif