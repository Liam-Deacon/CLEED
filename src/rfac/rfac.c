/*********************************************************************
 *                           RFAC.C
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.10.10 - Creation
 *********************************************************************/

/*!
 * \file
 * \brief Low level routines associated with #rfac_iv and #rfac_iv_data
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rfac.h"


/*!
 * Returns a string corresponding to the type of RFactor
 *
 * \param type #rfactor_type enum
 * \return name of RFactor
 */
inline const char *rfactor_name(rfactor_type type) {
  switch (type)
  {
    case RP_FACTOR: {return "Rp"; break;}
    case R1_FACTOR: {return "R1"; break;}
    case R2_FACTOR: {return "R2"; break;}
    case RB_FACTOR: {return "Rb1"; break;}
    default: break;
  }
  return "Unknown RFactor type";
}

inline void rfactor_print(rfactor_type type, real r_min, real rr,
    real s_min, real e_range)
{
  const char *rf = rfactor_name(type);
#if CONTROL
  fprintf(STDCTR, "%s = ", rf);
  fprintf(STDCTR, "%.6f, RR = %.6f (shift = %4.1f, eng. overlap = %.1f)\n",
          r_min, rr, s_min, e_range);
#endif
  fprintf(STDOUT, "%.6f %.6f %.2f %.2f\t\t#  ", r_min, rr, s_min, e_range);
  fprintf(STDCTR, "%s  RR  shift  range\n", rf);
}

/*!
 * \brief intialize #rfac_iv object.
 *
 * \return pointer to #rfac_iv object.
 * \retval NULL if memory cannot be allocated.
 */
rfac_iv *rfac_iv_init(void)
{
  rfac_iv *_iv = (rfac_iv *) calloc(1, sizeof(rfac_iv));

  if (_iv == NULL) return NULL;

  /* set defaults */
  _iv->data = NULL;
  _iv->smooth = false;
  _iv->sort = true;
  _iv->equidist = true;
  _iv->n_eng = 0;

  return(_iv);
}

/*!
 * \brief allocate memory for #rfac_iv with \p n_eng data points for
 * #rfac_iv_data member.
 *
 * \return pointer to #rfac_iv object.
 * \retval NULL if memory cannot be allocated.
 */
rfac_iv *rfac_iv_alloc(size_t n_eng)
{
  rfac_iv *_iv = (rfac_iv *) malloc(sizeof(rfac_iv));

  if (_iv == NULL) return NULL;

  rfac_iv_data *_data = (rfac_iv_data *) malloc(n_eng*sizeof(rfac_iv_data));

  if (_data == NULL)
  {
    free(_iv);
    return(NULL);
  }

  /* set defaults */
  _iv->data = _data;
  _iv->smooth = false;
  _iv->sort = true;
  _iv->equidist = true;
  _iv->n_eng = n_eng;

  return(_iv);
}

rfac_args *rfac_args_init(void)
{
  rfac_args *args = (rfac_args*) calloc(1, sizeof(rfac_args));

  if (args == NULL) return NULL;

  strcpy(args->ctr_file, "\0");
  strcpy(args->the_file, "\0");

  args->s_ini = -10.;
  args->s_fin =  10.;
  args->s_step = 0.5;

  args->r_type = RP_FACTOR;         /* default R factor: rp */

  args->all_groups = 0;             /* display only averge over R-factors */
  args->vi = 4.0;                   /* im. part of opt. potential */

  strcpy(args->iv_file, "\0");             /* no output of IV curves */
  strcpy(args->out_file, "\0");            /* will be treated as "stdout" */

  return (args);
}

/*!
 * Free #rfac_iv structure instance from memory.
 */
void rfac_iv_free(rfac_iv *_iv)
{
  if (_iv != NULL)
  {
    if (_iv->data != NULL) free(_iv->data);
    _iv->data = NULL;
    free(_iv);
    _iv = NULL;
  }
}

