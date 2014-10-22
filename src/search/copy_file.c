/************************************************************************
 *                            COPY_FILE.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 ************************************************************************/

/*! \file copy_file.c
 *
 * \author Liam Deacon
 *
 * This file contains the simple copy_file() function for copying the contents
 * of one file into another at a binary level. It effectively duplicates
 * the Unix \c cp or Windows \c copy command line programs, but within a
 * internal C environment.
 *
 */

#include <stdio.h>
#include "copy_file.h"

/*!
 * \fn copy_file
 *
 * \brief copies contents of \p src into \p dst
 *
 * \retval 0 if successful.
 * \retval -1 if \p src cannot be opened for binary reading.
 * \retval 1 if \p dst cannot be opened for binary writing.
 */
int copy_file(const char *dst, const char *src)
{
  FILE *ptr_old, *ptr_new;
  int a;

  ptr_old = fopen(src, "rb");
  ptr_new = fopen(dst, "wb");

  if(ptr_old == NULL)  return(-1); /* failed to open old file */

  if(ptr_new == NULL) /* failed to open new file */
  {
      fclose(ptr_old);
      return(1);
  }

  while(1)
  {
      a = fgetc(ptr_old);

      if(!feof(ptr_old)) fputc(a, ptr_new);
      else break;
  }

  fclose(ptr_new);
  fclose(ptr_old);

  return(0);
}
