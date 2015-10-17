/*********************************************************************
 *                        REFINP.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   SU/17.04.91
 *   GH/05.08.92
 *   CS/11.08.93
 *********************************************************************/

/*! \file
 *
 * Contains mkiv_ref_inp() function for reading reference spot indices and positions.
 */

#include "mkiv.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
/*!
 * Reads the indices and positions of (at least 3) reflexes from standard input.
 *
 * \param[out] naux Pointer to number of reflexes in \p aux
 * \param[out] aux Pointer to #mkiv_reflex array which stores the indices and
 * positions of spots.
 * \param verb Flag for verbosity level of output.
 * \param[in] pos_file File path to *.pos file.
 * 
 * \return C style return code indication function success.
 * \retval 0 if successful.
 */
int mkiv_ref_inp(size_t *naux, mkiv_reflex *aux, int verb, const char *pos_file)
{
  size_t i;
  FILE *in_out;
  char file_path[FILENAME_MAX] = "mkiv.pos";
  char line_buffer[STRSZ];

  if (pos_file == NULL) strcpy(file_path, pos_file);

  fprintf(stdout, "Enter number of spots for determination of basis (max. 5):\n");
  fprintf(stdout, "\t<num>:\tinput through terminal\n");
  fprintf(stdout, "\tr:\tformer input will be read \n\t\tfrom file '%s'\n",
         file_path);
  fgets(line_buffer, STRSZ, stdin);

  if ( (verb & QUICK) || ( !(verb & QUICK) && *line_buffer == 'r' ) )
  {
    if ((in_out = fopen(file_path, "r")) == NULL)
    {
      ERROR_MSG("unable to read from file '%s'\n", file_path);
      ERROR_RETURN(-1);
    }
    else
    {
      fprintf(stdout, "Reading reference spot positions from '%s'\n",
              file_path);
    }

    fscanf(in_out, "%d\n", naux);
    if (QQ(verb)) printf("\t%3d reflexes:\n", *naux);
    for (i=0; i < *naux; i++)
    {
      fscanf(in_out, "%lf %lf\n", &(aux[i].lind1), &(aux[i].lind2) );
      fscanf(in_out, "%lf %lf\n", &(aux[i].xx), &(aux[i].yy) );
      if (QQ(verb)) printf("\t(%4.2f,%4.2f) at\t(%3.0f,%3.0f)\n",
                aux[i].lind1, aux[i].lind2, aux[i].xx, aux[i].yy );
    }
  }
  else
  {
    sscanf(line_buffer, "%d", naux);
    if ((in_out = fopen(file_path, "w")) == NULL)
    {
      ERROR_MSG("unable to '%s' for writing\n", file_path);
      ERROR_RETURN(-1);
    }
    fprintf(in_out, "%d\n", *naux);
    for (i=0; i < *naux; i++)
    {
      fprintf(stdout, " Enter indices for one reflex:\n");
      fscanf(stdin, "%lf%lf", &(aux[i].lind1), &(aux[i].lind2) );
      fprintf(in_out, "%lf %lf\n", aux[i].lind1, aux[i].lind2 );
      fprintf(stdout, " Enter horizontal and vertical components "
              "of reflex position;\n");
      scanf("%lf%lf", &(aux[i].xx), &(aux[i].yy) );
      fprintf(in_out, "%lf %lf\n", aux[i].xx, aux[i].yy );
    }
  }
  fclose(in_out);

  return(0);
}
