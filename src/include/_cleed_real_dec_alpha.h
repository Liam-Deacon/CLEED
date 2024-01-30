#ifndef _CLEED_REAL_DEC_ALPHA_H
#define _CLEED_REAL_DEC_ALPHA_H

#include <float.h>
#include <math.h>

#include "cc_attribute.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

typedef float real;

PURE static inline double sind(real x) { return sinf(x * (real)(M_PI / 180)); }
PURE static inline double cosd(real x) { return cosf(x * (real)(M_PI / 180)); }

PURE static inline real cleed_real_cos(real x) { return (cosf(x)); }
PURE static inline real cleed_real_cosd(real x) { return (cosd(x)); }
PURE static inline real cleed_real_sin(real x)  { return (sinf(x)); }
PURE static inline real cleed_real_sind(real x) { return (sind(x)); }
PURE static inline real cleed_real_atan2(real x, real y) { return atan2f(x, y); }

PURE static inline real cleed_real_cosh(real x) { return coshf(x); }
PURE static inline real cleed_real_sinh(real x) { return sinhf(x); }

PURE static inline real cleed_real_exp(real x) { return expf(x); }
PURE static inline real cleed_real_log(real x) { return logf(x); }

PURE static inline real cleed_real_fabs(real x) { return fabsf(x); }
PURE static inline real cleed_real_sqrt(real x) { return sqrtf(x); }

PURE static inline real cleed_real_cabs(real x, real y) { return hypotf(x, y); }
PURE static inline real cleed_real_hypot(real x, real y) { return hypotf(x, y); }

PURE static inline real cleed_real_nint(real x) { return (rintf(x));}


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
#endif


#endif /* _CLEED_REAL_DEC_ALPHA_H */