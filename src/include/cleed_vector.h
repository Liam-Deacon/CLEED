/*********************************************************************
 *													cleed_vector.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/29 Jan 2015 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * This file defines the standard vector data types for vectors in CLEED,
 * providing a unified interface through an abstract factory design pattern.
 */

#ifndef CLEED_VECTOR_H
#define CLEED_VECTOR_H

#include <stdlib.h>
#include "cleed_real.h"
#include "cleed_complex.h"

#if !__GNUC__
#define __attribute__(x) /* empty */
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif


#if USE_GSL == 1

#include <gsl/gsl_vector.h>

# if CLEED_REAL_IS_FLOAT

typedef gsl_vector_float cleed_vector;
typedef gsl_vector_complex_float cleed_vector_complex;

#define CLEED_VECTOR_GET(v, i)            gsl_vector_float_get((v),(i))
#define CLEED_VECTOR_SET(v, i, x)         gsl_vector_float_set((v),(i),(x))
#define CLEED_VECTOR_COMPLEX_GET(v, i)    &gsl_vector_complex_float_get(v, i).dat
#define CLEED_VECTOR_COMPLEX_SET(v, i, x)                                     \
  do { gsl_complex_float cmplx = {.dat[0] = (x.dat[0]), .dat[1] = (x.dat[0])};\
       gsl_vector_complex_set((v),(i), cmplx);                                \
  } while (0)

#define CLEED_VECTOR_ALLOC(i)               gsl_vector_float_calloc((i))
#define CLEED_VECTOR_FREE(v)                gsl_vector_float_free((v))
#define CLEED_VECTOR_COMPLEX_ALLOC(i)       gsl_vector_complex_float_calloc((i))
#define CLEED_VECTOR_COMPLEX_FREE(v)        gsl_vector_complex_float_free((v))

# else /* assume double precision floating point */

typedef gsl_vector cleed_vector;
typedef gsl_vector_complex cleed_vector_complex;

#define CLEED_VECTOR_GET(v, i)                gsl_vector_get((v),(i))
#define CLEED_VECTOR_SET(v, i, x)             gsl_vector_set((v),(i),(x))
#define CLEED_VECTOR_COMPLEX_GET(v, i)        gsl_vector_complex_get((v),(i))
#define CLEED_VECTOR_COMPLEX_SET(v, i, x)     gsl_vector_complex_set((v),(i),(x))

#define CLEED_VECTOR_ALLOC(i)                 gsl_vector_calloc((i))
#define CLEED_VECTOR_FREE(v)                  gsl_vector_free((v))
#define CLEED_VECTOR_COMPLEX_ALLOC(i)         gsl_vector_complex_calloc((i))
#define CLEED_VECTOR_COMPLEX_FREE(v)          gsl_vector_complex_free((v)

# endif /* CLEED_REAL_IS_FLOAT */

# if CLEED_REAL_IS_FLOAT

#   if __STDC_VERSION__ >= 199901L

/*!
 * Allocates memory for vector.
 *
 * \param n The number of vector elements to allocate.
 * \return Pointer to vector allocated memory.
 */
__attribute__((returns_nonnull, alloc_size(1)))
static inline cleed_vector *cleed_vector_alloc(size_t n)
{
  return gsl_vector_float_calloc(n);
}

/*!
 * Free memory held by vector.
 *
 * \param[in,out] v Pointer to vector to be freed.
 */
__attribute__((nonnull))
static inline void cleed_vector_free(cleed_vector *v)
{
  gsl_vector_float_free(n);
}

/*!
 * Gets the value held in the vector element given by \p index .
 *
 * \param v Pointer to vector.
 * \param index Element to retrieve data from.
 * \return Value held in \p index of \p v .
 */
__attribute__((nonnull))
static inline
cleed_real cleed_vector_get(const cleed_vector *v,
                            size_t index)
{
  gsl_vector_float_set(v, index)
}

/*!
 * Sets the \p value held in the \p index of \p v array.
 *
 * \param[in,out] v Pointer to vector to modify.
 * \param index Element to retrieve data from.
 * \param value Value to set.
 */
__attribute__((nonnull))
static inline void
cleed_vector_set(cleed_vector *v,
                 size_t index,
                 cleed_real value)
{
  v[index] = value;
}

/*!
 * Allocates memory for complex vector.
 *
 * \param n The number of vector elements to allocate.
 * \return Pointer to vector allocated memory.
 */
__attribute__((nonnull))
static inline cleed_vector_complex *
cleed_vector_complex_alloc(size_t n)
{
  return (cleed_vector_complex *)calloc(n, sizeof(cleed_vector_complex));
}

