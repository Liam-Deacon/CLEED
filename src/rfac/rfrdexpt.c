/*********************************************************************
 *                       RFRDEXPT.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.11.11 - creation.
 ********************************************************************/

/*! \file
 *  \brief Implementation of rfac_iv_read() function.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "rfac.h"       /* rf specific definitions */

/*!
 * Read experimental IV curve from file specified by filename and check if
 * energy values are equidistant and sorted.
 *
 * The order of the input file must be:
 * <energy> <intensity>
 * Comments are indicated by a '#'.
 *
 * \param[in] filename string containing the name of the file where the
 * input should be read from.
 *
 * \return pointer to the IV curve (#rfac_iv). The IV list is terminated
 * by a pair (energy, intensity) of negative values.
 *
 * \retval \c NULL if failed.
 */
rfac_iv_data *rfac_iv_read(const char *filename)
{

  size_t i;
  size_t lines;
  long offs, len;
  real max_int;

  char line_buffer[STRSZ];
  char *buffer;
  char fmt_buffer[STRSZ];
  rfac_iv *iv = rfac_iv_init();

  /*
   * copy input file to buffer
   * count lines  without comments (i.e. number of data pairs)
   * allocate memory for expt. IV curve
   */

  sprintf(fmt_buffer, "%%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rdexpt): opening \"%s\"\n", filename);
  #endif

  if( (buffer = file2buffer(filename)) == NULL)
  {
    ERROR_MSG("error in function file2buffer \n");
    return(NULL);
  }
  lines = rfac_nclines(buffer);

  CONTROL_MSG(CONTROL_X, "file content:\n%s\n", buffer);
  CONTROL_MSG(CONTROL_X, "lines = %d\n", lines);

  if (iv->data != NULL) free(iv->data);
  iv->data = (rfac_iv_data *) malloc((lines+1)* sizeof(rfac_iv_data));
  if( iv->data == NULL )
  {
    ERROR_MSG("allocation error (list) \n");
    exit(RFAC_ALLOCATION_ERROR);
  }

  CONTROL_MSG(CONTROL_X, "after malloc\n");

  /* write energy pairs to list */
  for ( i = 0, offs = 0L, max_int = 0.,
       iv->equidist = true , iv->sort = true ;
      ((len = bgets(buffer, offs, STRSZ, line_buffer))+1 ) /* orig. len > -1 */
      && (i < lines);  offs += len)
  {
    
    if(line_buffer[0] != '#')      /* no comment */
    {

      /* print eV[i] I[i] */
      CONTROL_MSG(CONTROL_X, "(%s): %e %e\n", filename,
                  iv->data[i].energy, iv->data[i].intens);

      if(sscanf(line_buffer, fmt_buffer, &iv->data[i].energy, &iv->data[i].intens) == 2)
      {

        /* search for max. intensity */
        if(iv->data[i].intens > max_int) max_int = iv->data[i].intens;

        /* check if list is sorted */
        if( (i > 0) && (iv->data[i].energy < iv->data[i-1].energy) )
        {
          iv->sort = false;
        }

        /* check equidistance */
        if( (i > 1) &&
            (cleed_real_fabs ( (2*iv->data[i-1].energy - iv->data[i].energy -
                       iv->data[i-2].energy)) >  ENG_TOLERANCE ) )
        {
          iv->equidist = 0;
          WARNING_MSG("\"%s\" is not equidistant (No:%d)\n", filename, i-1);
        }

        /* increment i */
        i++;

      } /* if sscanf */

    } /* if line_buffer[0] != '#' */

  } /* for */

  if (i==0)
  {
    WARNING_MSG("No input read from file \"%s\"\n", filename);
  }
  else
  {
    CONTROL_MSG(CONTROL, "last pair(%d): %f %f\n",
                 i-1, iv->data[i-1].energy, iv->data[i-1].intens);
  }

  if(iv->equidist == true)
  {
    CONTROL_MSG(CONTROL, "expt. IV curve is equidistant\n");
  }
  else
  {
    CONTROL_MSG(CONTROL, "expt. IV curve is not equidistant\n");
  }

  /* write available information to structure iv_cur. */
  iv->first_eng = iv->data[0].energy;
  iv->last_eng = iv->data[i-1].energy;
  iv->max_int = max_int;

  CONTROL_MSG(CONTROL, "iv_cur structure written\n");
 
  /* set last energy and intensities to termination value */
  iv->data[i].energy = (real) F_END_OF_LIST;  /* termination */
  iv->data[i].intens = (real) F_END_OF_LIST;  /* termination */
  iv->n_eng = i;

  CONTROL_MSG(CONTROL, "list terminations set\n");

  /*
   free previously allocated memory;
   return;
  */
  free(buffer);

  return(iv->data);
}
