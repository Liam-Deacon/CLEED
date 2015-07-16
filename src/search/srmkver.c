/*********************************************************************
 *													srsxbkup.c
 *
 *  Copyright (C) 2015 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*!
 * \file srsxbkup.c
 * \author Liam Deacon
 * \date 17 Mar 2015
 */
#include <stdio.h>
#include <strings.h>
#include <time.h>

#include "copy_file.h"
#include "search.h"

extern char *sr_project;

/*!
 * Writes the current state of the simplex search to a backup file.
 *
 * \param y Pointer to the current search vector.
 * \param p Pointer to the matrix of search parameters for the simplex vertices.
 * \param ndim Number of parameters in the search.
 * \return C code representing function success.
 * \retval \c 0 if successful.
 */
int sr_mkver(const cleed_vector *y, const cleed_basic_matrix *p, const size_t n_dim)
{
  size_t i, j;
  FILE *ver_stream;
  char ver_file[FILENAME_MAX];
  char old_file[FILENAME_MAX];
  char new_file[FILENAME_MAX];
  time_t result;

  /*****************************
   * Write y/p to backup file
   *****************************/

  /* remove 'cp' system call dependence */
  sprintf(old_file, "%s%s", sr_project, ".ver");
  sprintf(new_file, "%s%s", sr_project, ".vbk");
  if (copy_file(old_file, new_file))
  {
    ERROR_MSG("failed to copy file \"%s\" -> \"%s\"", old_file, new_file);
    return(1);
  }

  strcpy(ver_file, new_file);
  ver_stream = fopen(ver_file,"w");
  fprintf(ver_stream, "%u %u %s\n", n_dim, n_dim+1, sr_project);
  for (i=0; i < n_dim+1; i++)
  {
    fprintf(ver_stream, "%e ", cleed_vector_get(y, i));
    for(j=0; j < n_dim; j++)
    {
      fprintf(ver_stream, "%e ", cleed_basic_matrix_get(p, i, j, n_dim));
    }
    fprintf(ver_stream, "\n");
  }

  /* add date */
  result = time(NULL);
  fprintf(ver_stream, "%s\n", asctime(localtime(&result)));

  fclose(ver_stream);

  return(0);
}
