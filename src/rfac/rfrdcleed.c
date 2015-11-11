/*********************************************************************
 *                       RFRDCLEED.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Changes:
 *   GH/1995.08.11 - Creation (copy from rfrdvhbeams.c)
 *   GH/2000.10.12 - bug fixed in comparing neng with number of lines.
 *
 ********************************************************************/

/*! \file
 *
 *  Implementation file for reading theoretical IV curves from CLEED program
 *  using rfac_iv_data_read_cleed() function.
 */

#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "rfac.h"       /* rf specific definitions */

static const size_t LENGTH_OF_NUMBER = 15 ;  /* characters per intensity in input file */

/*!
 * Read theoretical IV curves (one per geometry) for the beams
 * listed in index_list from buffer.
 *
 * The input file must be compatible with the output from CLEED:
 *
 * <comment line(s)    ("# "  followed by arbitrary comment) >
 * <number of energies ("#en" followed by an integer number) >
 * <number of beams    ("#bn" followed by an integer number) >
 * <beam indices       ("#bi" followed by an integer and two real numbers) >
 * <energy> <intensity> <intensity> ..... (all in one line)
 *
 * \warning The number of beams must appear before the the first beam
 * index is read.
 *
 * Simultaneously it will be checked if the input list is ordered and
 * equidistant according to energy.
 *
 * \param *iv_cur pointer to structure #rfac_ivcur. All available
 * information about the theoretical IV curve will be stored in this
 * structure. sort and equidistant flags will be set.
 *
 * \param buffer file buffer containing theoretical input.
 *
 * \param index_list command line for interpreter rfac_intindl() . Syntax:
 * (<index1>,<index2>) {*<scale> +/- (<index1>,<index2>)*<scale>}
 *
 * \return pointer to the IV curve (#rfac_iv). The list is
 * terminated by a pair of negative values.
 *
 * \retval \c NULL if failed.
 */
