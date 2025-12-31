/*********************************************************************
 *                       CLEED_CORE.H
 *
 *  Core library entry points for CLEED.
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CLEED_CORE_H
#define CLEED_CORE_H

#include <stddef.h>

/**
 * @brief In-memory LEED curve representation.
 */
typedef struct cleed_curve {
  size_t n;
  double *E;
  double *I;
} cleed_curve;

/**
 * @brief Return the core library version string.
 */
const char *cleed_core_version(void);

#endif /* CLEED_CORE_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
