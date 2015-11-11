/*********************************************************************
 *                      BASIS.C
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *********************************************************************/

/*! \file
 *  \author Liam Deacon <liam.m.deacon@gmail.com>
 *
 * Implements methods for handling #basis objects.
 */

#include "basis.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static const double TOLERANCE = 0.01;

/*!
 * Initializes #basis object in memory.
 *
 * \return pointer to initialized #basis object.
 * \retval \c NULL if memory could not be allocated.
 */
basis *basis_init()
{
  basis *_basis = (basis*) malloc(sizeof(basis));
  if (_basis == NULL) return NULL;
  
  coord_set(&_basis->a[0], 0., 0., 0.);
  coord_set(&_basis->a[1], 0., 0., 0.);
  coord_set(&_basis->a[2], 0., 0., 0.);
  
  return(_basis);
}

/*!
 * Initalizes #basis object with basis vectors specified by \p a1 , \p a2
 * and \p a3 .
 *
 * \param[in] a1 pointer to #coord respresenting basis vector \f$ \vec{a_1} \f$
 * \param[in] a2 pointer to #coord respresenting basis vector \f$ \vec{a_2} \f$
 * \param[in] a3 pointer to #coord respresenting basis vector \f$ \vec{a_3} \f$
 * \return pointer to initialized #basis object.
 * \retval \c NULL if memory could not be allocated.
 */
basis *basis_init_vectors(const coord *a1, const coord *a2, const coord *a3)
{
  basis *_basis = (basis*) malloc(sizeof(basis));
  if (_basis == NULL) return NULL;

  basis_set_vectors(_basis, a1, a2, a3);
  
  return(_basis);
}

/*!
 * Frees #basis object \p _basis from memory. Assigns \p _basis to \c NULL on
 * completion.
 *
 * \param _basis object to be freed.
 */
void basis_free(basis *_basis)
{
  if (_basis != NULL) free(_basis);
  _basis = NULL;
}

/*!
 * Sets the basis vectors of \p basis
 *
 * \param[in,out] basis object whose basis vectors are to be set.
 * \param[in] a1 pointer to source #coord basis vector \f$ \vec{a_1} \f$
 * \param[in] a2 pointer to source #coord basis vector \f$ \vec{a_2} \f$
 * \param[in] a3 pointer to source #coord basis vector \f$ \vec{a_3} \f$
 */
void basis_set_vectors(basis *basis, const coord *a1, 
                       const coord *a2, const coord *a3)
{
  coord_set(&basis->a[0], a1->x, a1->y, a1->z);
  coord_set(&basis->a[1], a2->x, a2->y, a2->z);
  coord_set(&basis->a[2], a3->x, a3->y, a3->z);
}

/*!
 * Prints @basis::a values of the basis vectors in \p basis
 *
 * \param f file pointer for output e.g. \c stdout
 * \param[in] basis #basis object to read basis vector values from.
 * \note prints \c NULL if @basis::a is unallocated in memory.
 */
void basis_printf(FILE *f, const basis *basis)
{ 
  if (basis != NULL)
  {
    fprintf(f, "a1: ");
    coord_printf(f, &basis->a[0]);
    fprintf(f, "a2: ");
    coord_printf(f, &basis->a[1]);
    fprintf(f, "a3: ");
    coord_printf(f, &basis->a[2]);
  }
  else 
  {
    fprintf(f, "NULL\n");
  }
}

/*!
 * Copies \p src #basis values into \p dst
 *
 * \param dst destination #basis object to copy values into.
 * \param src source #basis object to copy values from.
 */
void basis_copy(basis *dst, const basis *src)
{
  if (dst == NULL) dst = basis_init();
  if (dst != NULL)
  {
    const basis *tmp = (const basis*) src;
    basis_set_vectors(dst, &tmp->a[0], &tmp->a[1], &tmp->a[2]);
  }
}

/*!
 * Returns the number of #basis objects allocated to the array \p _basis
 *
 * \param _basis array of #basis objects.
 * \return number of #basis objects allocated in memory.
 * \bug This may not return the correct value.
 */
size_t basis_get_allocated_size(const basis *_basis)
{
	return (sizeof(_basis)/sizeof(basis));
}

