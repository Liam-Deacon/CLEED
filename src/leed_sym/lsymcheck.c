/*********************************************************************
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 *
 * Implements functions to check the rotational and mirror symmetry
 * of a LEED model.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "leed.h"
#include "leed_def.h"

static const double EPSILON = 0.001L; /*!< comparison threshold for floating point values */

/*! enum to indicate symmetry type */
typedef enum {
  ROT_SYM,      /*!< Rotational Symmetry */
  MIR_SYM       /*!< Mirror Symmetry */
} symmetry_type;

#if CONTROL
/*!
 * Prints control message stating which layers are to be tested (bulk or surface).
 */
static void leed_print_layer_test_type(leed_structure layer_type)
{
  switch(layer_type)
  {
    case(OVER):
      CONTROL_MSG(CONTROL, "testing the overlayers\n");
      break;
    case(BULK):
      CONTROL_MSG(CONTROL, "testing the bulk layers\n");
      break;
    default:
      CONTROL_MSG(CONTROL, "can not test "
        "symmetry, because the nature of the layer is unknown!\n");
  }
}

/*!
 * Prints warnings about rotational/mirror symmetry for lattice vector or layer
 *
 * \param i layer number
 * \param faux difference (for tolerance)
 * \param sym symmetry type (\c ROT_SYM or \c MIR_SYM)
 * \param i_mir mirror plane
 * \param alpha rotation about mirror plane
 *
 * \note \p i_mir and \p alpha are only used if \p sym is \c MIR_SYM
 *
 */
static inline void
leed_print_symmetry_warnings(
    size_t i, real faux, symmetry_type sym, size_t i_mir, real alpha)
{
  char buf[1024] = "\0";
  if(i < 4)
  {
    CONTROL_MSG(CONTROL, "lattice vector has no %s symmetry, difn = %e\n",
                sym == ROT_SYM ? "rotational" : "mirror", faux);
  }
  else
  {
    if (sym == MIR_SYM)
      sprintf(buf, " for mirror plane #%d alpha%f", i_mir, alpha);
    CONTROL_MSG(CONTROL, "layer%d has no %s symmetry%s, difn = %e\n",
        sym == ROT_SYM ? "rotational" : "mirror", buf,
        ((i % 2) != 0) ? (i-5)/2 : (i-4)/2, faux);
  } /* end else */
}
#else
#define leed_print_layer_type(layer_type)
#define leed_print_symmetry_warnings(...)
#endif

/*! Returns pointer to lattice vector of layer type to test */
static inline real
  *leed_get_symmetry_lattice_vector_for_layer(const leed_crystal * const p_cryst)
{
  if (p_cryst && p_cryst->layers)
  {
    return (p_cryst->layers[0].bulk_over == OVER) ? p_cryst->b : p_cryst->a;
  }
  else
    WARNING_MSG("'p_cryst%s' must be allocated\n", p_cryst ? "->layers" : "");

  return NULL;
}

/*!
 * Checks rotational symmetry.
 *
 * \param p_cryst Pointer to crystallographic structure to test.
 * \return number of rotational symmetries.
 * \warning \p p_cryst is assumed to be initialised in memory.
 */