/*!
 * Free memory held by complex vector \p v .
 *
 * \param[in,out] v Pointer to complex vector to be freed.
 */
__attribute__((nonnull))
static inline void cleed_vector_complex_free(cleed_vector_complex *v)
{
  if (v != NULL) { free(v); }
  v = NULL;
}

/*!
 * Gets the value held in the complex vector element given by \p index .
 *
 * \param v Pointer to vector.
 * \param index Element to retrieve data from.
 * \return Pointer to complex datum.
 */
__attribute__((nonnull))
static inline cleed_complex *
cleed_vector_complex_get(const cleed_vector_complex *v,
                         size_t index)
{
  return (cleed_complex *)(v + 2*index);
}

/*!
 * Sets the \p value held in the \p index of \p v array.
 *
 * \param[in,out] v Pointer to complex vector to modify.
 * \param index Element to retrieve data from.
 * \param value Value to set.
 */
__attribute__((nonnull))
static inline void
cleed_vector_complex_set(cleed_vector_complex *v,
                         size_t index,
                         cleed_complex value)
{
  v[2*index] = value[0];
  v[2*index + 1] = value[1];
}

#   endif /* __STDC_VERSION__ */

# else /* CLEED_REAL_IS_DOUBLE */

#   if __STDC_VERSION__ >= 199901L

/*!
 * Allocates memory for vector.
 *
 * \param n The number of vector elements to allocate.
 * \return Pointer to vector allocated memory.
 */
__attribute__((returns_nonnull, alloc_size(1)))
static inline cleed_vector *cleed_vector_alloc(size_t n)
{
  return gsl_vector_calloc(n);
}

/*!
 * Free memory held by vector.
 *
 * \param[in,out] v Pointer to vector to be freed.
 */
__attribute__((nonnull))
static inline void cleed_vector_free(cleed_vector *v)
{
  gsl_vector_free(v);
}

/*!
 * Gets the value held in the vector element given by \p index .
 *
 * \param v Pointer to vector.
 * \param index Element to retrieve data from.
 * \return Value held in \p index of \p v .
 */
__attribute__((nonnull))
static inline cleed_real
cleed_vector_get(const cleed_vector *v, size_t index)
{
  return (gsl_vector_get(v, index));
}

/*!
 * Sets the \p value held in the \p index of \p v array.
 *
 * \param[in,out] v Pointer to vector to modify.
 * \param index Element to retrieve data from.
 * \param value Value to set.
 */
__attribute__((nonnull))
static inline void
cleed_vector_set(cleed_vector *v, size_t index, cleed_real value)
{
  gsl_vector_set(v, index, value);
}

/* now the same for complex vectors */

/*!
 * Allocates memory for vector.
 *
 * \param n The number of vector elements to allocate.
 * \return Pointer to vector allocated memory.
 */
__attribute__((returns_nonnull, alloc_size(1)))
static inline cleed_vector_complex *cleed_vector_complex_alloc(size_t n)
{
  return (cleed_vector_complex *)calloc(n, sizeof(cleed_vector_complex));
}

/*!
 * Free memory held by complex vector \p v .
 *
 * \param[in,out] v Pointer to complex vector to be freed.
 */
__attribute__((nonnull))
static inline void
cleed_vector_complex_free(cleed_vector_complex *v)
{
  if (v != NULL) { free(v); }
  v = NULL;
}

/*!
 * Gets the value held in the complex vector element given by \p index .
 *
 * \param v Pointer to vector.
 * \param index Element to retrieve data from.
 * \return Pointer to complex datum.
 */
__attribute__((nonnull))
static inline cleed_complex *
cleed_vector_complex_get(const cleed_vector_complex *v, size_t index)
{
  return (cleed_complex*)(v + 2*index);
}

/*!
 * Sets the \p complex value held in the \p index of \p v array.
 *
 * \param[in,out] v Pointer to complex vector to modify.
 * \param index Element to retrieve data from.
 * \param value Value to set.
 */
__attribute__((nonnull))
static inline void
cleed_vector_complex_set(cleed_vector_complex *v,
                         size_t index,
                         cleed_complex value)
{
  gsl_vector_complex_set(v, index, value);
}

#   endif /* __STDC_VERSION__ */
# endif /* CLEED_REAL_IS_FLOAT */

#else

typedef cleed_real cleed_vector;
typedef cleed_real cleed_vector_complex;