rfac_iv *rfac_iv_read_cleed(rfac_ivcur *iv_cur,
    char *buffer, char *index_list)
{

  int iaux;
  size_t i;
  size_t i_read, i_str;             /* counters */
  size_t i_eng, i_beam;             /* counters */
  size_t lines;
  size_t n_beam, n_eng;

  long int len, offs, data_offs;
  size_t buffer_len;

  real max_int,                    /* list of max. intensity */ 
       int_sum;                    /* sum of all intensities */

  char *line_buffer = NULL;
  char fmt_buffer[STRSZ];

  rfac_iv *iv = rfac_iv_init();
  rfac_iv_data *list = NULL;
  rfac_spot *beam = NULL;

  /********************************************************************
   * Read non-data input:
   * - skip comment.
   * - find number of energies in this file;
   * - find number of beams in this file;
   * - read indices.
   ********************************************************************/

  if (iv == NULL)
  {
    ERROR_MSG("failed to allocate theoretical iv\n");
    exit(ENOMEM);
  }

  /* Allocate STRSZ bytes for line_buffer to read non-data input */
  line_buffer = (char *)calloc(STRSZ+1, sizeof(char));

  /* Reset n_beam, e_eng */
  n_beam = 0;
  n_eng = 0;
  beam = NULL;
  list = NULL;

  i_read = 0;

  offs = data_offs = 0L;

  /* Read first comment line */
  offs += (long int)bgets(buffer, offs, STRSZ, line_buffer);

  /* Check the current line for usable information */
  while( *line_buffer == '#')
  {
 
    /* Read number of beams and allocate memory for list beam */
    if(strncmp(line_buffer+1, "bn", 2) == 0)
    {
      sscanf(line_buffer+3, "%u", &n_beam);
      beam = (rfac_spot *) calloc( n_beam+1, sizeof(rfac_spot));
     
      if( beam == NULL )
      {
        ERROR_MSG("allocation error (beam)\n");
        exit(ENOMEM);
      }
      
    } /* bn */

    /* Read beam indices */
    if(strncmp(line_buffer+1, "bi", 2) == 0)
    {
      if( beam == NULL)
      {
        ERROR_MSG("attempt to read beam indices into unallocated list\n");
        exit(ENOMEM);
      }

      sscanf(line_buffer+3, "%u", &i_beam);
      if(i_beam < n_beam)
      {
        sprintf(fmt_buffer, "%%d %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
        sscanf(line_buffer+3, fmt_buffer,
               &iaux,                        /* not used */
               &beam[i_beam].index1,         /* beam indices */
               &beam[i_beam].index2 );
        i_read ++;

        CONTROL_MSG(CONTROL, "\t%d:\t(%5.2f,%5.2f)\n",
                i_beam, beam[i_beam].index1, beam[i_beam].index2 );
      }
      else
      {
        ERROR_MSG("beam index exceeds limit (%d/%d)\n", i_beam, n_beam);
        exit(ENOMEM);
      }
      
    } /* bi */

    /* Read number of energies */
    if(strncmp(line_buffer+1, "en", 2) == 0)
    {
      sscanf(line_buffer+3, "%u", &n_eng);
    } /* en */

    /* Read next line */
    data_offs = offs;
    offs += (long int)bgets(buffer, offs, STRSZ, line_buffer);

  } /* while *line_buffer == "#" */

  /********************************************************************
   * At this point all necessary information should be known
   * - Check some of the numbers
   * - Call cr_intindl
   * - Allocate memory for list
   ********************************************************************/

  if (i_read != n_beam)
  {
    ERROR_MSG("numbers of beams do not match (read: %d/n_beam: %d)\n",
              i_read, n_beam);
    exit(EINVAL);
  }

  /* Changed 12.10.00: if( (lines = rfac_nclines(buffer+offs) ) != n_eng) */
  if( (lines = rfac_nclines(buffer+data_offs) ) != n_eng)
  {
    ERROR_MSG("numbers of energies do not match (lines: %d/n_eng: %d)\n",
              lines, n_eng);
    exit(EINVAL);
  }

  rfac_intindl(index_list, beam, n_beam);

  /*
   * Allocate list
   * number of IV curves = number of data sets per energy (n_geo = 1)
   * length of one IV curve = n_eng
   */
  list = (rfac_iv_data *) calloc((n_eng+1), sizeof(rfac_iv_data));
  
  if( list == NULL)
  {
    ERROR_MSG("allocation error (list) \n");
    exit(ENOMEM);
  }

  /********************************************************************
   * Write energy intensity pairs to list
   ********************************************************************/

  /* Allocate enough memory for line_buffer */
  free(line_buffer);
  buffer_len = n_beam * LENGTH_OF_NUMBER;
  line_buffer = (char *)calloc(buffer_len, sizeof(char));

  CONTROL_MSG(CONTROL, "start reading intensities.\n");
  CONTROL_MSG(CONTROL, "%d bytes for line_buffer.\n", n_beam*LENGTH_OF_NUMBER);

  /* preset some values */
  iv->sort = true;              /* reset sort flag */
  iv->equidist = true;          /* reset equidistance flag */

  max_int = 0.;
  int_sum = 0.;
  i_eng = 0;

  for ( offs = data_offs ;
      ((len = bgets(buffer, offs, (long)buffer_len, line_buffer)) > -1) &&
      (i_eng < n_eng);  
      offs += (long)len)
  {
    if(line_buffer[0] != '#')      /* no comment */
    {
      CONTROL_MSG(CONTROL, "i_eng = %d ", i_eng);

      /* Read energy first, then intensities. */
      i_str = 0;
      sprintf(fmt_buffer, "%%%sf", CLEED_REAL_FMT);
      sscanf(line_buffer+i_str, fmt_buffer, &(list[i_eng].energy) );
      
      /* go to end of line ? */
      while(line_buffer[i_str] == ' ') i_str++;
      while(line_buffer[i_str] != ' ') i_str++;
      
#     if CONTROL_X
      fprintf(STDCTR, "e: %f i:", list[i_eng].energy);
#     endif
  
      sprintf(fmt_buffer, "%%%sf", CLEED_REAL_FMT);
      for( i_beam = 0; (i_beam <= n_beam) &&
           (sscanf(line_buffer+i_str, fmt_buffer,
             &(beam[i_beam].f_val2) ) != EOF);
           i_beam++)
      {
        while(line_buffer[i_str] == ' ') i_str++;
        while(line_buffer[i_str] != ' ') i_str++;
      
#       if CONTROL_X
        fprintf(STDCTR, "%f ", beam[i_beam].f_val2);
#       endif
      }
      
#     ifdef CONTROL_X
      fprintf(STDCTR, "\n");
#     endif
     
      /* calculate average intensities and check maximum */
      list[i_eng].intens = 0.;
      for (i=0; i < n_beam; i ++)
      {
        list[i_eng].intens +=  beam[i].f_val1 *        /* scaling factor */
                               beam[i].f_val2 ;        /* intensity */
      }

      CONTROL_MSG(CONTROL, "eng: %f int: %f\n", list[i_eng].energy,
                  list[i_eng].intens);

      if ( list[i_eng].intens > max_int )
        max_int = list[i_eng].intens;

      /* Increment i_eng if integrated intensity is non-zero */
      int_sum += list[i_eng].intens;

      if( int_sum > ZERO_TOLERANCE)
      {
        /* check if list is sorted */
        if( (i_eng > 0) && (list[i_eng].energy < list[i_eng-1].energy) )
         iv->sort = false;

        /* check equidistance */
        if( (i_eng > 1) &&
           (cleed_real_fabs((2*list[i_eng-1].energy - 
              list[i_eng].energy - list[i_eng-2].energy)) >  ENG_TOLERANCE ))
        {
          iv->equidist = false;

          WARNING_MSG("theoretical input is not equidistant (Eng:%.1f)\n",
                      list[i_eng-1].energy);
        }
        i_eng++;
 
      }  /* if int_sum > ZERO_TOLERANCE */
      else int_sum = 0.;
      
    }  /* if *linebuffer != '#' */
    
  }  /* for i_eng */

  CONTROL_MSG(CONTROL, "1st/last eng(%d): %.1f/%.1f ",
              i_eng, list[0].energy, list[i_eng-1].energy);

  /* Write all available information to structure iv_cur */
  iv->data = list;
  iv->n_eng = i_eng;
  iv->first_eng = list[0].energy;
  iv->last_eng = list[i_eng-1].energy;
  iv->max_int = max_int;
 
  /*
   * Find beam with maximum contribution to average.
   * This beam will be used as spot ID.
   */
  iv_cur->spot_id.f_val1 = cleed_real_fabs(beam[0].f_val1);
  iv_cur->spot_id.i_val1 = 0;

  for (i_beam=1; i_beam < n_beam; i_beam ++)
  {
    if (cleed_real_fabs(beam[i_beam].f_val1) > iv_cur->spot_id.f_val1 )
    {
      iv_cur->spot_id.f_val1 = cleed_real_fabs(beam[i_beam].f_val1);
      iv_cur->spot_id.i_val1 = (int)i_beam;
    }
  }
 
  iv_cur->spot_id.index1 = beam[iv_cur->spot_id.i_val1].index1;
  iv_cur->spot_id.index2 = beam[iv_cur->spot_id.i_val1].index2;
  free(beam);

  CONTROL_MSG(CONTROL, "spot_id: (%5.2f,%5.2f)\n",
          iv_cur->spot_id.index1, iv_cur->spot_id.index2);
  
  /* set last energy and intensities to termination value */
  list[i_eng].energy = (real) F_END_OF_LIST;
  list[i_eng].intens = (real) F_END_OF_LIST;

  free(line_buffer);



  return(iv);
}  /* end of function cr_rdcleed */
