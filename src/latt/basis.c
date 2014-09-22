#include "basis.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#ifndef TOLERANCE
#define TOLERANCE 1E-2
#endif

basis_t *basis_init()
{
  basis_t *basis = (basis_t*) malloc(sizeof(basis_t));
  
  coord_set(&basis->a[0], 0., 0., 0.);
  coord_set(&basis->a[1], 0., 0., 0.);
  coord_set(&basis->a[2], 0., 0., 0.);
  
  return (basis);
}

basis_t *basis_init_vectors(const coord_t *a1, 
                            const coord_t *a2, const coord_t *a3)
{
  basis_t *basis = (basis_t*) malloc(sizeof(basis_t));
 
  basis_set_vectors(basis, a1, a2, a3);
  
  return (basis);
}

void basis_free(basis_t *basis)
{
  if (basis != NULL)
    free(basis);
  basis = NULL;
}

void basis_set_vectors(basis_t *basis, const coord_t *a1, 
                       const coord_t *a2, const coord_t *a3)
{
  coord_set(&basis->a[0], a1->x, a1->y, a1->z);
  coord_set(&basis->a[1], a2->x, a2->y, a2->z);
  coord_set(&basis->a[2], a3->x, a3->y, a3->z);
}

void basis_printf(FILE *f, const basis_t *basis)
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

void basis_copy(basis_t *dst, const basis_t *src)
{
  const basis_t *tmp =  (const basis_t*) src;
  basis_set_vectors(dst, &tmp->a[0], &tmp->a[1], &tmp->a[2]);
}

size_t basis_get_allocated_size(const basis_t *basis)
{
	return (sizeof(basis)/sizeof(basis_t));
}

void basis_get_vectors(const basis_t *basis, coord_t *a1, 
                       coord_t *a2, coord_t *a3)
{
  coord_copy(a1, &basis->a[0]);
  coord_copy(a2, &basis->a[1]);
  coord_copy(a3, &basis->a[2]);
}

coord_t *basis_rotate_normal(const coord_t *nor, double **R)
{
  coord_t *new_normal = coord_init();

  coord_set(new_normal,
                R[0][0] * nor->x + R[0][1] * nor->y + R[0][2] * nor->z,
                R[1][0] * nor->x + R[1][1] * nor->y + R[1][2] * nor->z,
                R[2][0] * nor->x + R[2][1] * nor->y + R[2][2] * nor->z);
  return (new_normal);
}

coord_t *basis_rotate_vector_list(const coord_t *list, size_t n, double **R)
{
  size_t i;
  double faux_x, faux_y, faux_z;
  coord_t *rotated_list;
  
  if (n > 0)
  {
    rotated_list = (coord_t*) malloc(sizeof(coord_t) * n);
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

basis_t *basis_rotate_basis(const basis_t *basis, double **R)
{
  basis_t *new_basis;
  coord_t *faux = coord_init();

  new_basis = basis_init();
  
  coord_copy(faux, &basis->a[0]);
  
  coord_set(&new_basis->a[0], 
                R[0][0] * faux->x + R[0][1] * faux->y + R[0][2] * faux->z,
                R[1][0] * faux->x + R[1][1] * faux->y + R[1][2] * faux->z,
                R[2][0] * faux->x + R[2][1] * faux->y + R[2][2] * faux->z);

  coord_copy(faux, &basis->a[1]);
  
  coord_set(&new_basis->a[1], 
                R[0][0] * faux->x + R[0][1] * faux->y + R[0][2] * faux->z,
                R[1][0] * faux->x + R[1][1] * faux->y + R[1][2] * faux->z,
                R[2][0] * faux->x + R[2][1] * faux->y + R[2][2] * faux->z);

  coord_copy(faux, &basis->a[2]);
  
  coord_set(&new_basis->a[2], 
                R[0][0] * faux->x + R[0][1] * faux->y + R[0][2] * faux->z,
                R[1][0] * faux->x + R[1][1] * faux->y + R[1][2] * faux->z,
                R[2][0] * faux->x + R[2][1] * faux->y + R[2][2] * faux->z);
  
  return (new_basis);
}

basis_t *basis_angle_rotate(const basis_t *a, double alpha, 
                            double beta, double gamma)
{
  basis_t *new_basis = NULL;

  return (new_basis);
}

double **normal_get_rotation_matrix(const coord_t *normal)
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

basis_t *basis_rotate_parallel_to_x_axis_rhs(const basis_t *basis, double **R)
{
  double faux_x;
  basis_t *rot_basis = basis_init();
  
  faux_x = sqrt(basis->a[0].x*basis->a[0].x + basis->a[0].y*basis->a[0].y);

  R[0][0] = basis->a[0].x / faux_x;
  R[0][1] = basis->a[0].y / faux_x;

  R[1][0] = -R[0][1];
  R[1][1] = R[0][0];
  
  /* rotate lattice vectors b1, b2, b3 such that b1 || x-axis */
  rot_basis->a[0].x = R[0][0] * basis->a[0].x + R[0][1] * basis->a[0].y;
  rot_basis->a[0].y = R[1][0] * basis->a[0].x + R[1][1] * basis->a[0].y;

  rot_basis->a[1].x = R[0][0] * basis->a[1].x + R[0][1] * basis->a[1].y;
  rot_basis->a[1].y = R[1][0] * basis->a[1].x + R[1][1] * basis->a[1].y;

  rot_basis->a[2].x = R[0][0] * basis->a[2].x + R[0][1] * basis->a[2].y;
  rot_basis->a[2].y = R[1][0] * basis->a[2].x + R[1][1] * basis->a[2].y;
  
  return (rot_basis);
}

coord_t *basis_get_a1(const basis_t *basis)
{
  coord_t *a1 = (coord_t*) &basis->a[0];
  return (a1);
}

coord_t *basis_get_a2(const basis_t *basis)
{
  coord_t *a2 = (coord_t*) &basis->a[1];
  return (a2);
}

coord_t *basis_get_a3(const basis_t *basis)
{
  coord_t *a3 = (coord_t*) &basis->a[2];
  return (a3);
}

void basis_set_a1(basis_t *basis, const coord_t *a1)
{
  coord_copy(&basis->a[0], a1);
}

void basis_set_a2(basis_t *basis, const coord_t *a2)
{
  coord_copy(&basis->a[1], a2);
}

void basis_set_a3(basis_t *basis, const coord_t *a3)
{
  coord_copy(&basis->a[2], a3);
}

coord_t *basis_get_normal(const basis_t *basis, const miller_hkl_t *hkl)
{
  const coord_t *a1 = BASIS_VECTOR_PTR(basis, 0);
  const coord_t *a2 = BASIS_VECTOR_PTR(basis, 1);
  const coord_t *a3 = BASIS_VECTOR_PTR(basis, 2);
  coord_t *normal = coord_init();
  
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
