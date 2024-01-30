/*********************************************************************
 *                      cleed_permutation.h
 *
 *  Copyright 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 *
 * Provides an abstract factory design pattern for permutations (used in
 * linear algebra).
 */

#ifndef CLEED_PERMUTATION_H
#define CLEED_PERMUTATION_H

#include <stdlib.h>
#include <string.h>

#if USE_GSL == 1
#include <gsl/gsl_permutation.h>
#endif

#if !__GNUC__
#define __attribute__(x) /* empty */
#endif

#ifdef __cplusplus
namespace cleed {
#endif

#if ((USE_CBLAS) || (USE_LAPACK) || (USE_MKL ))

typedef int cleed_permutation;

#elif USE_GSL == 1

#define GSL_ONLY 1 /* GSL only */

typedef gsl_permuation cleed_permutation;

__attribute__((alloc_size(1)))
static inline cleed_permutation *cleed_permutation_alloc(size_t n)
{
  return gsl_permutation_alloc(n);
}

__attribute__((nonnull))
static inline void cleed_permutation_free(cleed_permutation *p)
{
  gsl_permutation_free(p); p = NULL;
}

__attribute__((nonnull(2)))
static inline void cleed_permutation_fprintf(FILE *f, const cleed_permutation *p, size_t n)
{
  gsl_permutation_fprintf(f, p, " %d"); n = n*1;
}

#else

typedef size_t cleed_permutation;

#endif

#if (! GSL_ONLY)

__attribute__((alloc_size (1)))
static inline cleed_permutation *cleed_permutation_alloc(size_t n)
{
  return (cleed_permutation *)calloc( (n+1), sizeof(cleed_permutation));
}

__attribute__((nonnull))
static inline void cleed_permutation_free(cleed_permutation *p)
{
  if (p != NULL) { free(p); } p = NULL;
}

__attribute__((nonnull(2)))
static inline void cleed_permutation_fprintf(FILE *f, const cleed_permutation *p, size_t n)
{
  size_t i; for (i=0; i<n; i++) fprintf(f, " %d", p[i]);
}

#endif

#ifdef GSL_ONLY
# undef GSL_ONLY
#endif

#ifdef __cplusplus
} /* namespace CLEED */
#endif /* __cplusplus */

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* CLEED_PERMUTATION_H */
