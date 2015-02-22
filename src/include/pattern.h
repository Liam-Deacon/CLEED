/*********************************************************************
 *                           PATTERN.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.07.20 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 * \brief Header file for #pattern \c struct and associated functions.
 */

#ifndef PATTERN_H
#define PATTERN_H

#include "basis_vector.h"
#include "matrix_2x2.h"
#include "spots.h"
#include "gh_stddef.h"
#include <stdbool.h>

#if (!__GNUC__)
# define __attribute__(x) /* empty to disable GCC attribute extension */
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
#include <list>

namespace cleed {
extern "C" {
#endif

/*! \typedef pattern
 *  \brief Structure containing information needed for LEED pattern generation.
 */
typedef struct _pattern
{
  char *title;              /*!< pointer to title string */
  basis_vector a1;          /*!< a1 basis vector */
  basis_vector a2;          /*!< a2 basis vector */
  matrix_2x2 *M_SS;         /*!< array of superstructure matrices */
  size_t n_domains;         /*!< number of superstructures */
  double radius;            /*!< maximum radius for valid spots */
  bool square;              /*!< switch for circular or square valid area */
} pattern;

extern pattern pattern_default; 

/* pattern functions */
__attribute__((returns_nonnull, alloc_size(1)))
pattern *pattern_alloc(size_t n_domains);

__attribute__((returns_nonnull, alloc_size(1)))
pattern *pattern_init(size_t n_domains);

__attribute__((returns_nonnull, nonnull))
pattern *pattern_read(FILE *file);

__attribute__((nonnull))
void pattern_free(pattern *pat);

__attribute__((nonnull))
double pattern_get_radius(const pattern *pat);

__attribute__((nonnull))
bool pattern_is_square(const pattern *pat);

__attribute__((nonnull))
bool pattern_domain_is_commensurate(const pattern *pat, size_t domain);

__attribute__((nonnull))
size_t pattern_get_n_domains(const pattern *pat);

__attribute__((nonnull, returns_nonnull))
const char *pattern_get_title(const pattern *pat);

__attribute__((nonnull(2)))
void pattern_printf(FILE *stream, const pattern *pat);

__attribute__((nonnull))
void pattern_set_a1(pattern *pat, const basis_vector *a1);

__attribute__((nonnull))
void pattern_set_a2(pattern *pat, const basis_vector *a2);

__attribute__((nonnull))
void pattern_set_radius(pattern *pat, double radius);

__attribute__((nonnull(1)))
void pattern_set_title(pattern *pat, const  char *title);

__attribute__((nonnull))
void pattern_set_square(pattern *pat, bool is_square);

__attribute__((nonnull))
int pattern_set_max_domains(pattern *pat, size_t n_domains);

__attribute__((nonnull))
void pattern_set_superstructure_matrix(pattern *pat, const matrix_2x2 *mat,
                                       size_t domain);
        
__attribute__((nonnull, returns_nonnull))
const matrix_2x2 *get_superstructure_matrix(const pattern *pat, size_t domain);

__attribute__((nonnull, returns_nonnull))
spots *pattern_calculate_substrate_spots(const pattern *pat);

__attribute__((nonnull, returns_nonnull))
spots *pattern_calculate_superstructure_spots(const pattern *pat,
                                              size_t domain);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
} /* namespace cleed */
#endif /* __cplusplus */

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* PATTERN_H */
