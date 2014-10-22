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
 *  using rf_rdcleed() function.
 */

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "rfac.h"       /* rf specific definitions */

#define LENGTH_OF_NUMBER 15   /* > number of characters per intensity in
                                 input file */

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
 * \param index_list command line for interpreter rf_intindl() . Syntax:
 * (<index1>,<index2>) {*<scale> +/- (<index1>,<index2>)*<scale>}
 *
 * \return pointer to the IV curve (#rfac_iv_data). The list is
 * terminated by a pair of negative values.
 *
 * \retval \c NULL if failed.
 */
rfac_iv_data *rf_rdcleed(rfac_ivcur *iv_cur, char *buffer, char *index_list)
{

  int iaux;
  size_t i;
  size_t i_read, i_str;             /* counters */
  size_t i_eng, i_beam;             /* counters */
  size_t lines, len;
  size_t n_beam, n_eng;

  long int offs, data_offs;
  size_t buffer_len;

  real max_int,                    /* list of max. intensity */ 
       int_sum;                    /* sum of all intensities */

  char *line_buffer;

  rfac_iv_data *list;
  rfac_spot *beam;

/********************************************************************
 * Read non-data input:
 * - skip comment.
 * - find number of energies in this file;
 * - find number of beams in this file;
 * - read indices.
 ********************************************************************/

  /* Allocate STRSZ bytes for line_buffer to read non-data input */
  line_buffer = (char *)malloc( STRSZ * sizeof(char) + 1 );

  /* Reset n_beam, e_eng */
  n_beam = 0;
  n_eng = 0;
  beam = NULL;
  list = NULL;

  i_read = 0;

  offs = data_offs = 0L;

  /* Read first comment line */
  offs += (long int)bgets(buffer, offs, STRSZ, line_buffer);

/************************************************
 * Check the current line for usable information
 ************************************************/
  while( *line_buffer == '#')
  {
 
    /* Read number of beams and allocate memory for list beam */
    if(strncmp(line_buffer+1, "bn", 2) == 0)
    {
      sscanf(line_buffer+3, "%u", &n_beam);
      beam = (rfac_spot *) malloc( (n_beam+1)*sizeof(rfac_spot)*13);
     
      if( beam == NULL )
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (cr_rdcleed): allocation error (beam)\n");
        #endif
        exit(RFAC_ALLOCATION_ERROR);
      }
      
    } /* bn */

    /* Read beam indices */
    if(strncmp(line_buffer+1, "bi", 2) == 0)
    {
      if( beam == NULL)
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (cr_rdcleed): "
            "attempt to read beam indices into unallocated list\n");
        #endif
        exit(RFAC_ALLOCATION_ERROR);
      }

      sscanf(line_buffer+3, "%u", &i_beam);
      if(i_beam < n_beam)
      {
        #ifdef REAL_IS_DOUBLE
        sscanf(line_buffer+3, "%d %lf %lf",
        #endif
        #ifdef REAL_IS_FLOAT
        sscanf(line_buffer+3, "%d %f %f",
        #endif
              &iaux,                        /* not used */
              &beam[i_beam].index1,         /* beam indices */
              &beam[i_beam].index2 );
        i_read ++;

        #ifdef CONTROL
        fprintf(STDCTR, "(cr_rdcleed):\t%d:\t(%5.2f,%5.2f)\n",
                i_beam, beam[i_beam].index1, beam[i_beam].index2 );
        #endif
      }
      else
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (cr_rdcleed): "
                "beam index exceeds limit (%d/%d)\n", i_beam, n_beam);
        #endif
        exit(RFAC_ALLOCATION_ERROR);
      }
      
    } /* bi */

    /* Read number of energies */
    if(strncmp(line_buffer+1, "en", 2) == 0)
    {
      sscanf(line_buffer+3, "%u", &n_eng);
    } /* en */

/************************************************
 * Read next line
 ************************************************/

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
    #ifdef ERROR
    fprintf(STDERR,
      "*** error (cr_rdcleed): numbers of beams do not match "
      "(read: %d/n_beam: %d)\n", i_read, n_beam);
    #endif
    exit(RFAC_INVALID_NUMBER_OF_BEAMS);
  }

  /* Changed 12.10.00: if( (lines = rf_nclines(buffer+offs) ) != n_eng) */
  if( (lines = rf_nclines(buffer+data_offs) ) != n_eng)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (cr_rdcleed): "
            "numbers of energies do not match (lines: %d/n_eng: %d)\n",
            lines, n_eng);
    #endif
    exit(RFAC_INVALID_NUMBER_OF_ENERGIES);
  }

  rf_intindl(index_list, beam, n_beam);

  /*
   * Allocate list
   * number of IV curves = number of data sets per energy (n_geo = 1)
   * length of one IV curve = n_eng
   */
  list = (rfac_iv_data *) malloc((n_eng+1)*sizeof(rfac_iv_data)*13);
  
  if( list == NULL)
  {
    #ifdef ERROR   /* error output */
    fprintf(STDERR, "*** error (cr_rdcleed): allocation error (list) \n");
    #endif
    exit(RFAC_ALLOCATION_ERROR);
  }

