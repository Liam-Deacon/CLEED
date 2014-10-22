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
 *  \brief Implementation of rf_rdexpt() function.
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
 * \param[in,out] iv_cur pointer to structure #rfac_iv. All available
 * information about the theoretical IV curve will be stored in this
 * structure.
 *
 * \param[in] filename string containing the name of the file where the
 * input should be read from.
 *
 * \return pointer to the IV curve (#rfac_iv). The IV list is terminated
 * by a pair (energy, intensity) of negative values.
 *
 * \retval \c NULL if failed.
 */
rfac_iv_data *rf_rdexpt(rfac_ivcur *iv_cur, const char *filename )
{

  size_t i;
  size_t lines, len;
  long offs;
  real max_int;

  char line_buffer[STRSZ];
  char *buffer;
  rfac_iv *iv = rfac_iv_init();

  /*
   * copy input file to buffer
   * count lines  without comments (i.e. number of data pairs)
   * allocate memory for expt. IV curve
   */
  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rdexpt): opening \"%s\"\n", filename);
  #endif

  if( (buffer = file2buffer(filename)) == NULL)
  {
    #ifdef ERROR
    fprintf(STDERR, " *** error (cr_rdexpt): error in function file2buffer \n");
    #endif
    return(NULL);
  }
  lines = rf_nclines(buffer);

  #ifdef CONTROL_X
  fprintf(STDCTR, "(cr_rdexpt): file content:\n%s\n", buffer);
  fprintf(STDCTR, "(cr_rdexpt): lines = %d\n", lines);
  #endif

  if (iv->data != NULL) free(iv->data);
  iv->data = (rfac_iv_data *) malloc((lines+1)* sizeof(rfac_iv_data));
  if( iv->data == NULL )
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (cr_rdexpt): allocation error (list) \n");
    #endif
    exit(RFAC_ALLOCATION_ERROR);
  }

  #ifdef CONTROL_X
  fprintf(STDCTR, "(cr_rdexpt): after malloc\n");
  #endif

  /* write energy pairs to list */
  for ( i = 0, offs = 0L, max_int = 0.,
       iv->equidist = true , iv->sort = true ;
      ((len = bgets(buffer, offs, STRSZ, line_buffer))+1 ) /* orig. len > -1 */
      && (i < lines);  offs += (long)len)
  {
    
    if(line_buffer[0] != '#')      /* no comment */
    {

      #ifdef CONTROL_XX
      /* print eV[i] I[i] */
      fprintf(STDCTR, "(cr_rdexpt): (%s): %e %e\n", filename,
              iv.data[i].energy, iv.data[i].intens);
      #endif

      #ifdef REAL_IS_DOUBLE
      if(sscanf(line_buffer, "%lf %lf", &iv.data[i].energy,
                &iv.data[i].intens) == 2)
      #endif
      #ifdef REAL_IS_FLOAT
      if(sscanf(line_buffer, "%f %f", &iv->data[i].energy,
                &iv->data[i].intens) == 2)
      #endif
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
            (R_fabs ( (2*iv->data[i-1].energy - iv->data[i].energy -
                       iv->data[i-2].energy)) >  ENG_TOLERANCE ) )
        {
          iv->equidist = 0;
          #ifdef WARNING
          fprintf(STDWAR, "*** warning (cr_rdexpt):"
                    " \"%s\" is not equidistant (No:%d)\n", filename, i-1);
          #endif
        }

        /* increment i */
        i++;

      } /* if sscanf */

    } /* if line_buffer[0] != '#' */

  } /* for */

  if (i==0)
  {
    printf("(cr_rdexpt): *** No input read from file \"%s\"\n", filename);
  }
  #ifdef CONTROL
  else
  {
    fprintf(STDCTR, "(cr_rdexpt): last pair(%d): %f %f\n",
                 i-1, iv->data[i-1].energy, iv->data[i-1].intens);
  }

  if(iv->equidist == true)
  {
    fprintf(STDCTR, "(cr_rdexpt): expt. IV curve is equidistant\n");
  }
  else
  {
    fprintf(STDCTR, "(cr_rdexpt): expt. IV curve is not equidistant\n");
  }
  #endif /* CONTROL */

  /* write available information to structure iv_cur. */
  iv->first_eng = iv->data[0].energy;
  iv->last_eng = iv->data[i-1].energy;
  iv->max_int = max_int;

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rdexpt): iv_cur structure written\n");
  #endif
 
  /* set last energy and intensities to termination value */
  iv->data[i].energy = FEND_OF_LIST;  /* termination */
  iv->data[i].intens = FEND_OF_LIST;  /* termination */
  iv->n_eng = i;

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rdexpt): list terminations set\n");
  #endif

  /*
   free previously allocated memory;
   return;
  */
  free(buffer);

  return(iv->data);
}
