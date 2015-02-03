/*********************************************************************
 *                       RFINTINDL.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:  Program calculates different R factors.
 *
 * Changes:
 *   GH/1992.09.29 - Creation
 *   GH/1995.08.30 - Adaption to CRFAC
 *   GH/1997.08.30 - type = void
 ********************************************************************/

/*! \file
 *  \brief Implementation of rfac_intindl() function.
 */

#include <stdio.h>
#include <strings.h>    /* for strlen */
#include <string.h>
#include <math.h>

#include "rfac.h"       /* rf specific definitions */

/*!
 * Interpreter for averaging different beams from theoretical input
 * file.
 *
 * A scaling factor will be assigned to beams which should be averaged
 * (scaling factor : beam.f_val1).
 *
 * Syntax:
 *  (<index1>,<index2>) {*<scale> +/- (<index1>,<index2>) * <scale> }
 *
 * \param[in] command_line
 *
 * \param[in,out] *beam list of beam indices from input file.
 * The output, beam.f_val1, is a scaling factor for averging over beams.
 * Can be 0. (ignore) or a number.
 *
 * \param[in] n_beam number of beams
 *
 */
void rfac_intindl(char *command_line, rfac_spot *beam, size_t n_beam)
{
  size_t i, j;

  real test1, test2;
  real scale;
  char fmt_buffer[STRSZ];

  sprintf(fmt_buffer, "%%%sf", CLEED_REAL_FMT);

  /* reset all scaling factors in list beam */
  for (j=0; j < n_beam; j++)
  {
    (beam+j)->f_val1 = 0.;
  }

  i = 0;
  scale = 1.0;
  test1 = (real) F_END_OF_LIST;
  test2 = (real) F_END_OF_LIST;

  do
  {
    while ( (command_line[i] != '(') && (command_line[i] != '\0') )
	  i++;

    /* index */
    if(command_line[i] == '(')
    {
      i++;
      sscanf(command_line+i, fmt_buffer, &test1);
      while (command_line[i] != ',') i++;
      i++;
      sscanf(command_line+i, fmt_buffer, &test2);
      while (command_line[i] != ')') i++;
      i++;
    }

    while ( (command_line[i] != '+') && (command_line[i] != '-') &&
            (command_line[i] != '*') && (command_line[i] != '\0') )
    {
      i++;
    }

    /* postscript scale factor */
    if(command_line[i] == '*')
    {
      i++;
      if(scale < 0. )
      {
        sscanf(command_line+i, fmt_buffer, &scale);
      }
      else sscanf(command_line+i, fmt_buffer, &scale);

      /* skip over possible signs of scaling factor */
      while ( (command_line[i] < '0') || (command_line[i] > '9') ) i++;
    }

    /* check compatibility with input indices and clear scale. */
    for (j=0; ( ( R_fabs( (beam+j)->index1 - test1) +
                R_fabs( (beam+j)->index2 - test2) ) > IND_TOLERANCE) 
	                && (j < n_beam); j++);
    {
      if (j < n_beam)
      {
        (beam+j)->f_val1 = scale;
        #ifdef PRINT  /* control output, if required */
        printf(">rfac_intindl: beam:(%5.2f,%5.2f) scale: %5.2f\n",
               (beam+j)->index1, (beam+j)->index2, (beam+j)->f_val1);
        #endif
      }
      else if(test1 > (real) F_END_OF_LIST)
        printf(">rfac_intindl: Could not find (%5.2f,%5.2f) in input file\n",
               test1, test2);
      else
        printf(">rfac_intindl: Error in commandline\n");
    }

    /* reset scale and test1/2 for next beam */
    test1 = (real) F_END_OF_LIST;
    test2 = (real) F_END_OF_LIST;
    scale = 1.0;

    while ( (command_line[i] != '+') && (command_line[i] != '-') &&
            (command_line[i] != '\0') ) i++;

    /* signs */
    if(command_line[i] == '+')
    {
      i++;
    }

    if(command_line[i] == '-')
    {
      scale = -1.;
      i++;
    }

  }  /* do */

  while(i < strlen(command_line) );

}