int leed_check_rotation_sym(const leed_crystal * const p_cryst)
{
  size_t i, i_c, i_d;
  size_t i_atoms, n_rot, i_rot;
  int ctrol = 0;

  size_t n_size = 2*p_cryst->n_layers + 4; /* size for alloc */

  real a1x, a1y, a2x, a2y; /* 2-dim lattice shifttors: 1=x, 2=y 0 is not used */
  real det;                /* Determinant */

  real **R_n = NULL;       /* Rot.-matrix */
  real *vecrot = NULL;
  real *integ = NULL;      /* in case of rot.sym., it must be an integer */
  real *position = NULL;   /* intermediate storage for rotated atom positions */

  real faux;
  real vaux[2];

  real *a = leed_get_symmetry_lattice_vector_for_layer(p_cryst);

  /* do initial checks */
  leed_print_layer_test_type(p_cryst->layers[0].bulk_over);

  /* setup lattice vectors */
  a1x = a[1];
  a1y = a[3];
  a2x = a[2];
  a2y = a[4];

  det = (a1x * a2y - a1y * a2x);

  /* Allocate memory */
  CLEED_ALLOC_CHECK(vecrot = (real *) calloc(n_size, sizeof(real)));
  CLEED_ALLOC_CHECK(integ = (real *) calloc(n_size, sizeof(real)));
  n_rot = p_cryst->n_rot;

  /* calculate rotation matrix */
  R_n = leed_beam_get_rotation_matrices(n_rot);

  #ifdef CONTROL_X
  for(i_rot = 1; i_rot < n_rot; i_rot++)
  {
    fprintf(STDCTR, "%d: (%6.3f %6.3f)\n", i_rot, R_n[i_rot][1], R_n[i_rot][2]);
    fprintf(STDCTR, "    (%6.3f %6.3f)\n",        R_n[i_rot][3], R_n[i_rot][4]);
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
          leed_print_symmetry_warnings(i, faux, ROT_SYM, 0, 0.);
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
  CLEED_ALLOC_CHECK(position = (real *) calloc(n_size, sizeof(real)));

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
                if(cleed_real_fabs(vaux[0]) > EPSILON ||
                    cleed_real_fabs(vaux[1]) > EPSILON)
                {
                  ctrol++ ;
                } /* if cleed_real_fabs */

              } /* if IS_EQUAL_REAL */

            } /* for i_d */

            if(ctrol < p_cryst->layers[i].n_atoms)
              ctrol = 0;
            else
            {
              fprintf(STDCTR, "no rotational symmetry for atom%d in layer%d\n",
                  i_c, i);
            }

          } /* if ctrol */

        } /* for i_c */

      } /* for i_rot */
    }

  } /* for i (layer) */

  /* free rotation matrix */
  leed_beam_rotation_matrix_free(R_n);

  /* reset p_cryst->n_rot when all o.k. */
  if(ctrol == 0)
    p_cryst->n_rot = n_rot;
  else
    p_cryst->n_rot = 1;

  /* WARNING_LOG if no rotational symmetry */
  if(p_cryst->n_rot == 1)
  {
    WARNING_MSG("No rotational symmetry\n");
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
 * \retval -1 if failed and \c EXIT_ON_ERROR is not defined.
 * \warning \p p_cryst is assumed to be initialised in memory.
 */
int leed_check_mirror_sym(const leed_crystal * const p_cryst)
{
  size_t i, i_c, i_d;
  size_t i_atoms, n_mir, i_mir;
  int ctrol = 0;
  size_t n_size = 2*p_cryst->n_layers + 4; /* size for alloc */

  real a1x, a1y, a2x, a2y; /* 2-dim lattice shifttors: 1=x, 2=y 0 is not used */
  real det;                /* Determinante */

  real R_m[5] = {0., 1., 1., 1., 1.};  /* mirror -matrix*/
  real *vecrot = NULL;
  real *integ = NULL;      /* in case of rot.sym., it must be an integer */
  real *position = NULL;   /* intermediate storage for reflectd atom positions */

  real faux;
  real vaux[2];

  real *a = leed_get_symmetry_lattice_vector_for_layer(p_cryst);

  /* do initial checks */
  leed_print_layer_test_type(p_cryst->layers[0].bulk_over);

  /* setup lattice vectors */
  a1x = a[1];
  a1y = a[3];
  a2x = a[2];
  a2y = a[4];

  det = (a1x * a2y - a1y * a2x);

  /* Allocate */
  CLEED_ALLOC_CHECK(vecrot = (real *) calloc(n_size, sizeof(real)));
  CLEED_ALLOC_CHECK(integ = (real *) calloc(n_size, sizeof(real)));
  n_mir = p_cryst->n_mir;

  /* TEST REFLECTION */
  for(i_mir=0; i_mir < n_mir; i_mir++)
  {
    if(ctrol == 0)
    {
      R_m[1] = cleed_real_cos(2* p_cryst->alpha[i_mir]);
      R_m[2] = cleed_real_sin(2* p_cryst->alpha[i_mir]);
      R_m[3] =  R_m[2];
      R_m[4] = -R_m[1];

      #ifdef CONTROL_X
      fprintf(STDCTR, "%d: (%6.3f %6.3f)\n", i_mir, R_m[1], R_m[2]);
      fprintf(STDCTR, "    (%6.3f %6.3f)\n",        R_m[3], R_m[4]);
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
          leed_print_symmetry_warnings(i, faux, MIR_SYM,
              n_mir+1, p_cryst->alpha[i_mir]*RAD_TO_DEG);

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

  /*
   * test mirror symmetry for composite layer.
   * therefore reflect all atoms in the layer
   * check if you reach this position .......
   */

  /*
   * find the layer with max atoms and allocate storage space.
   * n_size is the maximum space needed.
   */
  n_size = p_cryst->layers[0].n_atoms;
  for(i=0; i < p_cryst->n_layers; i++)
  {
    n_size = MAX(p_cryst->layers[i].n_atoms, n_size);
  }
  n_size = 3 * n_size + 1;

  /* allocate memory */
  CLEED_ALLOC_CHECK(position = (real *) calloc(n_size, sizeof(real)));

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
                if(cleed_real_fabs(vaux[0]) > EPSILON ||
                    cleed_real_fabs(vaux[1]) > EPSILON)
                {
                  ctrol++ ;
                }
              }

            } /* for i_d */

            if(ctrol < p_cryst->layers[i].n_atoms) ctrol = 0;
            else
            {
              CONTROL_MSG(CONTROL, "no mirror symmetry in "
                  "mirror plane #%d(alpha %f) for atom%d in layer%d\n",
                  i_mir, p_cryst->alpha[i_mir], i_c, i);
            }

          } /* if ctrol */

        } /* for i_c */

      } /* for i_mir */

    }

  } /* for i (layer) */

  /* reset p_cryst->n_rot when all okay */
  if(ctrol == 0)
    p_cryst->n_mir = n_mir;
  else
    p_cryst->n_mir = 0;

  /* WARNING_LOG if no rotational symmetry */
  if(p_cryst->n_mir == 0)
  {
    WARNING_MSG("no mirror symmetry\n");
  }

  /* free memory */
  free(vecrot);
  free(integ);
  free(position);

  return(p_cryst->n_mir);
} /* end of function leed_check_mirror_sym */
