/*********************************************************************
 *                        SR_RNG.C
 *
 *  GPL-3.0-or-later
 *********************************************************************/

#include "sr_rng.h"

static uint64_t sr_rng_next_u64(sr_rng *rng)
{
  /* xorshift64* (Marsaglia / Vigna style), deterministic and fast. */
  uint64_t x = rng->state;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  rng->state = x;
  return x * 2685821657736338717ULL;
}

void sr_rng_seed(sr_rng *rng, uint64_t seed)
{
  if (seed == 0) seed = 0x9e3779b97f4a7c15ULL;
  rng->state = seed;
  /* Warm up. */
  (void)sr_rng_next_u64(rng);
}

double sr_rng_uniform01(sr_rng *rng)
{
  /* Use top 53 bits to build an IEEE-754 double in [0,1). */
  uint64_t u = sr_rng_next_u64(rng);
  uint64_t mantissa = u >> 11;
  return (double)mantissa * (1.0 / 9007199254740992.0); /* 2^53 */
}