/********************************************************************
 * Write energy intensity pairs to list
 ********************************************************************/

  /* Allocate enough memory for line_buffer */
  free(line_buffer);
  buffer_len = n_beam * LENGTH_OF_NUMBER;
  line_buffer = (char *)malloc(buffer_len * sizeof(char)*13);

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rdcleed): start reading intensities.\n");
  fprintf(STDCTR, "(cr_rdcleed): %d bytes for line_buffer.\n", n_beam * 15);
  #endif

  /* preset some values */
  iv_cur->theory->sort = true;              /* reset sort flag */
  iv_cur->theory->equidist = true;          /* reset equidistance flag */

  max_int = 0.;
  int_sum = 0.;
  i_eng = 0;
  
  for ( offs = data_offs ;
      ((len = bgets(buffer, offs, buffer_len, line_buffer)) > -1) && 
      (i_eng < n_eng);  
      offs += (long)len)
  {
    if(line_buffer[0] != '#')      /* no comment */
    {
      #ifdef CONTROL
      fprintf(STDCTR, "(cr_rdcleed): i_eng = %d ", i_eng);
      #endif

/************************************************
 * Read energy first, then intensities.
 ************************************************/
      i_str = 0;
      #ifdef REAL_IS_DOUBLE
      sscanf(line_buffer+i_str, "%lf", &(list[i_eng].energy) );
      #endif
      #ifdef REAL_IS_FLOAT
      sscanf(line_buffer+i_str, "%f", &(list[i_eng].energy) );
      #endif
      
      /* go to end of line ? */
      while(line_buffer[i_str] == ' ') i_str++;
      while(line_buffer[i_str] != ' ') i_str++;
      
      #ifdef CONTROL_X
      fprintf(STDCTR, "e: %f i:", list[i_eng].energy);
      #endif
  
      for( i_beam = 0; 
          (i_beam <= n_beam) && 
          #ifdef REAL_IS_DOUBLE
          (sscanf(line_buffer+i_str,"%lf", &(beam[i_beam].f_val2) ) != EOF);
          #endif
          #ifdef REAL_IS_FLOAT
          (sscanf(line_buffer+i_str,"%f", &(beam[i_beam].f_val2) ) != EOF);
          #endif
          i_beam++)
      {
        while(line_buffer[i_str] == ' ') i_str++;
        while(line_buffer[i_str] != ' ') i_str++;
      
        #ifdef CONTROL_X
        fprintf(STDCTR, "%f ", beam[i_beam].f_val2);
        #endif
      }
      
      #ifdef CONTROL_X
      fprintf(STDCTR, "\n");
      #endif
     
      /* calculate average intensities and check maximum */
      list[i_eng].intens = 0.;
      for (i=0; i < n_beam; i ++)
      {
        list[i_eng].intens += 
            beam[i].f_val1 *        /* scaling factor */
            beam[i].f_val2 ;        /* intensity */
      }

      #ifdef CONTROL
      fprintf(STDCTR, "eng: %f int: %f\n", list[i_eng].energy, 
           list[i_eng].intens);
      #endif

      if ( list[i_eng].intens > max_int )
        max_int = list[i_eng].intens;

/******************************************************
 * Increment i_eng if integrated intensity is non-zero
 ******************************************************/
      int_sum += list[i_eng].intens;

      if( int_sum > ZERO_TOLERANCE)
      {
        /* check if list is sorted */
        if( (i_eng > 0) && (list[i_eng].energy < list[i_eng-1].energy) )
         iv_cur->theory->sort = false;

        /* check equidistance */
        if( (i_eng > 1) &&
           (R_fabs((2*list[i_eng-1].energy - 
              list[i_eng].energy - list[i_eng-2].energy)) >  ENG_TOLERANCE ))
        {
          iv_cur->theory->equidist = false;

          #ifdef WARNING
          fprintf(STDWAR, "* warning (cr_rdcleed): "
                  "theor. input is not equidistant (Eng:%.1f)\n", 
                  list[i_eng-1].energy);
          #endif
        }
        i_eng++;
 
      }  /* if int_sum > ZERO_TOLERANCE */
      else 
      {
        int_sum = 0.;
      }
      
    }  /* if *linebuffer != '#' */
    
  }  /* for i_eng */

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rdcleed): 1st/last eng(%d): %.1f/%.1f ",
          i_eng, list[0].energy, list[i_eng-1].energy);
  #endif

  /* Write all available information to structure iv_cur */
  iv_cur->theory->n_eng = i_eng;
  iv_cur->theory->first_eng = list[0].energy;
  iv_cur->theory->last_eng = list[i_eng-1].energy;
  iv_cur->theory->max_int = max_int;
 
  /*
   * Find beam with maximum contribution to average.
   * This beam will be used as spot ID.
   */
  iv_cur->spot_id.f_val1 = R_fabs(beam[0].f_val1);
  iv_cur->spot_id.i_val1 = 0;

  for (i_beam=1; i_beam < n_beam; i_beam ++)
  {
    if (R_fabs(beam[i_beam].f_val1) > iv_cur->spot_id.f_val1 )
    {
      iv_cur->spot_id.f_val1 = R_fabs(beam[i_beam].f_val1);
      iv_cur->spot_id.i_val1 = i_beam;
    }
  }
 
  iv_cur->spot_id.index1 = beam[iv_cur->spot_id.i_val1].index1;
  iv_cur->spot_id.index2 = beam[iv_cur->spot_id.i_val1].index2;
  free(beam);

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rdcleed): spot_id: (%5.2f,%5.2f)\n",
          iv_cur->spot_id.index1, iv_cur->spot_id.index2);
  #endif
  
  /* set last energy and intensities to termination value */
  list[i_eng].energy = F_END_OF_LIST;
  list[i_eng].intens = F_END_OF_LIST;

  free(line_buffer);

  return(list);

}  /* end of function cr_rdcleed */
/********************************************************************/
