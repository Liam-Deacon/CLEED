/************************************************************************
 *                            BASIS.H
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Change Log:
 *   LD/2014.07.30 - Creation
 ************************************************************************/
 
/*!
 * \file
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 * \brief Provides basis vectors data type and associated functions.
 */

#ifndef BASIS_H
#define BASIS_H

#include "coord.h"
#include "miller_index.h"

#if !__GNUC__
#define __attribute__(x) /* empty */
#define auto_basis
#else
#define auto_basis __attribute__((cleanup(basis_free)))
#endif

 /*!
 * Returns the number of #basis objects allocated to the array \p basis_array
 *
 * \param basis_array of #basis objects.
 * \return number of #basis objects allocated in memory.
 * \bug This will not return the correct value if \p basis_array is a pointer.
 */
#define basis_get_allocated_size(basis_array) (sizeof((basis_array))/sizeof(basis))

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*!
 *  \brief Structure for the three basis vectors \f$ \vec{a}_1 \f$ ,
 *  \f$ \vec{a}_2 \f$ , & \f$ \vec{a}_3 \f$ .
 */
typedef struct {
  coord a[3];       /*!< Basis vectors \f$ \vec{a}_1 \f$, \f$ \vec{a}_2 \f$,
                     * & \f$ \vec{a}_3 \f$.
                     * \note uses C indexing (first element is \c a[0] ) */
} basis; 

/* rotation functions */
__attribute__((nonnull, returns_nonnull))
    basis *basis_matrix_rotate(basis *a, double **R);
__attribute__((nonnull, returns_nonnull))
    basis *basis_ptr_rotate(basis *a, double *R[]);
__attribute__((nonnull, returns_nonnull))
    basis *basis_angle_rotate(const basis *a, double alpha, double beta, double gamma);
__attribute__((nonnull, returns_nonnull))
    basis *basis_rotate_basis(const basis *basis, double **R);
__attribute__((nonnull, returns_nonnull))
    coord *basis_rotate_normal(const coord *nor, double **R);
__attribute__((nonnull, returns_nonnull))
    coord *basis_rotate_vector_list(const coord *list, size_t n, double **R);
__attribute__((nonnull, returns_nonnull))
    basis *basis_rotate_parallel_to_x_axis_rhs(const basis *basis, double **R);

__attribute__((nonnull, returns_nonnull))
    double **normal_get_rotation_matrix(const coord *normal);

/* basic basis functions */
__attribute__((returns_nonnull)) basis *basis_init(void);
__attribute__((nonnull)) void basis_free(basis *basis);
__attribute__((nonnull)) void basis_copy(basis *dst, const basis *src);
__attribute__((nonnull, returns_nonnull))
    basis *basis_init_vectors(const coord *a1, const coord *a2, const coord *a3);

__attribute__((nonnull))
  void basis_get_vectors(const basis *basis, coord *a1, coord *a2, coord *a3);
__attribute__((nonnull, returns_nonnull)) coord *basis_get_a1(const basis *basis);
__attribute__((nonnull, returns_nonnull)) coord *basis_get_a2(const basis *basis);
__attribute__((nonnull, returns_nonnull)) coord *basis_get_a3(const basis *basis);
__attribute__((nonnull, returns_nonnull))
    coord *basis_get_normal(const basis *basis, const miller_hkl *hkl);

__attribute__((nonnull)) void basis_set_a1(basis *basis, const coord *a1);
__attribute__((nonnull)) void basis_set_a2(basis *basis, const coord *a2);
__attribute__((nonnull)) void basis_set_a3(basis *basis, const coord *a3);
__attribute__((nonnull)) void basis_set_vectors(basis *basis,
    const coord *a1, const coord *a2, const coord *a3);

__attribute__((nonnull(2))) void basis_printf(FILE *f, const basis *basis);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
#endif /* ifdef __cplusplus */

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* BASIS_H */
