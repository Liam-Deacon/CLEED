/*********************************************************************
 *                           RFAC.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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

#include "rfac.h"
#include <stdbool.h>
#include <stdlib.h>

/*!
 * \brief intialize #rfac_iv object.
 *
 * \return pointer to #rfac_iv object.
 * \retval NULL if memory cannot be allocated.
 */
rfac_iv *rfac_iv_init()
{
  rfac_iv *_iv = (rfac_iv *) malloc(sizeof(rfac_iv));

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

/*!
 * Free #rfac_iv object instance from memory.
 */
void rfac_iv_free(rfac_iv *_iv)
{
  if (_iv != NULL)
  {
    if (_iv->data != NULL) free(_iv->data);
    free(_iv);
  }
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