/*!
 * Updates \p a1 \p a2 and \p a3 to the vector values found in \p basis
 *
 * \param[in] basis Object containing basis vectors to copy from.
 * \param[out] a1 pointer to #coord respresenting basis vector \f$ \vec{a_1} \f$
 * \param[out] a2 pointer to #coord respresenting basis vector \f$ \vec{a_2} \f$
 * \param[out] a3 pointer to #coord respresenting basis vector \f$ \vec{a_3} \f$
 *
 * Internally this function makes calls to coord_copy() to set the basis
 * vector coordinates.
 *
 * \note The #coord objects \p a1 , \p a2 and \p a3 are assumed to be
 * initialized in memory before entering the function.
 */
void basis_get_vectors(const basis *basis, coord *a1, 
                       coord *a2, coord *a3)
{
  coord_copy(a1, &basis->a[0]);
  coord_copy(a2, &basis->a[1]);
  coord_copy(a3, &basis->a[2]);
}

/*!
 * Rotates the normal to the surface \p nor by the rotation matrix \p R
 *
 * It performs the following operation:
 *
 * \f$ \vec{N^\prime} = (R_{0,0} N_x + R_{0,1} N_y + R_{0,2} N_z,
 *                       R_{1,0} N_x + R_{1,1} N_y + R_{1,2} N_z,
 *                       R_{2,0} N_x + R_{2,1} N_y + R_{2,2} N_z) \f]
 *
 * \param nor The original vector normal to the surface.
 * \param R The rotation matrix \f$ \textbf{R} \f$ .
 * \return New rotated vector normal \f$ \vec{N^\prime \f$ .
 */
coord *basis_rotate_normal(const coord *nor, double **R)
{
  coord *new_normal = coord_init();

  coord_set(new_normal,
                R[0][0] * nor->x + R[0][1] * nor->y + R[0][2] * nor->z,
                R[1][0] * nor->x + R[1][1] * nor->y + R[1][2] * nor->z,
                R[2][0] * nor->x + R[2][1] * nor->y + R[2][2] * nor->z);
  return (new_normal);
}

/*!
 * Rotates the \p list of vectors using the rotation matrix \p R
 *
 * It performs the following operation:
 *
 * \f[ \vec{A^'_i} = \vec{A_{0,i}} \times \textbf{R} \mbox{for} i = 0...n \f]
 *
 * \param list An \p n dimensional array of original vectors \f$ \vec{A_0} \f$ .
 * \param n The number of elements \f$ n \f$ in the vector \p list
 * \param R The 3x3 rotation matrix \f$ \textbf{R} \f$
 * \return An array of rotated vectors \f$ \vec{A^'} \f$ .
 * \retval \c NULL if memory cannot be allocated for the rotated vector list.
 */
coord *basis_rotate_vector_list(const coord *list, size_t n, double **R)
{
  size_t i;
  double faux_x, faux_y, faux_z;
  coord *rotated_list;
  
  if (n > 0)
  {
    rotated_list = (coord*) malloc(sizeof(coord) * n);
  }
  else {
    return NULL;
  }
  
  if (!rotated_list) return NULL;
  
  for(i = 0; i < n; i++)
  {
    faux_x = list[i].x; 
    faux_y = list[i].y; 
    faux_z = list[i].z;

    rotated_list[i].x = R[0][0] * faux_x + R[0][1] * faux_y + R[0][2] * faux_z;
    rotated_list[i].y = R[1][0] * faux_x + R[1][1] * faux_y + R[1][2] * faux_z;
    rotated_list[i].z = R[2][0] * faux_x + R[2][1] * faux_y + R[2][2] * faux_z;
  }
  
  return (rotated_list);
}

/*!
 * Rotates \p _basis vectors by rotation matrix \p R
 *
 * \param[in] _basis Original basis vectors, \f$ \vec{a_1}, \vec{a_2},
 * \vec{a_3} \f$ .
 * \param[in] R Rotation matrix \f$ \textbf{R} \f$
 * \return Rotated basis vectors \f$ \vec{a^'_1}, \vec{a^'_2}, \vec{a^'_3} \f$ .
 */
basis *basis_rotate_basis(const basis *_basis, double **R)
{
  basis *new_basis;
  coord *faux = coord_init();

  new_basis = basis_init();
  
  coord_copy(faux, &_basis->a[0]);
  
  coord_set(&new_basis->a[0], 
                R[0][0] * faux->x + R[0][1] * faux->y + R[0][2] * faux->z,
                R[1][0] * faux->x + R[1][1] * faux->y + R[1][2] * faux->z,
                R[2][0] * faux->x + R[2][1] * faux->y + R[2][2] * faux->z);

  coord_copy(faux, &_basis->a[1]);
  
  coord_set(&new_basis->a[1], 
                R[0][0] * faux->x + R[0][1] * faux->y + R[0][2] * faux->z,
                R[1][0] * faux->x + R[1][1] * faux->y + R[1][2] * faux->z,
                R[2][0] * faux->x + R[2][1] * faux->y + R[2][2] * faux->z);

  coord_copy(faux, &_basis->a[2]);
  
  coord_set(&new_basis->a[2], 
                R[0][0] * faux->x + R[0][1] * faux->y + R[0][2] * faux->z,
                R[1][0] * faux->x + R[1][1] * faux->y + R[1][2] * faux->z,
                R[2][0] * faux->x + R[2][1] * faux->y + R[2][2] * faux->z);
  
  return (new_basis);
}

