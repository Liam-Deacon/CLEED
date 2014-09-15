/*********************************************************************
GH/02.06.94

Macros for complex operations
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CPL_MACRO_H
#define CPL_MACRO_H

#include <math.h>

#define cri_abs R_cabs  /** ABS - absolute values **/
/*
#define cri_mul( p_res_r, p_res_i, fac1_r, fac1_i, fac2_r, fac2_i)   \
               { real f__x1_r, f__x1_i, f__x2_r, f__x2_i;            \
                 f__x1_r = fac1_r; f__x1_i = fac1_i;                 \
                 f__x2_r = fac2_r; f__x2_i = fac2_i;                 \
                 *(p_res_r) = f__x1_r * f__x2_r - f__x1_i * f__x2_i; \
                 *(p_res_r) = f__x1_r * f__x2_i + f__x1_i * f__x2_r; }                                                     
*/
#endif /* CPL_MACRO_H */


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif