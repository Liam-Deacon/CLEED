#ifndef _CLEED_REAL_IBM_H
#define _CLEED_REAL_IBM_H

#include <float.h>
#include <math.h>

#include "cc_attribute.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

typedef float real;

PURE static inline double sind(real x) { return sinf(x * (real)(M_PI / 180)); }
PURE static inline double cosd(real x) { return cosf(x * (real)(M_PI / 180)); }

PURE static inline real cleed_real_cos(double x) { return ((real) cos(x)); }
PURE static inline real cleed_real_cosd(double x) { return ((real) cosd((real)x)); }
PURE static inline real cleed_real_sin(double x)  { return ((real) sin(x)); }
PURE static inline real cleed_real_sind(double x) { return ((real) sind((real)x)); }
PURE static inline real cleed_real_atan2(double x, double y) { return (real) atan2(x, y); }

PURE static inline real cleed_real_cosh(double x) { return (real) cosh(x); }
PURE static inline real cleed_real_sinh(double x) { return (real) sinh(x); }

PURE static inline real cleed_real_exp(double x) { return (real) exp(x); }
PURE static inline real cleed_real_log(double x) { return (real) log(x); }

PURE static inline real cleed_real_fabs(double x) { return (real) fabs(x); }
PURE static inline real cleed_real_sqrt(double x) { return (real) sqrt(x); }

PURE static inline real cleed_real_cabs(double x, double y) { return (real) hypot(x, y); }
PURE static inline real cleed_real_hypot(double x, double y) { return (real) hypot(x, y); }

PURE static inline real cleed_real_nint(double x) { return ((real)rint(x));}


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
#endif


#endif /* _CLEED_REAL_IBM_H */