/*!
 * Performs a deep copy of an #rfac_iv structure.
 *
 * \param[out] dest Pointer to copied structure.
 * \param[in] src Pointer to source structure.
 *
 * \warning Nothing will be done if \p src is \c NULL .
 */
void rfac_iv_copy(rfac_iv *dest, const rfac_iv *src)
{
  if (src == NULL) return; /* nothing to do */

  /* allocate memory for dest if needed */
  if (dest == NULL)
    if ( ( dest = (rfac_iv *) malloc(sizeof(rfac_iv)) ) == NULL)
        return;

  /* free data is exists; to be allocated new memory later */
  if (dest->data != NULL) free(dest->data);

  /* copy member contents */
  memcpy(dest, src, sizeof(rfac_iv));

  /* allocate memory for new data */
  dest->data = (rfac_iv_data *) malloc(dest->n_eng*sizeof(rfac_iv_data));

  if (dest->data == NULL)
  {
    free(dest);
    return;
  }

  /* copy data */
  memcpy(dest->data, src->data, dest->n_eng*sizeof(rfac_iv_data));
}

/*!
 * Frees an #rfac_ivcur structure from memory.
 *
 * \param[in] ivcur Pointer to #rfac_ivcur structure to free.
 */
void rfac_ivcur_free(rfac_ivcur *ivcur)
{
  if (ivcur == NULL) return;

  rfac_iv_free(ivcur->experimental);
  rfac_iv_free(ivcur->theory);

  free(ivcur);
  ivcur = NULL;
}

/*!
 * Frees an array of #rfac_ivcur structures from memory.
 *
 * \param[in] ivcur Pointer to #rfac_ivcur structure to free.
 * \warning \p ivcur array must be terminated with #END_OF_GROUP_ID for
 * the \c group_id member of the final #rfac_iv_cur in the array.
 */
void rfac_ivcur_free_all(rfac_ivcur *ivcur)
{
  size_t n=0, i;

  if (ivcur == NULL) return;
  while(ivcur->group_id != END_OF_GROUP_ID)
  {
    n++;
  }
  for (i=n; i > 0; i--) /* free curves in reverse order - possibly safer? */
      rfac_ivcur_free(&ivcur[i]);
  free(ivcur);
}

/*!
 * Converts @rfac_iv::data of \p iv into arrays of \p x and \p fx
 *
 * \param[in] iv \c struct containing IV data to convert.
 * \param[out] x pointer to array of energy values.
 * \param[out] fx pointer to array of intensity values.
 * \param[out] n pointer to \code unsigned int \endcode . This is the number of
 * elements in \p x and \p fx .
 *
 * \return integer representing success of function.
 * \retval #RFAC_SUCCESS on successful completion.
 * \retval #RFAC_ALLOC_ERROR if memory cannot be allocated for \p x or \p fx
 *
 * \warning memory will be reallocated if \p x or \p fx are not \c NULL when
 * passed to the function.
 *
 * \note arrays \p x and \p fx will not necessarily be terminated by
 * #F_END_OF_LIST
 */
int rfac_iv_to_arrays(const rfac_iv *iv, double *x, double *fx, size_t *n)
{
  size_t i;

  /* allocate memory */
  if (x != NULL) x = (double *) realloc(x, iv->n_eng);
  else x = (double *) malloc(iv->n_eng * sizeof(double));

  if (x == NULL) return (RFAC_ALLOCATION_ERROR);

  if (fx != NULL) fx = (double *) realloc(fx, iv->n_eng);
  else fx = (double *) malloc(iv->n_eng * sizeof(double));

  if (fx == NULL)
  {
    free(x);
    return (RFAC_ALLOCATION_ERROR);
  }

  /* convert energies */
  for (i=0; i<iv->n_eng; i++)
  {
    x[i] = iv->data[i].energy;
    fx[i] = iv->data[i].intens;
  }

  *n = iv->n_eng;

  return (RFAC_SUCCESS);
}
