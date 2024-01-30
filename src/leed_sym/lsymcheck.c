/*********************************************************************
  WB/16.09.98
  file contains function:

  leed_check_rotation_sym
  leed_check_mirror_sym

Changes:
*********************************************************************/

/*! \file
 *
 * Implements leed_check_rotation_sym() function to check that the rotational
 * symmetry of the LEED model is valid.
 */
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "leed.h"
#include "leed_def.h"

#ifndef EPSILON
#define EPSILON 0.001
#endif


/*!
 * Checks rotational symmetry.
 *
 * \param p_cryst Pointer to crystallographic structure to test.
 * \return number of rotational symmetries.
 */
int leed_check_rotation_sym(leed_crystal *p_cryst)
{
  size_t i, i_c, i_d;
  size_t i_atoms, n_rot, i_rot;
  int ctrol;
  size_t n_size;

  real a1x, a1y, a2x, a2y; /* 2-dim lattice shifttors: 1=x, 2=y 0 is not used */
  real det;                /* Determinante */

  real **R_n;              /* Rot.-matrix */
  real *vecrot;
  real *integ;             /* in case of rot.sym., it must be an integer */
  real *position;          /* intermediate storage for rotated atom positions */

  real faux;
  real vaux[2];


  if(p_cryst->layers[0].bulk_over == OVER)
  {
    a1x = p_cryst->b[1];
    a1y = p_cryst->b[3];
    a2x = p_cryst->b[2];
    a2y = p_cryst->b[4];

    n_size = 2*p_cryst->n_layers + 4;
    det = (a1x * a2y - a1y * a2x);
  }
  else
  {
    a1x = p_cryst->a[1];
    a1y = p_cryst->a[3];
    a2x = p_cryst->a[2];
    a2y = p_cryst->a[4];

    det = (a1x * a2y - a1y * a2x);
    n_size = 2*p_cryst->n_layers + 4;
  }

  #ifdef CONTROL
  if(p_cryst->layers[0].bulk_over == OVER)
  {
    fprintf(STDCTR, "(leed_check_rotation_sym): test the overlayers\n");
  }
  else if(p_cryst->layers[0].bulk_over == BULK)
  {
    fprintf(STDCTR, "(leed_check_rotation_sym): test the bulk layers\n");
  }
  else
  {
    fprintf(STDCTR, "(leed_check_rotation_sym): can not test rotational "
        "symmetry, because the nature of the layer is unknown!\n");
  }
  #endif

  /* Allocate memory */
  vecrot = (real *) malloc(n_size * sizeof(real));
  integ = (real *) malloc(n_size * sizeof(real));
  n_rot = p_cryst->n_rot;

  /* calculate rotation matrix */
  R_n = leed_beam_get_rotation_matrices(n_rot);
  ctrol = 0;

  #ifdef CONTROL_X
  for(i_rot = 1; i_rot < n_rot; i_rot++)
  {
    fprintf(STDCTR, "%lu: (%6.3f %6.3f)\n", i_rot, R_n[i_rot][1], R_n[i_rot][2]);
    fprintf(STDCTR, "    (%6.3f %6.3f)\n",         R_n[i_rot][3], R_n[i_rot][4]);
  }
  #endif

  /* TEST ROTATION */
  for(i_rot = 1; i_rot < n_rot; i_rot++)
  {
    if(ctrol == 0)
    {
      /* rotate lattice vector */
      vecrot[0] = a1x * R_n[i_rot][1] + a1y * R_n[i_rot][2];
      vecrot[1] = a1x * R_n[i_rot][3] + a1y * R_n[i_rot][4];
      vecrot[2] = a2x * R_n[i_rot][1] + a2y * R_n[i_rot][2];
      vecrot[3] = a2x * R_n[i_rot][3] + a2y * R_n[i_rot][4];

      /* rotate reg_shifts */
      for(i=0;i < p_cryst->n_layers; i++)
      {
        vecrot[2*i+4] = p_cryst->layers[i].reg_shift[1] * R_n[i_rot][1] +
                          p_cryst->layers[i].reg_shift[2] * R_n[i_rot][2];
        vecrot[2*i+5] = p_cryst->layers[i].reg_shift[1] * R_n[i_rot][3] +
                          p_cryst->layers[i].reg_shift[2] * R_n[i_rot][4];
      }

      /* build the difference */
      for(i=0; i < p_cryst->n_layers; i++)
      {
        vecrot[2*i+4] -= p_cryst->layers[i].reg_shift[1];
        vecrot[2*i+5] -= p_cryst->layers[i].reg_shift[2];
      }

      /* formula for ...  of lattice vector */
      integ[0] = cleed_real_fabs((vecrot[1] * a1x - vecrot[0] * a1y)/det) ;
      integ[1] = cleed_real_fabs((vecrot[1] * a2x - vecrot[0] * a2y)/(-det));
      integ[2] = cleed_real_fabs((vecrot[3] * a1x - vecrot[2] * a1y)/det) ;
      integ[3] = cleed_real_fabs((vecrot[3] * a2x - vecrot[2] * a2y)/(-det));

      for(i=0; i < p_cryst->n_layers; i++)
      {
        integ[4 + 2 * i] =
            cleed_real_fabs((vecrot[2*i+5] * a1x - vecrot[2*i+4] * a1y)/det) ;
        integ[5 + 2 * i] =
            cleed_real_fabs((vecrot[2*i+5] * a2x - vecrot[2*i+4] * a2y)/(-det));
      }

      /* test for integer */
      for(i=0; i < n_size; i++)
      {
        faux = integ[i] - cleed_real_nint(integ[i]);
        if(cleed_real_fabs(faux) > EPSILON)
        {
          ctrol = 1;

          #ifdef CONTROL
          if(i < 4)
          {
            fprintf(STDCTR, "(leed_check_rotation_sym): "
                "lattice vector has no rotational symmetry, difn = %e\n", faux);
          }
          else
          {
            if((i % 2) != 0)
            {
              fprintf(STDCTR, "(leed_check_rotation_sym): layer%lu has no "
                  "rotational symmetry, difn = %e\n", ((i-5)/2), faux);
            }
            else
            {
              fprintf(STDCTR,"(leed_check_rotation_sym): layer%lu has no "
                  "rotational symmetry, difn = %e\n", ((i-4)/2), faux);
            } /* if i % 2 */

          } /* end else */
          #endif

        } /* if cleed_real_fabs ...*/

      } /*for i */

      /* reset p_cryst->n_rot when all are okay */
      if(ctrol != 0)
      {
        p_cryst->n_rot = 1;
        n_rot = 1;
      }

    }/* if */

  }/* for */


  /* test rot.symmetry for composite layer.
   * therefore rotate all atoms in the layer
   * check if you reach this position .......
   */

  /* find the layer with max atoms and allocate storage space.
   * n_size is the maximum space needed */
  n_size = p_cryst->layers[0].n_atoms;
  for(i = 0 ;i < p_cryst->n_layers ; i++)
  {
    n_size = MAX(p_cryst->layers[i].n_atoms, n_size);
  }
  n_size = 3 * n_size + 1;

  /* allocate */
  position = (real *) malloc(n_size * sizeof(real));

  for(i=0; i < p_cryst->n_layers; i++)
  {
    if(p_cryst->layers[i].n_atoms > 1 && ctrol == 0)
    {
      for(i_rot=1; i_rot < n_rot; i_rot++)
      {
        for(i_atoms=0; i_atoms < p_cryst->layers[i].n_atoms; i_atoms++)
        {
          position[3*i_atoms] =
              p_cryst->layers[i].atoms[i_atoms].pos[1] * R_n[i_rot][1] +
              p_cryst->layers[i].atoms[i_atoms].pos[2] * R_n[i_rot][2];
          position[3*i_atoms+1] =
              p_cryst->layers[i].atoms[i_atoms].pos[1] * R_n[i_rot][3] +
              p_cryst->layers[i].atoms[i_atoms].pos[2] * R_n[i_rot][4];
          position[3*i_atoms+2] = p_cryst->layers[i].atoms[i_atoms].pos[3];
        }

        /* formula for ...  of lattice vec */
        for(i_c=0; i_c < p_cryst->layers[i].n_atoms; i_c++)
        {
          if(ctrol == 0)
          {
            for(i_d=0; i_d < p_cryst->layers[i].n_atoms; i_d++)
            {
              if( IS_EQUAL_REAL(position[3*i_c+2], position[3*i_d+2]))
              {
                vaux[0] = position[3*i_c] -
                          p_cryst->layers[i].atoms[i_d].pos[1];
                vaux[1] = position[3*i_c+1] -
                          p_cryst->layers[i].atoms[i_d].pos[2];
                integ[0] = cleed_real_fabs((vaux[1] * a1x - vaux[0] * a1y)/det) ;
                integ[1] = cleed_real_fabs((vaux[1] * a2x - vaux[0] * a2y)/(-det));

                vaux[0] = integ[0] - cleed_real_nint(integ[0]);
                vaux[1] = integ[1] - cleed_real_nint(integ[1]);
                if(cleed_real_fabs(vaux[0]) > EPSILON || cleed_real_fabs(vaux[1]) > EPSILON)
                {
                  ctrol++ ;
                } /* if cleed_real_fabs */

              } /* if IS_EQUAL_REAL */

            } /* for i_d */

            if(ctrol < p_cryst->layers[i].n_atoms) ctrol = 0;
            else
            {
              fprintf(STDCTR, "no rotational symmetry for atom%lu in layer%lu\n", i_c, i);
            }

          } /* if ctrol */

        } /* for i_c */

      } /* for i_rot */
    }

  } /* for i (layer) */

  /* free rotation matrix */
  leed_beam_rotation_matrix_free(R_n);

  /* reset p_cryst->n_rot when all o.k. */
  if(ctrol == 0) p_cryst->n_rot = n_rot;
  else p_cryst->n_rot = 1;

  /* WARNING_LOG if no rotational symmetry */
  if(p_cryst->n_rot == 1)
  {
    ;
    #ifdef WARNING_LOG
    fprintf(STDWAR, "*warning (leed_check_rotation_sym): "
        "No rotational symmetry\n");
    #endif
  }

  /* free memory */
  free(vecrot);
  free(integ);
  free(position);

  return(p_cryst->n_rot);
} /* end of function leed_check_rotation_sym */


