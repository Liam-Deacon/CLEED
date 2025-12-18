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

// cppcheck-suppress missingIncludeSystem
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sr_rng.h
 * @brief Deterministic RNG used by SEARCH routines.
 *
 * This RNG exists to:
 * - avoid `rand()`/`drand48()` variability across platforms/toolchains
 * - make optimisation behaviour reproducible for regression testing
 * - keep SEARCH free of external RNG dependencies
 */

/**
 * @brief RNG state container.
 *
 * The generator is intentionally small; callers should treat the
 * representation as opaque and use @ref sr_rng_seed and
 * @ref sr_rng_uniform01.
 */
typedef struct sr_rng {
  uint64_t state;
} sr_rng;

/**
 * @brief Return the current raw RNG state.
 *
 * Primarily intended for tests and diagnostics.
 *
 * @param rng RNG instance.
 * @return Current internal state.
 */
static inline uint64_t sr_rng_state(const sr_rng *rng)
{
  return rng->state;
}

/**
 * @brief Seed the RNG.
 *
 * A seed of 0 is mapped to a non-zero value to avoid getting stuck in
 * a degenerate state.
 *
 * @param rng RNG instance to seed.
 * @param seed Seed value (0 is allowed).
 */
void sr_rng_seed(sr_rng *rng, uint64_t seed);

/**
 * @brief Generate a uniform deviate in the range [0, 1).
 *
 * @param rng RNG instance.
 * @return Uniform random value in [0, 1).
 */
double sr_rng_uniform01(sr_rng *rng);

#ifdef __cplusplus
}
#endif

#endif /* SR_RNG_H */