/*!
 * Rotates the #basis vectors \p a ...
 *
 * \param a Basis vectors \f$ a_1, a_2 \mbox{and} a_3 \f$ .
 * \param alpha
 * \param beta
 * \param gamma
 * \return Rotated basis vectors \f$ \vec{a^'_1}, \vec{a^'_2} \mbox{and}
 * \vec{a^'_3} \f$
 *
 * \todo Implement rotation
 */
basis *basis_angle_rotate(const basis *a, double alpha, 
                            double beta, double gamma)
{
  basis *new_basis = (basis*) calloc(1, sizeof(basis));
  if (new_basis == NULL) return NULL;

  /* rotate original basis vectors */

  return (new_basis);
}

/*!
 * Returns a 3x3 rotation matrix \f$ R \f$ for a given surface \p normal
 *
 * \param normal The plane perpendicular to the surface, \f$ N \f$ .
 * \return 3x3 rotation matrix.
 * \retval \c NULL if memory cannot be allocated.
 * \bug SEGFAULT upon malloc of memory for dynamic matrix allocation.
 */
double **normal_get_rotation_matrix(const coord *normal)
{
  double faux_x;
  double normal_len;
  double cos_t, cos_p, sin_t, sin_p;

  /* allocate memory for dynamic matrix */
  double **R = malloc(sizeof(double *) * 3);
  if (R == NULL) return NULL;

  R[0] = (double*) malloc(sizeof(double) * 3);
  R[1] = (double*) malloc(sizeof(double) * 3);
  R[2] = (double*) malloc(sizeof(double) * 3);

  if (R[0] == NULL || R[1] == NULL || R[2] == NULL)
  {
    free(R[0]);
    free(R[1]);
    free(R[2]);
    free(R);
    return NULL;
  }

  /* calculate rotation matrix */
  normal_len = coord_get_magnitude(normal);
  cos_t = normal->z / normal_len;
  sin_t = sqrt ( 1 - cos_t*cos_t);

  faux_x = sqrt(normal->x*normal->x + normal->y*normal->y);
 
  if (fabs(faux_x) > TOLERANCE)
  {
    cos_p = normal->x / faux_x;
    sin_p = normal->y / faux_x;
  }
  else
  {
    cos_p = 1.;
    sin_p = 0.;
  }

  R[0][0] = cos_t * cos_p;
  R[0][1] = cos_t * sin_p;
  R[0][2] = -sin_t;

  R[1][0] = -sin_p;
  R[1][1] =  cos_p;
  R[1][2] =  0.0;

  R[2][0] = sin_t * cos_p;
  R[2][1] = sin_t * sin_p;
  R[2][2] = cos_t;

  return (R);
}

/*!
 * Rotates \p _basis such that it becomes parallel to x-axis and uses
 * right-handed coordinates.
 *
 * \param[in] _basis Original basis vectors \f$ \vec{a_1}, \vec{a_2} \mbox{and}
 * \vec{a_3} \f$
 * \param[in] R 3x3 rotation matrix \f$ \textbf{R} \f$
 * \return The rotated #basis vectors \f$ \vec{a^'_1}, \vec{a^'_2}
 * \mbox{and} \vec{a^'_3} \f$
 * \retval \c NULL if memory cannot be allocated for a new #basis object.
 */