/*!
 * Tests the mirror symmetry and returns the number of mirror planes found.
 *
 * \param p_cryst pointer to \c struct containing information about the bulk or
 * overlayer structure.
 * \return the number of mirror planes.
 */
int leed_check_mirror_sym(leed_crystal *p_cryst)
{
  size_t i, i_c, i_d;
  size_t i_atoms, n_mir, i_mir;
  int ctrol;
  size_t n_size;

  real a1x, a1y, a2x, a2y; /* 2-dim lattice shifttors: 1=x, 2=y 0 is not used */
  real det;                /* Determinante */

  real R_m[5];             /* mirror -matrix*/
  real *vecrot;
  real *integ;             /* in case of rot.sym., it must be an integer */
  real *position;          /* intermediate storage for reflectd atom positions */

  real faux;
  real vaux[2];

  R_m[1] = R_m[2] = R_m[3] = R_m[4] = 1.0;


  if(p_cryst->layers[0].bulk_over == OVER)
  {
    a1x = p_cryst->b[1];
    a1y = p_cryst->b[3];
    a2x = p_cryst->b[2];
    a2y = p_cryst->b[4];

    n_size = 2*p_cryst->n_layers + 4;
    det = (a1x * a2y - a1y * a2x);
  }
  else
  {
    a1x = p_cryst->a[1];
    a1y = p_cryst->a[3];
    a2x = p_cryst->a[2];
    a2y = p_cryst->a[4];

    det = (a1x*a2y - a1y*a2x);
    n_size = 2*p_cryst->n_layers + 4;
  }

  #ifdef CONTROL
  if((p_cryst->layers + 0)->bulk_over == OVER)
  {
    fprintf(STDCTR, "(leed_check_mirror_sym): testing the overlayers\n");
  }
  else if((p_cryst->layers + 0)->bulk_over == BULK)
  {
    fprintf(STDCTR, "(leed_check_mirror_sym): testing the bulk\n");
  }
  else
  {
    fprintf(STDCTR, "(leed_check_mirror_sym): can not test mirror symmetry, "
        "because the nature of the layer is unknown!\n");
  }
  #endif

  /* Allocate */
  vecrot = (real *) malloc(n_size * sizeof(real));
  integ = (real *) malloc(n_size * sizeof(real));
  n_mir = p_cryst->n_mir;
  ctrol = 0;

  /* TEST REFLECTION */
  for(i_mir=0; i_mir < n_mir; i_mir++)
  {
    if(ctrol == 0)
    {
      R_m[1] = cleed_real_cos(2* p_cryst->alpha[i_mir]);
      R_m[2] = cleed_real_sin(2* p_cryst->alpha[i_mir]);
      R_m[3] = R_m[2];
      R_m[4] = - R_m[1];

      #ifdef CONTROL_X
      fprintf(STDCTR, "%lu: (%6.3f %6.3f)\n", i_mir, R_m[1], R_m[2]);
      fprintf(STDCTR, "    (%6.3f %6.3f)\n",         R_m[3], R_m[4]);
      #endif

      /* reflect lattice vector */
      vecrot[0]= a1x * R_m[1] + a1y * R_m[2];
      vecrot[1]= a1x * R_m[3] + a1y * R_m[4];
      vecrot[2]= a2x * R_m[1] + a2y * R_m[2];
      vecrot[3]= a2x * R_m[3] + a2y * R_m[4];

      /* reflect reg_shifts */
      for(i=0; i < p_cryst->n_layers; i++)
      {
        vecrot[2*i+4] = p_cryst->layers[i].reg_shift[1] * R_m[1] +
                          p_cryst->layers[i].reg_shift[2] * R_m[2];
        vecrot[2*i+5] = p_cryst->layers[i].reg_shift[1] * R_m[3] +
                          p_cryst->layers[i].reg_shift[2] * R_m[4];
      }

      /* calculate the difference */
      for(i=0; i < p_cryst->n_layers; i++)
      {
        vecrot[2*i+4] -= p_cryst->layers[i].reg_shift[1];
        vecrot[2*i+5] -= p_cryst->layers[i].reg_shift[2];
      }

      /* formula for ...  of lattice vector */
      integ[0] = cleed_real_fabs((vecrot[1] * a1x - vecrot[0] * a1y)/det) ;
      integ[1] = cleed_real_fabs((vecrot[1] * a2x - vecrot[0] * a2y)/(-det));
      integ[2] = cleed_real_fabs((vecrot[3] * a1x - vecrot[2] * a1y)/det) ;
      integ[3] = cleed_real_fabs((vecrot[3] * a2x - vecrot[2] * a2y)/(-det));

      for(i=0; i < p_cryst->n_layers; i++)
      {
        integ[4 + 2 * i] =
            cleed_real_fabs((vecrot[2*i+5] * a1x - vecrot[2*i+4] * a1y)/det) ;
        integ[5 + 2 * i] =
            cleed_real_fabs((vecrot[2*i+5] * a2x - vecrot[2*i+4] * a2y)/(-det));
      }

      /* test for integer */
      for(i=0; i < n_size ; i++)
      {
        faux = integ[i] - cleed_real_nint(integ[i]);
        if(cleed_real_fabs(faux) > EPSILON)
        {
          ctrol = 1;

          #ifdef CONTROL
          if(i < 4)
          {
            fprintf(STDCTR, "(leed_check_mirror_sym): "
                    "no mirror symmetry for lattice vector in case of "
                    "mirror plane #%ld alpha%f , difn = %e\n",
                    i_mir+1, p_cryst->alpha[i_mir]*RAD_TO_DEG, faux);
          }
          else
          {
            if((i % 2) != 0)
            {
              fprintf(STDCTR, "(leed_check_mirror_sym): layer%ld has no mirror "
                  "symmetry for mirror plane #%ld alpha%f , difn = %e\n",
                  ((i-5)/2), n_mir+1, p_cryst->alpha[i_mir]*RAD_TO_DEG, faux);
            }
            else
            {
              fprintf(STDCTR, "(leed_check_mirror_sym): layer%ld has no mirror "
                  "symmetry for mirror plane #%ld alpha%f , difn = %e\n",
                  ((i-4)/2), n_mir+1, p_cryst->alpha[i_mir]*RAD_TO_DEG, faux);
            }
          }/* end else */
          #endif

        }/* if cleed_real_fabs ...*/

      }/*for i */

      /* if something is wrong then reset p_cryst->n_mir and exit */
      if(ctrol != 0)
      {
        p_cryst->n_mir = 0;
        n_mir = 0;
        exit(0);
      }

    }/* if */

  }/* for */

  /* test mirror symmetry for composite layer.
   * therefore reflect all atoms in the layer
   * check if you reach this position .......
   */

  /* find the layer with max atoms and allocate storage space.
   * n_size is the maximum space needed. */
  n_size = p_cryst->layers[0].n_atoms;
  for(i=0; i < p_cryst->n_layers; i++)
  {
    n_size = MAX(p_cryst->layers[i].n_atoms, n_size);
  }
  n_size = 3 * n_size + 1;

  /* allocate memory */
  position = (real *) malloc(n_size * sizeof(real));

  for(i=0; i < p_cryst->n_layers; i++)
  {
    if(p_cryst->layers[i].n_atoms > 1 && ctrol == 0)
    {
      for(i_mir=0; i_mir < n_mir; i_mir++)
      {
        for(i_atoms=0; i_atoms < p_cryst->layers[i].n_atoms; i_atoms++)
        {
          position[3*i_atoms] =
              p_cryst->layers[i].atoms[i_atoms].pos[1] * R_m[1] +
              p_cryst->layers[i].atoms[i_atoms].pos[2] * R_m[2];
          position[3*i_atoms+1] =
              p_cryst->layers[i].atoms[i_atoms].pos[1] * R_m[3] +
              p_cryst->layers[i].atoms[i_atoms].pos[2] * R_m[4];
          position[3*i_atoms+2] = p_cryst->layers[i].atoms[i_atoms].pos[3];
        }

        /* formula for ...  of lattice vec */
        for(i_c = 0; i_c < p_cryst->layers[i].n_atoms; i_c++)
        {
          if(ctrol == 0)
          {
            for(i_d = 0; i_d < p_cryst->layers[i].n_atoms; i_d++)
            {
              if( IS_EQUAL_REAL(position[3*i_c+2], position[3*i_d+2]))
              {
                vaux[0] = position[3*i_c] -
                          p_cryst->layers[i].atoms[i_d].pos[1];
                vaux[1] = position[3*i_c+1] -
                          p_cryst->layers[i].atoms[i_d].pos[2];
                integ[0] = cleed_real_fabs((vaux[1] * a1x - vaux[0] * a1y)/det) ;
                integ[1] = cleed_real_fabs((vaux[1] * a2x - vaux[0] * a2y)/(-det));

                vaux[0] = integ[0] - cleed_real_nint(integ[0]);
                vaux[1] = integ[1] - cleed_real_nint(integ[1]);
                if(cleed_real_fabs(vaux[0]) > EPSILON || cleed_real_fabs(vaux[1]) > EPSILON)
                {
                  ctrol++ ;
                }
              }

            } /* for i_d */

            if(ctrol < p_cryst->layers[i].n_atoms) ctrol = 0;
            else
            {
              fprintf(STDCTR, "(leed_check_mirror_sym): no mirror symmetry in "
                  "mirror plane #%ld(alpha %f) for atom%lu in layer%lu\n",
                  i_mir, p_cryst->alpha[i_mir], i_c, i);
            }

          } /* if ctrol */

        } /* for i_c */

      } /* for i_mir */

    }

  } /* for i (layer) */

  /* reset p_cryst->n_rot when all okay */
  if(ctrol == 0) p_cryst->n_mir = n_mir;
  else p_cryst->n_mir = 0;

  /* WARNING_LOG if no rotational symmetry */
  if(p_cryst->n_mir == 0)
  {
    #ifdef WARNING_LOG
    fprintf(STDWAR, "*warning (leed_check_mirror_sym): no mirror symmetry\n");
    #endif
  }

  /* free memory */
  free(vecrot);
  free(integ);
  free(position);

  return(p_cryst->n_mir);
} /* end of function leed_check_mirror_sym */
