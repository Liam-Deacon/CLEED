/*
 * rfac.c
 *
 *  Created on: 10 Oct 2014
 *      Author: Liam Deacon
 */

#include "rfac.h"
#include <stdbool.h>
#include <stdlib.h>

/*!
 * \fn rfac_iv_init
 *
 * \brief intialize \ref rfac_iv object.
 *
 * \return pointer to \ref rfac_iv object.
 *
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
 * \fn rfac_iv_alloc
 *
 * \brief allocate memory for \ref rfac_iv with \p n_eng data points for
 * \ref rfac_iv_data member.
 *
 * \return pointer to \ref rfac_iv object.
 *
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
 * \fn rfac_iv_free
 *
 * \brief free \ref rfac_iv object from memory.
 */
void rfac_iv_free(rfac_iv *_iv)
{
  if (_iv != NULL)
  {
    if (_iv->data != NULL) free(_iv->data);
    free(_iv);
  }
}