basis *basis_rotate_parallel_to_x_axis_rhs(const basis *_basis, double **R)
{
  double faux_x;
  basis *rot_basis = basis_init();
  if (rot_basis == NULL) return NULL;
  
  faux_x = sqrt(_basis->a[0].x*_basis->a[0].x + _basis->a[0].y*_basis->a[0].y);

  R[0][0] = _basis->a[0].x / faux_x;
  R[0][1] = _basis->a[0].y / faux_x;

  R[1][0] = -R[0][1];
  R[1][1] =  R[0][0];
  
  /* rotate lattice vectors b1, b2, b3 such that b1 || x-axis */
  rot_basis->a[0].x = R[0][0] * _basis->a[0].x + R[0][1] * _basis->a[0].y;
  rot_basis->a[0].y = R[1][0] * _basis->a[0].x + R[1][1] * _basis->a[0].y;

  rot_basis->a[1].x = R[0][0] * _basis->a[1].x + R[0][1] * _basis->a[1].y;
  rot_basis->a[1].y = R[1][0] * _basis->a[1].x + R[1][1] * _basis->a[1].y;

  rot_basis->a[2].x = R[0][0] * _basis->a[2].x + R[0][1] * _basis->a[2].y;
  rot_basis->a[2].y = R[1][0] * _basis->a[2].x + R[1][1] * _basis->a[2].y;
  
  return (rot_basis);
}

/*!
 * Returns the basis vector \f$ \vec{a_1} \f$ from \p _basis
 *
 * \param[in] _basis Object containing basis vectors.
 * \return @basis::a[0] of \p _basis
 */
coord *basis_get_a1(const basis *_basis)
{
  coord *a1 = (coord*) &_basis->a[0];
  return (a1);
}

/*!
 * Returns the basis vector \f$ \vec{a_2} \f$ from \p _basis
 *
 * \param[in] _basis Object containing basis vectors.
 * \return @basis::a[1] of \p _basis
 */
coord *basis_get_a2(const basis *_basis)
{
  coord *a2 = (coord*) &_basis->a[1];
  return (a2);
}

/*!
 * Returns the basis vector \f$ \vec{a_3} \f$ from \p _basis
 *
 * \param[in] _basis Object containing basis vectors.
 * \return @basis::a[2] of \p _basis
 */
coord *basis_get_a3(const basis *_basis)
{
  coord *a3 = (coord*) &_basis->a[2];
  return (a3);
}

/*!
 * Copies \f$ \vec{a_1} \f$ from \p a1 to \c a[0] member of \p _basis
 *
 * \param[out] _basis
 * \param[in] a1
 */
void basis_set_a1(basis *_basis, const coord *a1)
{
  coord_copy(&_basis->a[0], a1);
}

/*!
 * Copies \f$ \vec{a_2} \f$ from \p a2 to @basis::a[1] of \p _basis
 *
 * \param[out] _basis
 * \param[in] a2
 */
void basis_set_a2(basis *_basis, const coord *a2)
{
  coord_copy(&_basis->a[1], a2);
}

/*!
 * Copies \f$ \vec{a_3} \f$ from \p a3 to @basis::a[2] of \p _basis
 *
 * \param[out] _basis
 * \param[in] a3
 */
void basis_set_a3(basis *_basis, const coord *a3)
{
  coord_copy(&_basis->a[2], a3);
}

/*!
 * Calculates the surface normal \f$ \vec{N} \f$ to the \p _basis vectors
 * \f$ \vec{a_1}, \vec{a_2} \mbox \vec{a_3} \f$
 *
 * \param _basis The basis vectors
 * \param hkl The Miller surface plane (hkl)
 * \return The surface normal \f$ \vec{N} \f$
 */
coord *basis_get_normal(const basis *_basis, const miller_hkl *hkl)
{
  const coord *a1 = (const coord *) &_basis->a[0];
  const coord *a2 = (const coord *) &_basis->a[1];
  const coord *a3 = (const coord *) &_basis->a[2];
  coord *normal = coord_init();
  
  if (normal == NULL) return NULL;

  /* h, k, l not all 0 */
  if( (fabs(hkl->h) > TOLERANCE) || 
      (fabs(hkl->k) > TOLERANCE) || 
      (fabs(hkl->l) > TOLERANCE) )
  { 
    normal->x = (hkl->h*(a2->y*a3->z - a2->z*a3->y) + 
                 hkl->k*(a3->y*a1->z - a3->z*a1->y) + 
                 hkl->l*(a1->y*a2->z - a1->z*a2->y) ); 
    
    normal->y = (hkl->h*(a2->z*a3->x - a2->x*a3->z) + 
                 hkl->k*(a3->z*a1->x - a3->x*a1->z) + 
                 hkl->l*(a1->z*a2->x - a1->x*a2->z) );
    
    normal->z = (hkl->h*(a2->x*a3->y - a2->y*a3->x) + 
                 hkl->k*(a3->x*a1->y - a3->y*a1->x) + 
                 hkl->l*(a1->x*a2->y - a1->y*a2->x) ); 
  }
  
  return (normal);
}
