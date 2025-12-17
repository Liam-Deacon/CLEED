/*********************************************************************
 *                        SR_RNG.H
 *
 *  GPL-3.0-or-later
 *
 *  Deterministic RNG for SEARCH routines. This avoids libc rand()
 *  variability across platforms and enables regression testing.
 *********************************************************************/

#ifndef SR_RNG_H
#define SR_RNG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sr_rng {
  uint64_t state;
} sr_rng;

/* Seeds the RNG; a seed of 0 is mapped to a non-zero value. */
void sr_rng_seed(sr_rng *rng, uint64_t seed);

/* Returns a uniform deviate in [0, 1). */
double sr_rng_uniform01(sr_rng *rng);

#ifdef __cplusplus
}
#endif

#endif /* SR_RNG_H */

