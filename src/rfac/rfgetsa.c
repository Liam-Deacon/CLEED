/*********************************************************************
 *                           RFGETSA.C
 *
 *  Copyright 2014-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*!
 * \file
 * \brief Provides function for reading number of angles from input file
 */

#include "rfac.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

/*!
 * Returns the number of angle inputs as given by 'sa:' commnad string in
 * the CLEED input file.
 */
size_t rfac_get_number_of_angles(const char *control_filename)
{
  size_t length = 0;
  size_t sa = 0;
  char *ext = NULL;
  char linebuffer[STRSZ] = "";
  char fname_string[FILENAME_MAX] = "";
  char project_name[FILENAME_MAX] = "";
  FILE *fp = NULL;

  /* Read sa value from the .bul file */
  if (strlen(control_filename) > 3)
  {
    /* assume project name is control filename minus file extension */
    ext = strrchr(control_filename, '.');
    if (ext)
      length = ext - control_filename;
    else
      length = strlen(control_filename); /* use full path name */
  }

  strncpy(project_name, control_filename,
          (length < sizeof(project_name)-1) ? length : sizeof(project_name)-1);
  project_name[sizeof(project_name)-1] = '\0';

  strncpy(fname_string, control_filename,
          (length < sizeof(fname_string)-1) ? length : sizeof(fname_string)-1);

  if (strlen(fname_string) + 4 >= sizeof(fname_string)-1 ||
      strlen(fname_string) + 4 > FILENAME_MAX ||
      length > sizeof(fname_string)-1)
  {
    ERROR_MSG("string length too long\n");
    exit(ENAMETOOLONG);
  }
  sprintf(fname_string+length, ".bul");

  if ((fp = fopen(fname_string, "r")) == NULL)
  {
    ERROR_MSG("");
    perror(fname_string);
    exit(EIO);
  }

  while (fgets(linebuffer, STRSZ, fp) != NULL)
  {
    if (!strncasecmp(linebuffer, "sa:", 3))
    {
      if (sscanf(linebuffer+3, " %u", &sa) < 1)
      {
        ERROR_MSG("could not read value from '%s'", linebuffer);
      }
    }
  }  /*while*/

  fclose(fp);

  return sa;
}
