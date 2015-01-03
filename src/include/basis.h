/************************************************************************
 *                            BASIS.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 * \brief Provides basis vectors data type and associated functions.
 */

#ifndef BASIS_H
#define BASIS_H

#include "coord.h"
#include "miller_index.h"

#define BASIS_VECTOR_PTR(_basis, i)  ((coord*)_basis->a+(i*sizeof(_basis)))

#define CALC_ROTATION_MATRIX(normal, R)                                       \
{                                                                             \
  if (fabs(sqrt(normal->x*normal->x + normal->y*normal->y)) > TOLERANCE)      \
  {                                                                           \
    R[0][0] = (normal->z / coord_get_magnitude(normal)) *                     \
              (normal->x / sqrt(normal->x*normal->x + normal->y*normal->y));  \
    R[0][1] = (normal->z / coord_get_magnitude(normal)) *                     \
              (normal->y / sqrt(normal->x*normal->x + normal->y*normal->y));  \
    R[0][2] = -sqrt ( 1 - (normal->z /                                        \
                coord_get_magnitude(normal) * normal->z /                     \
                coord_get_magnitude(normal)));                                \
                                                                              \
    R[1][0] = -sqrt ( 1 - (normal->z /                                        \
                        coord_get_magnitude(normal) * normal->z /             \
                        coord_get_magnitude(normal)));                        \
    R[1][1] =  normal->x / sqrt(normal->x*normal->x + normal->y*normal->y);   \
    R[1][2] =  0.0;                                                           \
                                                                              \
    R[2][0] = sqrt ( 1 - (normal->z /                                         \
                coord_get_magnitude(normal) * normal->z /                     \
                coord_get_magnitude(normal))) *                               \
                (normal->x / sqrt(normal->x*normal->x + normal->y*normal->y));\
    R[2][1] = sqrt ( 1 - (normal->z /                                         \
                coord_get_magnitude(normal) * normal->z /                     \
                coord_get_magnitude(normal))) *                               \
                (normal->y / sqrt(normal->x*normal->x + normal->y*normal->y));\
    R[2][2] = normal->z / coord_get_magnitude(normal);                        \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    R[0][0] = normal->z / coord_get_magnitude(normal);                        \
    R[0][1] = 0.;                                                             \
    R[0][2] = -sqrt ( 1 - (normal->z /                                        \
                coord_get_magnitude(normal) * normal->z /                     \
                coord_get_magnitude(normal)));                                \
                                                                              \
    R[1][0] =  0.;                                                            \
    R[1][1] =  1.;                                                            \
    R[1][2] =  0.;                                                            \
                                                                              \
    R[2][0] = sqrt(1 - (normal->z / coord_get_magnitude(normal) * normal->z / \
                    coord_get_magnitude(normal)));                            \
    R[2][1] = 0.;                                                             \
    R[2][2] = normal->z / coord_get_magnitude(normal);                        \
  }                                                                           \
}

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

basis *basis_matrix_rotate(basis *a, double **R);
basis *basis_ptr_rotate(basis *a, double *R[]);
basis *basis_angle_rotate(const basis *a, double alpha, double beta,
                          double gamma);

basis *basis_rotate_basis(const basis *basis, double **R);
coord *basis_rotate_normal(const coord *nor, double **R);
coord *basis_rotate_vector_list(const coord *list, size_t n, double **R);
basis *basis_rotate_parallel_to_x_axis_rhs(const basis *basis, double **R);
basis *basis_init();
void basis_free(basis *basis);
void basis_copy(basis *dst, const basis *src);
basis *basis_init_vectors(const coord *a1, const coord *a2, const coord *a3);

coord *basis_get_a1(const basis *basis);
coord *basis_get_a2(const basis *basis);
coord *basis_get_a3(const basis *basis);
void basis_get_vectors(const basis *basis, coord *a1, coord *a2, coord *a3);
coord *basis_get_normal(const basis *basis, const miller_hkl *hkl);
size_t basis_get_allocated_size(const basis *basis);

void basis_set_a1(basis *basis, const coord *a1);
void basis_set_a2(basis *basis, const coord *a2);
void basis_set_a3(basis *basis, const coord *a3);
void basis_set_vectors(basis *basis, const coord *a1, const coord *a2, const coord *a3);

void basis_printf(FILE *f, const basis *basis);
double **normal_get_rotation_matrix(const coord *normal);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

/*!
 * \brief C++ class for #basis and its associated functions.
 */
class Basis {
  public:
    Basis();
    Basis(const basis *a);
    ~Basis();
    
    /* setters */
    void setA1(const Coordinate &a1);
    void setA1(double a1_x, double a1_y, double a1_z);
    void setA2(const Coordinate &a3);
    void setA2(double a2_x, double a2_y, double a2_z);
    void setA3(const Coordinate &a3);
    void setA3(double a3_x, double a3_y, double a3_z);
    void setBasis(const Coordinate &a1, const Coordinate &a2, const Coordinate &a3);
    void setBasis(const Basis &basis);
    void setBasis(const Basis *basis);
    
    /* getters */
    const Coordinate getA1();
    const Coordinate getA2();
    const Coordinate getA3();
    
    /* other methods */
    double **calculateRotationMatrix();
    void rotate(double **R);
    void rotate(double alpha, double beta, double gamma);
    
  private:
    basis *basis_ptr;
    
    friend class Coordinate;
};


} /* namespace cleed */ 

#endif /* ifdef __cplusplus */

#endif /* BASIS_H */
