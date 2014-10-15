#include "basis.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#ifndef TOLERANCE
#define TOLERANCE 1E-2
#endif

basis *basis_init()
{
  basis *_basis = (basis*) malloc(sizeof(basis));
  
  coord_set(&_basis->a[0], 0., 0., 0.);
  coord_set(&_basis->a[1], 0., 0., 0.);
  coord_set(&_basis->a[2], 0., 0., 0.);
  
  return(_basis);
}

basis *basis_init_vectors(const coord *a1, 
                            const coord *a2, const coord *a3)
{
  basis *_basis = (basis*) malloc(sizeof(basis));
 
  basis_set_vectors(_basis, a1, a2, a3);
  
  return(_basis);
}

void basis_free(basis *_basis)
{
  if (_basis != NULL)
    free(_basis);
  _basis = NULL;
}

void basis_set_vectors(basis *basis, const coord *a1, 
                       const coord *a2, const coord *a3)
{
  coord_set(&basis->a[0], a1->x, a1->y, a1->z);
  coord_set(&basis->a[1], a2->x, a2->y, a2->z);
  coord_set(&basis->a[2], a3->x, a3->y, a3->z);
}

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

void basis_copy(basis *dst, const basis *src)
{
  const basis *tmp =  (const basis*) src;
  basis_set_vectors(dst, &tmp->a[0], &tmp->a[1], &tmp->a[2]);
}

size_t basis_get_allocated_size(const basis *basis)
{
	return (sizeof(basis)/sizeof(basis));
}

void basis_get_vectors(const basis *basis, coord *a1, 
                       coord *a2, coord *a3)
{
  coord_copy(a1, &basis->a[0]);
  coord_copy(a2, &basis->a[1]);
  coord_copy(a3, &basis->a[2]);
}

coord *basis_rotate_normal(const coord *nor, double **R)
{
  coord *new_normal = coord_init();

  coord_set(new_normal,
                R[0][0] * nor->x + R[0][1] * nor->y + R[0][2] * nor->z,
                R[1][0] * nor->x + R[1][1] * nor->y + R[1][2] * nor->z,
                R[2][0] * nor->x + R[2][1] * nor->y + R[2][2] * nor->z);
  return (new_normal);
}

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

basis *basis_angle_rotate(const basis *a, double alpha, 
                            double beta, double gamma)
{
  basis *new_basis = NULL;

  return (new_basis);
}

double **normal_get_rotation_matrix(const coord *normal)
{
 double faux_x;
 double normal_len;
 double cos_t, cos_p, sin_t, sin_p;
 double **R = malloc(sizeof(double *) * 3);
 
 if (R == NULL) return NULL;

 R[0] = (double*) malloc(sizeof(double) * 3);
 R[1] = (double*) malloc(sizeof(double) * 3);
 R[2] = (double*) malloc(sizeof(double) * 3);

 normal_len = COORD_MAGNITUDE(normal);
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

basis *basis_rotate_parallel_to_x_axis_rhs(const basis *_basis, double **R)
{
  double faux_x;
  basis *rot_basis = basis_init();
  
  faux_x = sqrt(_basis->a[0].x*_basis->a[0].x + _basis->a[0].y*_basis->a[0].y);

  R[0][0] = _basis->a[0].x / faux_x;
  R[0][1] = _basis->a[0].y / faux_x;

  R[1][0] = -R[0][1];
  R[1][1] = R[0][0];
  
  /* rotate lattice vectors b1, b2, b3 such that b1 || x-axis */
  rot_basis->a[0].x = R[0][0] * _basis->a[0].x + R[0][1] * _basis->a[0].y;
  rot_basis->a[0].y = R[1][0] * _basis->a[0].x + R[1][1] * _basis->a[0].y;

  rot_basis->a[1].x = R[0][0] * _basis->a[1].x + R[0][1] * _basis->a[1].y;
  rot_basis->a[1].y = R[1][0] * _basis->a[1].x + R[1][1] * _basis->a[1].y;

  rot_basis->a[2].x = R[0][0] * _basis->a[2].x + R[0][1] * _basis->a[2].y;
  rot_basis->a[2].y = R[1][0] * _basis->a[2].x + R[1][1] * _basis->a[2].y;
  
  return (rot_basis);
}

coord *basis_get_a1(const basis *basis)
{
  coord *a1 = (coord*) &basis->a[0];
  return (a1);
}

coord *basis_get_a2(const basis *basis)
{
  coord *a2 = (coord*) &basis->a[1];
  return (a2);
}

coord *basis_get_a3(const basis *basis)
{
  coord *a3 = (coord*) &basis->a[2];
  return (a3);
}

void basis_set_a1(basis *basis, const coord *a1)
{
  coord_copy(&basis->a[0], a1);
}

void basis_set_a2(basis *basis, const coord *a2)
{
  coord_copy(&basis->a[1], a2);
}

void basis_set_a3(basis *basis, const coord *a3)
{
  coord_copy(&basis->a[2], a3);
}

coord *basis_get_normal(const basis *basis, const miller_hkl *hkl)
{
  const coord *a1 = BASIS_VECTOR_PTR(basis, 0);
  const coord *a2 = BASIS_VECTOR_PTR(basis, 1);
  const coord *a3 = BASIS_VECTOR_PTR(basis, 2);
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