# define CLEED_VECTOR_ALLOC(n)         ( (cleed_vector*)calloc((n), sizeof(cleed_vector)) )
# define CLEED_VECTOR_FREE(v)          (free((v)))
# define CLEED_VECTOR_GET(v, i)        ( (v)[(i)] )
# define CLEED_VECTOR_SET(v, i, x)     do { (v)[(i)] = (x); } while(0)

# define CLEED_VECTOR_COMPLEX_ALLOC(n)         ( (cleed_vector*)calloc((2*n), sizeof(cleed_vector)) )
# define CLEED_VECTOR_COMPLEX_FREE(v)          (free((v)))
# define CLEED_VECTOR_COMPLEX_GET(v, i)        ( (v)[(i)] )
# define CLEED_VECTOR_COMPLEX_SET(v, i, x)     do{ (v)[2*(i)] = (x)[0]; (v)[2*(i) + 1] = (x)[0]; }while(0)


# if __STDC_VERSION__ >= 199901L

/*!
 * Allocates memory for vector.
 *
 * \param n The number of vector elements to allocate.
 * \return Pointer to vector allocated memory.
 */
__attribute__((returns_nonnull, alloc_size(1)))
static inline cleed_vector *cleed_vector_alloc(size_t n)
{
  return (cleed_vector *)calloc(n, sizeof(cleed_vector));
}

/*!
 * Free memory held by vector.
 *
 * \param[in,out] v Pointer to vector to be freed.
 */
static inline void cleed_vector_free(cleed_vector *v)
{
  if (v != NULL) { free(v); } v = NULL;
}

/*!
 * Gets the value held in the vector element given by \p index .
 *
 * \param v Pointer to vector.
 * \param index Element to retrieve data from.
 */
__attribute__((nonnull))
static inline cleed_real
cleed_vector_get(const cleed_vector *v, size_t index)
{
  return v[index];
}

/*!
 * Sets the \p value held in the \p index of \p v array.
 *
 * \param[in,out] v Pointer to vector to modify.
 * \param index Element to retrieve data from.
 * \param value Value to set.
 */
__attribute__((nonnull))
static inline void
cleed_vector_set(cleed_vector *v, size_t index, cleed_real value)
{
  v[index] = value;
}

/* now the same for complex vectors */

/*!
 * Allocates memory for a complex vector.
 *
 * \param n The number of vector elements to allocate.
 * \return Pointer to vector allocated memory.
 */
__attribute__((returns_nonnull, alloc_size(1)))
static inline cleed_vector_complex *cleed_vector_complex_alloc(size_t n)
{
  return (cleed_vector_complex *)calloc(n, sizeof(cleed_vector_complex));
}

/*!
 * Free memory held by complex vector \p v .
 *
 * \param[in,out] v Pointer to vector to be freed.
 */
__attribute__((nonnull))
static inline void
cleed_vector_complex_free(cleed_vector_complex *v)
{
  if (v != NULL) { free(v); } v = NULL;
}

/*!
 * Gets the value held in the complex vector element given by \p index .
 *
 * \param v Pointer to vector.
 * \param index Element to retrieve data from.
 * \return Pointer to complex datum.
 */
__attribute__((nonnull))
static inline cleed_complex *
cleed_vector_complex_get(const cleed_vector_complex *v, size_t index)
{
  return (cleed_complex*)(v + 2*index);
}

/*!
 * Sets the \p value held in the \p index of \p v array.
 *
 * \param[in,out] v Pointer to complex vector to modify.
 * \param index Element to retrieve data from.
 * \param value Value to set.
 */
__attribute__((nonnull))
static inline void
cleed_vector_complex_set(cleed_vector_complex *v,
                         size_t index,
                         cleed_complex value)
{
  v[2*index] = value[0];
  v[2*index + 1] = value[1];
}

# endif

/* no inline functions if using ANSI C (C89) */
# if __STDC_VERSION__ < 199901L

# define cleed_vector_alloc       CLEED_VECTOR_ALLOC
# define cleed_vector_free        CLEED_VECTOR_FREE
# define cleed_vector_get         CLEED_VECTOR_GET
# define cleed_vector_set         CLEED_VECTOR_SET

# define cleed_vector_complex_alloc  CLEED_VECTOR_COMPLEX_ALLOC
# define cleed_vector_complex_free   CLEED_VECTOR_COMPLEX_FREE
# define cleed_vector_complex_get    CLEED_VECTOR_COMPLEX_GET
# define cleed_vector_complex_set    CLEED_VECTOR_COMPLEX_SET

#endif

#endif /* USE_NATIVE */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif

#ifdef __attribute__
# undef __attribute__ /* contain __attribute__ macro usage within this header */
#endif

#endif /* CLEED_VECTOR_H */
