/***********************************************************************
LD/29.06.2014
file contains function:

  real ran1(long *idum)

 random number generator of Park and Miller using minstd
 function from GSL

 Changes:
LD/29.06.2014 - Creation of open source version of ran1 function
2026-01-09    - Replace rand()/srand() with portable xorshift64 algorithm
***********************************************************************/
#include "real.h"
// cppcheck-suppress missingIncludeSystem
#include <limits.h>
// cppcheck-suppress missingIncludeSystem
#include <stdint.h>

/**
 * @brief Internal state for the xorshift64* PRNG.
 *
 * This implementation uses the xorshift64* algorithm (Vigna 2016), which
 * provides better statistical properties and portability compared to libc
 * rand(). The state is module-static (not thread-safe without external sync).
 */
static uint64_t ran1_state = 0;

/**
 * @brief Advance the xorshift64* PRNG and return next 64-bit value.
 */
static uint64_t ran1_next64(void)
{
  uint64_t s = ran1_state;
  s ^= s >> 12;
  s ^= s << 25;
  s ^= s >> 27;
  ran1_state = s;
  return s * UINT64_C(0x2545F4914F6CDD1D);
}

real ran1(long *idum)

/************************************************************************

 Minimal random number generator by Park and Miller with Bays-Durham
 shuffle and added safeguards. Returns a uniform random deviate between
 0.0 and 1.0 (exclusive of the endpointvalues).

 Call with idum a negative integer to initialize; thereafter do not
 alter idum inbetween successive deviates in a sequence. RNMX should
 approximate the largest floating value that is less than 1.0.

************************************************************************/
{
    /* Initialize or reseed when idum is negative or state is zero */
    if (*idum < 0 || ran1_state == 0) {
        /* Use absolute value of idum as seed; ensure non-zero state */
        uint64_t seed = (uint64_t)(*idum);
        if (*idum < 0) {
            /* Avoid signed overflow when idum == LONG_MIN. */
            seed = (uint64_t)(0 - seed);
        }
        ran1_state = (seed == 0) ? UINT64_C(0x853c49e6748fea9b) : seed;
        /* Warm up the generator */
        (void)ran1_next64();
        /* Convention: set idum positive after initialization */
        if (*idum < 0) {
            if (*idum == LONG_MIN) {
                *idum = LONG_MAX;
            } else {
                *idum = -(*idum);
            }
        }
    }

    /* Produce a random number in (0.0, 1.0) exclusive */
    uint64_t u = ran1_next64();
    /* Map [0, 2^53-1] -> [1, 2^53] -> (0, 1) exclusive of endpoints */
    return (real)(((u >> 11) + 1) * (1.0 / 9007199254740994.0));
}
